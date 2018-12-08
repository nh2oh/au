#include "ntl_t.h"
#include <string>
#include <regex>
#include <ctype.h>  // std::tolower()
#include <cmath>  // std::abs()
#include <algorithm>

// The set of chars allowed in an ntl_t
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
	if (!valid_string(str_in)) {
		std::abort();
	}
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

//
// Determines if a string represents a valid SPN note (A,B,...G), whether or 
// not it contains an octave specifier, and whether or not it contains one or 
// more sharp,flat modifiers ('#' or '&' immediately following the note-letter but
// preceeding the octave-specifier, if present).  Parses the octave specifier if and
// sharp,flat modifiers, if present.  Note that '#' or '&' chars in the string are
// only considered to be sharp,flat modifiers if the string qualifies as a valid SPN
// note letter, since '#' and '&' are valid chars for general (non-SPN) note-letters.  
//
// Note also that in principle the &,# modifiers could change the octave specified in
// the ntstr, for example C&(5) is the same as B(4).  The octave returned is always
// that specified in the input string (the presence of net & or # chars also change the
// ntl, but this is much too complex an issue to deal with in a simple string parsing
// function; the caller needs to deal with it).  
//
spn_ntstr_parsed parse_spn_ntstr(const std::string& s) {
	spn_ntstr_parsed res {};

	ntstr_parsed ntstr_parse_res = parse_ntstr(s);
	res.is_valid = ntstr_parse_res.is_valid;
	res.is_oct_set = ntstr_parse_res.is_oct_set;
	res.ntl = ntstr_parse_res.ntl;
	res.oct = ntstr_parse_res.oct;
	res.ntl_base = ntstr_parse_res.ntl;
	if (!res.is_valid) {
		return res;
	}

	std::string ntstr_nooct = ntstr_parse_res.ntl.print();
	// ntstr_nooct so as not to pass a temporary to regex_match()
	
	std::regex rx("([ABCDEFG]{1,1})([&#]*)?");
	std::smatch rx_matches {};
	if (!std::regex_match(ntstr_nooct, rx_matches, rx)) {
		res.nflat = 0;
		res.nsharp = 0;
		res.ntl_base = res.ntl;  // No parsable &,# modifiers
		res.is_valid_spn = false;
		res.ntl = ntstr_parse_res.ntl;
	} else {
		res.is_valid_spn = true;
		res.ntl_base = ntl_t {rx_matches[1].str()};
	}

	if (rx_matches[2].matched) {
		for (int i=0; i<rx_matches[2].str().size(); ++i) {
			if (rx_matches[2].str()[i] == '#') { ++res.nsharp; }
			if (rx_matches[2].str()[i] == '&') { ++res.nflat; }
		}
	}

	return res;
}




octn_t::octn_t(int octn_in) {
	m_octn = octn_in;
}
int octn_t::to_int() const {
	return m_octn;
}
std::string octn_t::print() const {
	return std::to_string(m_octn);
}

bool octn_t::operator==(const octn_t& rhs) const {
	return m_octn==rhs.m_octn;
}
bool octn_t::operator!=(const octn_t& rhs) const {
	return !(m_octn==rhs.m_octn);
}
bool octn_t::operator<(const octn_t& rhs) const {
	return  m_octn<rhs.m_octn;
}
bool octn_t::operator<=(const octn_t& rhs) const {
	return  m_octn<=rhs.m_octn;
}
bool octn_t::operator>(const octn_t& rhs) const {
	return  m_octn>rhs.m_octn;
}
bool octn_t::operator>=(const octn_t& rhs) const {
	return  m_octn>=rhs.m_octn;
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
			s += ntl.print() + "(" + oct.print() + ")";
			break;
		}
		case fmt::lp: {  // TODO:  Gross
			// in "\absolute" mode, c' (middle C) => C(4) in SPN
			int n_mod = oct.to_int()-3;
			std::string mods {};
			if (n_mod > 0) {
				mods = std::string(n_mod,'\'');
			} else if (n_mod < 0) {
				mods = std::string(std::abs(n_mod),',');
			}
			s += ntl.print();
			std::transform(s.begin(),s.end(),s.begin(), ::tolower);
			s += mods;
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

//
// Determines if a string represents a valid note and whether or not it contains
// an octave specifier (a number surrounded by '(' and ')' at the end of the 
// string); parses the octave specifier if present.  
//
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


