#pragma once
#include <string> // for declaring the print() members

//-----------------------------------------------------------------------------
// The scd_t class
//
// An scd_t represents a scale degree of some unspecified scale.  It is a
// representation of an absolute position within a scale.  scd_t is 
// nothing more than a thin wrapper around int.  It can be implictly
// constructed from a literal int, but only converted back explictly.  
//
// scd_t*scd_t has no meaning and is not defined.  
// scd_t/scd_t, scd_t/double and double/scd_t all internally upconvert the
// internal representation to double before dividing, then return double.  
// This is so no precision is lost; the user has to manually re-cast to 
// scd_t.  
//
// TODO:  Deprecate this nonsense ?
//

class scd_t {
public:
	scd_t() = default;
	explicit scd_t(int);

	std::string print() const;
	int to_int() const;
	double to_double() const;

	scd_t& operator++(); // prefix
	scd_t operator++ (int); // postfix
	scd_t& operator+=(scd_t const&);
	scd_t& operator-=(scd_t const&);
private:
	int m_scd {0};
};

scd_t operator+(scd_t const&, scd_t const&);
scd_t operator-(scd_t const&, scd_t const&);
scd_t operator*(int const&, scd_t const&);
double operator/(scd_t const&, scd_t const&);
double operator/(scd_t const&, double const&);
double operator/(double const&, scd_t const&);

bool operator==(scd_t const&, scd_t const&);
bool operator<(scd_t const&, scd_t const&);
bool operator>(scd_t const&, scd_t const&);
bool operator>=(scd_t const&, scd_t const&);
bool operator<=(scd_t const&, scd_t const&);


//-----------------------------------------------------------------------------
// The octn_t class
//
// An octn_t represents an absolute "octave _number_" on some unspecified
// scale.  Like scd_t, it is a measure of absolute position.  Since scales are
// free to define the "octave" any way they please, an octn_t in general 
// has no relationship with frequency (see oct_t) or frequency intervals.  
//

class octn_t {
public:
	octn_t() = default;
	explicit octn_t(scd_t,int);
	explicit octn_t(int);

	int to_int() const;
	std::string print() const;

	friend bool operator==(octn_t const&, octn_t const&);
	friend bool operator<(octn_t const&, octn_t const&);
	friend bool operator>(octn_t const&, octn_t const&);
	friend bool operator>=(octn_t const&, octn_t const&);
	friend bool operator<=(octn_t const&, octn_t const&);
private:
	int m_octn {0};
};


//-----------------------------------------------------------------------------
// The rscdoctn_t class
//

class rscdoctn_t {
public:
	rscdoctn_t() = default;
	explicit rscdoctn_t(scd_t, int);  // arg2 is num scds/octave

	scd_t to_scd(octn_t) const;
	int to_int() const;
	std::string print() const;

	rscdoctn_t& operator+=(rscdoctn_t const&);
	rscdoctn_t& operator-=(rscdoctn_t const&);

	// ==, <, > need to be able to check m_n
	friend bool operator==(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator<(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator>(rscdoctn_t const&, rscdoctn_t const&);
private:
	int fold(int,int);
	int m_n {1};
	int m_rscd {0};
};

rscdoctn_t operator+(rscdoctn_t const&, rscdoctn_t const&);
rscdoctn_t operator-(rscdoctn_t const&, rscdoctn_t const&);


