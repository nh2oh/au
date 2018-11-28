#include "cent_oct_t.h"
#include "frq_t.h"  // NB: Not #included in cent_oct_t.h; frq_t fwd-declared
#include <string>
#include <cmath>  // std::pow(), std::log2(), std::round()



cent_t::cent_t(const frq_t& frq_from, const frq_t& frq_to) { 
	m_cents = static_cast<int>(std::round(1200*std::log2(frq_to/frq_from)));
}
cent_t::cent_t(int cents_in) { 
	m_cents = cents_in;
}
cent_t::cent_t(oct_t oct_in) { 
	//m_cents = static_cast<int>(std::round(1200*oct_in.to_double()));
	m_cents = (cent_t {oct_in}).to_int();
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




oct_t::oct_t(double num_octs) {
	m_num_cents = static_cast<int>(std::round(num_octs*1200.0));
}
oct_t::oct_t(int num_octs) {
	m_num_cents = 1200*num_octs;
}
oct_t::oct_t(const cent_t& cents_in) {
	m_num_cents = cents_in.to_int();
}
oct_t::oct_t(frq_t frq_num, frq_t frq_denom) {
	cent_t cents {frq_num,frq_denom};
	m_num_cents = cents.to_int();
}
oct_t operator""_octs(unsigned long long literal_in) {
	return oct_t {static_cast<int>(literal_in)};
}

std::string oct_t::print() const {
	return std::to_string(m_num_cents/1200.0);
}
double oct_t::to_double() const {
	return static_cast<double>(m_num_cents/1200.0);
}
double oct_t::to_int() const {  // FP divide
	return static_cast<int>(std::round(m_num_cents/1200.0));
}

oct_t::operator cent_t() {
	return cent_t {m_num_cents};
}


bool oct_t::operator==(const oct_t& rhs) const {
	return m_num_cents==rhs.m_num_cents;
}
bool oct_t::operator!=(const oct_t& rhs) const {
	return m_num_cents!=rhs.m_num_cents;
}
bool oct_t::operator<(const oct_t& rhs) const {
	return m_num_cents<rhs.m_num_cents;
}
bool oct_t::operator>(const oct_t& rhs) const {
	return m_num_cents>rhs.m_num_cents;
}
bool oct_t::operator<=(const oct_t& rhs) const {
	return m_num_cents<=rhs.m_num_cents;
}
bool oct_t::operator>=(const oct_t& rhs) const {
	return m_num_cents>=rhs.m_num_cents;
}
oct_t oct_t::operator+=(const oct_t& rhs) {
	m_num_cents += rhs.m_num_cents;
	return *this;
}
oct_t oct_t::operator-=(const oct_t& rhs) {
	m_num_cents -= rhs.m_num_cents;
	return *this;
}
oct_t oct_t::operator*=(double rhs) {
	m_num_cents *= rhs;
	return *this;
}
oct_t oct_t::operator/=(double denom) {
	m_num_cents /= denom;
	return *this;
}

oct_t operator/(oct_t num, double denom) {
	return num/=denom;
}
oct_t operator*(oct_t lhs, double rhs) {
	return lhs*=rhs;
}
oct_t operator*(double lhs, oct_t rhs) {
	return rhs*=lhs;
}
oct_t operator+(oct_t lhs, const oct_t& rhs) {
	return lhs+=rhs;
}
oct_t operator-(oct_t lhs, const oct_t& rhs) {
	return lhs-=rhs;
}



