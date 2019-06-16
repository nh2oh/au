#include "nv_t.h"
#include "dbklib\math.h"
#include <string>
#include <vector>
#include <cmath>  // std::pow(), std::log2, std::round
#include <numeric>  // std::gcd()
#include <regex>

const int d_t::max_nplet {5};
const double d_t::min_duration = 1.0/std::pow(2,12);  
// 1/2^12 => 1/4096, 1/2^(12+5) => the duration of the fif'th dot on a
// 5-dotted 4096'th note
// double2ab() has to deal with these values in splitting apart a dotted
// 1/4096 note.  

// TODO:  Ctor calling ctor => gross
d_t::d_t(common_duration_t d) {
	auto dint = static_cast<int>(d);
	if (dint != -9999) {  // -9999 is a special value signaling d::z
		int n = std::abs(dint)%10;
		int m = -1*(dint/10);
		*this = d_t {d_t::mn {m,n}};
	} else {
		a_=0; b_=0;
	}
}

d_t::d_t(const mn& mn_in) {
	a_ = static_cast<int>(std::pow(2,mn_in.n+1))-1;
	b_ = mn_in.m + mn_in.n;
	this->reduce();
}

// d ~ some number of whole notes (=> proportional to time).
d_t::d_t(double d) {
	bool is_neg = (d<0);
	if (is_neg) { d = std::abs(d); }

	// For any note of duration d > 0 w/ 0-dots:  
	// frexp(d) => f == 0.5, exp ~ integer, where m = -(exp-1).
	// For a d>0 _with_ dots, 0.5 < f < 1.0, exp ~ integer, where 
	// m = -(exp-1), n=0 is the corresponding d_t w/ 0-dots.  The "remainder," 
	// d - 0.5*std::pow(2,exp), is the contribution of the dots to d.  
	//
	d_t temp {};  // TODO:  ctor calling ctor => gross
	a_ = 0;  b_ = 0;  // init state matters b/c using operator+=():  *this +=

	double duration_minduration_maxdot {d_t::min_duration/std::pow(2,5)};
	while(d>=duration_minduration_maxdot) {
		int exp = 0;
		double f = frexp(d,&exp);
		temp.a_ = 1; temp.b_ = -1*(exp-1);
		temp.reduce();

		*this += temp;
		d -= 0.5*std::pow(2,exp);
	}

	if (is_neg) {
		temp.a_=0; temp.b_=0;
		*this=temp-*this;
	}
}

bool d_t::singlet_exists() const {
	return this->mn_exists();
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

	if (this->mn_exists()) {
		res.push_back(d_t{this->to_mn()});
	} else {
		d_t remain = *this;
		while (!remain.mn_exists() && remain.to_double() > d_t::min_duration) {
			auto p = static_cast<int>(std::floor(std::log2(remain.a_)));

			d_t temp = remain;
			temp.a_ = 1; temp.b_ -= p;
			res.push_back(temp);

			remain -= temp;
		}
	}
	
	return res;
}


// The first n elements of the result must sum to exactly d1; the sum of all the elements in the
// result == this, even if d1 > this.
std::vector<d_t> d_t::to_singlets_partition(const d_t& d1) const {
	std::vector<d_t> res {};
	if (d1 > *this) { return to_singlets(); }
	
	for (auto e : d1.to_singlets()) {
		res.push_back(e);
	}
	auto rem = *this - d1;
	for (auto e : rem.to_singlets()) {
		res.push_back(e);
	}

	return res;
}

// The elments n elements in the return sum to exactly d1.  No singlet 
// following n is > dmax.  
std::vector<d_t> d_t::to_singlets_partition_max(const d_t& d1, const d_t& dmax) const {
	std::vector<d_t> res {};
	if (d1 >= *this) { return this->to_singlets(); }
	
	auto rem = *this - d1;
	auto dmaxs = dmax.to_singlets();
	auto d1s = d1.to_singlets();
	res.insert(res.end(),d1s.begin(),d1s.end());
	while (rem > dmax) {
		res.insert(res.end(),dmaxs.begin(),dmaxs.end());
		rem -= dmax;
	}
	auto rems = rem.to_singlets();
	res.insert(res.end(),rems.begin(),rems.end());

	return res;
}

