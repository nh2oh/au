#include "scd_t.h"
#include "..\util\au_error.h"
#include "..\util\au_util.h"
#include <string>
#include <cmath>  // floor()


scd_t::scd_t(int i) {
	m_value=i;
}
scd_t::operator int() const {
	return m_value;
}

scd_t& scd_t::operator++() { // prefix
	++m_value;
	return *this;
}
scd_t scd_t::operator++(int dummy_int) { // postfix
	scd_t copy_preinc = *this;
	++m_value;
	return copy_preinc;
}
scd_t& scd_t::operator--() { // prefix
	--m_value;
	return *this;
}
scd_t scd_t::operator--(int dummy_int) { // postfix
	scd_t copy_preinc = *this;
	--m_value;
	return copy_preinc;
}

/*
//-----------------------------------------------------------------------------
// The scd_t class

scd_t::scd_t(int int_in) {
	m_scd = int_in;
}
std::string scd_t::print() const {
	return dbk::bsprintf("%d",m_scd);
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
	scd_t newscd {m_scd+1};
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

*/


//-----------------------------------------------------------------------------
// Class octn_t

octn_t::octn_t(int octn_in) {
	m_octn = octn_in;
}
octn_t::octn_t(scd_t scd_in, int num_scds) {
	m_octn = static_cast<int>(std::floor(scd_in/scd_t{num_scds}));
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


rscdoctn_t::rscdoctn_t(scd_t scd_in, int n_in) {  // arg2 is num scds in octave
	au_assert(n_in > 0,"rscdoctn_t(scd_t scd_in, int n_in):  n_in <= 0");
	//m_rscd = fold(scd_in.to_int(),n_in);
	m_rscd = fold(scd_in,n_in);
	m_n = n_in;
}
scd_t rscdoctn_t::to_scd(octn_t o) const {
	return scd_t{o.to_int()*m_n + m_rscd};
}
int rscdoctn_t::to_int() const {
	return m_rscd;
}
std::string rscdoctn_t::print() const {
	std::string s {};
	s += m_rscd + " => {" + m_n;
	s += "}";
	return s;
}

int rscdoctn_t::fold(int scd, int pivot) {
	double r = static_cast<double>(scd)/static_cast<double>(pivot);
	return scd - pivot*static_cast<int>(std::floor(r));
}
rscdoctn_t& rscdoctn_t::operator+=(rscdoctn_t const& rhs) {
	au_assert(m_n == rhs.m_n);
	m_rscd = fold(m_rscd+rhs.m_rscd,m_n);
	return *this;
}
rscdoctn_t& rscdoctn_t::operator-=(rscdoctn_t const& rhs) {
	au_assert(m_n == rhs.m_n);
	m_rscd = fold(m_rscd-rhs.m_rscd,m_n);
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
	return ((lhs.m_n == rhs.m_n) && (lhs.m_rscd == rhs.m_rscd));
}
bool operator<(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	au_assert(lhs.m_n == rhs.m_n);
	return(lhs.m_rscd < rhs.m_rscd);
}
bool operator>(rscdoctn_t const& lhs, rscdoctn_t const& rhs) {
	au_assert(lhs.m_n == rhs.m_n);
	return(lhs.m_rscd > rhs.m_rscd);
}

