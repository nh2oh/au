#include "midi_container.h"
#include "dbklib\binfile.h"
#include <iostream>
#include <string>
#include <vector>

namespace mc {


void midi_example() {
	auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID").d;
	//auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\test.mid").d;
	auto rawfile_check_result = validate_smf(&rawfiledata[0],rawfiledata.size());

	smf_container_t mf {rawfile_check_result};
	
	auto h = mf.get_header();
	std::cout << print(h) << std::endl << std::endl;
	auto t1 = mf.get_track(1);
	std::cout << print(t1) << std::endl << std::endl;
}

// Default value of sep == ' '; see header
std::string print_hexascii(const unsigned char *p, int n, const char sep) {
	std::string s {};  s.reserve(3*n);

	std::array<unsigned char,16> nybble2ascii {'0','1','2','3','4','5',
		'6','7','8','9','A','B','C','D','E','F'};

	for (int i=0; i<n; ++i) {
		s += nybble2ascii[((*p) & 0xF0)>>4];
		s += nybble2ascii[((*p) & 0x0F)];
		s += sep;
		++p;
	}

	return s;
}




std::string print(const event_type& et) {
	if (et == event_type::meta) {
		return "meta";
	} else if (et == event_type::midi) {
		return "midi";
	} else if (et == event_type::sysex) {
		return "sysex";
	}
}


detect_chunk_type_result_t detect_chunk_type(const unsigned char *p, int32_t max_size) {
	detect_chunk_type_result_t result {};
	result.p = p;
	if (max_size < 8) {
		result.is_valid = false;
		return result;
	}

	// All chunks begin w/ A 4-char identifier
	std::string idstr {};
	std::copy(p,p+4,std::back_inserter(idstr));
	if (idstr == "MThd") {
		result.type = chunk_type::header;
	} else if (idstr == "MTrk") {
		result.type = chunk_type::track;
	} else {
		result.type = chunk_type::unknown;
	}
	p+=4;

	auto data_length = midi_raw_interpret<int32_t>(p);
	p+=4;

	if (data_length < 0) {
		result.msg = "MIDI chunks must have data length >= 0";
		result.is_valid = false;
		return result;
	}
	if (data_length+8 > max_size) {
		result.msg = "data_length+8 > max_size";
		result.is_valid = false;
		return result;
	}
	/*
	if (result.type == chunk_type::header && data_length != 6) {
		result.msg = "MThd chunks must have a data length of 6";
		result.is_valid = false;
		return result;
	} else if (result.type == chunk_type::track) {
		auto first_event = parse_mtrk_event_type(p);
		if (!(first_event.is_valid) || first_event.type != event_type::midi
			|| !midi_event_has_status_byte(p)) {
			result.msg = "MTrk chunks must begin w/a MIDI event containing a status byte";
			result.is_valid = false;
			return result;
		}
	}*/
	result.size = 4 + 4 + data_length;  // id + length_field + ...

	result.is_valid = true;
	return result;
}


mthd_container_t::mthd_container_t(const detect_chunk_type_result_t& mthd) {
	if (!mthd.is_valid || mthd.type != chunk_type::header) {
		std::abort();
	}
	this->p_=mthd.p;
	this->size_=mthd.size;
}
int16_t mthd_container_t::format() const {
	return midi_raw_interpret<int16_t>(this->p_+8);
}
int16_t mthd_container_t::ntrks() const {
	return midi_raw_interpret<int16_t>(this->p_+8+2);
}
uint16_t mthd_container_t::division() const {
	return midi_raw_interpret<uint16_t>(this->p_+8+2+2);
}
int32_t mthd_container_t::size() const {
	return this->size_;
}
int32_t mthd_container_t::data_size() const {
	return this->size_-8;
}

parse_meta_event_result_t parse_meta_event(const unsigned char *p) {
	parse_meta_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p != 0xFF) {
		result.is_valid = false;
		return result;
	}
	++p;

	result.type = *p;
	++p;

	auto length_field = midi_interpret_vl_field(p);
	result.data_size = 2 + length_field.N + length_field.val;

	result.is_valid = true;
	return result;
}