bool d_t::weird() const {
	return a_ >= 100;
}

// TODO:  Shitty that these can't be relied on... move into a putative nv_t
// s.t. they always work.  
int d_t::ndot() const {
	if (this->mn_exists()) {
		auto mn = this->to_mn();
		return mn.n;
	}
	return 0;
}
int d_t::base() const {
	if (this->mn_exists()) {
		auto mn = this->to_mn();
		return mn.m;
	}
	return 0;
}

d_t d_t::base_nv() const {
	if (!is_mersenne(std::abs(a_))) {
		return *this;
	}
	// There exists a singlet representation but it may be < 0
	int sign = a_ < 0 ? -1 : 1;
	d_t temp_positive = *this;  temp_positive.a_ = std::abs(temp_positive.a_);
	auto mn_positive = temp_positive.to_mn();
	mn_positive.n = 0;
	d_t result_positive {mn_positive};
	return sign*result_positive;
}

std::string d_t::print(d_t::opts o) const {
	auto vs = to_singlets();
	std::string s {};

	bool tie = (vs.size() > 1);
	for (int i=0; i<vs.size(); ++i) {
		bool firstiter = (i == 0);
		bool lastiter = (i == vs.size()-1);
		//auto curr_mn = vs[i].m_ab.to_mn();
		auto curr_mn = vs[i].to_mn();

		auto bv = std::pow(2,curr_mn.m);
		if (curr_mn.m > 0) { // nv() < the whole note
			if (!o.denom_only) {
				s += "1/";
			}
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

bool d_t::set_base(int m_in) {
	if (this->mn_exists()) {
		auto mn = this->to_mn();
		mn.m = m_in;
		*this = d_t {mn};
		//m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::set_dots(int ndots) {
	if (this->mn_exists()) {
		auto mn =this->to_mn();
		mn.n = ndots;
		*this = d_t {mn}; //m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::add_dots(int ndots) {  // default value ndots == 1
	if (this->mn_exists()) {
		auto mn =this->to_mn();
		mn.n += ndots;
		*this = d_t {mn}; //m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::rm_dots(int ndots) {
	if (this->mn_exists()) {
		auto mn = this->to_mn();
		mn.n = std::max(mn.n-ndots,0);
		*this = d_t {mn};  //m_ab = ab {mn};
		return true;
	}
	return false;
}
bool d_t::rm_dots() {
	if (this->mn_exists()) {
		auto mn = this->to_mn();
		mn.n = 0;
		*this = d_t {mn}; //m_ab = ab {mn};
		return true;
	};
	return false;
}

// d = a/(2^b)
// Durations are always stored in reduced form so that this->to_mn() will return the "reduced" 
// m,n-form.  For example, the getter d_t.base() simply checks mn_exists() and returns 
// this->to_mn().m
void d_t::reduce() {
	while (a_ != 0 && a_%2 == 0) {
		a_ = a_/2;
		b_ -= 1;
	}
}

d_t::mn d_t::to_mn() const {
	if (!this->mn_exists()) { std::abort(); }
	auto n = static_cast<int>(std::log2(a_+1)-1.0);
	return d_t::mn {b_-n, n};
}

double d_t::to_double() const {
	return static_cast<double>(a_)/std::pow(2,b_);
}

// Can this duration be represented as a singlet?  That is, is there
// an m,n representation?
bool d_t::mn_exists() const {
	return (a_>0 && is_mersenne(a_));
}


d_t& d_t::operator+=(const d_t& rhs) {
	// a,b-form requires a be an integer, therefore 2^db must be >= 0
	// because the a term of the sum is: a_ = a_*(2^db)+rhs.a_
	auto db = rhs.b_ - b_;  // "delta b"
	if (db >= 0) {
		a_ = (a_)*static_cast<int>(std::pow(2,db)) + rhs.a_;
		b_ = rhs.b_;
	} else {
		a_ = (rhs.a_)*static_cast<int>(std::pow(2,-1*db)) + a_;
		// b_ unchanged;  TODO:  Really?
	}
	this->reduce();
	return *this;
}

d_t& d_t::operator-=(const d_t& rhs) {
	auto db = rhs.b_ - b_;  // "delta b"
	if (db >= 0) {
		a_ = (a_)*static_cast<int>(std::pow(2,db)) - rhs.a_;
		b_ = rhs.b_;
	} else {
		a_ = a_ - (rhs.a_)*static_cast<int>(std::pow(2,-1*db));
		// b_ unchanged;  TODO:  Really?
	}
	this->reduce();
	return *this;
}

d_t operator+(d_t lhs, const d_t& rhs) {
	return lhs += rhs;
}
d_t operator-(d_t lhs, const d_t& rhs) {
	return lhs -= rhs;
}
d_t& d_t::operator*=(const double& n) {
	*this = d_t {n*(this->to_double())};
	return *this;
}
d_t& d_t::operator/=(const double& d) {
	*this = d_t {this->to_double()/d};
	return *this;
}
double operator/(const d_t& n, const d_t& d) {
	return n.to_double()/d.to_double();
	//double f = static_cast<double>(n.a_)/static_cast<double>(d.a_);
	//auto db = d.b_ - n.b_;
	//return f*std::pow(2,db);
}
bool d_t::operator<(const d_t& rhs) const {
	return aprx_lt(this->to_double(),rhs.to_double());
}
bool d_t::operator>(const d_t& rhs) const {
	return aprx_gt(this->to_double(),rhs.to_double());
}
// TODO:  This does not need to use the fp aprx functions if it can be assumed that each d_t 
// is in reduced form.  
bool d_t::operator==(const d_t& rhs) const {
	return aprx_eq(this->to_double(),rhs.to_double());
	//return aprx_eq(m_ab.val(), rhs.m_ab.val());
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
//d_t operator*(const int& lhs, d_t rhs) {
//	return (rhs *= lhs);
//}
d_t operator*(d_t lhs, const double& rhs) {
	return (lhs *= rhs);
}
d_t operator/(d_t n, const double& d) {
	return n /= d;
}

d_t gcd(const d_t first, const d_t second) {
	auto a = std::gcd(first.a_,second.a_);
	auto b = std::pow(2,std::max(first.b_,second.b_));
	return d_t {a/b};
}




dt_str_parsed parse_dt_string(const std::string& s) {
	dt_str_parsed res {};

	std::regex rx {"1/(\\d+)(\\.*)?"};
	std::smatch rx_matches {};  // std::smatch == std::match_results<std::string::const_interator>
	if (!std::regex_match(s,rx_matches,rx)) {
		res.is_valid = false;
		return res;
	}
	int denom {std::stoi(rx_matches[1].str())};
	if (denom == 0) {
		res.is_valid = false;
		return res;
	}
	double bv = std::log2(denom);
	if (!aprx_eq(static_cast<double>(denom), std::pow(2.0,bv))) {
		res.is_valid = false;  // not a power of 2
		return res;
	}
	res.denom = denom;
	res.is_valid = true;

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




nv_t::nv_t(common_duration_t d) {
	from_duration(d_t {d});
}
nv_t::nv_t(const nv_t::mn& mn) {
	m_ = mn.m;
	n_ = mn.n;
}
nv_t::nv_t(double d) {  // A duration spanning this many whole notes.
	from_duration(d_t {d});
}
nv_t::nv_t(const d_t& d) {  // std::abort() if no m,n-form exists
	from_duration(d);
}

void nv_t::from_duration(const d_t& d) {
	if (!d.singlet_exists()) { std::abort(); }
	auto mn = d.to_mn();
	m_ = mn.m;
	n_ = mn.n;
}





