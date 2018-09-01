#include "nv_t.h"
#include "../util/au_error.h"
#include "../util/au_util.h"
#include "../util/au_algs_math.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round
#include <algorithm> // for swap()

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







const int tie_t::max_nplet {5};

tie_t::tie_t(nv_t const& nv1) {
	//auto mn = nvt2mn(nv1);
	//auto ab = mn2ab(mn);
	//m_a = ab.a; m_b = ab.b;
	m_ab = nvt2ab(nv1);
}
tie_t::tie_t(int const& n_nv_in, nv_t const& nv_in) {
	auto ab_in = nvt2ab(nv_in);
	m_ab = ab_in; 
	for (int i=1; i<n_nv_in; ++i) {
		m_ab = add_ab(m_ab,ab_in);
	}
	//auto mn = nvt2mn(nv_in);
	//auto ab = mn2ab(mn);
	//m_a = n_nv_in*(ab.a); m_b = ab.b;
}
tie_t::tie_t(nv_t const& nv1, nv_t const& nv2) {
	auto ab1 = nvt2ab(nv1); auto ab2 = nvt2ab(nv2);
	m_ab = add_ab(ab1,ab2);
	//auto mn1 = nvt2mn(nv1); auto mn2 = nvt2mn(nv2);
	//auto ab1 = mn2ab(mn1); auto ab2 = mn2ab(mn2);
	//if (ab1.b > ab2.b) { std::swap(ab1,ab2); }

	//m_a = (ab1.a)*std::pow(2,(ab2.b-ab1.b)) + ab2.a;
	//m_b = std::pow(2,ab2.b);
	//m_b = ab2.b;
}

tie_t::tie_t(nv_t const&, nv_t const&,  nv_t const&) {
	//...
}
tie_t::tie_t(std::vector<nv_t> const& nvs) {
	for (auto e : nvs) {
		m_ab = add_ab(m_ab,nvt2ab(e));
	}
}

// Can this duration be represented as a single nv_t ?
bool tie_t::singlet_exists() const {
	return is_mersenne(m_ab.a);
}

tie_t::nvt_ab tie_t::add_ab(nvt_ab ab1, nvt_ab ab2) const {
	if (ab1.b > ab2.b) { std::swap(ab1,ab2); }
		// The static_cast<int> below requires (ab1.a)*std::pow(2,db)
		// actually be an integer... 2^db needs to be >=1
	auto db = ab2.b - ab1.b;
	auto a3 = static_cast<int>((ab1.a)*(std::pow(2,db))) + ab2.a;
	auto b3 = ab2.b;
	return nvt_ab {a3,b3};
}

// Convert the duration to a tuplet of nv_t's, the sum of which
// is equivalent to the duration.  
//
// Algorithm:
// Given a d = a/(2^b) with integer a >= 1, integer b (which holds provided
// d arose as either an nv_t or a sum of nv_t's), decompose it into some
// set of nv_t's which if summed would have a duration == d.  
//
// CASE 1:
// If a is a mersenne number, d has an nv_t representation, so just calculate
// m,n and return the corresponding nv_t.  
//
// CASE 2:
// If a is _not_ a mersenne number, break d into a sum:
// d = a/(2^b) = (2^p)/(2^b) + a2/(2^b)
// where (2^p) is the largest integer power of 2 not > a.  The first term 
// thus reduces easily, and the sum becomes:
// d = 1/(2^(b-p) + (a-2^p)/(2^b)
// Since 1 is a mersenne number and b-p is an integer, the first term has
// an nv_t representation with n = 0 and m = b-p.  Note that since the 
// numerator of the first term is always 1, the corresponding nv_t always
// has zero dots.  Note also that (since durations are always > 0) the
// second term is always a shorter duration than the first.  
// 
// Compute the nv_t for the first term and push_back the result into the
// result std::vector<nv_t>.  
// 
// Set d == the second term (a == a-2^p, b == 2^b) and repeat the process
// with the new d.  
//
std::vector<nv_t> tie_t::to_nvs() const {
	auto ab = m_ab;
	std::vector<nv_t> res {};

	while (ab.a>0) {
		if (is_mersenne(ab.a)) {
			auto n = std::log2(ab.a+1)-1.0;
			nvt_mn mn = {ab.b-static_cast<int>(n), static_cast<int>(n)};
			res.push_back(mn2nvt(mn));
			break;
		} else {
			auto p = static_cast<int>(std::floor(std::log2(ab.a)));
			nvt_mn mn {ab.b-p,0};
			res.push_back(mn2nvt(mn));  // ab1 converted to an mn
			ab.a = ab.a-std::pow(2,p);
			wait();
		}
	}
	return res;
}

// Always possible.  Every m,n representation has an a,b representation.  
tie_t::nvt_ab tie_t::mn2ab(tie_t::nvt_mn const& mn) const {
	int a = static_cast<int>(std::pow(2,mn.n+1))-1;
	int b = mn.m + mn.n;
	return nvt_ab {a,b};
}

nv_t tie_t::mn2nvt(tie_t::nvt_mn const& mn) const {
	return nv_t {1.0/std::pow(2,mn.m),mn.n};
}

tie_t::nvt_mn tie_t::nvt2mn(nv_t const& nvt_in) const {
	nv_t unitnv {1,0};
	auto m = static_cast<int>((-1)*std::log2(nvt_in.base()/unitnv));
	return nvt_mn {m,nvt_in.ndot()};
}

tie_t::nvt_ab tie_t::nvt2ab(nv_t const& nvt_in) const {
	auto mn = nvt2mn(nvt_in);
	return mn2ab(mn);
}



