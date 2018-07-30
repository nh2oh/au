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

class rscdoctn_t;
class scd_t {
public:
	explicit scd_t() = default;
	scd_t(int);
	explicit scd_t(rscdoctn_t);

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
	explicit octn_t() = default;
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
// An rscdoctn_t tags an scd_t with a number (m_n) indicating the number of
// scds per octave on the scale to which the scd belongs.  This way, an 
// rscdoctn_t can be converted to a reduced scd and an octave number.  
//
// Although a general scale is so unconstrained that "number of scale degrees
// per octave" may not be a useful concept, it is useful for most normal
// scales.  Hence the concept of the "reduced scd" exists external to and 
// independently of a scale.  
//
// + and - produce the same result as + and - on the corresponding pair of
// scd_t's.  Two rscdoctn_t's are == if they convert to the same scd_t.  
//
// Internal conversions from the rscd,octn,n representation to the scd 
// representation require a lot of explicit casts and depend on subtle and 
// probably nonsense behavior of some of the scd_t operators.  It is tempting
// to just store the "scd" m_scd as a double and downcast to int in the 
// rare case the value needs to come out.  
//

class rscdoctn_t {
public:
	explicit rscdoctn_t() = default;
	explicit rscdoctn_t(scd_t, octn_t, int); // arg3 is num scds/octave
	explicit rscdoctn_t(scd_t, int);  // arg2 is num scds/octave

	scd_t to_rscd() const;
	scd_t to_scd() const;
	octn_t to_octn() const;
	std::string print() const;

	rscdoctn_t& operator+=(rscdoctn_t const&);
	rscdoctn_t& operator-=(rscdoctn_t const&);

	// ==, <, > need to be able to check m_n
	friend bool operator==(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator<(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator>(rscdoctn_t const&, rscdoctn_t const&);
private:
	int m_n {1};
	scd_t m_scd {0};
};

rscdoctn_t operator+(rscdoctn_t const&, rscdoctn_t const&);
rscdoctn_t operator-(rscdoctn_t const&, rscdoctn_t const&);


