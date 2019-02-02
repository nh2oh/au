#include "midi_container.h"



namespace mc {

mc::detect_chunk_type_result_t detect_midi_chunk_type(const unsigned char *p) {
	mc::detect_chunk_type_result_t result {};
	
	// All chunks begin w/ A 4-char identifier
	std::string idstr {};
	std::copy(p,p+4,std::back_inserter(idstr));
	if (idstr == "MThd") {
		result.type = mc::midi_chunk_t::header;
	} else if (idstr == "MTrk") {
		result.type = mc::midi_chunk_t::track;
	} else {
		result.type = mc::midi_chunk_t::unknown;
	}
	p+=4;

	int32_t data_length = midi_raw_interpret<int32_t>(p);
	p+=4;

	if (data_length < 0) {
		result.msg = "MIDI chunks must have data length >= 0";
		result.is_valid = false;
		return result;
	}
	if (result.type == mc::midi_chunk_t::header && data_length != 6) {
		result.msg = "MThd chunks must have a data length of 6";
		result.is_valid = false;
		return result;
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
	if (timediv_type == mc::midi_time_division_field_type_t::SMPTE) {
		s += "(SMPTE) WTF";
	} else if (timediv_type == mc::midi_time_division_field_type_t::ticks_per_quarter) {
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





int32_t mtrk_event_container_t::size() const {
	int32_t result {0};
	auto p = this->beg_;
	auto delta_t_vl = midi_interpret_vl_field(p);
	p += delta_t_vl.N;
	result += delta_t_vl.N;
	
	switch (detect_mtrk_event_type(this->beg_)) {
		case mtrk_event_container_t::event_type::midi:  
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
		case mtrk_event_container_t::event_type::meta:
			p += 2;  // Go past the 0xFF ('id') and 'type' bytes
			result += 2;
			auto vl_len_field = midi_interpret_vl_field(p);
			result += (vl_len_field.N + vl_len_field.val);
			return result;
		case mtrk_event_container_t::event_type::sysex:
			p += 1;  // Go past the 'id' byte
			result += 1;
			auto vl_len_field = midi_interpret_vl_field(p);
			result += (vl_len_field.N + vl_len_field.val);
			return result;
		default:  
			std::abort();
	}
}

int32_t mtrk_event_container_t::data_length() const {
	return (this->size() - midi_interpret_vl_field(this->beg_).N);
}
const unsigned char *mtrk_event_container_t::begin() const {
		return this->beg_;
}


mtrk_event_container_t::event_type detect_mtrk_event_type(const unsigned char *p) {
	// All mtrk events begin with a delta-time occupying a maximum of 4 bytes
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		return mtrk_event_container_t::event_type::invalid;
	}
	p += delta_t_vl.N;

	if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		return mtrk_event_container_t::event_type::sysex;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		return mtrk_event_container_t::event_type::meta;
	} else {
		// midi event
		return mtrk_event_container_t::event_type::midi;
	}
}


midi_vl_field_interpreted delta_time(const mtrk_event_container_t& mtrkev) {
	return midi_interpret_vl_field(mtrkev.begin());
}

/*
midi_vl_field_interpreted sysex_event_length(const unsigned char *p) {

midi_vl_field_interpreted midi_event_length(const unsigned char*);
midi_vl_field_interpreted meta_event_length(const unsigned char*);
*/






};  // namespace mc






