#include "cent_oct_t.h"
#include "frq_t.h"  // NB: Not #included in cent_oct_t.h; frq_t fwd-declared
#include <string>
#include <cmath>  // std::pow(), std::log2(), std::round()



cent_t::cent_t(const frq_t& from, const frq_t& to) { 
	m_cents = static_cast<int>(std::round(1200*std::log2(to/from)));
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

int cent_t::to_int() const {
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
cent_t cent_t::operator*=(int rhs) {
	m_cents *= rhs;
	return *this;
}
cent_t cent_t::operator/=(int denom) {
	m_cents /= denom;
	return *this;
}

cent_t operator/(cent_t num, int denom) {
	return num/=denom;
}
cent_t operator*(cent_t lhs, int rhs) {
	return lhs*=rhs;
}
cent_t operator*(int lhs, cent_t rhs) {
	return rhs*=lhs;
}
cent_t operator+(cent_t lhs, const cent_t& rhs) {
	return lhs+=rhs;
}
cent_t operator-(cent_t lhs, const cent_t& rhs) {
	return lhs-=rhs;
}

frq_t operator+(frq_t lhs, const cent_t& rhs) {
	double exp = std::round(static_cast<double>(rhs.to_int())/1200.0);
	return lhs *= std::pow(2,exp);
}
frq_t operator-(frq_t lhs, const cent_t& rhs) {
	double exp = -1*std::round(static_cast<double>(rhs.to_int())/1200.0);
	return lhs *= std::pow(2,exp);
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
double oct_t::to_double() const {  // FP divide - round - truncate - return
	return static_cast<double>(m_num_cents/1200.0);
}
int oct_t::to_int() const {  // FP divide - round - truncate - return
	return static_cast<int>(std::round(m_num_cents/1200.0));
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
oct_t oct_t::operator*=(double rhs) {  // FP mult - round - truncate - return
	//m_num_cents *= rhs;
	m_num_cents = std::round(static_cast<double>(m_num_cents) * rhs);
	return *this;
}
oct_t oct_t::operator/=(double denom) {  // FP divide - round - truncate - return
	m_num_cents = std::round(static_cast<double>(m_num_cents)/denom);
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

frq_t operator+(frq_t lhs, const oct_t& rhs) {
	double exp = std::round(rhs.to_double()/1200.0);
	return lhs *= std::pow(2,exp);
}
frq_t operator-(frq_t lhs, const oct_t& rhs) {
	double exp = -1*std::round(rhs.to_double()/1200.0);
	return lhs *= std::pow(2,exp);
}

