#include "nv_t.h"
#include "../util/au_error.h"
#include <cmath>


// statics
double nv_t::min_bv = std::pow(2,-10);  // == 1/1024
double nv_t::max_bv = std::pow(2,3);  // 8 => 4 d-w
int nv_t::max_n = 5;

nv_t::nv_t() {  // creates the "unit nv"
	m_m = 0;
	m_n = 0;
}
nv_t::nv_t(double const& bv, int const& ndot) {
	au_assert((ndot >= 0 && ndot < max_n && bv>= min_bv && bv <= max_bv), __FUNCTION__);

	double epsilon = min_bv;
	int m = static_cast<int>(std::round(std::log2(1.0/bv)));
	au_assert((std::abs(std::pow(2,-m)-bv) < epsilon),
		"nv_t::nv_t(double const& bv, int const& ndot):  (std::abs(std::pow(2,-m)-bv) < epsilon)");
	
	m_m = m;
	m_n = ndot;
}

// Getters
std::string nv_t::print() const {
	std::string s {};

	return s;
}
int nv_t::ndot() const {
	return m_n;
}
nv_t nv_t::base() const {
	return nv_t {bv(),0};
}

// Setters
nv_t& nv_t::set_dots(int const& ndots) {
	au_assert(ndots < nv_t::max_n,
		"nv_t::set_dots(int const& ndots):  ndots >= nv_t::max_n");
	return *this;
}
nv_t& nv_t::add_dot(int const& ndots) {  // default value ndots == 1
	int new_n = m_n + ndots;
	au_assert(new_n < nv_t::max_n,
		"nv_t::add_dot(int const& ndots):  new_n >= nv_t::max_n");
	m_n = new_n;
	return *this;
}
nv_t& nv_t::rm_dots(int const& ndots) {
	int new_n = m_n - ndots;
	au_assert(new_n >= 0,
		"nv_t::rm_dot(int const& ndots):  new_n < 0");
	m_n = new_n;
	return *this;
}
nv_t& nv_t::rm_dots() {
	m_n = 0;
	return *this;
}

// Private methods
double nv_t::nv() const {
	return std::pow(2,-m_m)*(2.0 - std::pow(2,m_n));
}
double nv_t::bv() const {
	return std::pow(2,-m_m);
}


// Operators
double operator/(nv_t const& num, nv_t const& denom) {
	return num.nv()/denom.nv();
}
bool operator==(nv_t const& lhs, nv_t const& rhs) {
	return(lhs.m_m == rhs.m_m && lhs.m_n == rhs.m_n);
}
bool operator!=(nv_t const& lhs, nv_t const& rhs) {
	return(lhs.m_m != rhs.m_m || lhs.m_n != rhs.m_n);
}
bool operator<(nv_t const& lhs, nv_t const& rhs) {
	if (lhs.m_m != rhs.m_m) {
		return (lhs.m_m < rhs.m_m);
	} else {
		return (lhs.m_n < rhs.m_n);
	}
}
bool operator>(nv_t const& lhs, nv_t const& rhs) {
	if (lhs.m_m != rhs.m_m) {
		return (lhs.m_m > rhs.m_m);
	} else {
		return (lhs.m_n > rhs.m_n);
	}
}
bool operator<=(nv_t const& lhs, nv_t const& rhs) {
	return((lhs.m_m == rhs.m_m && lhs.m_n == rhs.m_n) || 
		(lhs.m_m < rhs.m_m || lhs.m_n < lhs.m_n));
}
bool operator>=(nv_t const& lhs, nv_t const& rhs) {
	return((lhs.m_m == rhs.m_m && lhs.m_n == rhs.m_n) || 
		(lhs.m_m > rhs.m_m || lhs.m_n > lhs.m_n));
}











