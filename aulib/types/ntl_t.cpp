#include "ntl_t.h"
#include "scd_t.h"  // Defines octn_t
#include "..\util\au_error.h"  // TODO:  Drop
#include <string>
#include <regex>



//-----------------------------------------------------------------------------
// The ntl_t class

const char *ntl_t::m_allowed {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567989_-[]#&"};

ntl_t::ntl_t(const std::string& str_in) {
	set_ntl(str_in);
}

ntl_t::ntl_t(const char* char_in) {
	set_ntl(std::string(char_in));
}

bool ntl_t::valid_string(const std::string& str_in) {
	if (str_in.size() == 0) { return false; }
	auto pos_illegal = str_in.find_first_not_of(m_allowed,0);
	return pos_illegal == std::string::npos;
}

void ntl_t::set_ntl(const std::string& str_in) {
	if (!valid_string(str_in)) { std::abort(); }
	m_ntl = str_in;
}

std::string ntl_t::print() const {
	return m_ntl;
}

bool ntl_t::operator==(const ntl_t& rhs) const {
	return (m_ntl == rhs.m_ntl);
}
bool operator!=(const ntl_t& lhs, const ntl_t& rhs) {
	return !(lhs == rhs);
}

ntl_t operator""_ntl(const char *literal_in, size_t length) {
	return ntl_t {literal_in};
}




note_t::note_t(ntl_t n, octn_t o, frq_t f) {
	ntl = n;
	oct = o;
	frq = f;
}

std::string note_t::print(note_t::fmt f) const {
	std::string s {};
	switch(f) {
		case fmt::ntl: {
			s += ntl.print();
			break;
		}
		case fmt::ntlo: {
			s += ntl.print() + "(" + std::to_string(oct.to_int()) + ")";
			break;
		}
	}

	return s;
}

bool note_t::operator==(const note_t& rhs) const {
	return (ntl==rhs.ntl && oct==rhs.oct && frq==rhs.frq);
}
bool operator!=(const note_t& lhs, const note_t& rhs) {
	return !(lhs==rhs);
}

ntstr_parsed parse_ntstr(const std::string& s) {
	ntstr_parsed result {false, false, ntl_t{"C"}, octn_t{0}};

	std::regex rx("([a-zA-Z0-9#&\\[\\]_\\-]+)(?:\\((-?\\d+)\\))?");
	std::smatch rx_matches {};
	if (!std::regex_match(s, rx_matches, rx)) {
		return result;
	}

	if (rx_matches[1].matched && !rx_matches[2].matched) {  // octave not set
		if (ntl_t::valid_string(rx_matches[1].str())) {
			result.ntl = ntl_t {rx_matches[1].str()};
		}
		result.is_valid = true;
		result.is_oct_set = false;
	} else if (rx_matches[1].matched && rx_matches[2].matched) {  // octave set
		if (ntl_t::valid_string(rx_matches[1].str())) {
			result.ntl = ntl_t {rx_matches[1].str()};
		}
		result.oct = octn_t {std::stoi(rx_matches[2].str())};
		result.is_valid = true;
		result.is_oct_set = true;
	}

	return result;
}

