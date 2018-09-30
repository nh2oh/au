#include "nv_t.h"
#include "../util/au_util.h"
#include "../util/au_algs_math.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round
#include <numeric>  // std::gcd()


const int d_t::max_nplet {5};
const double d_t::min_duration = 1.0/std::pow(2,12);  
// 1/2^12 => 1/4096, 1/2^(12+5) => the duration of the fif'th dot on a
// 5-dotted 4096'th note
// double2ab() has to deal with these values in splitting apart a dotted
// 1/4096 note.  

d_t::d_t(common_duration_t d) {
	auto dint = static_cast<int>(d);
	// -9999 is a special value signaling d::z
	if (dint != -9999) {
		int n = std::abs(dint)%10;
		int m = -1*(dint/10);
		m_ab = ab{d_t::mn{m,n}};
	} else {
		m_ab = ab{0,0};
	}
}

d_t::d_t(const mn& mn_in) {
	m_ab = ab{mn_in};
}

d_t::d_t(double d) {
	// The argument ratio_w is interpreted as a duration, that is, as being 
	// proportional to the whole-note (also => proportional to time ).
	//
	m_ab = dbl2ab(d);
}

d_t::ab d_t::dbl2ab(double d) const {
	bool is_neg = (d<0);
	if (is_neg) { d = -1*d; }

	// For any note of duration d > 0 w/ 0-dots:  
	// frexp(d) => f == 0.5, exp ~ integer, where m = -(exp-1).
	// For a d>0 _with_ dots, 0.5 < f < 1.0, exp ~ integer, where 
	// m = -(exp-1), n=0 is the corresponding d_t w/ 0-dots.  The "remainder," 
	// d - 0.5*std::pow(2,exp), is the contribution of the dots to d.  
	//
	d_t::ab res {};

	auto duration_minduration_maxdot = d_t::min_duration/std::pow(2,5);
	//auto duration_minduration_maxdot = 1.0/std::pow(2,12);

	while(d>=duration_minduration_maxdot) {
		int exp = 0;
		auto f = frexp(d,&exp);
		d_t::ab t {1,-1*(exp-1)};
		res = res + t;
		d -= 0.5*std::pow(2,exp);
	}

	if (is_neg) { res = ab{0,0}-res; }
	return res;
}

bool d_t::singlet_exists() const {
	return m_ab.singlet_exists();
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

	if (singlet_exists()) {
		res.push_back(d_t{m_ab.to_mn()});
	} else {
		auto curr_ab = m_ab;
		while (!curr_ab.singlet_exists() && curr_ab.val() > d_t::min_duration) {
			auto p = static_cast<int>(std::floor(std::log2(curr_ab.get_a())));
			ab ab_nodots_singlet {1,curr_ab.get_b()-p};
			res.push_back(d_t{ab_nodots_singlet.to_mn()});
			curr_ab = curr_ab - ab_nodots_singlet;
		}
	}
	
	return res;
}


// The elments [0,n] must sum to exactly d1
std::vector<d_t> d_t::to_singlets_partition(const d_t& d1) const {
	std::vector<d_t> res {};
	if (d1 > *this) { return to_singlets(); }
	
	auto rem = *this - d1;
	for (auto e : d1.to_singlets()) {
		res.push_back(e);
	}
	for (auto e : rem.to_singlets()) {
		res.push_back(e);
	}

	return res;
}

// The elments n elements in the return sum to exactly d1.  No singlet 
// following n is > dmax.  
std::vector<d_t> d_t::to_singlets_partition_max(const d_t& d1, const d_t& dmax) const {
	std::vector<d_t> res {};
	if (d1 >= *this) { return to_singlets(); }
	
	auto rem = *this - d1;
	//for (auto e : d1.to_singlets()) {
	//	res.push_back(e);
	//}
	auto dmaxs = dmax.to_singlets();
	auto d1s = d1.to_singlets();
	res.insert(res.end(),d1s.begin(),d1s.end());
	while (rem > dmax) {
		res.insert(res.end(),dmaxs.begin(),dmaxs.end());
		//for (auto e : dmax.to_singlets()) {
		//	res.push_back(e);
		//}
		rem -= dmax;
	}
	//for (auto e : rem.to_singlets()) {
	//	res.push_back(e);
	//}
	auto rems = rem.to_singlets();
	res.insert(res.end(),rems.begin(),rems.end());

	//for (auto e : rem.to_singlets_partition_max(dmax, dmax)) {
	//	res.push_back(e);
	//}

	return res;
}

bool d_t::weird() const {
	return m_ab.get_a() >= 100;
}

int d_t::ndot() const {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		return mn.n;
	}
	return 0;
}
int d_t::base() const {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		return mn.m;
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
		auto curr_mn = vs[i].m_ab.to_mn();

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
		wait();
	}
	return s;
}

