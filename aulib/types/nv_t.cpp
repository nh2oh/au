#include "nv_t.h"
#include "../util/au_error.h"
#include "../util/au_algs_math.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round
#include <algorithm> // for swap()
/*
const nv_t nv::dw {2.0,0};
const nv_t nv::dwd {2.0,1}; const nv_t nv::dwdd {2.0,2};
const nv_t nv::w{1,0};
const nv_t nv::wd{1,1}; const nv_t nv::wdd{1,2};
const nv_t nv::h{0.5,0};
const nv_t nv::hd{0.5,1}; const nv_t nv::hdd{0.5,2};
const nv_t nv::q{0.25,0};
const nv_t nv::qd{0.25,1}; const nv_t nv::qdd{0.25,2};
const nv_t nv::e{0.125,0};
const nv_t nv::ed{0.125,1}; const nv_t nv::edd{0.125,2};
const nv_t nv::s{0.0625,0};
const nv_t nv::sd{0.0625,1}; const nv_t nv::sdd{0.0625,2};
const nv_t nv::t{0.03125,0};
const nv_t nv::td{0.03125,1}; const nv_t nv::tdd{0.03125,2};*/

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

nv_t::nv_t(nvc e_class_nv_in) {
	if (e_class_nv_in == nvc::dw) {
		m_m = bv_exponent(2.0);
		m_n = 0;
	} else if (e_class_nv_in == nvc::dwd) {
		m_m = bv_exponent(2.0);
		m_n = 1;
	} else if (e_class_nv_in == nvc::dwdd) {
		m_m = bv_exponent(2.0);
		m_n = 2;
	} else if (e_class_nv_in == nvc::w) {
		m_m = bv_exponent(1.0);
		m_n = 0;
	} else if(e_class_nv_in == nvc::wd) {
		m_m = bv_exponent(1.0);
		m_n = 1;
	} else if(e_class_nv_in == nvc::wdd) {
		m_m = bv_exponent(1.0);
		m_n = 2;
	}
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
// Returns a vector of unique, zero-dotted nv_t's which, if their nv()'s 
// were summed, would == the nv() of the input.  The nv() of each element
// (i+1) == 1/2 element i.  
std::vector<nv_t> nv_t::explode() const {
	std::vector<nv_t> result {};
	for (int i=0; i<m_n; ++i) {
		result.push_back(nv_t{bv()/std::pow(2,i),0});
	}
	return result;
}
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










tuplet_t::tuplet_t() {};
tuplet_t::tuplet_t(nv_t const& nv1) {
	auto mn = nvt_to_mn(nv1);
	auto ab = mn2ab(mn);
	m_a = ab.a; m_b = ab.b;
}
tuplet_t::tuplet_t(int const& n_nv_in, nv_t const& nv_in) {
	auto mn = nvt_to_mn(nv_in);
	auto ab = mn2ab(mn);
	m_a = n_nv_in*(ab.a); m_b = ab.b;
}
tuplet_t::tuplet_t(nv_t const& nv1, nv_t const& nv2) {
	auto mn1 = nvt_to_mn(nv1); auto mn2 = nvt_to_mn(nv2);
	auto ab1 = mn2ab(mn1); auto ab2 = mn2ab(mn2);
	if (ab1.b > ab2.b) { std::swap(ab1,ab2); }

	m_a = (ab1.a)*std::pow(2,(ab2.b-ab1.b)) + ab2.a;
	m_b = std::pow(2,ab2.b);
}

tuplet_t::tuplet_t(nv_t const&, nv_t const&,  nv_t const&) {
	//...
}
tuplet_t::tuplet_t(std::vector<nv_t> const&) {
	//...
}

bool tuplet_t::singlet_exists() const {
	return isapproxint(std::log2(m_a+1),6);
}

tuplet_t::nvt_ab tuplet_t::mn2ab(tuplet_t::nvt_mn const& mn) const {
	// a = sum(i=0,i=n,2^i) = 2^(n+1)-1
    // b = m+n
	int a = static_cast<int>(std::pow(2,mn.n+1))-1;
	int b = mn.m + mn.n;
	return nvt_ab {a,b};
}

tuplet_t::nvt_mn tuplet_t::nvt_to_mn(nv_t const& nvt_in) const {
	auto unitnv = nv_t(1,0);
	auto m = static_cast<int>((-1)*std::log2(nvt_in.base()/unitnv));

	return nvt_mn {m,nvt_in.ndot()};
}





