#include<string>
#include<cmath>
#include "scd_t.h"
#include "..\util\au_util_all.h"

//-----------------------------------------------------------------------------
// The scd_t class

scd_t::scd_t(int int_in) {
	m_scd = int_in;
}

//int scd_t::to_int() const {
//	return m_scd;
//}

scd_t::operator double() const {
	return static_cast<double>(m_scd);
}
scd_t::operator int() const {
	return static_cast<int>(m_scd);
}

bool operator==(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.m_scd == rhs.m_scd);
}
bool operator<(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.m_scd < rhs.m_scd);
}
bool operator>(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.m_scd > rhs.m_scd);
}
bool operator>=(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.m_scd >= rhs.m_scd);
}
bool operator<=(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.m_scd <= rhs.m_scd);
}
scd_t& scd_t::operator++() { // prefix
	++m_scd;
	return *this;
}
scd_t scd_t::operator++ (int dummy_int) { // postfix
	scd_t newscd = m_scd+1;
	return newscd;
}
scd_t operator+(scd_t const& lhs, scd_t const& rhs) {
	return scd_t {lhs.m_scd + rhs.m_scd};
}
scd_t operator-(scd_t const& lhs, scd_t const& rhs) {
	return scd_t {lhs.m_scd - rhs.m_scd};
}


//-----------------------------------------------------------------------------
// Class octn_t

octn_t::octn_t(int octn_in) {
	m_octn = octn_in;
}

std::string octn_t::print() const {
	return std::to_string(m_octn);
}

octn_t::operator double() const {
	return static_cast<double>(m_octn);
}
octn_t::operator int() const {
	return static_cast<int>(m_octn);
}

bool operator==(octn_t const& lhs, octn_t const& rhs) {
	return (lhs.m_octn==rhs.m_octn);
}
bool operator<(octn_t const& lhs, octn_t const& rhs) {
	return (lhs.m_octn<rhs.m_octn);
}
bool operator>(octn_t const& lhs, octn_t const& rhs) {
	return (lhs.m_octn>rhs.m_octn);
}
bool operator<=(octn_t const& lhs, octn_t const& rhs) {
	return (lhs.m_octn<=rhs.m_octn);
}
bool operator>=(octn_t const& lhs, octn_t const& rhs) {
	return (lhs.m_octn>=rhs.m_octn);
}


//-----------------------------------------------------------------------------
// The rscdoctn_t class


rscdoctn_t::rscdoctn_t(int rscd_in, octn_t octn_in, int n_in) {
	if (n_in <= 0 || std::abs(rscd_in) >= n_in) {
		au_error("rscdoctn_t::rscdoctn_t(...):  n_in <= 0 || std::abs(rscd_in) >= n_in");
	}
	m_rscd = rscd_in;
	m_octn = octn_in;
	m_n = n_in;
}

rscdoctn_t::rscdoctn_t(scd_t rscd_in, octn_t octn_in, int n_in) {
	if (n_in <= 0 || std::abs(int{rscd_in}) >= n_in) {
		au_error("rscdoctn_t::rscdoctn_t(...):  n_in <= 0 || std::abs(int{rscd_in}) >= n_in");
	}
	m_rscd = rscd_in;
	m_octn = octn_in;
	m_n = n_in;
}

rscdoctn_t::rscdoctn_t(scd_t scd_in, int n_in) {
	if (n_in <= 0) {
		au_error("rscdoctn_t::rscdoctn_t(scd_t scd_in, int n_in):  n_in <= 0");
	}
	double ratio = static_cast<double>(scd_in)/static_cast<double>(n_in);
	int oct = static_cast<int>(std::floor(ratio));
	int rscd = static_cast<int>(scd_in) - oct*n_in;

	m_rscd = scd_t {rscd};
	m_octn = octn_t {oct};
	m_n = n_in;
}

int rscdoctn_t::rscd_to_int() const {
	return static_cast<int>(m_rscd);
}

// Conversion to scd_t
rscdoctn_t::operator scd_t() const {
	return scd_t {static_cast<int>(m_rscd) + m_n*(static_cast<int>(m_octn))};
}

// Conversion to octn_t
rscdoctn_t::operator octn_t() const {
	return m_octn;
}

rscdoctn_t operator+(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	if (lhs.m_n != rhs.m_n) {
		au_error("operator+(rscdoctn_t const& lhs, rscdoctn_t const& rhs):  lhs.m_n != rhs.m_n");
	}

	auto scd_sum = static_cast<double>(static_cast<scd_t>(lhs)+static_cast<scd_t>(rhs));

	auto ratio = scd_sum/static_cast<double>(lhs.m_n);
	int oct = static_cast<int>(std::floor(ratio));
	int rscd = static_cast<int>(scd_sum - oct*(lhs.m_n));

	return rscdoctn_t{rscd,octn_t{oct},lhs.m_n};
}

rscdoctn_t operator-(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	if (lhs.m_n != rhs.m_n) {
		au_error("operator-(rscdoctn_t const& lhs, rscdoctn_t const& rhs):  lhs.m_n != rhs.m_n");
	}

	auto scd_diff = static_cast<double>(static_cast<scd_t>(lhs)-static_cast<scd_t>(rhs));

	auto ratio = scd_diff/static_cast<double>(lhs.m_n);
	int oct = static_cast<int>(std::floor(ratio));
	int rscd = static_cast<int>(scd_diff - oct*(lhs.m_n));

	return rscdoctn_t{rscd,octn_t{oct},lhs.m_n};
}

bool operator==(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	if (lhs.m_n != rhs.m_n) {
		au_error("operator==(rscdoctn_t const& lhs, rscdoctn_t const& rhs):  lhs.m_n != rhs.m_n");
	}
	return (lhs.m_rscd == rhs.m_rscd && lhs.m_octn == rhs.m_octn);
}

bool operator<(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	if (lhs.m_n != rhs.m_n) {
		au_error("operator<(rscdoctn_t const& lhs, rscdoctn_t const& rhs):  lhs.m_n != rhs.m_n");
	}
	return(static_cast<scd_t>(lhs) < static_cast<scd_t>(rhs));
}
bool operator>(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	if (lhs.m_n != rhs.m_n) {
		au_error("operator>(rscdoctn_t const& lhs, rscdoctn_t const& rhs):  lhs.m_n != rhs.m_n");
	}
	return(static_cast<scd_t>(lhs) < static_cast<scd_t>(rhs));
}

