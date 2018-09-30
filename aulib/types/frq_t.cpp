#include "frq_t.h"
#include "cent_oct_t.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_util.h" // bsprintf()
#include <string>
#include <cmath>  // pow(), log2()
#include <cstdlib> // div()... not in <cmath>

// Static members
const int frq_t::m_prec {9};

frq_t::frq_t(double const& frq_in, unit const& unit_in) {
	switch(unit_in) {
		case unit::Hz:  m_unit = 0; break;
		case unit::kHz: m_unit = 3; break;
		case unit::MHz: m_unit = 6; break;
		case unit::GHz: m_unit = 9; break;
	}
	m_frq = fmax(0.0, frq_in*(std::pow(10,m_unit)));
}

std::string frq_t::print(int const& prec) const {
	std::string fmt {"%"};
	if (prec > 0) { 
		fmt += "." + std::to_string(prec);
	}
	fmt += "f";
	fmt += " %s";

	std::string unitstr {};
	switch (m_unit) {
		case 0: unitstr = "Hz"; break;
		case 3: unitstr = "kHz"; break;
		case 6: unitstr = "MHz"; break;
		case 9: unitstr = "GHz"; break;
		default: unitstr = "? unit"; break;
	}

	return bsprintf(fmt.c_str(),m_frq,unitstr.c_str());
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
	return aprx_eq(m_frq,rhs.m_frq); //isapproxeq(m_frq, rhs.m_frq, m_prec);
}
bool frq_t::operator>(frq_t const& rhs) const {
	return aprx_gt(m_frq,rhs.m_frq); // m_frq > rhs.m_frq;
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


