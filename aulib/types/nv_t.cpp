#include "nv_t.h"
#include "../util/au_util_all.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round

//-----------------------------------------------------------------------------
// Public & private statics
int const nv_t::min_bv_exponent = -3;  // => 8, private
int const nv_t::max_bv_exponent = 10;  // => 1/1024, private
double const nv_t::min_bv = 1.0/std::pow(2,nv_t::max_bv_exponent); // public
double const nv_t::max_bv = 1.0/std::pow(2,nv_t::min_bv_exponent); // public
int const nv_t::max_ndots = 5; // public

bool nv_t::bv_isvalid(double const& bv) { // public
	if (bv < min_bv || bv > max_bv) {
		return false;
	}
	return true;
}
bool nv_t::ndots_isvalid(int const& ndot) { // public
	return (ndot >= 0 && ndot <= max_ndots);
}
double nv_t::quantize_bv(double const& bv) { // public
	return 1.0/std::pow(2,bv_exponent(bv));
}
int nv_t::bv_exponent(double const& bv) {  // private
	return static_cast<int>(std::round(std::log2(1.0/bv)));
}

//-----------------------------------------------------------------------------
// Constructors
nv_t::nv_t() {  // creates the "unit nv"
	m_m = 0;
	m_n = 0;
}
nv_t::nv_t(double const& bv, int const& ndot) {
	au_assert((ndots_isvalid(ndot) && bv_isvalid(bv)), __FUNCTION__);
	m_m = bv_exponent(bv);
	m_n = ndot;
}

//-----------------------------------------------------------------------------
// Getters
std::string nv_t::print() const {
	std::string s {};
	if (m_m > 0) { // nv() < the whole note
		s += "1/";
		s += std::to_string(static_cast<int>(std::pow(2,m_m)));
	} else { // nv() >= the whole note
		s += std::to_string(static_cast<int>(std::pow(2,-1*m_m)));
	}
	s += std::string(m_n,'.');
	return s;
}
int nv_t::ndot() const {
	return m_n;
}
nv_t nv_t::base() const {
	return nv_t {bv(),0};
}

/*
nv_t nv_t::split(int n) const {
	au_assert(false,"nv_t.split() is not yet implemented");
	return nv_t();
};

// Returns a vector of zero-dotted nv_t's which, if their nv()'s were summed
// would == the nv() of the input.  Each element (i+1) == 1/2 element i
std::vector<nv_t> nv_t::explode() const {
	std::vector<nv_t> result {};
	for (int i=0; i<m_n; ++i) {
		result.push_back(nv_t{bv()/std::pow(2,i),0});
	}
	return result;
};
*/
//-----------------------------------------------------------------------------
// Setters; public, non-static
nv_t& nv_t::set_base(double const& bv) {
	au_assert(bv_isvalid(bv), __FUNCTION__);

	m_m = bv_exponent(bv);
	m_n = 0;

	return *this;
}
nv_t& nv_t::set_dots(int const& ndots) {
	au_assert(ndots_isvalid(ndots), __FUNCTION__);
	return *this;
}
nv_t& nv_t::add_dot(int const& ndots) {  // default value ndots == 1
	int new_n = m_n + ndots;
	au_assert(ndots_isvalid(new_n), __FUNCTION__);
	m_n = new_n;
	return *this;
}
nv_t& nv_t::rm_dots(int const& ndots) {
	int new_n = m_n - ndots;
	au_assert(ndots_isvalid(new_n), __FUNCTION__);
	m_n = new_n;
	return *this;
}
nv_t& nv_t::rm_dots() {
	m_n = 0;
	return *this;
}

//-----------------------------------------------------------------------------
// Private non-static methods
double nv_t::nv() const {
	return (1.0/std::pow(2,m_m))*(2.0 - 1.0/std::pow(2,m_n));
}
double nv_t::bv() const {
	return 1.0/std::pow(2,m_m);
}

//-----------------------------------------------------------------------------
// Operators
double operator/(nv_t const& num, nv_t const& denom) {
	return num.nv()/denom.nv();
}
bool operator==(nv_t const& lhs, nv_t const& rhs) {
	return(lhs.nv() == rhs.nv());
}
bool operator!=(nv_t const& lhs, nv_t const& rhs) {
	return(!(rhs==lhs));
}
bool operator<(nv_t const& lhs, nv_t const& rhs) {
	return (lhs.nv() < rhs.nv());
}
bool operator>(nv_t const& lhs, nv_t const& rhs) {
	return (lhs.nv() > rhs.nv());
}
bool operator<=(nv_t const& lhs, nv_t const& rhs) {
	return (lhs.nv() <= rhs.nv());
}
bool operator>=(nv_t const& lhs, nv_t const& rhs) {
	return (lhs.nv() >= rhs.nv());
}


/*
std::vector<nv_t> nvsum(std::vector<nv_t> nvs) {
	if (n_unique(nvs) < nvs.size()) {
		auto smallest = nvs[0].explode().back();
		for (int i=0; i<nvs.size(); ++i) {
			if (nvs[i].explode().back() < smallest) {
				smallest = nvs[i].explode().back();
			}
		}
	
		int n = 0;
		for (int i=0; i<nvs.size(); ++i) {
			n += static_cast<int>(nvs[i]/smallest);
		}
		auto Nx = std::vector<nv_t>(n,smallest);
		nvsum(Nx);
	}
	// Nx...
}


std::vector<nv_t> nvsum(std::vector<nv_t>,std::vector<nv_t>);
std::vector<nv_t> nvsum_finalize(std::vector<nv_t>);
*/

