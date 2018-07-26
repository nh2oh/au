#pragma once

//-----------------------------------------------------------------------------
// The scd_t class
//
// An scd_t represents a scale degree of some unspecified scale.  It is a
// representation of an absolute position within a scale.  scd_t is 
// nothing more than a thin wrapper around int.  It can be implictly
// constructed from a literal int, but only converted back explictly.  
//
// scd_t*scd_t and scd_t/scd_t have no meaning and are not defined.  
//
// 
//

class scd_t {
public:
	explicit scd_t() = default;
	scd_t(int);

	//int to_int() const;
	explicit operator double() const;
	explicit operator int() const;

	scd_t& operator++(); // prefix
	scd_t operator++ (int); // postfix
	friend bool operator==(scd_t const&, scd_t const&);
	friend bool operator<(scd_t const&, scd_t const&);
	friend bool operator>(scd_t const&, scd_t const&);
	friend bool operator>=(scd_t const&, scd_t const&);
	friend bool operator<=(scd_t const&, scd_t const&);
	friend scd_t operator+(scd_t const&, scd_t const&);
	friend scd_t operator-(scd_t const&, scd_t const&);
private:
	int m_scd {0};
};


//-----------------------------------------------------------------------------
// The octn_t class
//
// An octn_t represents an absolute "octave _number_" on some unspecified
// scale.  Like scd_t, it is a measure of absolute position.  Since scales are
// free to define the "octave" any way they please, an octn_t in general 
// has no relationship with frequency (see oct_t) or frequency intervals.  
//
// octn_t's can only be constructed from int explictlyt.  They can only be
// converted to int (must be explicit).  +,-,*,/ with octn_t and all other 
// types is undefined.  
//
//

class octn_t {
public:
	explicit octn_t() = default;
	explicit octn_t(int);

	std::string print() const;

	explicit operator double() const;
	explicit operator int() const;

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
// An rscdoctn_t represents an scd_t as a "reduced scale degree" (which is 
// implemented an an scd_t), a corresponding octave number (an octn_t), and
// the number of scale degrees per octave (int) on the scale.  If the number
// of scd's per octave is known, an scd_t can be converted to an rscdoctn_t.  
//
// Although a general scale is so unconstrained that "number of scale degrees
// per octave" may not be a useful concept, it is useful for most normal
// scales.  Hence the concept of the "reduced scd" exists external to and 
// independently of a scale.  
//
// For some reason, there is a conversion poerator to scd_t, but scd_t has no
// constructor from rscdoctn_t.  
//
// + and - produce the same result as + and - on the corresponding pair of
// scd_t's.  Two rscdoctn_t's are == if they convert to the same scd_t.  
//


class rscdoctn_t {
public:
	explicit rscdoctn_t() = default;
	explicit rscdoctn_t(int, octn_t, int);
	explicit rscdoctn_t(scd_t, octn_t, int);
	explicit rscdoctn_t(scd_t, int);

	int rscd_to_int() const;

	explicit operator scd_t() const;
	explicit operator octn_t() const;

	friend rscdoctn_t operator+(rscdoctn_t const&, rscdoctn_t const&);
	friend rscdoctn_t operator-(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator==(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator<(rscdoctn_t const&, rscdoctn_t const&);
	friend bool operator>(rscdoctn_t const&, rscdoctn_t const&);
private:
	scd_t m_rscd {0};
	octn_t m_octn {0};
	int m_n {1};
};

