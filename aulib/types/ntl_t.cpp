#include "ntl_t.h"
#include <string>
#include <regex>
#include <ctype.h>  // std::tolower()
#include <cmath>  // std::abs()
#include <algorithm>

//
// Determines if a string represents a valid note letter.  '(' and ')' are not included;
// an oct specifier is not considered to be part of an ntl.  
//
bool is_valid_ntl(const std::string& s) {
	if (s.size() == 0) { return false; }
	const std::string allowed {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz01234567989_-[]#&"};
	auto pos_illegal = s.find_first_not_of(allowed,0);
	return pos_illegal == std::string::npos;
}

//
// Determines if a string represents a valid note and whether or not it contains
// an octave specifier (a number surrounded by '(' and ')' at the end of the 
// string); parses the octave specifier if present.  The string returned in 
// ntlostr_parsed.ntl_str does _not_ include the octave specifier.  
// 
ntlostr_parsed parse_ntlostr(const std::string& s) {
	ntlostr_parsed result {};  result.is_valid = false;

	std::regex rx {"^([a-zA-Z0-9#&\\[\\]_\\-]+)(?:\\((-?\\d+)\\))?"};
	std::smatch rx_matches {};
	if (!std::regex_match(s, rx_matches, rx)) {
		return result;
	}
	result.is_valid = rx_matches[1].matched;
	result.ntl_str = rx_matches[1].str();
	result.is_oct_set = rx_matches[2].matched;
	if (rx_matches[2].matched) {
		result.oct = std::stoi(rx_matches[2].str());
	}

	return result;
}

//
// Determines if a string represents a valid SPN note (A,B,...G), whether or 
// not it contains an octave specifier, and whether or not it contains one or 
// more sharp,flat modifiers ('#' or '&' immediately following the note-letter but
// preceeding the octave-specifier).  Parses the octave specifier  and sharp,flat modifiers, 
// if present.  Note that '#' or '&' chars in a "generic" note-string are only considered 
// to be sharp,flat modifiers if the string qualifies as a valid SPN note letter, since '#'
// and '&' are valid chars for general (non-SPN) note-letters.  
//
// Note also that in principle the &,# modifiers could change the octave of the corresponding
// note.  For example C&(5) is the same as B(4).  The octave returned is always
// that specified in the input string (the presence of net & or # chars also change the
// ntl, but this is much too complex an issue to deal with in a simple string parsing
// function; the caller needs to deal with it).  
//
spn_ntstr_parsed parse_spn_ntstr(const std::string& s) {
	spn_ntstr_parsed result {};  result.is_valid = false;

	std::regex rx {"^([ABCDEFG]{1,1})([&#]+)?(?:\\((-?\\d+)\\))?"};
	std::smatch rx_matches {};
	if (!std::regex_match(s, rx_matches, rx)) {
		return result;
	}
	result.is_valid = true;
	result.ntl_base_str = rx_matches[1].str();

	if (rx_matches[2].matched) {
		result.nsharp = std::count(rx_matches[2].first,rx_matches[2].second,'#');
		result.nflat = rx_matches[2].length() - result.nsharp;
	} else {
		result.nsharp = 0;
		result.nflat = 0;
	}

	result.is_oct_set = rx_matches[3].matched;
	if (rx_matches[3].matched) {
		result.oct = std::stoi(rx_matches[3].str());
	}

	return result;
}



ntl_t::ntl_t(const std::string& str_in) {
	set_ntl(str_in);
}
ntl_t::ntl_t(const char* char_in) {
	set_ntl(std::string(char_in));
}
void ntl_t::set_ntl(const std::string& str_in) {
	if (!is_valid_ntl(str_in) || str_in.size() > m_ntl.size()) {
		std::abort();
	}
	//m_ntl = str_in;
	std::copy(str_in.begin(),str_in.end(),m_ntl.begin());
}
std::string ntl_t::print() const {
	//std::find(m_ntl.begin(),m_ntl.end(),'\0');
	return std::string {m_ntl.begin(),std::find(m_ntl.begin(),m_ntl.end(),'\0')};
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



