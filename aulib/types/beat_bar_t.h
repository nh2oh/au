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
	std::string print() const;

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
	explicit bar_t(int);
	explicit bar_t(double);

	double to_double() const;
	std::string print() const;

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