bool d_t::set_base(int const& m_in) {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		mn.m = m_in;
		m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::set_dots(int const& ndots) {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		mn.n = ndots;
		m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::add_dots(int const& ndots) {  // default value ndots == 1
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		mn.n += ndots;
		m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::rm_dots(int const& ndots) {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		mn.n = std::max(mn.n-ndots,0);
		m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::rm_dots() {
	if (m_ab.singlet_exists()) {
		auto mn = m_ab.to_mn();
		mn.n = 0;
		m_ab = ab {mn};
		return true;
	};
	return false;
}



d_t& d_t::operator+=(const d_t& rhs) {
	m_ab = m_ab + rhs.m_ab;
	return *this;
}

d_t& d_t::operator-=(const d_t& rhs) {
	m_ab = m_ab - rhs.m_ab;
	return *this;
}

d_t operator+(d_t lhs, const d_t& rhs) {
	return lhs += rhs;
}

d_t operator-(d_t lhs, const d_t& rhs) {
	return lhs -= rhs;
}
d_t& d_t::operator*=(const double& n) {
	m_ab = dbl2ab(m_ab.val()*n);
	return *this;
}
d_t& d_t::operator/=(const double& d) {
	m_ab = dbl2ab(m_ab.val()/d);
	return *this;
}
double operator/(const d_t& n, const d_t& d) {
	double f = static_cast<double>(n.m_ab.get_a())/static_cast<double>(d.m_ab.get_a());
	auto db = d.m_ab.get_b() - n.m_ab.get_b();
	return f*std::pow(2,db);
	//return (n.m_ab.val() / d.m_ab.val());
}
bool d_t::operator<(const d_t& rhs) const {
	return aprx_lt(m_ab.val(),rhs.m_ab.val()); // (m_ab.val() < rhs.m_ab.val());
}
bool d_t::operator>(const d_t& rhs) const {
	return aprx_gt(m_ab.val(),rhs.m_ab.val()); // (m_ab.val() > rhs.m_ab.val());
}
bool d_t::operator==(const d_t& rhs) const {
	return aprx_eq(m_ab.val(), rhs.m_ab.val());
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

d_t operator*(const double& lhs, d_t rhs) {
	return (rhs *= lhs);
}
d_t operator*(d_t lhs, const double& rhs) {
	return (lhs *= rhs);
}
d_t operator/(d_t n, const double& d) {
	return n /= d;
}

d_t gcd(const d_t first, const d_t second) {
	auto a = std::gcd(first.m_ab.get_a(),second.m_ab.get_a());
	auto b = std::pow(2,std::max(first.m_ab.get_b(),second.m_ab.get_b()));
	return d_t {a/b};
}

d_t::ab::ab(const d_t::mn& mn_in) {
	a = static_cast<int>(std::pow(2,mn_in.n+1))-1;
	b = mn_in.m + mn_in.n;
	reduce();
}

d_t::ab::ab(int a_in, int b_in) {
	a = a_in;
	b = b_in;
	reduce();
}

void d_t::ab::reduce() {
	while (a > 0 && a%2 == 0) {
		a = a/2;
		b -= 1;
	}
}

// You have to check if a singlet_exists() first, otherwise the 
// answer could be _way_ wrong.  
d_t::mn d_t::ab::to_mn() const {
	auto n = static_cast<int>(std::log2(a+1)-1.0);
	return d_t::mn{b-n, n};
}

double d_t::ab::val() const {
	return static_cast<double>(a)/std::pow(2,b);
}

int d_t::ab::get_a() const {
	return a;
}

int d_t::ab::get_b() const {
	return b;
}

// Can this duration be represented as a singlet?  That is, is there
// an m,n representation?
bool d_t::ab::singlet_exists() const {
	return is_mersenne(a);
}

d_t::ab d_t::ab::operator+(const d_t::ab& rhs) const {
	// a,b-form requires a be an integer, therefore 2^db must be >= 1
	// because the a term of the sum is: m_a = m_a*(2^db)*d2.m_a
	ab res {};
	auto db = rhs.b - b;  // "delta b"
	if (db >= 0) {
		res.a = (a)*static_cast<int>(std::pow(2,db)) + rhs.a;
		res.b = rhs.b;
	} else {
		res.a = (rhs.a)*static_cast<int>(std::pow(2,-1*db)) + a;
		res.b = b;
	}

	res.reduce();
	return res;
}

d_t::ab d_t::ab::operator-(const d_t::ab& rhs) const {
	// a,b-form requires a be an integer, therefore 2^db must be >= 1
	// because the a term of the sum is: m_a = m_a*(2^db)*d2.m_a
	ab res {};
	auto db = rhs.b - b;  // "delta b"
	if (db >= 0) {
		res.a = (a)*static_cast<int>(std::pow(2,db)) - rhs.a;
		res.b = rhs.b;
	} else {
		res.a = a - (rhs.a)*static_cast<int>(std::pow(2,-1*db));
		res.b = b;
	}
	
	res.reduce();
	return res;
}







std::vector<autests::dtset> autests::make_dt_set(int m_min, int n_min, int m_max, int n_max) {
	//int m_min = -3; // octuple whole-note
	//int m_max = 12; // 4096'th note 
	//int n_min = 0; // no dots
	//int n_max = 5; 
	std::vector<autests::dtset> result {};
	for (int m=m_max; m>=m_min; --m) {  // smallest bv -> largest bv
		for (int n=n_min; n<=n_max; ++n) {
			result.push_back({d_t {d_t::mn{m,n}}, m, n});
		}
	}

	return result;
}
