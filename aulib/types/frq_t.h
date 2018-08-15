#pragma once
#include "cent_oct_t.h"
#include <string>


//
// The frq_t class
//
// Represents a frequency in units of Hz, kHz, MHz, ...
// As frequencies < 0 Hz make no physical sense, a frq_t only represents
// values >= 0.  The constructor interprets the value argument v as 
// v = fmax(v, 0.0). So, 
//
// frq_t myfrq {-23.123, Hz};
// is interpreted as: 
// frq_t myfrq {23.123, Hz};
//
// This is also enforced by any opperators which might create a frequency
// < 0.  For example, if the rhs argument is > the lhs argument,
// operator-(frq_t,frq_t) returns a frq_t representing 0 Hz (the same 
// behavior as when the constructor is called with a value < 0).  
//
// Internally, the value is stored as a double in "units" of Hz no matter
// how the object was constructed.  For example, if a frq_t object is 
// constructed like: 
// frq_t myfrq {100, kHz};
// the private variable myfrq.m_frq == 0.1, but myfrq.m_unit => kHz.  
// This is done so frq_t objects can be compared, added, divided, etc without
// having to invoke mathematical operations involving the unit system.  The
// value of m_unit is relevant only for print() and for interpreting the
// value passed to the constructor.  
//
// frq_t/frq_t, returns a double, not a frq_t.  Thus, the numerical 
// value of a frq_t can be extracted with operator/(frq_t).  To convert a 
// frq_t object myfrq to, for example, a "number of kHz:"  
// 
// auto number_of_kHz = myfrq/frq_t{1,kHz};
//
// Undefined operations include:
// frq_t*frq_t => a (frq_t)^2 which is not a type i have defined
// 1/frq_t => a (frq_t)^-1 ... as above
// frq_t +,- double,int, etc
//
//
class frq_t {
public:
	enum class unit : int {
		Hz=0,
		kHz=3,
		MHz=6,
		GHz=9
	};

	explicit frq_t() = default;
	explicit frq_t(double const&, unit const& =frq_t::unit::Hz);

	std::string print(int =3) const;

	double operator/(frq_t const&) const;
	frq_t& operator/=(double const&);
	frq_t& operator*=(double const&);
	frq_t& operator+=(frq_t const&);
	frq_t& operator-=(frq_t const&);
	bool operator==(frq_t const&) const;
	bool operator>(frq_t const&) const;

private:
	double m_frq {1.0};  // Units are always Hz
	int m_unit {0}; // 1=>Hz, 1000=>kHz, 10^6=>MHz, ...
	
	static const int m_prec;
};

frq_t operator+(frq_t, frq_t const&);
frq_t operator-(frq_t, frq_t const&);
frq_t operator*(double const&, frq_t);
frq_t operator*(frq_t, double const&);
bool operator<(frq_t const&, frq_t const&);
bool operator!=(frq_t const&, frq_t const&);
bool operator>=(frq_t const&, frq_t const&);
bool operator<=(frq_t const&, frq_t const&);

frq_t operator+(frq_t, cent_t const&);
frq_t operator-(frq_t, cent_t const&);

