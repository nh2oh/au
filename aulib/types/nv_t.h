#pragma once
#include <string>
#include <vector>

//
// Notes
// Combining nv_t's:
// a = b + b
//   a.base() = 2*b.base() => a.m_m = b.m_m-1
//   a.ndot() = b.ndot() => a.m_n = b.m_n
//   Ex:  (1/4).. + (1/4).. = (1/4 + 1/8 + 1/16) + (1/4 + 1/8 + 1/16)
//                          = 1/2 + 1/4 + 1/8
//                          = (1/2)..
//
// a = b + b + b, b.ndot() > 0
//   Ex:  (1/4). + (1/4). + (1/4). = (1/2). + (1/4). = (1/2 + 1/4) + (1/4 + 1/8)
//                                 = ((1/2).).
//        The overall nv() should == 1/1 + 1/8 = 9/8
//        Is there any x st: x + x/2 = 9/8 ?  x = 3/4, but this can not
//        be represented as 1/2^m, and is therefore not a valid bv.  
//        Is there any x st: x + x/2 + x/4 + ... = 9/8 ?
//        x*(1 + 1/2 + 1/4 + ... + 1/32) = x*(63/32) => x = 4/7, but 4/7
//        is not a valid bv either.  
//
// a = b + b + b, b.ndot() == 0
//   a.base = 2*b.base()
//   a.ndot = 1
//   a = (b+b).
//
// SUMMARY:  Not possible to represent a sum of 3 identical terms.  
//
//
// a = b + b + b + b + b, b.ndot()==1
//   a.base() = 4*b.base() => a.m_m = b.m_m-2
//   a.ndot() = 3 => a.m_n = 3
//   Ex:  (1/8). + (1/8). + (1/8). + (1/8). + (1/8). = (1/4).+(1/4).+(1/8). = (1/2).+(1/8).
//                          = 1/4 + 1/8 + 1/4 + 1/8 + 1/8 + 1/16
//                          = 1/2 + 1/4 + 1/8 + 1/16
//                          = (1/2)...
//
// a = b + b + b + b + b, b.ndot()==0
//   Ex:  (1/4) + (1/4) + (1/4) + (1/4) + (1/4) = (1/2)+(1/2)+(1/4) = (1/1)+(1/4)
//                          = 1/4 + 1/4 + 1/4 + 1/4 + 1/8 + 1/8
//                          = (1/2 + 1/4 + 1/8) + (1/4 + 1/8)
//                          = (1/2).. + (1/4).
//                          No valid bv...
//


// Represents a note value as a function of two parameters m and n (m_m
// and m_n).  
// Internally, note-value = (1/(2^m))*(2-1/(2^n))
// This calculation is hidden from the user.  The private function nv()
// implements this calculation.  The value returned by nv() is used to
// define the operators ==, !=, <, >, ... etc
//
// The value 1/(2^m) is called the "base value" (herein abbreviated "bv").  
// The class hides the parameter m from users; users may set and query the
// bv of a given nv_t, but not m.  This is done to allow possible extension
// in the future to unusual functional forms for the bv, for example,
// bv = 1/(3^m).  
// The range of allowed bv's is set by the public static constants min_bv
// and max_bv, which are calculated from the private static constants 
// min_bv_exponent, max_bv_exponent.  Note again that the representation
// of a bv as 1/2^some-exponent is hidden.  
// A user can obtain an nv_t of the same base value as some preexisting
// nv_t by calling the public base() method, which returns an nv_t.  
// Note that the bv is not returned as a double, again, to conceal the
// exact functional form.  The private bv() returns the bv as a double.  
//
// The parameter n (ndots) is the number of dots modifying the base note
// value.  This parameter _is_ exposed to the user, but the dependence of
// the internal note-value on n (ie, 2 - 1/(2^n)) is hidden.  The range of
// allowed values of n is 0 to the public static constant max_ndots.  
//
// An nv_t can be constructed by passing a bv (double) and ndot pair to the
// constructor; ex, nv_t {0.25,0} to create an undotted quarter-note.  Note
// that the constructor rounds the bv passed in by the caller to the nearest
// representable by the function 1/2^m for integer m within the range
// [max_bv_exponent, min_bv_exponent] (note that the min exponent => the max
// bv and the max exponent => the min bv).  
//


class nv_t {
public:
	explicit nv_t();
		// Creates a whole-note:  bv == 1, ndots == 0
	explicit nv_t(double const&,int const& =0);  // base-value (=1/2^m), n-dots
		// The bv argument is quantized.  That is,
		// nv_t(quantize_bv(my_bv),my_ndot).bv() == quantize_bv(my_bv)
		// In general:  nv_t(my_bv,my_ndot).bv() != my_bv

	std::string print() const;
	int ndot() const;
	nv_t base() const;  // The nv_t w/the same value of m_m, but with m_n = 0
	//nv_t split(int) const;
	// somenv.split(n) returns the nv_t for which n*nv() == somenv.nv()
	//std::vector<nv_t> explode() const;

	nv_t& set_base(double const&);
	nv_t& set_dots(int const&);
	nv_t& add_dot(int const& =1);
	nv_t& rm_dots(int const&);
	nv_t& rm_dots();  // Remove all the dots, if any

