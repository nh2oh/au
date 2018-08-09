#pragma once
#include <string> // for validate_bpm_str()

//-----------------------------------------------------------------------------
// Class beat_t
// Represents some number of beats

class beat_t {
public:
	explicit beat_t() = default;
	explicit beat_t(int);
	explicit beat_t(double);

	double to_double() const;

	beat_t& operator+=(beat_t const&);
	beat_t& operator-=(beat_t const&);
	
private:
	double m_beats {0.0};
};

beat_t operator+(beat_t const&, beat_t const&);
beat_t operator-(beat_t const&, beat_t const&);
beat_t operator-(beat_t const&);
double operator/(beat_t const&, beat_t const&);
double operator/(double const&, beat_t const&);
beat_t operator/(beat_t const&, double const&);
beat_t operator*(double const&, beat_t const&);
beat_t operator*(beat_t const&, double const&);
bool operator==(beat_t const&, beat_t const&);
bool operator!=(beat_t const&, beat_t const&);
bool operator>(beat_t const&, beat_t const&);
bool operator<(beat_t const&, beat_t const&);
bool operator>=(beat_t const&, beat_t const&);
bool operator<=(beat_t const&, beat_t const&);

//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars
class bar_t {
public:
	explicit bar_t() = default;
	explicit bar_t(double);

	double to_double() const;

	bar_t& operator+=(bar_t const&);
	bar_t& operator-=(bar_t const&);
private:
	double m_bars {0.0};
};

bar_t operator+(bar_t const&, bar_t const&);
bar_t operator-(bar_t const&, bar_t const&);
double operator/(bar_t const&, bar_t const&);
double operator/(double const&, bar_t const&);
bar_t operator/(bar_t const&, double const&);
bar_t operator*(bar_t const&, double const&);
bar_t operator*(double const&, bar_t const&);
bool operator==(bar_t const&, bar_t const&);
bool operator!=(bar_t const&, bar_t const&);
bool operator>(bar_t const&, bar_t const&);
bool operator<(bar_t const&, bar_t const&);
bool operator>=(bar_t const&, bar_t const&);
bool operator<=(bar_t const&, bar_t const&);












struct bpm_str_helper {
	bool is_valid {false};

	// Some kind of helpful error message (if is_valid == false) or possibly
	// some other helper message if the user has entered something crazy or
	// unusual that parses as a valid bpm.
	std::string msg {};

	// Use to indicate some sort of unusual condition where is_valid==true but
	// we nonetheless want to alert the user about something.  
	int flags {0};

	// Created from the components of the parsed input string.  Things like
	// leading and trailing spaces are stripped.  
	std::string str_clean {};

	// If the input is valid, this is set from the input string.  Can be used
	// by a constructor.
	double bpm {0.0};
};

bpm_str_helper validate_bpm_str(std::string const&);


