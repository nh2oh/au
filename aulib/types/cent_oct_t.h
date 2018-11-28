#pragma once
#include <string>

class oct_t; 
class frq_t; 
//
// The cent_t class
//
// A special numeric type representing frq ratios in "cents."  A "cent" is 1200*std::log2(frqr),
// where 1200 ~ "cents per octave" and log2(frqr) is the (dimensionless) log2 of a frequency 
// ratio, which is the number of octaves separating the two frequencies.  Only integral "number
// of cent's" values are allowed.  For this reason i do not declare arithmetic operators with
// doubles; only ints.  The only point of such operators would be to effect the operation in double-
// precision and then round, as opposed to the usual c++ way of truncating then carrying out an int
// operation, but this behavior is unexpected from the perspective of most s++ programmers.  You 
// want double-precision math, do it yourself:  static_cast<double>(cent_t.to_int()) ...
// Note that there is no ctor from double to hammer home this point; the user has to explictly 
// cast back to int.  
//
// cent_t has special arithmetic with frq_t:  
// frq_t operator+(frq_t,cent_t) => std::pow(2,(cent_t/1200))*frq_t
// Note that order-of-operations matter!!
// (frq_a + cent_a) + (frq_b + cent_b)		~[a*fa + b*fb]
// is different from:
// (frq_a + cent_b) + (frq_b + cent_a)		~[b*fa + a*fb]
// and:
// (frq_a + frq_b) + (cent_a + cent_b)
//
// The corresponding operator+(cent_t, frq_t), operator-(cent_t,frq_t) are both declared 
// deleted.  The frq_t must be on the lhs and the cent_t on the rhs.  All operators involving
// a 1/cent_t are also deleted, as is cent_t*cent_t.  
//
// It could possibly be useful to have operator/(frq_t,frq_t) return a 
// cent_t.   
//
//
class cent_t {
public:
	cent_t() = default;
	explicit cent_t(const frq_t&, const frq_t&);  // Interpret as a frq_t ratio arg1/arg2
	explicit cent_t(int);  // Interpret as a number-of-cents
	cent_t(oct_t);  // Number-of-octaves, not "absolute" "octave-number"
		// Note:  Silent conversion!

	// Getters 
	double to_int() const;
	std::string print() const;

	bool operator==(const cent_t&) const;
	bool operator!=(const cent_t&) const;
	bool operator<(const cent_t&) const;
	bool operator>(const cent_t&) const;
	bool operator<=(const cent_t&) const;
	bool operator>=(const cent_t&) const;
	cent_t operator+=(const cent_t&);
	cent_t operator-=(const cent_t&);
	cent_t operator*=(const cent_t&)=delete;
	cent_t operator/=(const cent_t&)=delete;
	cent_t operator*=(int);
	cent_t operator/=(int);
private:
	int m_cents {0};
};
cent_t operator""_cent(unsigned long long);

cent_t operator/(cent_t, int);
cent_t operator*(cent_t, int);
cent_t operator*(int, cent_t);
cent_t operator+(cent_t, const cent_t&);
cent_t operator-(cent_t, const cent_t&);

// Deleted operators
void operator+(const cent_t&, const frq_t&)=delete;
void operator-(const cent_t&, const frq_t&)=delete;
void operator*(cent_t, cent_t)=delete;

template <typename Tnum, typename Tresult>
Tresult operator/(Tnum, cent_t)=delete;  // Involves 1/cent_t


//
// The oct_t class
// Exactly like cents, but represents an interval between two frequencies in "units" of 
// octaves, rather than cents.  Internally represented as an integer number-of-cents.  This
// makes it easy to convert reversibly to and from cents.  
//
// Very important:  oct_t represents a frq interval in terms of cents/1200.  It does _not_
// represent the "absolute" "octave number" defined by a scale to denote a range of frqs,
// which i call octn_t.  The latter refers to an absolute range in frq space which a scale 
// is allowed to define in any way it likes; in particular, a scale is free to assign 
// frq_t's f1 and f2 where f2 = 4*f1 (or any other factor) to the same octn_t.  However, 
// f1 and f2 thus defined are always 2 oct_t's apart.  
//
// The cent_t class contains a single-argument non-explicit ctor from oct_t, so that an oct_t
// will spontaneously convert to a cent_t.  All mathematical operations between oct_t and 
// cent_t rely on this silent conversion.  
//

class oct_t {
public:
	oct_t() = default;
	explicit oct_t(const cent_t&);
	explicit oct_t(frq_t, frq_t);  // Interpret as a frq_t ratio
	explicit oct_t(double);  // Interpret as a number-of-octs
	explicit oct_t(int);  // Interpret as a number-of-octs

	std::string print() const;
	double to_double() const;
	double to_int() const;

	explicit operator cent_t();

	bool operator==(const oct_t&) const;
	bool operator!=(const oct_t&) const;
	bool operator<(const oct_t&) const;
	bool operator>(const oct_t&) const;
	bool operator<=(const oct_t&) const;
	bool operator>=(const oct_t&) const;
	oct_t operator+=(const oct_t&);
	oct_t operator-=(const oct_t&);
	oct_t operator*=(const oct_t&)=delete;
	oct_t operator/=(const oct_t&)=delete;
	oct_t operator*=(double);
	oct_t operator/=(double);
private:
	int m_num_cents {0};
};
oct_t operator""_octs(unsigned long long);

oct_t operator/(oct_t, double);
oct_t operator*(oct_t, double);
oct_t operator*(double, oct_t);
oct_t operator+(oct_t, const oct_t&);
oct_t operator-(oct_t, const oct_t&);

// Deleted operators
void operator+(const oct_t&, const frq_t&)=delete;
void operator-(const oct_t&, const frq_t&)=delete;
void operator*(oct_t, oct_t)=delete;

template <typename Tnum, typename Tresult>
Tresult operator/(Tnum, oct_t)=delete;  // Involves 1/oct_t

