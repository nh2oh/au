#include "cent_oct_t.h"
#include "frq_t.h"
#include <string>
#include <cmath>  // std::pow(), std::log2(), std::round()



cent_t::cent_t(const frq_t& frq_from, const frq_t& frq_to) { 
	m_cents = static_cast<int>(std::round(1200*std::log2(frq_to/frq_from)));
}
cent_t::cent_t(int cents_in) { 
	m_cents = cents_in;
}
cent_t::cent_t(oct_t oct_in) { 
	m_cents = static_cast<int>(std::round(1200*oct_in.to_double()));
}
cent_t operator""_cent(unsigned long long literal_in) {
	return cent_t {static_cast<int>(literal_in)};
}

double cent_t::to_int() const {
	return m_cents;
}
std::string cent_t::print() const {
	return std::to_string(m_cents);
}

bool cent_t::operator==(const cent_t& rhs) const {
	return m_cents==rhs.m_cents;
}
bool cent_t::operator!=(const cent_t& rhs) const {
	return m_cents!=rhs.m_cents;
}
bool cent_t::operator<(const cent_t& rhs) const {
	return m_cents<rhs.m_cents;
}
bool cent_t::operator>(const cent_t& rhs) const {
	return m_cents>rhs.m_cents;
}
bool cent_t::operator<=(const cent_t& rhs) const {
	return m_cents<=rhs.m_cents;
}
bool cent_t::operator>=(const cent_t& rhs) const {
	return m_cents>=rhs.m_cents;
}
cent_t cent_t::operator+=(const cent_t& rhs) {
	m_cents += rhs.m_cents;
	return *this;
}
cent_t cent_t::operator-=(const cent_t& rhs) {
	m_cents -= rhs.m_cents;
	return *this;
}
cent_t cent_t::operator*=(double rhs) {
	m_cents *= rhs;
	return *this;
}
cent_t cent_t::operator/=(double denom) {
	m_cents /= denom;
	return *this;
}

cent_t operator/(cent_t num, double denom) {
	return num/=denom;
}
cent_t operator*(cent_t lhs, double rhs) {
	return lhs*=rhs;
}
cent_t operator*(double lhs, cent_t rhs) {
	return rhs*=lhs;
}
cent_t operator+(cent_t lhs, const cent_t& rhs) {
	return lhs+=rhs;
}
cent_t operator-(cent_t lhs, const cent_t& rhs) {
	return lhs-=rhs;
}







//-----------------------------------------------------------------------------
// The oct_t class

oct_t::oct_t(double num_octs) {
	m_oct = num_octs;
}

oct_t::oct_t(cent_t cents_in) {
	m_oct = (cents_in.to_int())/1200;
}
oct_t::oct_t(frq_t frq_num, frq_t frq_denom) {
	auto num_cents = cent_t {frq_num,frq_denom};
	m_oct = (num_cents.to_int())/1200;
}

double oct_t::to_double() const {
	return m_oct;
}

