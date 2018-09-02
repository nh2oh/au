#include "nv_t.h"
#include "../util/au_util.h"
#include "../util/au_algs_math.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round


const int d_t::max_nplet {5};

d_t::d_t(common_duration_t d) {
	auto dint = static_cast<int>(d);
	int n = std::abs(dint%10);
	int m = dint/10;
	
	auto ab_in = mn2ab({m,n});
	m_a = ab_in.a; m_b = ab_in.b;
}

d_t::d_t(const mn& mn_in) {
	auto ab_in = mn2ab(mn_in);
	m_a = ab_in.a; m_b = ab_in.b;
}

// Called by multiple constructors and other internal methods
d_t::ab d_t::mn2ab(const d_t::mn& mn_in) const {
	int a = static_cast<int>(std::pow(2,mn_in.n+1))-1;
	int b = mn_in.m + mn_in.n;
	return d_t::ab {a,b};
}

// You have to check if a singlet_exists() first, otherwise the 
// answer could be _way_ wrong.  
d_t::mn d_t::ab2mn(const d_t::ab& ab_in) const {
	auto n = static_cast<int>(std::log2(ab_in.a+1)-1.0);
	return d_t::mn{ab_in.b-n, n};
}

// Can this duration be represented as a singlet?  That is, is there
// an m,n representation?
bool d_t::singlet_exists() const {
	return is_mersenne(m_a);
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
std::vector<d_t> d_t::to_singlets() const {
	std::vector<d_t> res {};

	auto a = m_a; auto b = m_b;
	while (a>0) {
		if (is_mersenne(a)) {
			auto n = static_cast<int>(std::log2(a+1)-1.0);
			res.push_back(d_t{d_t::mn{b-n, n}});
			break;
		} else {
			auto p = static_cast<int>(std::floor(std::log2(a)));
			res.push_back(d_t{d_t::mn{b-p, 0}});
			a = a-static_cast<int>(std::pow(2,p));
			wait();
		}
	}

	return res;
}

int d_t::ndot() const {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		return mn_rep.n;
	}
	return 0;
}
int d_t::base() const {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		return mn_rep.m;
	}
	return 0;
}

std::string d_t::print() const {
	auto vs = to_singlets();
	std::string s {};

	bool tie = (vs.size() > 1);
	for (int i=0; i<vs.size(); ++i) {
		bool firstiter = (i == 0);
		bool lastiter = (i == vs.size()-1);
		auto curr_mn = ab2mn({vs[i].m_a,vs[i].m_b});

		auto bv = std::pow(2,curr_mn.m);
		if (curr_mn.m > 0) { // nv() < the whole note
			s += "1/";
			s += std::to_string(static_cast<int>(bv));
		} else { // nv() >= the whole note
			s += std::to_string(static_cast<int>(1.0/bv));
		}

		if (tie && !firstiter) {
			s += ")";
		}
		s += std::string(curr_mn.n, '.');
		if (tie && !lastiter) {
			s += "(";
		}
	}
	return s;
}

bool d_t::set_base(int const& m_in) {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		mn_rep.m = m_in;
		auto ab_rep = mn2ab(mn_rep);
		m_a = ab_rep.a; m_b = ab_rep.b;
		return true;
	}
	return false;
}
bool d_t::set_dots(int const& ndots) {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		mn_rep.n = ndots;
		auto ab_rep = mn2ab(mn_rep);
		m_a = ab_rep.a; m_b = ab_rep.b;
		return true;
	}
	return false;
}
bool d_t::add_dots(int const& ndots) {  // default value ndots == 1
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		mn_rep.n += ndots;
		auto ab_rep = mn2ab(mn_rep);
		m_a = ab_rep.a; m_b = ab_rep.b;
		return true;
	}
	return false;
}
bool d_t::rm_dots(int const& ndots) {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		mn_rep.n = std::max(mn_rep.n-ndots,0);
		auto ab_rep = mn2ab(mn_rep);
		m_a = ab_rep.a; m_b = ab_rep.b;
		return true;
	}
	return false;
}
bool d_t::rm_dots() {
	if (singlet_exists()) {
		auto mn_rep = ab2mn({m_a,m_b});
		mn_rep.n = 0;
		auto ab_rep = mn2ab(mn_rep);
		m_a = ab_rep.a; m_b = ab_rep.b;
		return true;
	};
	return false;
}