parse_sysex_event_result_t parse_sysex_event(const unsigned char *p) {
	parse_sysex_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p != 0xF0 || *p != 0xFF) {
		result.is_valid = false;
		return result;
	}
	result.type = *p;
	++p;

	auto length_field = midi_interpret_vl_field(p);
	result.data_size = 1 + length_field.N + length_field.val;

	result.is_valid = true;
	return result;
}

parse_midi_event_result_t parse_midi_event(const unsigned char *p, unsigned char prev_status_byte) {
	parse_midi_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	result.data_size = 0;
	if ((*p & 0x80) == 0x80) {  // Present message has a status byte
		result.has_status_byte = true;
		result.status_byte = *p;
		result.data_size += 1;
		++p;
	} else {
		result.has_status_byte = false;
		if ((prev_status_byte& 0x80) != 0x80) {
			// prev_status_byte is invalid
			result.is_valid = false;
			return result;
		}
		result.status_byte = prev_status_byte;
		// Not incrementing p; *p is the first data byte in running-status mode
	}
	// At this point, p points at the first data byte

	if ((result.status_byte & 0xF0) == 0xC0 || (result.status_byte & 0xF0) == 0xD0) {
		result.n_data_bytes = 1;
	} else {
		result.n_data_bytes = 2;
	}
	result.data_size += result.n_data_bytes;

	// Check first data byte
	if ((*p & 0x80) != 0) {
		result.is_valid = false;
		return result;
	}

	// Optionally check second data byte
	if (result.n_data_bytes == 2) {
		++p;
		if ((*p & 0x80) != 0) {
			result.is_valid = false;
			return result;
		}
	}

	result.is_valid = true;
	return result;
}

std::string print(const mthd_container_t& mthd) {
	std::string s {};
	s += ("Data size = " + std::to_string(mthd.data_size()) + "    \n");
	s += ("Format type = " + std::to_string(mthd.format()) + "    \n");
	s += ("Num Tracks = " + std::to_string(mthd.ntrks()) + "    \n");

	s += "Time Division = ";
	auto timediv_type = detect_midi_time_division_type(mthd.division());
	if (timediv_type == midi_time_division_field_type_t::SMPTE) {
		s += "(SMPTE) WTF";
	} else if (timediv_type == midi_time_division_field_type_t::ticks_per_quarter) {
		s += "(ticks-per-quarter-note) ";
		s += std::to_string(interpret_tpq_field(mthd.division()));
	}
	s += "\n";

	return s;
}





/*
int16_t fmt_type(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin();
	return midi_raw_interpret<int16_t>(p);
};
int16_t num_trks(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin()+2;
	return midi_raw_interpret<int16_t>(p);
};*/

midi_time_division_field_type_t detect_midi_time_division_type(uint16_t division_field) {
	if ((division_field>>15) == 1) {
		return midi_time_division_field_type_t::SMPTE;
	} else {
		return midi_time_division_field_type_t::ticks_per_quarter;
	}

	//const unsigned char *p = mthd.begin()+4;
	//if ((*p)>>7 == 1) {
	//	return midi_time_division_field_type_t::SMPTE;
	//} else {
	//	return midi_time_division_field_type_t::ticks_per_quarter;
	//}
}
// assumes midi_time_division_field_type_t::ticks_per_quarter
uint16_t interpret_tpq_field(uint16_t division_field) {
	return division_field&(0x7FFF);
	
	//const unsigned char *p = mthd.begin()+4;
	//std::array<unsigned char,2> field {(*p)&0x7F, *(++p)};
	//return midi_raw_interpret<uint16_t>(&(field[0]));
}
// assumes midi_time_division_field_type_t::SMPTE
midi_smpte_field interpret_smpte_field(uint16_t division_field) {
	midi_smpte_field result {};
	result.time_code_fmt = static_cast<int8_t>(division_field>>8);
	result.units_per_frame = static_cast<uint8_t>(division_field&0xFF);
	
	//const unsigned char *p = mthd.begin()+8;
	//midi_smpte_field result {};
	//result.time_code_fmt = static_cast<int8_t>(*p);
	//result.units_per_frame = static_cast<uint8_t>(*(++p));
	return result;
}



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------



