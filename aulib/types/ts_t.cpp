#include "ts_t.h"
#include "beat_bar_t.h"
#include "nv_t.h"
#include "..\util\au_error.h"
#include "..\util\au_util.h"
#include <string>
#include <optional>


ts_t::ts_t(beat_t const& num, d_t const& denom, bool const& is_compound_in) {
	au_assert(num > beat_t{0.0});

	m_compound = is_compound_in;
	if (!is_compound_in) { // "Simple" time signature
		m_bpb = num;
		m_beat_unit = denom;
	} else { // Compound time signature
		// The numerator is the "number of into-3 subdivisions of the beat"-per
		// bar.  Thus, dividing by 3 yields the number of beats-per bar.  
		m_bpb = num/3.0;

		// The denominator is the "note-value per (3-part) beat subdivision."
		// That is, 3 notes of value denom == 1 beat.  
		// A group of three identical note-values x is equivalent a single nv
		// having twice the duration of x, 2x, dotted once: (2x).

		auto beat_unit = 2*denom; au_assert(beat_unit.add_dots(1));
		m_beat_unit = beat_unit;
	}
}

ts_t::ts_t(std::string const& str_in) {
	from_string(str_in);
}

// Called by the constructor ts_t(std::string const&)
// Defined as its own function because ... ???
void ts_t::from_string(std::string const& str_in) {  
	auto o_matches = rx_match_captures("(\\d+)/(\\d+)(c)?",str_in);
	if (!o_matches || (*o_matches).size() != 4) {
		au_error("Could not parse ts string literal");
	}
	auto matches = *o_matches;

	double bt_per_bar {std::stod(*(matches[1]))};
	int dv_per_bt = std::stoi(*(matches[2]));
	au_assert((bt_per_bar > 0 && dv_per_bt > 0), "No (-) values in a ts");
	
	bool is_compound {false};
	if (matches[3]) { is_compound = true; }

	m_beat_unit = d_t {d_t::mn{dv_per_bt,0}};
	m_bpb = beat_t {bt_per_bar};
	m_compound = is_compound;
}

ts_t operator""_ts(const char *literal_in, size_t length) {
	return ts_t {std::string {literal_in}};
}

d_t ts_t::beat_unit() const {
	return m_beat_unit;
}

d_t ts_t::bar_unit() const {
	return (m_bpb/beat_t{1})*(m_beat_unit);
}

beat_t ts_t::beats_per_bar() const {
	return m_bpb;
}

std::string ts_t::print() const {
	std::string s = m_bpb.print() + "/" + m_beat_unit.print();
	if (m_compound) { s += "c"; }
	
	return s;
}

bool ts_t::operator==(ts_t const& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}

bool operator!=(ts_t const& lhs, ts_t const& rhs) {
	return !(lhs == rhs);
}