d_t& d_t::operator+=(const d_t& d2) {
	// a,b-form requires a be an integer, therefore 2^db must be >= 1
	// because the a term of the sum is: m_a = m_a*(2^db)*d2.m_a
	auto db = d2.m_b - m_b;  // "delta b"
	if (db >= 0) {
		m_a = (m_a)*static_cast<int>(std::pow(2,db)) + d2.m_a;
		m_b = d2.m_b;
	} else {
		m_a = (d2.m_a)*static_cast<int>(std::pow(2,-1*db)) + m_a;
		m_b = m_b;
	}

	return *this;
}


// TODO:  Check this for both branches
d_t& d_t::operator-=(const d_t& d2) {
	// a,b-form requires a be an integer, therefore 2^db must be >= 1
	// because the a term of the sum is: m_a = m_a*(2^db)*d2.m_a
	auto db = d2.m_b - m_b;  // "delta b"
	if (db >= 0) {
		m_a = (m_a)*static_cast<int>(std::pow(2,db)) - d2.m_a;
		m_b = d2.m_b;
	} else {
		m_a = (d2.m_a)*static_cast<int>(std::pow(2,-1*db)) - m_a;
		m_b = m_b;
	}

	return *this;
}

d_t operator+(d_t lhs, const d_t& rhs) {
	return lhs += rhs;
}
d_t operator-(d_t lhs, const d_t& rhs) {
	return lhs -= rhs;
}
d_t& d_t::operator*=(const int& n) {
	m_a *= n;
	return *this;
}
bool d_t::operator<(const d_t& d) const {
	return (static_cast<double>(m_a)/std::pow(2,m_b) < 
		static_cast<double>(d.m_a)/std::pow(2,d.m_b));
}
bool d_t::operator>(const d_t& d) const {
	return (static_cast<double>(m_a)/std::pow(2,m_b) > 
		static_cast<double>(d.m_a)/std::pow(2,d.m_b));
}
bool d_t::operator==(const d_t& d) const {
	return (aprx_eq(static_cast<double>(m_a)/std::pow(2,m_b), 
		static_cast<double>(d.m_a)/std::pow(2,d.m_b)));
}
bool operator!=(const d_t& lhs, const d_t& rhs) {
	return !(lhs==rhs);
}
bool operator<=(const d_t& lhs, const d_t& rhs) {
	return (lhs < rhs || lhs == rhs);
}
bool operator>=(const d_t& lhs, const d_t& rhs) {
	return (lhs > rhs || lhs == rhs);
}


/*
//-----------------------------------------------------------------------------
// nv_t

int const nv_t::min_bv_exponent = -3;  // => 8, private
int const nv_t::max_bv_exponent = 10;  // => 1/1024, private
double const nv_t::min_bv = 1.0/std::pow(2,nv_t::max_bv_exponent); // public
double const nv_t::max_bv = 1.0/std::pow(2,nv_t::min_bv_exponent); // public
int const nv_t::max_ndots = 5; // public

nv_t::nv_t(common_duration_t nv) {
	m_d = d_t(nv);
}

nv_t::nv_t(int m, int n) {
	m_d = mn2d({m,n});
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



//-----------------------------------------------------------------------------
// Setters; public, non-static
nv_t& nv_t::set_base(double const& bv) {
	au_assert(bv_isvalid(bv), __FUNCTION__);

	m_m = bv_exponent(bv);
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

// This defines m,n form by relating an m,n pair to a d
d_t nv_t::mn2d(const nv_t::mn& mn) const {
	int a = static_cast<int>(std::pow(2,mn.n+1))-1;
	int b = mn.m + mn.n;
	return d_t {a,b};
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






tie_t::tie_t(nv_t const& nv1) {
	m_ab = nvt2ab(nv1);
}
tie_t::tie_t(int const& n_nv_in, nv_t const& nv_in) {
	auto ab_in = nvt2ab(nv_in);
	m_ab = ab_in; 
	for (int i=1; i<n_nv_in; ++i) {
		m_ab = add_ab(m_ab,ab_in);
	}
}
tie_t::tie_t(nv_t const& nv1, nv_t const& nv2) {
	auto ab1 = nvt2ab(nv1); auto ab2 = nvt2ab(nv2);
	m_ab = add_ab(ab1,ab2);
}

tie_t::tie_t(std::vector<nv_t> const& nvs) {
	for (auto e : nvs) {
		m_ab = add_ab(m_ab,nvt2ab(e));
	}
}





















*/



