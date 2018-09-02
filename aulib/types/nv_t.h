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
	nv_t() = default;
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



class tie_t {
public:
	tie_t()=default;
	explicit tie_t(nv_t const&);
	explicit tie_t(int const&, nv_t const&);
	explicit tie_t(nv_t const&, nv_t const&);
	explicit tie_t(std::vector<nv_t> const&);

	bool singlet_exists() const;
	std::vector<nv_t> to_nvs() const;
private:
	struct nvt_mn {
		// This is essentially the internal representation of an nv_t; a
		// duration d = (1/(2^m))*(2-1/(2^n)) can be represented as a single
		// nv_t iff:
		// m is an integer on (-infty,infty) and
		// n is an integer on [0,infty).  
		int m {0};
		int n {0};
	};
	struct nvt_ab {
		// An alternative, more general representation of a duration.  _Any_ 
		// duration d that can be represented an an nv_t _or_ as a sum of 
		// nv_t's can be represented as d = a/(2^b) (this is a consequence 
		// of the definition of an nv_t as a combination of integers m,n with 
		// n>=0).  However, not all durations d = a/(2^b) can be represented
		// by a single nv_t.  For a duration d to have a representation as a
		// single nv_t, 
		// 1)  a must be a mersenne number:
		//   a == {1,3,7,15,31,63,127,..., 2^(N+1)-1, ... infty}
		//   This is the seq generated by sum(2^i,i=0,N) for all integer N >=0
		//   => a must be an integer on [1,infty)
		// 2)  b must be an integer on (-infty,infty)
		//
		// If both (1) and (2) are true, d=a/(2^b) can be represented as a single
		// nv_t with m,n such that:
		// a = 2^(n+1) - 1  <=>  n = log2(a+1) - 1
		// b = n + m        <=>  m = b - log2(a+1) + 1
		//
		// For any pair of durations d1 = a1/2^b1, d2 = a2/2^b2, their sum 
		// d3 = d1 + d2 = a3/2^b3 with
		// a3 = a1*2^(b2-b1) + a2
		// b3 = b2
		// Or alternatively (and equivalently)
		// a3 = a1 + a2/(2^(b2-b1))
		// b3 = b1
		//
		// d3 does not nec. have a representation as a single nv_t, even if both
		// d1 and d2 did.  To check if d3 has an nv_t representation, it is only
		// necessary to check if a3 is a mersenne number (condition (2) for b3 
		// will always hold if d1, d2 had nv_t representations).  
		//
		int a{0};
		int b{0};
	};
	nvt_ab add_ab(nvt_ab, nvt_ab) const;


	// Default => 0 ... not a valid nv_t
	nvt_ab m_ab {0,0};
	static const int max_nplet;
	
	nv_t mn2nvt(tie_t::nvt_mn const&) const;
	nvt_mn nvt2mn(nv_t const&) const;
	nvt_ab nvt2ab(nv_t const&) const;
	nvt_ab mn2ab(nvt_mn const&) const;
};

// Some other member recording how this should be displayed; possibly
// based on which constructor was called.  Or maybe not: don't nec.
// want to litter fundamental datastructures w/ display logic...
// a "tie" is a concept that exists independently of how some arbitrary
// person want's it displayed...



enum class common_duration_t {
	// The final digit is the number of dots, the leading digits are the
	// exponent of the base-value (m).  
	ow = 8, owd = 81, owdd = 82, owddd = 83,
	qw = 20, qwd = 21, qwdd = 22, qwddd = 23,
	dw = 10, dwd = 11, wdd = 12, dwddd = 13,
	w = 00, wd = 01, wdd = 02, wddd = 03,
	h = -10, hd = -11, hdd = -12, hddd = -13,
	q = -20, qd = -21, qdd = -22, qddd = -23,
	e = -30, ed = -31, edd = -32, eddd = -33,
	sx = -40, sxd = -41, sxdd = -42, sxddd = -43,
	t = -50, td = -51, tdd = -52, tddd = -53,
	sf = -60, sfd = -61, dfdd = -62, sfddd = -63,
	ote = -70, oted = -71, otedd = -72, oteddd = -73,
	tfs = -80, tfsd = -81, tfsdd = -82, tfsddd = -83,
	ftw = -90, ftwd = -91, ftwdd = -92, ftwddd = -93,
	ttwf = -100, ttwfd = -101, ttwfdd = -102, ttwfddd = -103, 
	twfe = -110, twfe = -111, twfe = -112, twfe = -113,
	fnsx = -120, fnsx = -121, fnsx = -122, fnsx = -123
};


// Implements duration math
// Contained within the rest_t, nv_t, de_t,...
// Just represents time-span.  Could mean a group of tied notes, could 
// represent a sequence of non-tied notes, could represent a rest...  

class d_t {
public:
	d_t()=default;
	explicit d_t(common_duration_t);

	d_t& operator+=(const d_t&);
	d_t& operator-=(const d_t&);
	d_t& operator*=(const int&);
	d_t& operator/=(const int&);

private:
	int m_a {0};
	int m_b {0};
};

d_t operator-(const d_t&, const d_t&);
d_t operator+(const d_t&, const d_t&);
double operator/(const d_t&, const d_t&);
d_t operator/(const d_t&, const int&);
d_t operator*(const int&, const d_t&);
d_t operator*(const d_t&, const int&);




