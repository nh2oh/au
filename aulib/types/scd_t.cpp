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

scd_t& scd_t::operator+=(const scd_t& rhs) {
	m_value += rhs.m_value;
	return *this;
}
scd_t& scd_t::operator-=(const scd_t& rhs) {
	m_value -= rhs.m_value;
	return *this;
}
scd_t& scd_t::operator+=(const int& rhs) {
	m_value += rhs;
	return *this;
}
scd_t& scd_t::operator-=(const int& rhs) {
	m_value -= rhs;
	return *this;
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


