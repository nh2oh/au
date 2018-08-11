#pragma once
#include <string>

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


// TODO:  Make static
/*namespace nv {
	// Common note values.  
	// TODO:  Make static
	nv_t dw {2,0}; nv_t dwd {2,1}; nv_t dwdd {2,2};
	nv_t w {1,0}; nv_t wd {1,1}; nv_t wdd {1,2};
	nv_t h {0.5,0}; nv_t hd {0.5,1}; nv_t hdd {0.5,2};
	nv_t q {0.25,0}; nv_t qd {0.25,1}; nv_t qdd {0.25,2};
	nv_t e {0.125,0}; nv_t ed {0.125,1}; nv_t edd {0.125,2};
	nv_t s {0.0625,0}; nv_t sd {0.0625,1}; nv_t sdd {0.0625,2};
	nv_t t {0.03125,0}; nv_t td {0.03125,1}; nv_t tdd {0.03125,2};
};*/

// Adding and subtracting nv's does not make sense:  There is no nv
// that can represent the span of, say, (q + q.).  Although /some/ 
// "sums" may correspond to the spans of single nv's, not all do.  
// What we're really saying is "the amount of time occupied by 'this sum'
// is equal to "the amount of time occupied by 'that sum'."  We are
// implictly converting to time.  












