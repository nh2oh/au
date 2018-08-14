#include "ntl_t.h"
#include "scd_t.h"
#include "..\util\au_error.h"
#include <string>
#include <regex>


//-----------------------------------------------------------------------------
// The ntl_t class

const std::string ntl_t::m_illegal = "()[] ;,";

ntl_t::ntl_t() { }

ntl_t::ntl_t(std::string const& str_in) {
	set_ntl(str_in);
}

ntl_t::ntl_t(const char* char_in) {
	set_ntl(std::string(char_in));
}

void ntl_t::set_ntl(std::string const& str_in) {
	au_assert(str_in.size() > 0, "ntl_t::set_ntl(std::string str_in): str_in.size() > 0");
	
	for (auto const& ci : m_illegal) { // ci ~ "current illegal"
		for (auto const& csin : str_in) { // csin ~ "current str_in"
			if (ci == csin) {
				au_error("ntl_t::set_ntl(std::string str_in):  ci == csin");
			}
		}
	}

	m_ntl = str_in;
}

std::string ntl_t::print() const {
	return m_ntl;
}

bool operator==(ntl_t const& lhs, ntl_t const& rhs) {
	return (lhs.m_ntl == rhs.m_ntl);
}
bool operator!=(ntl_t const& lhs, ntl_t const& rhs) {
	return (lhs.m_ntl != rhs.m_ntl);
}

ntl_t operator""_ntl(const char *literal_in, size_t length) {
	return ntl_t {literal_in};
}

//-----------------------------------------------------------------------------
// The ntstr_t class

ntstr_t::ntstr_t(std::string str_in) {
	//std::cout << "running constructor ntstr_t(std::string)..." << std::endl;
	std::regex rx("([a-zA-Z0-9#&]+)(?:\\((-?\\d+)\\))?");
	std::smatch rx_matches;
	if (!std::regex_match(str_in, rx_matches, rx)) {
		au_error("!std::regex_match(str_in, rx_matches, rx)");
	}

	if (rx_matches.size() == 2) { // ntl, not ntlo
		m_ntl = ntl_t(rx_matches[1].str());
		m_octn = octn_t{0};
	}
	else if (rx_matches.size() == 3) { // ntlo, not ntl
		m_ntl = ntl_t(rx_matches[1].str());
		m_octn = octn_t{std::stoi(rx_matches[2].str())};
	}
	else {
		au_error("rx_matches.size() ???");
	}
}

ntstr_t::ntstr_t(ntl_t ntl_in, int octn_in) {
	m_ntl = ntl_in;
	m_octn = octn_t{octn_in};
}

ntstr_t::ntstr_t(ntl_t ntl_in, octn_t octn_in) {
	m_ntl = ntl_in;
	m_octn = octn_in;
}

std::string ntstr_t::print() const {
	return std::string(m_ntl.print() + "(" + m_octn.print() + ")");
}

ntstr_t::operator ntl_t() const {
	return m_ntl;
}

ntstr_t::operator octn_t() const {
	return m_octn;
}

bool operator==(ntstr_t const& lhs, ntstr_t const& rhs) {
	return ((lhs.m_ntl == rhs.m_ntl) &&	(lhs.m_octn == rhs.m_octn));
}

