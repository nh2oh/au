#include "midi_container.h"
#include "dbklib\binfile.h"


namespace mc {

detect_chunk_type_result_t detect_chunk_type(const unsigned char *p) {
	detect_chunk_type_result_t result {};
	
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

	int32_t data_length = midi_raw_interpret<int32_t>(p);
	p+=4;

	if (data_length < 0) {
		result.msg = "MIDI chunks must have data length >= 0";
		result.is_valid = false;
		return result;
	}

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
	}
	result.is_valid = true;
	return result;
}



std::string print(const mthd_container_t& mthd) {
	std::string s {};

	s += ("Format type = " + std::to_string(fmt_type(mthd)) + "    \n");
	s += ("Num Tracks = " + std::to_string(num_trks(mthd)) + "    \n");

	s += "Time Division = ";
	auto timediv_type = detect_midi_time_division_type(mthd);
	if (timediv_type == midi_time_division_field_type_t::SMPTE) {
		s += "(SMPTE) WTF";
	} else if (timediv_type == midi_time_division_field_type_t::ticks_per_quarter) {
		s += "(ticks-per-quarter-note) ";
		s += std::to_string(interpret_tpq_field(mthd));
	}
	s += "\n";

	return s;
}






int16_t fmt_type(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin();
	return midi_raw_interpret<int16_t>(p);
};
int16_t num_trks(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin()+2;
	return midi_raw_interpret<int16_t>(p);
};

midi_time_division_field_type_t detect_midi_time_division_type(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin()+4;
	if ((*p)>>7 == 1) {
		return midi_time_division_field_type_t::SMPTE;
	} else {
		return midi_time_division_field_type_t::ticks_per_quarter;
	}
}
// assumes midi_time_division_field_type_t::ticks_per_quarter
uint16_t interpret_tpq_field(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin()+4;

	std::array<unsigned char,2> field {(*p)&0x7F, *(++p)};
	return midi_raw_interpret<uint16_t>(&(field[0]));
}
// assumes midi_time_division_field_type_t::SMPTE
midi_smpte_field interpret_smpte_field(const mthd_container_t& mthd) {
	const unsigned char *p = mthd.begin()+8;

	midi_smpte_field result {};
	result.time_code_fmt = static_cast<int8_t>(*p);
	result.units_per_frame = static_cast<uint8_t>(*(++p));
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
unsigned char *mtrk_event_container_t::begin() const {
	return this->p_;
}
// Starts just after the delta-time
unsigned char *mtrk_event_container_t::data_begin() const {
	return (this->p_ + midi_interpret_vl_field(this->p_).N);
}
unsigned char *mtrk_event_container_t::end() const {
	return this->p_ + this->size_;
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
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char *p) {
	validate_mtrk_chunk_result_t result {};

	detect_chunk_type_result_t chunk_detect = detect_chunk_type(p);
	if (!(chunk_detect.is_valid) || chunk_detect.type != chunk_type::track) {
		result.is_valid = false;
		result.msg += "!(chunk_detect.is_valid) || chunk_detect.type != midi_chunk_t::track\n";
		result.msg += ("chunk_detect.msg = " + chunk_detect.msg + "\n");
		return result;
	}

	// Validate each mtrk event in the chunk
	auto mtrk_reported_size = midi_raw_interpret<int32_t>(p+4);
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
			curr_event_length = curr_event.delta_t.N + sx.size;
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
			curr_event_length = curr_event.delta_t.N + mt.size;
		} else if (curr_event.type == event_type::midi) {
			auto md = parse_midi_event(p+i,most_recent_midi_status_byte);
			if (!(md.is_valid)) {
				result.is_valid = false;
				result.msg = "!(md.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + md.size;
			if (!(md.running_status)) {
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













mtrk_event_container_t mtrk_container_iterator::operator*() const {
	const unsigned char *p = this->container_->p_+this->container_offset_;
	auto sz = parse_midi_event(p, this->midi_status_).size;
	return mtrk_event_container_t {p,sz};
}

mtrk_container_iterator& mtrk_container_iterator::operator++() {
	const unsigned char *curr_p = this->container_->p_ + this->container_offset_;
	parse_mtrk_event_result_t curr_event = parse_mtrk_event_type(curr_p);
	int32_t curr_size {0};
	if (curr_event.type==event_type::sysex) {
		auto sx = parse_sysex_event(curr_p);
		curr_size = sx.size;
	} else if (curr_event.type==event_type::meta) {
		auto mt = parse_meta_event(curr_p);
		curr_size = mt.size;
	} else if (curr_event.type==event_type::midi) {
		auto md = parse_midi_event(curr_p,this->midi_status_);
		curr_size = md.size;
	}

	this->container_offset_ += curr_size;
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
		if (!md.running_status) {
			// The present (new) event contains a status byte
			this->midi_status_ = midi_event_get_status_byte(new_p);
		}
	}

	return *this;
}






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
}


};  // namespace mc






