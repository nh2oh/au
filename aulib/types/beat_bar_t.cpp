#include "beat_bar_t.h"
#include "..\util\au_algs_math.h"  // aprx_eq()
#include "..\util\au_util.h"  // bsprintf() for  *.print()
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
beat_t operator""_bt(const char *lit) {
	return beat_t{std::stod(lit)};
}
std::string beat_t::print() const {
	return bsprintf("%.3f",m_beats);
}

beat_t& beat_t::operator+=(const beat_t& rhs) {
	m_beats += rhs.m_beats;
	return *this;
}
beat_t& beat_t::operator-=(const beat_t& rhs) {
	m_beats -= rhs.m_beats;
	return *this;
}
beat_t& beat_t::operator*=(const double& rhs) {
	m_beats *= rhs;
	return *this;
}
beat_t& beat_t::operator/=(const double& rhs) {
	m_beats /= rhs;
	return *this;
}
double operator/(const beat_t& num, const beat_t& denom) {
	return num.m_beats/denom.m_beats;
}
bool beat_t::operator<(const beat_t& rhs) const {
	return m_beats < rhs.m_beats;
}
bool beat_t::operator>(const beat_t& rhs) const {
	return m_beats > rhs.m_beats;
}
bool beat_t::operator==(const beat_t& rhs) const {
	return aprx_eq(m_beats,rhs.m_beats);
}
bool operator<=(const beat_t& lhs, const beat_t& rhs) {
	return (lhs < rhs || lhs == rhs);
}
bool operator>=(const beat_t& lhs, const beat_t& rhs) {
	return (lhs > rhs || lhs == rhs);
}
bool operator!=(const beat_t& lhs, const beat_t& rhs) {
	return !(lhs==rhs);
}
beat_t operator+(beat_t lhs, const beat_t& rhs) {
	return (lhs += rhs);
}
beat_t operator-(beat_t lhs, const beat_t& rhs) {
	return (lhs -= rhs);
}
beat_t operator*(const double& lhs, beat_t rhs) {
	return rhs *= lhs;
}
beat_t operator*(beat_t lhs, const double& rhs) {
	return lhs *= rhs;
}
beat_t operator/(beat_t num, const double& denom) {
	return num/=denom;
}



//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars

bar_t::bar_t(double beats_in) {
	m_bars = beats_in;
}
bar_t::bar_t(int beats_in) {
	m_bars = static_cast<double>(beats_in);
}
double bar_t::to_double() const {
	return m_bars;
}
bar_t operator""_br(const char *lit) {
	return bar_t{std::stod(lit)};
}
std::string bar_t::print() const {
	return bsprintf("%.3f",m_bars);
}
bar_t bar_t::full() const {
	return bar_t {std::floor(m_bars)};
}
bar_t bar_t::next() const {
	return bar_t {std::floor(m_bars)+1.0};  // "current" + 1
}
//bar_t bar_t::remain() const {  // "next - current position"
//	return bar_t {std::floor(m_bars) + 1.0 - m_bars};
//}
double bar_t::fremain() const {  // "fraction remaining"
	if (aprx_eq(std::ceil(m_bars),m_bars)) {
		return 1.0;
	}
	return (std::ceil(m_bars) - m_bars);
}
bool bar_t::isexact() const {
	return aprx_int(m_bars);
}
bar_t& bar_t::operator+=(const bar_t& rhs) {
	m_bars += rhs.m_bars;
	return *this;
}
bar_t& bar_t::operator-=(const bar_t& rhs) {
	m_bars -= rhs.m_bars;
	return *this;
}
bar_t& bar_t::operator*=(const double& rhs) {
	m_bars *= rhs;
	return *this;
}
bar_t& bar_t::operator/=(const double& rhs) {
	m_bars /= rhs;
	return *this;
}
double operator/(const bar_t& num, const bar_t& denom) {
	return num.m_bars/denom.m_bars;
}
bool bar_t::operator<(const bar_t& rhs) const {
	return m_bars < rhs.m_bars;
}
bool bar_t::operator>(const bar_t& rhs) const {
	return m_bars > rhs.m_bars;
}
bool bar_t::operator==(const bar_t& rhs) const {
	return aprx_eq(m_bars,rhs.m_bars);
}
bool operator<=(const bar_t& lhs, const bar_t& rhs) {
	return (lhs < rhs || lhs == rhs);
}
bool operator>=(const bar_t& lhs, const bar_t& rhs) {
	return (lhs > rhs || lhs == rhs);
}
bool operator!=(const bar_t& lhs, const bar_t& rhs) {
	return !(lhs==rhs);
}
bar_t operator+(bar_t lhs, const bar_t& rhs) {
	return (lhs += rhs);
}
bar_t operator-(bar_t lhs, const bar_t& rhs) {
	return (lhs -= rhs);
}
bar_t operator*(const double& lhs, bar_t rhs) {
	return rhs *= lhs;
}
bar_t operator*(bar_t lhs, const double& rhs) {
	return lhs *= rhs;
}
bar_t operator/(bar_t num, const double& denom) {
	return num/=denom;
}


//-----------------------------------------------------------------------------
// Class tempo_t

tempo_t::tempo_t(double const& beats_in) {
	//au_assert(beats_in >= 0.0);
	m_bpm = beat_t{beats_in};
}

tempo_t::tempo_t(beat_t const& beats_in, double const& dt_in) {
	//au_assert(beats_in >= beat_t{0});
	//au_assert(dt_in>0.0);
	auto miliseconds = std::chrono::milliseconds(static_cast<int>(dt_in*1000*1000));
	m_bpm = beats_in*dt_in;
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::minutes const& dt_in) {
	//au_assert(beats_in >= beat_t{0});
	//au_assert(dt_in>std::chrono::minutes(0));
	m_bpm = beats_in*(dt_in/dt_in);
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::seconds const& dt_in) {
	//au_assert(beats_in >= beat_t{0});
	//au_assert(dt_in>std::chrono::minutes(0));
	m_bpm = beats_in*(std::chrono::milliseconds(60*1000)/std::chrono::milliseconds(dt_in));
}
tempo_t::tempo_t(beat_t const& beats_in, std::chrono::milliseconds const& dt_in) {
	//au_assert(beats_in >= beat_t{0});
	//au_assert(dt_in>std::chrono::minutes(0));
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

