#include "midi_container.h"



namespace mc {



std::string print(const midi_chunk_container_t<whatever::header>& mthd) {
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






int16_t fmt_type(const midi_chunk_container_t<whatever::header>& mthd) {
	const unsigned char *p = mthd.p_+8;
	return midi_raw_interpret<int16_t>(p);
};
int16_t num_trks(const midi_chunk_container_t<whatever::header>& mthd) {
	const unsigned char *p = mthd.p_+10;
	return midi_raw_interpret<int16_t>(p);
};

midi_time_division_field_type_t detect_midi_time_division_type(const midi_chunk_container_t<whatever::header>& mthd) {
	const unsigned char *p = mthd.p_+12;
	if ((*p)>>7 == 1) {
		return midi_time_division_field_type_t::SMPTE;
	} else {
		return midi_time_division_field_type_t::ticks_per_quarter;
	}
}
// assumes midi_time_division_field_type_t::ticks_per_quarter
uint16_t interpret_tpq_field(const midi_chunk_container_t<whatever::header>& mthd) {
	const unsigned char *p = mthd.p_+12;

	std::array<unsigned char,2> field {(*p)&0x7F, *(++p)};
	return mc::midi_raw_interpret<uint16_t>(&(field[0]));
}
// assumes midi_time_division_field_type_t::SMPTE
midi_smpte_field interpret_smpte_field(const midi_chunk_container_t<whatever::header>& mthd) {
	const unsigned char *p = mthd.p_+12;

	midi_smpte_field result {};
	result.time_code_fmt = static_cast<int8_t>(*p);
	result.units_per_frame = static_cast<uint8_t>(*(++p));
	return result;
}


};  // namespace mc






