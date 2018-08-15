#include "frq_t.h"
#include "cent_oct_t.h"
#include "..\util\au_algs_math.h"
#include <string>
#include <cmath>  // pow(), log2()
#include <cstdlib> // div()... not in <cmath>

// Static members
const int m_prec {9};

frq_t::frq_t(double const& frq_in, unit const& unit_in) {
	switch(unit_in) {
		case unit::Hz:  m_unit = 0; break;
		case unit::kHz: m_unit = 3; break;
		case unit::MHz: m_unit = 6; break;
		case unit::GHz: m_unit = 9; break;
	}
	m_frq = fmax(0.0, frq_in*(std::pow(10,m_unit)));
}

std::string frq_t::print(int prec) const {
	if (prec < 0) { prec = 0; }
	auto scf = static_cast<int>(std::pow(10,prec));  // "scale factor"
	auto frq_sr = static_cast<int>(std::round(m_frq*scf));  // "frq scaled, rounded"
	
	std::string s {};
	s += std::to_string(frq_sr/scf);
	s += ".";
	s += std::to_string(frq_sr%scf);

	switch (m_unit) {
		case 0: s += "Hz"; break;
		case 3: s += "kHz"; break;
		case 6: s += "MHz"; break;
		case 9: s += "GHz"; break;
		default: s += "? unit"; break;
	}

	return s;
}

// Member operators
// The /, *=, +=, -= operators do not need to deal with m_unit to make
// the calculation because m_frq is always stored in Hz.  
// Likewise, == and > can compare m_frq directly and ignore m_unit.  
double frq_t::operator/(frq_t const& denom) const {
	return m_frq/(denom.m_frq);
}
frq_t& frq_t::operator*=(double const& d) {
	m_frq *= d;
	m_frq = fmax(m_frq,0.0);
	return *this;
}
frq_t& frq_t::operator/=(double const& d) {
	m_frq /= d;
	m_frq = fmax(m_frq,0.0);
	return *this;
}
frq_t& frq_t::operator+=(frq_t const& rhs) {
	m_frq += rhs.m_frq;
	return *this;
}
frq_t& frq_t::operator-=(frq_t const& rhs) {
	m_frq -= rhs.m_frq;
	m_frq = fmax(m_frq,0.0);
	return *this;
}
bool frq_t::operator==(frq_t const& rhs) const {
	return isapproxeq(m_frq, rhs.m_frq, m_prec);
}
bool frq_t::operator>(frq_t const& rhs) const {
	return m_frq > rhs.m_frq;
}


// Nonmember operators
// For *, + and -, the units of the result are those of the lhs.  Since
// these return a new object, the lhs is always passed in by value.  
frq_t operator+(frq_t lhs, frq_t const& rhs) {
	return (lhs += rhs);
}
frq_t operator-(frq_t lhs, frq_t const& rhs) {
	return (lhs -= rhs);
}
frq_t operator*(double const& lhs, frq_t rhs) {
	return (rhs*=lhs);
}
frq_t operator*(frq_t lhs, double const& rhs) {
	return (lhs*=rhs);
}

bool operator!=(frq_t const& lhs, frq_t const& rhs) {
	return !(lhs == rhs);
}
bool operator<(frq_t const& lhs, frq_t const& rhs) {
	return (!(lhs > rhs) && lhs != rhs);
}
bool operator>=(frq_t const& lhs, frq_t const& rhs) {
	return (lhs > rhs || lhs == rhs);
}
bool operator<=(frq_t const& lhs, frq_t const& rhs) {
	return (lhs < rhs || lhs == rhs);
}

frq_t operator+(frq_t lhs, cent_t const& rhs) {
	return lhs *= std::pow(2,(rhs.to_double()/1200));
}
frq_t operator-(frq_t lhs, cent_t const& rhs) {
	return lhs *= std::pow(2,((-rhs).to_double()/1200));
}


