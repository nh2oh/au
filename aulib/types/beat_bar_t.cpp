#include "beat_bar_t.h"
#include "..\util\au_util_all.h"
#include <string>
#include <chrono>



//-----------------------------------------------------------------------------
// Class beat_t

beat_t::beat_t(double beats_in) {
	m_beats = beats_in;
}
beat_t::beat_t(int beats_in) {
	m_beats = static_cast<double>(beats_in);
}
double beat_t::to_double() const {
	return m_beats;
}
std::string beat_t::print() const {
	if (isapproxint(m_beats,6)) {
		return std::to_string(static_cast<int>(m_beats));
	}
	return std::to_string(m_beats);
}
beat_t& beat_t::operator+=(beat_t const& rhs) {
	m_beats += rhs.m_beats;
	return *this;
}
beat_t& beat_t::operator-=(beat_t const& rhs) {
	m_beats -= rhs.m_beats;
	return *this;
}

beat_t operator+(beat_t const& lhs, beat_t const& rhs) {
	beat_t result {lhs};
	return (result+=rhs);
}
beat_t operator-(beat_t const& lhs, beat_t const& rhs) {
	beat_t result {lhs};
	return (result-=rhs);
}
beat_t operator-(beat_t const& rhs) {
	return beat_t{-1.0*(rhs.to_double())};
}

double operator/(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double())/(rhs.to_double());
}
double operator/(double const& lhs, beat_t const& rhs) {
	return (lhs/rhs.to_double());
}
beat_t operator*(double const& lhs, beat_t const& rhs) {
	return beat_t{lhs*rhs.to_double()};
}
beat_t operator/(beat_t const& lhs, double const& rhs) {
	return beat_t{lhs.to_double()/rhs};
}
beat_t operator*(beat_t const& lhs, double const& rhs) {
	return beat_t{rhs*lhs.to_double()};
}

bool operator==(beat_t const& lhs, beat_t const& rhs) {
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(beat_t const& lhs, beat_t const& rhs) {
	return !(lhs==rhs);
}
bool operator>(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}

//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars
bar_t::bar_t(double bars_in) {
	m_bars = bars_in;
}
bar_t::bar_t(int bars_in) {
	m_bars = bars_in;
}

double bar_t::to_double() const {
	return m_bars;
}
std::string bar_t::print() const {
	if (isapproxint(m_bars,6)) {
		return std::to_string(static_cast<int>(m_bars));
	}
	return std::to_string(m_bars);
}

bar_t& bar_t::operator+=(bar_t const& rhs) {
	m_bars += rhs.m_bars;
	return *this;
}
bar_t& bar_t::operator-=(bar_t const& rhs) {
	m_bars -= rhs.m_bars;
	return *this;
}
bar_t operator+(bar_t const& lhs, bar_t const& rhs) {
	bar_t result {lhs};
	return (result+=rhs);
}
bar_t operator-(bar_t const& lhs, bar_t const& rhs) {
	bar_t result {lhs};
	return (result-=rhs);
}
double operator/(bar_t const& lhs, bar_t const& rhs) {
	return ((lhs.to_double())/(rhs.to_double()));
}
bar_t operator/(bar_t const& lhs, double const& rhs) {
	return bar_t{lhs.to_double()/rhs};
}
double operator/(double const& lhs, bar_t const& rhs) {
	return (lhs/rhs.to_double());
}
bar_t operator*(bar_t const& lhs, double const& rhs) {
	return(bar_t{lhs.to_double()*rhs});
}
bar_t operator*(double const& lhs, bar_t const& rhs) {
	return(bar_t{rhs.to_double()*lhs});
}

bool operator==(bar_t const& lhs, bar_t const& rhs) {
	//return (std::abs((lhs-rhs).to_double()) <= std::pow(10,-6));
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(bar_t const& lhs, bar_t const& rhs) {
	return !(lhs==rhs);
}
bool operator>(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}


//-----------------------------------------------------------------------------
// Class tempo_t

tempo_t::tempo_t(double const& beats_in) {
	au_assert(beats_in >= 0.0);
	m_bpm = beat_t{beats_in};
}

tempo_t::tempo_t(beat_t const& beats_in, double const& dt_in) {
	au_assert(beats_in >= beat_t{0});
	au_assert(dt_in>0.0);
	auto miliseconds = std::chrono::milliseconds(static_cast<int>(dt_in*1000*1000));
	m_bpm = beats_in*dt_in;
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::minutes const& dt_in) {
	au_assert(beats_in >= beat_t{0});
	au_assert(dt_in>std::chrono::minutes(0));
	m_bpm = beats_in*(dt_in/dt_in);
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::seconds const& dt_in) {
	au_assert(beats_in >= beat_t{0});
	au_assert(dt_in>std::chrono::minutes(0));
	m_bpm = beats_in*(std::chrono::milliseconds(60*1000)/std::chrono::milliseconds(dt_in));
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::milliseconds const& dt_in) {
	au_assert(beats_in >= beat_t{0});
	au_assert(dt_in>std::chrono::minutes(0));
	m_bpm = beats_in*(std::chrono::milliseconds(60*1000)/dt_in);
}

tempo_t::operator beat_t() const {
	return m_bpm;
}

std::chrono::milliseconds operator/(beat_t const& num, tempo_t const& denom) {
	auto mins = num/static_cast<beat_t>(denom);
	return std::chrono::milliseconds(static_cast<int>(mins*60*1000));
}

beat_t operator*(tempo_t const& lhs, std::chrono::milliseconds const& rhs) {
	auto unitms = std::chrono::milliseconds(1);
	double r = (rhs/unitms);  // ms
	r /= (1000.0*60.0);  // minutes
	return r*static_cast<beat_t>(lhs);
}
beat_t operator*(std::chrono::milliseconds const& lhs, tempo_t const& rhs) {
	return (rhs*lhs);
}

