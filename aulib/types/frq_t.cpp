#include "frq_t.h"
#include "cent_oct_t.h"
#include "dbklib\math.h"
#include <string>
#include <exception>

// Static members
//const int frq_t::m_prec {9};

frq_t::frq_t(double d) {
	if (d < 0.0) { std::abort(); }
	m_frq = d;
}
/*frq_t::frq_t(double frq_in, unit const& unit_in) {
	switch(unit_in) {
		case unit::Hz:  m_unit = 0; break;
		case unit::kHz: m_unit = 3; break;
		case unit::MHz: m_unit = 6; break;
		case unit::GHz: m_unit = 9; break;
	}
	m_frq = fmax(0.0, frq_in*(std::pow(10,m_unit)));
}*/

std::string frq_t::print() const {
	/*std::string fmt {"%f"};
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

	return dbk::bsprintf(fmt.c_str(),m_frq,unitstr.c_str());*/
	return std::to_string(m_frq);
}

// Member operators
double frq_t::operator/(const frq_t& denom) const {
	return m_frq/(denom.m_frq);
}
frq_t& frq_t::operator*=(double d) {
	m_frq *= d;
	if (m_frq < 0.0) { std::abort(); }
	return *this;
}
frq_t& frq_t::operator/=(double d) {
	m_frq /= d;
	if (m_frq < 0.0) { std::abort(); }
	return *this;
}
frq_t& frq_t::operator+=(const frq_t& rhs) {
	m_frq += rhs.m_frq;
	return *this;
}
frq_t& frq_t::operator-=(const frq_t& rhs) {
	m_frq -= rhs.m_frq;
	if (m_frq < 0.0) { std::abort(); }
	return *this;
}
bool frq_t::operator==(const frq_t& rhs) const {
	return aprx_eq(m_frq,rhs.m_frq);
}
bool frq_t::operator!=(const frq_t& rhs) const {
	return !aprx_eq(m_frq,rhs.m_frq);
}
bool frq_t::operator<(const frq_t& rhs) const {
	return aprx_lt(m_frq,rhs.m_frq);
}
bool frq_t::operator>(const frq_t& rhs) const {
	return aprx_gt(m_frq,rhs.m_frq);
}
bool frq_t::operator<=(const frq_t& rhs) const {
	return !aprx_gt(m_frq,rhs.m_frq);
}
bool frq_t::operator>=(const frq_t& rhs) const {
	return !aprx_lt(m_frq,rhs.m_frq);
}

// Nonmember operators
// For *, + and -, the units of the result are those of the lhs.  Since these return a new object,
// the lhs is always passed in by value.  
frq_t operator+(frq_t lhs, const frq_t& rhs) {
	return (lhs += rhs);
}
frq_t operator-(frq_t lhs, const frq_t& rhs) {
	return (lhs -= rhs);
}
frq_t operator*(double lhs, frq_t rhs) {
	return (rhs*=lhs);
}
frq_t operator*(frq_t lhs, double rhs) {
	return (lhs*=rhs);
}