/*
int32_t mtrk_event_container_t::size() const {
	int32_t result {0};
	auto p = this->beg_;
	auto delta_t_vl = midi_interpret_vl_field(p);
	p += delta_t_vl.N;
	result += delta_t_vl.N;
	
	switch (detect_mtrk_event_type(this->beg_)) {
		case event_type::midi:  
			if (this->midi_status_ > 0) {
				result += 1;  // event contains a status byte (running_status == false)
			}
			char abs_ms = std::abs(this->midi_status_);
			if ((abs_ms & 0xF0) == 0xC0 || (abs_ms & 0xF0) == 0xD0) {  // MSBit == 1 => byte is a status byte
				result += 1;
			} else {
				result += 2;
			}
			return result;
		case event_type::meta:
			p += 2;  // Go past the 0xFF ('id') and 'type' bytes
			result += 2;
			auto vl_len_field = midi_interpret_vl_field(p);
			result += (vl_len_field.N + vl_len_field.val);
			return result;
		case event_type::sysex:
			p += 1;  // Go past the 'id' byte
			result += 1;
			auto vl_len_field = midi_interpret_vl_field(p);
			result += (vl_len_field.N + vl_len_field.val);
			return result;
		default:  
			std::abort();
	}
}*/

int32_t mtrk_event_container_t::data_size() const {
	return (this->size() - midi_interpret_vl_field(this->p_).N);
}
int32_t mtrk_event_container_t::delta_time() const {
	return midi_interpret_vl_field(this->p_).val;
}
int32_t mtrk_event_container_t::size() const {
	return this->size_;
}
// Assumes that this indicates a valid event
event_type mtrk_event_container_t::type() const {
	return detect_mtrk_event_type_unsafe(this->data_begin());
}
// Starts at the delta-time
const unsigned char *mtrk_event_container_t::begin() const {
	return this->p_;
}
// Starts just after the delta-time
const unsigned char *mtrk_event_container_t::data_begin() const {
	return (this->p_ + midi_interpret_vl_field(this->p_).N);
}
const unsigned char *mtrk_event_container_t::end() const {
	return this->p_ + this->size_;
}

std::string print(const mtrk_event_container_t& evnt) {
	std::string s {};
	s += ("delta_time == " + std::to_string(evnt.delta_time()) + ", ");
	s += ("type == " + print(evnt.type()) + ", ");
	s += ("data_size == " + std::to_string(evnt.data_size()) + ", ");
	s += ("size == " + std::to_string(evnt.size()) + "\n\t");
	s += print_hexascii(evnt.data_begin(), evnt.end()-evnt.data_begin(), ' ');

	return s;
}

event_type detect_mtrk_event_type_dtstart_unsafe(const unsigned char *p) {
	auto dt = midi_interpret_vl_field(p);
	p += dt.N;
	return detect_mtrk_event_type_unsafe(p);
}
// Pointer to the first data byte (following the delta-time), _not_ to the start of
// the delta-time.  
event_type detect_mtrk_event_type_unsafe(const unsigned char *p) {
	if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		return event_type::sysex;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		return event_type::meta;
	} else {
		// midi event
		return event_type::midi;
	}
}

// Pointer to the first byte of the vl delta-time field.
// If the delta-time validates, the event is either meta, sysex, or midi (by the std, 
// anything not sysex or meta is midi).  This function is therefore somewhat useless.
// You might as well just validate the delta-time then call detect_mtrk_event_type_unsafe().  
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char *p) {
	parse_mtrk_event_result_t result {};
	// All mtrk events begin with a delta-time occupying a maximum of 4 bytes
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		result.is_valid = true;
		result.type=event_type::sysex;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		result.is_valid = true;
		result.type = event_type::meta;
	} else {
		// midi event
		result.is_valid = true;
		result.type = event_type::midi;
	}
	return result;
}

bool midi_event_has_status_byte(const unsigned char *p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return (*p) & 0x80;
}
unsigned char midi_event_get_status_byte(const unsigned char* p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return *p;
}


