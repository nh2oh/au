#include "ntl_t.h"
#include "scd_t.h"
#include "..\util\au_error.h"
#include <string>
#include <regex>
#include <algorithm>


//-----------------------------------------------------------------------------
// The ntl_t class

ntl_t::ntl_t(const std::string& str_in) {
	set_ntl(str_in);
}

ntl_t::ntl_t(const char* char_in) {
	set_ntl(std::string(char_in));
}

void ntl_t::set_ntl(const std::string& str_in) {
	const std::string s_illegal {"()[] ;,"};  // NB: contains the space char: ' '
	auto i = std::find_first_of(str_in.begin(),str_in.end(),
		s_illegal.begin(),s_illegal.end());
	au_assert(str_in.size()>=1 && i==str_in.end(),
		"Empty string or string contains illegal chars");

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

//-----------------------------------------------------------------------------
// The ntstr_t class

ntstr_t::ntstr_t(std::string str_in) {
	from_string(str_in);
}
ntstr_t::ntstr_t(std::string str_in, int octn_in) {
	str_in += "(" + std::to_string(octn_in) + ")";
	from_string(str_in);
}
ntstr_t::ntstr_t(ntl_t ntl_in, int octn_in) {
	m_ntl = ntl_in;
	m_octn = octn_t {octn_in};
}
ntstr_t::ntstr_t(ntl_t ntl_in, octn_t octn_in) {
	m_ntl = ntl_in;
	m_octn = octn_in;
}

void ntstr_t::from_string(const std::string& str_in) {
	std::regex rx("([a-zA-Z0-9#&]+)(?:\\((-?\\d+)\\))?");
	std::smatch rx_matches;
	if (!std::regex_match(str_in, rx_matches, rx)) {
		au_error("!std::regex_match(str_in, rx_matches, rx)");
	}
	au_assert(rx_matches.size()==3 &&
		rx_matches[0].matched &&
		(rx_matches[1].matched || rx_matches[2].matched),
		"Unexpected number of captures in rx_match and/or pattern of matches captures.");

	if (rx_matches[1].matched && !rx_matches[2].matched) {
		m_ntl = ntl_t {rx_matches[1].str()};
		m_octn = octn_t {0};
	} else if (rx_matches[1].matched && rx_matches[2].matched) {
		m_ntl = ntl_t(rx_matches[1].str());
		m_octn = octn_t{std::stoi(rx_matches[2].str())};
	}
}

std::string ntstr_t::print() const {
	return std::string(m_ntl.print() + "(" + m_octn.print() + ")");
}
const ntl_t& ntstr_t::ntl() const {
	return m_ntl;
}
const octn_t& ntstr_t::oct() const {
	return m_octn;
}
ntstr_t::operator ntl_t() const {
	return m_ntl;
}

bool ntstr_t::operator==(const ntstr_t& rhs) const {
	return (m_ntl==rhs.m_ntl &&	m_octn==rhs.m_octn);
}

bool operator!=(const ntstr_t& lhs, const ntstr_t& rhs) {
	return !(lhs==rhs);
}

