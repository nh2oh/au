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

struct nv_str_helper {
	bool is_valid {false};

	// Some kind of helpful error message (if is_valid == false) or possibly
	// some other helper message if the user has entered something crazy or
	// unusual that parses as a valid nv.  
	std::string msg {};

	// Use to indicate some sort of unusual condition where is_valid==true but
	// we nonetheless want to alert the user about something.  
	int flags {0};

	// Created from the components of the parsed input string.  Things like
	// leading and trailing spaces are stripped.  
	std::string str_clean {};
	
	note_value nv {};
};

struct nvset_str_helper {
	bool is_valid {false};

	// Some kind of helpful error message (if is_valid == false) or possibly
	// some other helper message if the user has entered something crazy or
	// unusual that parses as a valid nv.  
	std::string msg {};

	// Use to indicate some sort of unusual condition where is_valid==true but
	// we nonetheless want to alert the user about something.  
	int flags {0};

	// Created from the components of the parsed input string.  Things like
	// leading and trailing spaces are stripped.  
	std::string str_clean {};

	std::vector<note_value> nvset {};
};

nv_str_helper validate_nv_str(std::string const&);
nvset_str_helper validate_nvset_str(std::string const&);
nvset_str_helper validate_nvset(std::vector<note_value> const&);

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





