#pragma once
#include <string>
#include <optional>
#include <vector>
class ts_t;
class beat_t;
class bar_t;

//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative amount of time spanned by a single note.  Storage is
// in decimal, not "rational" form.  A "quarter note" has m_nv == 0.25, a
// double-whole note has m_nv == 2, etc.  
//
// There is no such thing as a note_value of 0, and there is no such thing as a
// note_value < 0.  Note values are _relative_ (to eachother) durations: (-) and
// 0 -values make no sense.  
//
//
struct nv_base_dots {
	double base_value;
	int ndot;
};

class note_value {
public:
	explicit note_value() = default;
	explicit note_value(double);
	explicit note_value(double,int);  // base_value, ndots
	explicit note_value(ts_t,beat_t);

	//std::optional<nv_base_dots> exact2() const;
	std::optional<int> ndot() const;
	std::optional<double> undot_value() const;
	
	std::string print(int=0) const;
	double to_double() const;

	note_value& operator+=(note_value const&);
	note_value& operator-=(note_value const&);
private:
	double m_nv {1.0};

	std::optional<nv_base_dots> exact() const;
};

namespace notevalueopts {
enum {
	printapproxifnotexact = 1,
	someotheropt = 2,
};
};

note_value operator""_nv(const char *literal_in, size_t length);

note_value operator+(note_value const&, note_value const&);
note_value operator-(note_value const&, note_value const&);
double operator/(note_value const&, note_value const&);
	// how many rhs-notes fit in the span of an lhs-note?

note_value operator*(note_value const&, double const&);
note_value operator*(double const&, note_value const&);
bool operator==(note_value const&, note_value const&);
bool operator!=(note_value const&, note_value const&);
bool operator>(note_value const&, note_value const&);
bool operator<(note_value const&, note_value const&);
bool operator>=(note_value const&, note_value const&);
bool operator<=(note_value const&, note_value const&);



// Note-value user input helper
//
// The purpose of this class is to provide a container to hold std::string
// input entered by a user, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to a note_value.  
//`
//
class nv_uih {
public:
	nv_uih();
	nv_uih(std::string const&);
	// nv_uih(std::string const&, predicates...);  Future...
	// Also possibly some way to set custom messages, flags dfns...

	void update(std::string const&);
	bool is_valid() const;
	int flags() const;
	note_value get() const;
private:
	struct nv_str_parts {
		double num {0.0};
		double denom {0.0};
		int ndots {0.0};
	};
	std::optional<nv_str_parts> parse_nv_str_() const;
		// Parses str_last_; does not set any internal variables (note the const
		// declaration).  Called by the constructor and by update().  

	bool is_valid_ {false};
	std::string msg_ {""};
	int flags_ {0};
	std::string str_last_ {""};
		// The std::string passed to the most recent call to update(), or that
		// passed to the constructor if update() has not yet been called.
		// Allows the object to decide if any change has been made and make a
		// decision if further processing is needed.  

	std::optional<note_value> nv_ {};
		// If str_last_ can be parsed to create a note_value, nv_ is that value.  
		// std::optional is used to prevent imposing some sort of "default" ts
		// on a user of the nv_uih class.  Since the constructor takes a 
		// string, not an nv_t object, there's a chance that the nv_uih is
		// created with an "invalid" input.  This is ok where the argument to
		// the constructor comes from a user.  
};




//struct nv_str_helper {
//	bool is_valid {false};
//
//	// Some kind of helpful error message (if is_valid == false) or possibly
//	// some other helper message if the user has entered something crazy or
//	// unusual that parses as a valid nv.  
//	std::string msg {};
//
//	// Use to indicate some sort of unusual condition where is_valid==true but
//	// we nonetheless want to alert the user about something.  
//	int flags {0};
//
//	// Created from the components of the parsed input string.  Things like
//	// leading and trailing spaces are stripped.  
//	std::string str_clean {};
//	
//	note_value nv {};
//};
//
//struct nvset_str_helper {
//	bool is_valid {false};
//
//	// Some kind of helpful error message (if is_valid == false) or possibly
//	// some other helper message if the user has entered something crazy or
//	// unusual that parses as a valid nv.  
//	std::string msg {};
//
//	// Use to indicate some sort of unusual condition where is_valid==true but
//	// we nonetheless want to alert the user about something.  
//	int flags {0};
//
//	// Created from the components of the parsed input string.  Things like
//	// leading and trailing spaces are stripped.  
//	std::string str_clean {};
//
//	std::vector<note_value> nvset {};
//};
//
//nv_str_helper validate_nv_str(std::string const&);
//nvset_str_helper validate_nvset_str(std::string const&);
//nvset_str_helper validate_nvset(std::vector<note_value> const&);

