#pragma once
#include <string> // for declaring the print() members

//-----------------------------------------------------------------------------
// The scd_t class
//
// An scd_t represents a scale degree of some unspecified scale.  It is a representation 
// of an absolute position within a scale.  scd_t is nothing more than a thin wrapper around 
// int and can be implictly converted to and from an int.  
//
//

class scd_t {
public:
	scd_t()=default;
	scd_t(int i);
	operator int() const;

	scd_t& operator++();  // prefix
	scd_t operator++(int);  // postfix
	scd_t& operator--();  // prefix
	scd_t operator--(int);  // postfix
	scd_t& operator+=(const int&);
	scd_t& operator-=(const int&);
	scd_t& operator+=(const scd_t&);
	scd_t& operator-=(const scd_t&);

	bool operator==(const scd_t&) const;
	bool operator<(const scd_t&) const;
	bool operator>(const scd_t&) const;
	bool operator>=(const scd_t&) const;
	bool operator<=(const scd_t&) const;
private:
	int m_value {0};
};

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