	friend double operator/(nv_t const&, nv_t const&);
		// Division is needed to get the nv without breaking encapsulation.  
		// The nv() can be extracted from an nv_t by dividing the nv_t by
		// a whole note, the "unit nv_t" nv_t {1,0}.  A whole note may also
		// be obtained by calling the constructor with no arguments, ie,
		// nv_t() == nv_t {1,0} == a whole note.  
	friend bool operator==(nv_t const&, nv_t const&);
	friend bool operator!=(nv_t const&, nv_t const&);
	friend bool operator<(nv_t const&, nv_t const&);
	friend bool operator>(nv_t const&, nv_t const&);
	friend bool operator<=(nv_t const&, nv_t const&);
	friend bool operator>=(nv_t const&, nv_t const&);

	static bool bv_isvalid(double const&);
		// Checks to see if the argument is outside the range [min_bv,max_bv]
	static bool ndots_isvalid(int const&);
		// Checks to see if the argument is outside the range [0,max_ndots]
	static double quantize_bv(double const&);
		// Returns the quantized bv; does _not_ check for the validity of the
		// argument (ie, does not run bv_isvalid()).  
		// "Quantized bv" means the member of the set:
		// {2^min_exp, 2^(min_exp+1), ..., 2^0, 2^1, ..., 2^max_exp}
		// closest in value to the argument.  min_exp, max_exp are defined by
		// the static members min_bv, max_bv.  

	static const double max_bv;
	static const double min_bv;
	static const int max_ndots;
private:
	// bv() is similar to the public getter base(), but returns a double,
	// not an nv_t
	double nv() const;  // == (1/(2^m))*(2-1/(2^n))
	double bv() const;  // == 1/(2^m)
	static int bv_exponent(double const&);

	int m_m {0};  // exponent of the base value
	int m_n {0};  // number of dots

	static const int min_bv_exponent;
	static const int max_bv_exponent;
};

//std::vector<nv_t> nvsum(std::vector<nv_t>);
//std::vector<nv_t> nvsum(std::vector<nv_t>,std::vector<nv_t>);
//std::vector<nv_t> nvsum_finalize(std::vector<nv_t>);
/*
class common_notevalue {
	common_notevalue(nv::

namespace nv {
	// Common note values.  
	inline const nv_t dw {2.0,0};
	inline const nv_t dwd {2.0,1}; inline const nv_t dwdd {2.0,2};
	inline const nv_t w {1,0};
	inline const nv_t wd {1,1}; inline const nv_t wdd {1,2};
	inline const nv_t h {0.5,0};
	inline const nv_t hd {0.5,1}; inline const nv_t hdd {0.5,2};
	inline const nv_t q {0.25,0};
	inline const nv_t qd {0.25,1}; inline const nv_t qdd {0.25,2};
	inline const nv_t e {0.125,0};
	inline const nv_t ed {0.125,1}; inline const nv_t edd {0.125,2};
	inline const nv_t s {0.0625,0};
	inline const nv_t sd {0.0625,1}; inline const nv_t sdd {0.0625,2};
	inline const nv_t t {0.03125,0};
	inline const nv_t td {0.03125,1}; inline const nv_t tdd {0.03125,2};
};
	

class nv {
public:
	static const nv_t dw;
	static const nv_t dwd; static const nv_t dwdd;
	static const nv_t w;
	static const nv_t wd; static const nv_t wdd;
	static const nv_t h;
	static const nv_t hd; static const nv_t hdd;
	static const nv_t q;
	static const nv_t qd; static const nv_t qdd;
	static const nv_t e;
	static const nv_t ed; static const nv_t edd;
	static const nv_t s;
	static const nv_t sd; static const nv_t sdd;
	static const nv_t t;
	static const nv_t td; static const nv_t tdd;
};
*/





//
// The nv() of nv_t can be represented as a/2^b, where:
// a = sum(i=0,i=n,2^i) = 2^(n+1)-1
// b = m+n
// Ex:  (1/4).. = 7/(2^4)
//
// a will belong to {1,3,7,15,31,63,127,...+ = 2^(n+1) - 1
// Thus, given an a, n can be calculated as:
// auto n = static_cast<int>(std::log2(a-1)-1)
// If the struct was created from a valid nv_t, std::log2(a-1) will be
// an integer and there should be no need to check.  
// Once n is found, m is m = b-n.  
//
// Adding nv_t's 
// For any two nv_t's 1,2 w/ a1,b1, a2,b2 (chosen such that b2>=b1, ie, b2
// is the note w/ the shorter duration) their sum, nv_t 3 with a3,b3 is:
// a3 = a1*2^(b2-b1) + a2
// b3 = 2^b2
// Here, a will _not_ necessarily belong to {1,3,7,...,2^(n+1)-1}.  If this
// is the case, the sum can not be represented as a single nv_t (ie, as a
// function of positive integers m, n), and will have to be represented as 
// a doublet or some sort of tuplet.  
//
//



class tuplet_t {
public:
	explicit tuplet_t();  // Default => single whole note
	explicit tuplet_t(nv_t const&);
	explicit tuplet_t(int const&, nv_t const&);
	explicit tuplet_t(nv_t const&, nv_t const&);
	explicit tuplet_t(nv_t const&, nv_t const&,  nv_t const&);
	explicit tuplet_t(std::vector<nv_t> const&);

	bool singlet_exists() const;
private:
	struct nvt_ab {
		int a{0};
		int b{0};
	};
	struct nvt_mn {
		int m {0};
		int n {0};
	};

	nvt_mn nvt_to_mn(nv_t const&) const;
	nvt_ab mn2ab(nvt_mn const&) const;

	// Equiv nv_t.nv() == a/2^b
	// Defaults equate to a single whole note (n=0,m=0)
	int m_a {1};  // 1 => 0 dots
	int m_b {0};  // a==1,b==0 => m ==0
	// T m_whatever
	// Some other member recording how this should be displayed; possibly
	// based on which constructor was called.  
};

