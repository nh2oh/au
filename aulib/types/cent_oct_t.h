#pragma once
#include <string>
#include <map>
#include <vector>

class oct_t;
class cent_t;
class frq_t; 
//-----------------------------------------------------------------------------
// The cent_t class
//
// A special numeric type representing frq ratios in "cents.  "
// A "cent" is 1200*std::log2(frqr), where 1200 ~ "cents per octave" and 
// log2(frqr) is the (dimensionless) log2 of a frequency ratio, which is
// the number of octaves separating the two frequencies.  
//
// This class is essentially just a thin wrapper around a double, but it
// defines special arithmetic with frq_t and with itself:
// - Delete's operator*(cent_t, cent_t) (note that in the case of frq_t
//		I decided to declare this meaningless operation as returning a 
//		double).  
// - Defines an implicit constructor from oct_t and a constructor from a frq 
//      pair.  
// - The algs to compose interval common-names are members, as is the 
//		static database of common-names for the small intervals.  These
//      functions do not really have to be implemented as a members.  It
//      hides the common-name database; not sure if this is good or bad...
//
// The class defines + and - operations with frq_t:
// frq_t operator+(frq_t,cent_t) => std::pow(2,(cent_t/1200))*frq_t
// Note that order-of-operations matter!!
// (frq_a + cent_a) + (frq_b + cent_b)		~[a*fa + b*fb]
// is different from:
// (frq_a + cent_b) + (frq_b + cent_a)		~[b*fa + a*fb]
// and:
// (frq_a + frq_b) + (cent_a + cent_b)
//
// The corresponding operator+(cent_t, frq_t), operator-(cent_t,frq_t)
// are both declared deleted.  The frq_t must be on the lhs and the cent_t
// on the rhs.  
// I also define a redundant nonmember helper: frq_t plus_cent(frq_t, cent_t)
//
// It could possibly be useful to have operator/(frq_t,frq_t) return a 
// cent_t.   
//


//
// TODO:  whatever()- > to_acname()
//
class cent_t {
public:
	cent_t() = default;
	explicit cent_t(frq_t, frq_t);  // Interpret as a frq_t ratio
	explicit cent_t(double);  // Interpret as a number-of-cents
	cent_t(oct_t); // Number-of-octaves, not octave-number...
	// Not explicit:  An oct_t will silently convert into a cent_t if needed
	//explicit cent_t(std::string);

	// Getters 
	double to_double() const;
	std::string print(int=0) const;

	// The int argument specifies which name to return if the interval
	// can be named in more than one way.  In this case, the name 
	// returned is that with idx = the int argument.  
	std::string to_fcname(int=0) const;  // full common name
	std::string to_acname(int=0) const; // abbreviated common name
	std::string whatever(int=0) const; // abbreviated common name

private:
	double m_cents {0.0};

	// These vectors are the same size. 
	const static std::map<cent_t,std::vector<std::string>> m_cipmap_cent;
};

cent_t operator""_cent(unsigned long long);

bool operator==(cent_t const&, cent_t const&);
bool operator<(cent_t const&, cent_t const&);
bool operator>(cent_t const&, cent_t const&);
bool operator>=(cent_t const&, cent_t const&);
bool operator<=(cent_t const&, cent_t const&);
cent_t operator+(cent_t const&, cent_t const&);
cent_t operator-(cent_t const&, cent_t const&);
cent_t operator-(cent_t const&);
double operator/(cent_t const&, cent_t const&);
double operator/(double const&, cent_t const&);  // 1/cent_t => loss of type
cent_t operator/(cent_t const&, double const&);
double operator*(cent_t, cent_t) = delete;
cent_t operator*(cent_t const&, double const&);
cent_t operator*(double const&, cent_t const&);

auto operator+(cent_t const&, frq_t const&) = delete;
auto operator-(cent_t const&, frq_t const&) = delete;

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


