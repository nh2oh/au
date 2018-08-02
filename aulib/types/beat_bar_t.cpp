#include "beat_bar_t.h"
#include "..\util\au_util_all.h"





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

double bar_t::to_double() const {
	return m_bars;
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








