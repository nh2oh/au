#pragma once
#include <string> // for *.print()
#include <chrono>

//-----------------------------------------------------------------------------
// Class beat_t
// Represents some number of beats

class beat_t {
public:
	beat_t() = default;
	explicit beat_t(int);
	explicit beat_t(double);

	double to_double() const;
	std::string print() const;

	beat_t& operator+=(const beat_t&);
	beat_t& operator-=(const beat_t&);
	beat_t& operator*=(const double&);
	beat_t& operator/=(const double&);
	friend double operator/(const beat_t&, const beat_t&);
	bool operator<(const beat_t&) const;
	bool operator>(const beat_t&) const;
	bool operator==(const beat_t&) const;
	
private:
	double m_beats {0.0};
};

beat_t operator""_bt(const char*);

bool operator<=(const beat_t&, const beat_t&);
bool operator>=(const beat_t&, const beat_t&);
bool operator!=(const beat_t&, const beat_t&);
beat_t operator+(beat_t, const beat_t&);
beat_t operator-(beat_t, const beat_t&);
beat_t operator*(const double&, beat_t);
beat_t operator*(beat_t, const double&);
beat_t operator/(beat_t, const double&);


//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars

class bar_t {
public:
	bar_t() = default;
	explicit bar_t(int);
	explicit bar_t(double);

	double to_double() const;
	std::string print() const;
	bar_t full() const;  // Number of full bars
	bar_t next() const;
	//bar_t remain() const;
	bool isexact() const;
	double fremain() const;

	bar_t& operator+=(const bar_t&);
	bar_t& operator-=(const bar_t&);
	bar_t& operator*=(const double&);
	bar_t& operator/=(const double&);
	friend double operator/(const bar_t&, const bar_t&);
	bool operator<(const bar_t&) const;
	bool operator>(const bar_t&) const;
	bool operator==(const bar_t&) const;
	
private:
	double m_bars {0.0};
};

bar_t operator""_br(const char*);

bool operator<=(const bar_t&, const bar_t&);
bool operator>=(const bar_t&, const bar_t&);
bool operator!=(const bar_t&, const bar_t&);
bar_t operator+(bar_t, const bar_t&);
bar_t operator-(bar_t, const bar_t&);
bar_t operator*(const double&, bar_t);
bar_t operator*(bar_t, const double&);
bar_t operator/(bar_t, const double&);


//-----------------------------------------------------------------------------
// Class tempo_t
// Represents some number of beats per minute

class tempo_t {
public:
	//using namespace std::chrono_literals;
	tempo_t() = default;  // 60 bpm
	explicit tempo_t(double const& beats_in);  // means beats-per-minute
	explicit tempo_t(beat_t const&, double const&); // arg2 == minutes
	explicit tempo_t(beat_t const&, std::chrono::minutes const&);
	explicit tempo_t(beat_t const&, std::chrono::seconds const&);
	explicit tempo_t(beat_t const&, std::chrono::milliseconds const&);

	explicit operator beat_t() const;

private:
	beat_t m_bpm {60};  // Per minute
};

std::chrono::milliseconds operator/(beat_t const&, tempo_t const&);
beat_t operator*(tempo_t const&, std::chrono::milliseconds const&); 
beat_t operator*(std::chrono::milliseconds const&, tempo_t const&); 