//
// Checks that p points at the start of a valid MTrk chunk (ie, the 'M' of MTrk...),
// then moves through the all events in the track and validates that each begins with a
// valid vl delta-time, is one of midi, meta, or sysex, and that for each such event a
// valid mtrk_event_container_t object can be created if necessary (it must be possible to
// determine the size in bytes of each event).  
//
// If the input is a valid MTrk chunk, the validate_mtrk_chunk_result_t returned can be
// passed to the ctor of mtrk_container_t to instantiate a valid object.  
//
// All the rules of the midi standard as regards sequences of MTrk events are validated
// here for the case of the single track indicated by the input.  Ex, that each midi 
// event has a status byte (either as part of the event or implictly through running 
// status), etc.  
// 
//
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char *p, int32_t max_size) {
	validate_mtrk_chunk_result_t result {};

	detect_chunk_type_result_t chunk_detect = detect_chunk_type(p,max_size);
	if (!(chunk_detect.is_valid) || chunk_detect.type != chunk_type::track) {
		result.is_valid = false;
		result.msg += "!(chunk_detect.is_valid) || chunk_detect.type != midi_chunk_t::track\n";
		result.msg += ("chunk_detect.msg = " + chunk_detect.msg + "\n");
		return result;
	}

	// Validate each mtrk event in the chunk
	auto mtrk_reported_size = chunk_detect.size;  // midi_raw_interpret<int32_t>(p+4);
	int32_t i {8};
	unsigned char most_recent_midi_status_byte {0};
	while (i<mtrk_reported_size) {
		int32_t curr_event_length {0};

		// Classify the present event as midi, sysex, or meta
		auto curr_event = parse_mtrk_event_type(p+i);
		if (!(curr_event.is_valid)) {
			result.is_valid = false;
			result.msg += "!(curr_event.is_valid)";
			return result;
		}

		if (curr_event.type == event_type::sysex) {
			// From the std (p.136):
			// Sysex events and meta-events cancel any running status which was in effect.  
			// Running status does not apply to and may not be used for these messages.
			//
			most_recent_midi_status_byte = unsigned char {0};
			auto sx = parse_sysex_event(p+i);
			if (!(sx.is_valid)) {
				result.is_valid = false;
				result.msg = "!(sx.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + sx.data_size;
		} else if (curr_event.type == event_type::meta) {
			// From the std (p.136):
			// Sysex events and meta-events cancel any running status which was in effect.  
			// Running status does not apply to and may not be used for these messages.
			//
			most_recent_midi_status_byte = unsigned char {0};
			auto mt = parse_meta_event(p+i);
			if (!(mt.is_valid)) {
				result.is_valid = false;
				result.msg = "!(mt.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + mt.data_size;
		} else if (curr_event.type == event_type::midi) {
			auto md = parse_midi_event(p+i,most_recent_midi_status_byte);
			if (!(md.is_valid)) {
				result.is_valid = false;
				result.msg = "!(md.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + md.data_size;
			if (!(md.has_status_byte)) {
				most_recent_midi_status_byte = midi_event_get_status_byte(p+i);
			}
		} else {
			std::abort();
		}

		i += curr_event_length;
	}

	if (i != mtrk_reported_size) {
		result.is_valid = false;
		result.msg += "i != mtrk_reported_size";
		return result;
	}

	result.is_valid = true;
	result.size = i;
	result.p = p;
	return result;
}

mtrk_container_t::mtrk_container_t(const validate_mtrk_chunk_result_t& mtrk) {
	if (!mtrk.is_valid) {
		std::abort();
	}

	this->p_ = mtrk.p;
	this->size_ = mtrk.size;
}
mtrk_container_t::mtrk_container_t(const unsigned char *p, int32_t sz) {
	this->p_ = p;
	this->size_ = sz;
}

int32_t mtrk_container_t::data_size() const {
	return midi_raw_interpret<int32_t>(this->p_+4);
}
int32_t mtrk_container_t::size() const {
	return this->data_size()+4;
}
mtrk_container_iterator mtrk_container_t::begin() const {
	// From the std p.135:  "The first event in each MTrk chunk must specify status"
	// thus we know midi_event_get_status_byte(this->p_+8) will succeed
	return mtrk_container_iterator {this, int32_t{8}, midi_event_get_status_byte(this->p_+8)};
}
mtrk_container_iterator mtrk_container_t::end() const {
	// Note that i am supplying an invalid midi status byte for this one-past-the-end 
	// iterator.  
	return mtrk_container_iterator {this, this->size(),unsigned char {0}};
}

std::string print(const mtrk_container_t& mtrk) {
	std::string s {};

	for (auto const& e : mtrk) {
		s += print(e);
		s += "\n";
	}

	return s;
}











mtrk_event_container_t mtrk_container_iterator::operator*() const {
	const unsigned char *p = this->container_->p_+this->container_offset_;
	auto sz = parse_midi_event(p, this->midi_status_).data_size;
	return mtrk_event_container_t {p,sz};
}

mtrk_container_iterator& mtrk_container_iterator::operator++() {
	// Get the size of the presently indicated event and increment 
	// this->container_offset_ by that ammount.  
	const unsigned char *curr_p = this->container_->p_ + this->container_offset_;
	parse_mtrk_event_result_t curr_event = parse_mtrk_event_type(curr_p);
	int32_t curr_size {0};
	if (curr_event.type==event_type::sysex) {
		auto sx = parse_sysex_event(curr_p);
		curr_size = sx.delta_t.N + sx.data_size;
	} else if (curr_event.type==event_type::meta) {
		auto mt = parse_meta_event(curr_p);
		curr_size = mt.delta_t.N + mt.data_size;
	} else if (curr_event.type==event_type::midi) {
		auto md = parse_midi_event(curr_p,this->midi_status_);
		curr_size = md.delta_t.N + md.data_size;
	}

	this->container_offset_ += curr_size;

	// If this was the last event set the midi_status_ to 0 and return.  The value
	// of 0 is also set for the value of midi_status_ when an iterator is returned
	// from the end() method of the parent container.  
	if (this->container_offset_ == this->container_->size_) {
		this->midi_status_ = 0;
		return *this;
	}

	// If this was not the last event, set midi_status_ for the event pointed to by 
	// this->container_offset_.  Below i refer to this as the "new" event.  
	const unsigned char *new_p = this->container_->p_ + this->container_offset_;
	parse_mtrk_event_result_t new_event = parse_mtrk_event_type(new_p);
	if (new_event.type==event_type::sysex) {
		this->midi_status_ = 0;  // sysex && meta events reset midi status
	} else if (new_event.type==event_type::meta) {
		this->midi_status_ = 0;  // sysex && meta events reset midi status
	} else if (new_event.type==event_type::midi) {
		// this->midi_status_ currently holds the midi status byte applicable to the
		// previous event
		auto md = parse_midi_event(new_p,this->midi_status_);
		if (!md.has_status_byte) {
			// The present (new) event contains a status byte
			this->midi_status_ = midi_event_get_status_byte(new_p);
		}
	}

	return *this;
}

bool mtrk_container_iterator::operator<(const mtrk_container_iterator& rhs) const {
	if (this->container_ == rhs.container_) {
		return this->container_offset_ < rhs.container_offset_;
	} else {
		return false;
	}
}
bool mtrk_container_iterator::operator==(const mtrk_container_iterator& rhs) const {
	// I *could* compare midi_status_ as well, however, it is an error condition if it's
	// different for the same offset.  
	if (this->container_ == rhs.container_) {
		return this->container_offset_ == rhs.container_offset_;
	} else {
		return false;
	}
}
bool mtrk_container_iterator::operator!=(const mtrk_container_iterator& rhs) const {
	return !(*this==rhs);
}

//
//TODO:  The offset checks are repetitive...
//
validate_smf_result_t validate_smf(const unsigned char *p, int32_t offset_end) {
	validate_smf_result_t result {};
	int n_tracks {0};
	int n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};

	int32_t offset {0};
	while (offset<offset_end) {  // For each chunk...
		detect_chunk_type_result_t curr_chunk = detect_chunk_type(p+offset,offset_end-offset);
		if (!curr_chunk.is_valid) {
			result.is_valid = false;
			result.msg += "!curr_chunk.is_valid\ncurr_chunk_type.msg== ";
			result.msg += curr_chunk.msg;
			return result;
		}

		if (curr_chunk.type == chunk_type::header) {
			if (offset > 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::header but offset > 0.  ";
				result.msg += "A valid midi file must contain only one MThd @ the very start.  \n";
				return result;
			}
		}

		if (curr_chunk.type == chunk_type::track) {
			if (offset == 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::track but offset == 0.  ";
				result.msg += "A valid midi file must begin w/an MThd chunk.  \n";
				return result;
			}
			validate_mtrk_chunk_result_t curr_track = validate_mtrk_chunk(p+offset,offset_end-offset);
			if (!curr_track.is_valid) {
				result.msg += "!curr_track.is_valid\ncurr_track.msg==";
				result.msg += curr_track.msg;
				return result;
			}
			++n_tracks;
		} else if (curr_chunk.type == chunk_type::unknown) {
			++n_unknown;
			if (offset == 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::unknown but offset == 0.  ";
				result.msg += "A valid midi file must begin w/an MThd chunk.  \n";
				return result;
			}
		}
		chunk_idxs.push_back({curr_chunk.type,offset,curr_chunk.size});
		offset += curr_chunk.size;
	}

	result.is_valid = true;
	result.chunk_idxs = chunk_idxs;
	result.n_mtrk = n_tracks;
	result.n_unknown = n_unknown;
	result.size = offset;
	result.p = p;

	return result;
}




/*
read_smf_result_t read_smf(const std::filesystem::path& fp) {
	read_smf_result_t result {};
	auto data = dbk::readfile(fp).d;
	std::vector<int32_t> chunk_offsets {};

	int32_t i {0};
	detect_chunk_type_result_t curr_chunk_type = detect_midi_chunk_type(&data[i]);
	if (!curr_chunk_type.is_valid || curr_chunk_type.type != chunk_type::header) {
		result.is_valid = false;
		result.msg += "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::header\n";
		result.msg += "A valid midi file must begin with an MThd chunk.  \n";
		return result;
	}
	chunk_offsets.push_back(i);
	mthd_container_t mthd {&data[i]};

	i += mthd.size();
	while (i<data.size()) {  // For each Track trunk...
		curr_chunk_type = detect_midi_chunk_type(&data[i]);
		if (!curr_chunk_type.is_valid || curr_chunk_type.type != chunk_type::track) {
			result.is_valid = false;
			result.msg += "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::track\n";
			result.msg += "A valid midi file must contain MTrk chunks following the MThd chunk.  \n";
			return result;
		}
		chunk_offsets.push_back(i);

		auto check_curr_mtrk = validate_mtrk_chunk(&data[i]);
		if (!(check_curr_mtrk.is_valid)) {
			result.is_valid = false;
			result.msg += check_curr_mtrk.msg;
			return result;
		}

		mtrk_container_t curr_mtrk {&data[i]};
		i += curr_mtrk.size();
	}

	result.smf = smf_container_t(chunk_offsets,data);
	return result;
}*/



smf_container_t::smf_container_t(const validate_smf_result_t& maybe_smf) {
	if (!maybe_smf.is_valid) {
		std::abort();
	}

	this ->chunk_idxs_ = maybe_smf.chunk_idxs;
	this->n_mtrk_ = maybe_smf.n_mtrk;
	this->n_unknown_ = maybe_smf.n_unknown;
	this->p_ = maybe_smf.p;
	this->size_ = maybe_smf.size;
}

int smf_container_t::n_tracks() const {
	return this->n_mtrk_;
}
int smf_container_t::n_chunks() const {
	return this->n_mtrk_ + this->n_unknown_ + 1;  // +1 => MThd
}
mthd_container_t smf_container_t::get_header() const {
	// I am assuming this->chunk_idxs_[0] has .type == chunk_type::header
	// and offser == 0
	return mthd_container_t {this->p_,this->chunk_idxs_[0].size};
}
mtrk_container_t smf_container_t::get_track(int n) const {
	int curr_trackn {0};
	for (const auto& e : this->chunk_idxs_) {
		if (e.type == chunk_type::track) {
			if (n == curr_trackn) {
				return mtrk_container_t {this->p_ + e.offset, e.size};
			}
			++curr_trackn;
		}
	}

	std::abort();
}
bool smf_container_t::get_chunk(int n) const {
	// don't yet have a generic chunk container

	return false;
}












};  // namespace mc






