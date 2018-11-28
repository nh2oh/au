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
// of cent's" values are allowed.  
//
// This class is essentially just a thin wrapper around an int, but it defines special 
// arithmetic with frq_t:  
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
	explicit cent_t(oct_t);  // Number-of-octaves, not "absolute" octave-number...

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
	cent_t operator*=(double);
	cent_t operator/=(double);
private:
	int m_cents {0};
};
cent_t operator""_cent(unsigned long long);

cent_t operator/(cent_t, double);
cent_t operator*(cent_t, double);
cent_t operator*(double, cent_t);
cent_t operator+(cent_t, const cent_t&);
cent_t operator-(cent_t, const cent_t&);

// Deleted operators
auto operator+(const cent_t&, const frq_t&)=delete;
auto operator-(const cent_t&, const frq_t&)=delete;

double operator*(cent_t, cent_t) = delete;

template <typename Tnum, typename Tresult>
Tresult operator/(Tnum, cent_t)=delete;  // Involves 1/cent_t





//-----------------------------------------------------------------------------
// The oct_t class
// Exactly like cents, but represents an interval between two frequencies in 
// units of octaves, rather than cents.  
//
// Very important:  oct_t represents a frq interval in terms of cents/1200: it
// does _not_ represent the "absolute" octave number of a scale (ie, a range of
// frqs), which i call octn_t.  The latter indicates some sort of absolute 
// position in frq space relative to some standard set of reference frqs defined
// to belong to the "0 octave" (defining this set is the job of a scale).  The
// former is just a mathematical operation defined on frq_t's.  A scale is 
// allowed to do crazy things, including span more than one octave:  In this
// case, several frq_t's may have the same octn_t, but at the same time be 
// > 1 oct_t apart.  
//
// A possibly valid alternative to declaring oct_t its own class is to
// introduce a "units" field in cent_t, so that a cent_t can have units of
// "cents," "milicents," "kilocents," "octaves," etc.  
//
// Since an oct_t spontaniously converts to a cent_t, no conversion or
// heterogeneous +,-,etc operators really need to be defined.  
//


// 
// It'd be good if oct_t could be put completely in terms of cent_t
//


class oct_t {
public:
	explicit oct_t() = default;
	explicit oct_t(cent_t);
	explicit oct_t(frq_t, frq_t);  // Interpret as a frq_t ratio
	explicit oct_t(double);  // Interpret as a number-of-octaves

	double to_double() const;
private:
	double m_oct {0};
};


