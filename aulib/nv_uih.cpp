#include "nv_uih.h"
#include "types/nv_t.h"
#include "util/au_util.h"
#include <string>
#include <optional>

/*
bool uih_pred_t::operator()(nv_t const&) {
	std::string msg {""};
};
*/

//-----------------------------------------------------------------------------
// The nv_uih class

nv_uih::nv_uih() {
	//...
}

nv_uih::nv_uih(std::string const& str_in) {
	// Do not set str_last_ before calling update()... update() checkes 
	// str_last_ before doing anything and will not proceed if 
	// str_in == str_last_.  
	update(str_in);
}

void nv_uih::update(std::string const& str_in) {
	if (str_in == m_str_last) {
		return;
	}
	m_str_last = str_in;

	auto str_in_parts = parse_nv_str();
	if (!str_in_parts) {
		m_is_valid = false;
		m_msg += "An nv is of the form n/d[.] where n and d are integers and [.]";
		m_msg += " is 0 or more \".\" characters; Ex: \"1/4\", \"1/4..\".  ";
		return;
	}

	auto nv_parts = *str_in_parts;
	// Addnl tests... weird magnitudes, large number of dots...

	nv_t nv {(nv_parts.num)/(nv_parts.denom), nv_parts.ndots};

	/*if (m_addnl_pred) {
		auto tf_addnl_pred = (*m_addnl_pred)(nv);
		if (!tf_addnl_pred) {
			m_is_valid = false;
			m_msg += (*m_addnl_pred).msg;
			return;
		}
	}*/

	m_nv = nv;
	m_is_valid = true;
	
}

bool nv_uih::is_valid() const {
	return m_is_valid;
}

int nv_uih::flags() const {
	return m_flags;
}

std::string nv_uih::message() const {
	return m_msg;
}

std::optional<nv_t> nv_uih::get() const {
	return m_nv;
}

// Parses m_str_last; does not read any other internal state, does not alter
// any internal state.  
// Called by the constructor.  
std::optional<nv_uih::nv_str_parts> nv_uih::parse_nv_str() const {
	auto o_matches = rx_match_captures("^\\s*([1-9]+)/([1-9]+)(\\.*)\\s*$",m_str_last);
	if (!o_matches || (*o_matches).size() != 4) {
		return {};
	}

	auto matches = *o_matches;

	double n = std::stod(*(matches[1]));
	double d = std::stod(*(matches[2]));
	int ndots = static_cast<int>((*matches[3]).size());
	
	return nv_str_parts {n, d, ndots};
}


bool operator==(nv_uih const& lhs, nv_uih const& rhs) {
	return (lhs.m_str_last == rhs.m_str_last);
}

bool operator!=(nv_uih const& lhs, nv_uih const& rhs) {
	return (lhs.m_str_last != rhs.m_str_last);
}

