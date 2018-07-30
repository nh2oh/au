#include <string>
#include <cmath>
#include "scd_t.h"
#include "..\util\au_util_all.h"

//-----------------------------------------------------------------------------
// The scd_t class

scd_t::scd_t(int int_in) {
	m_scd = int_in;
}
scd_t::scd_t(rscdoctn_t rscd_in) {
	m_scd = rscd_in.to_scd().to_int();
}

int scd_t::to_int() const {
	return m_scd;
}
double scd_t::to_double() const {
	return static_cast<double>(m_scd);
}

scd_t& scd_t::operator+=(scd_t const& rhs) {
	m_scd += rhs.m_scd;
	return *this;
}
scd_t& scd_t::operator-=(scd_t const& rhs) {
	m_scd -= rhs.m_scd;
	return *this;
}

scd_t operator+(scd_t const& lhs, scd_t const& rhs) {
	scd_t result {lhs};
	return result += rhs;
}
scd_t operator-(scd_t const& lhs, scd_t const& rhs) {
	scd_t result {lhs};
	return result -= rhs;
}
scd_t& scd_t::operator++() { // prefix
	++m_scd;
	return *this;
}
scd_t scd_t::operator++ (int dummy_int) { // postfix
	scd_t newscd = m_scd+1;
	return newscd;
}
scd_t operator*(int const& lhs, scd_t const& rhs) {
	return scd_t{lhs*(rhs.to_int())};
}
double operator/(scd_t const& lhs, scd_t const& rhs) {
	return lhs.to_double()/rhs.to_double();
}
double operator/(scd_t const& lhs, double const& rhs) {
	return lhs.to_double()/rhs;
}
double operator/(double const& lhs, scd_t const& rhs) {
	return lhs/rhs.to_double();
}

bool operator==(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.to_int() == rhs.to_int());
}
bool operator<(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.to_int() < rhs.to_int());
}
bool operator>(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.to_int() > rhs.to_int());
}
bool operator>=(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.to_int() >= rhs.to_int());
}
bool operator<=(scd_t const& lhs, scd_t const& rhs) {
	return (lhs.to_int() <= rhs.to_int());
}




//-----------------------------------------------------------------------------
// Class octn_t

octn_t::octn_t(int octn_in) {
	m_octn = octn_in;
}
int octn_t::to_int() const {
	return m_octn;
}
std::string octn_t::print() const {
	return std::to_string(m_octn);
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

rscdoctn_t::rscdoctn_t(scd_t rscd_in, octn_t octn_in, int n_in) {
	au_assert(n_in > 0 && rscd_in.to_int() < n_in);
	m_scd = n_in*octn_in.to_int() + rscd_in;
	m_n = n_in;
}
rscdoctn_t::rscdoctn_t(scd_t scd_in, int n_in) {  // arg2 is num scds in octave
	au_assert(n_in > 0,"rscdoctn_t(scd_t scd_in, int n_in):  n_in <= 0");
	m_scd = scd_in;
	m_n = n_in;
}

scd_t rscdoctn_t::to_rscd() const {
	// scd_t division operator upcasts both args to double and returns a double
	return scd_t{m_scd - m_n*static_cast<int>(std::floor(m_scd/m_n))};
}
scd_t rscdoctn_t::to_scd() const {
	return m_scd;
}
octn_t rscdoctn_t::to_octn() const {
	return octn_t {static_cast<int>(std::floor(m_scd/m_n))};   // operator scd_t/double
}

std::string rscdoctn_t::print() const {
	std::string s {};
	s += m_scd.to_int() + " => {" + this->to_rscd().to_int();
	s += ", " + this->to_octn().to_int();
	s += "}";
	return s;
}

rscdoctn_t& rscdoctn_t::operator+=(rscdoctn_t const& rhs) {
	au_assert(m_n == rhs.m_n);
	m_scd += rhs.m_scd;
	return *this;
}

rscdoctn_t& rscdoctn_t::operator-=(rscdoctn_t const& rhs) {
	au_assert(m_n == rhs.m_n);
	m_scd -= rhs.m_scd;
	return *this;
}

rscdoctn_t operator+(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	rscdoctn_t result {lhs};
	return result += rhs;  // Operator += checks for same m_n
}
rscdoctn_t operator-(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	rscdoctn_t result {lhs};
	return result -= rhs;  // Operator -= checks for same m_n
}

bool operator==(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	return ((lhs.m_n == rhs.m_n) && (lhs.m_scd == rhs.m_scd));
}
bool operator<(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	au_assert(lhs.m_n == rhs.m_n);
	return(lhs.m_scd < rhs.m_scd);
}
bool operator>(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	au_assert(lhs.m_n == rhs.m_n);
	return(lhs.m_scd > rhs.m_scd);
}

