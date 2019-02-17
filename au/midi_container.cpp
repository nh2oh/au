#include "midi_container.h"
#include "midi_raw.h"
#include "dbklib\binfile.h"
#include <iostream>
#include <string>
#include <vector>

namespace mc {


int midi_example() {
	auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID").d;
	//auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\test.mid").d;
	auto rawfile_check_result = validate_smf(&rawfiledata[0],rawfiledata.size(),
		"C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID");

	smf_container_t mf {rawfile_check_result};
	
	std::cout << print(mf) << std::endl << std::endl;

	//auto h = mf.get_header();
	//std::cout << print(h) << std::endl << std::endl;
	//auto t1 = mf.get_track(0);
	//std::cout << "TRACK 1\n" << print(t1) << std::endl << std::endl;
	//auto t2 = mf.get_track(1);
	//std::cout << "TRACK 2\n" << print(t2) << std::endl << std::endl;
	//auto t3 = mf.get_track(2);
	//std::cout << "TRACK 3\n" << print(t3) << std::endl << std::endl;

	return 0;
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


midi_time_division_field_type_t detect_midi_time_division_type(uint16_t division_field) {
	if ((division_field>>15) == 1) {
		return midi_time_division_field_type_t::SMPTE;
	} else {
		return midi_time_division_field_type_t::ticks_per_quarter;
	}
}
// assumes midi_time_division_field_type_t::ticks_per_quarter
uint16_t interpret_tpq_field(uint16_t division_field) {
	return division_field&(0x7FFF);
}
// assumes midi_time_division_field_type_t::SMPTE
midi_smpte_field interpret_smpte_field(uint16_t division_field) {
	midi_smpte_field result {};
	result.time_code_fmt = static_cast<int8_t>(division_field>>8);
	result.units_per_frame = static_cast<uint8_t>(division_field&0xFF);
	return result;
}


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
	//return this->data_size()+4;
	return this->size_;
}
mtrk_container_iterator_t mtrk_container_t::begin() const {
	// From the std p.135:  "The first event in each MTrk chunk must specify status"
	// thus we know midi_event_get_status_byte(this->p_+8) will succeed
	return mtrk_container_iterator_t {this, int32_t{8}, midi_event_get_status_byte(this->p_+8)};
}
mtrk_container_iterator_t mtrk_container_t::end() const {
	// Note that i am supplying an invalid midi status byte for this one-past-the-end 
	// iterator.  
	return mtrk_container_iterator_t {this, this->size(), unsigned char {0}};
}

std::string print(const mtrk_container_t& mtrk) {
	std::string s {};

	for (mtrk_container_iterator_t it = mtrk.begin(); it != mtrk.end(); ++it) {
		s += print(*it);
		s += "\n";
	}
	//for (auto const& e : mtrk) {
	//	s += print(e);
	//	s += "\n";
	//}

	return s;
}



mtrk_event_container_t mtrk_container_iterator_t::operator*() const {
	const unsigned char *p = this->container_->p_+this->container_offset_;
	auto sz = parse_midi_event(p, this->midi_status_).data_size;
	return mtrk_event_container_t {p,sz};
}

mtrk_container_iterator_t& mtrk_container_iterator_t::operator++() {
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

bool mtrk_container_iterator_t::operator<(const mtrk_container_iterator_t& rhs) const {
	if (this->container_ == rhs.container_) {
		return this->container_offset_ < rhs.container_offset_;
	} else {
		return false;
	}
}
bool mtrk_container_iterator_t::operator==(const mtrk_container_iterator_t& rhs) const {
	// I *could* compare midi_status_ as well, however, it is an error condition if it's
	// different for the same offset.  
	if (this->container_ == rhs.container_) {
		return this->container_offset_ == rhs.container_offset_;
	} else {
		return false;
	}
}
bool mtrk_container_iterator_t::operator!=(const mtrk_container_iterator_t& rhs) const {
	return !(*this==rhs);
}


smf_container_t::smf_container_t(const validate_smf_result_t& maybe_smf) {
	if (!maybe_smf.is_valid) {
		std::abort();
	}

	this->fname_ = maybe_smf.fname;
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

std::string smf_container_t::fname() const {
	return this->fname_;
}

std::string print(const smf_container_t& smf) {
	std::string s {};

	s += smf.fname();
	s += "\n";

	auto mthd = smf.get_header();
	s += "Header (MThd) \t(data_size = ";
	s += std::to_string(mthd.data_size()) ;
	s += ", size = ";
	s += std::to_string(mthd.size());
	s += "):\n";
	s += print(mthd);
	s += "\n";

	for (int i=0; i<smf.n_tracks(); ++i) {
		auto curr_trk = smf.get_track(i);
		s += ("Track (MTrk) " + std::to_string(i) 
			+ "\t(data_size = " + std::to_string(curr_trk.data_size())
			+ ", size = " + std::to_string(curr_trk.size()) + "):\n");
		s += print(curr_trk);
		s += "\n";
	}

	return s;
}










};  // namespace mc






