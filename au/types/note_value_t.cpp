#include "note_value_t.h"
#include "ts_t.h"
#include "beat_bar_t.h"
#include "..\util\au_util_all.h"
#include <numeric> // pow()
#include <optional>
#include <string>

//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative timespan

note_value::note_value(double nv_in) {
	m_nv = nv_in;
	au_assert(m_nv>0.0);
}

note_value::note_value(double base_value_in, int num_dots_in) {
	// nv = rv*(2-1/(2^n))
	m_nv = base_value_in*(2.0 - 1.0/std::pow(2,num_dots_in));
	au_assert(m_nv>0.0);
}

note_value::note_value(ts_t ts_in, beat_t nbeats) {
	m_nv = (ts_in.beat_unit().to_double())*(nbeats.to_double());
	au_assert(m_nv>0.0);
}

std::optional<nv_base_dots> note_value::exact() const {
	// nv = bv*(2-1/(2^n))
	double bv {0}; int num_dots {0};  // relative-value, number-of-dots
	for (true; num_dots<=5; ++num_dots) {
		bv = m_nv/(2-1/std::pow(2,num_dots));
		if (isapproxint(1.0/bv,6) || isapproxint(bv,6)) {
			return nv_base_dots {bv,num_dots};
		}
	}
	return {};
}

std::optional<nv_base_dots> note_value::exact2() const {
	// nv = bv*(2-1/(2^n)), for integer n >= 0
	// For the standard notes, bv = 1/(2^m), for integer m
	// nv = 1/2^(m-1) - 1/2^(n+m)

	// For any given m, n=0 => nv = 1/2^m, and n-> infinity => nv = 2/2^m
	// => nv = 1/2^(m-1); an infinite number of dots doubles the duration 
	// of the note.  
	//
	// For a d w/an unknown m,n, the largest possible m is d/2, corresponding
	// to an infinite number of dots.  IOW, our unknown d is
	// d_unkn = 1/2^(m_max)*(2-1/2^infty) => m_max = 1-log2(d_unkn).  The
	// smallest possible m is 2*d, IOW, if d_unkn = (1/2^m_min)(2-1/2^0).  
	// m_min = -log2(d_unkn).  
	// Hence, for any d_unkn, m falls between -log2(d_unkn) and 1-log2(d_unkn).  
	// Since an infinite number of dots is unlikely, m falls in the range
	// [ -log2(d_unkn), 1-log2(d_unkn) )
	// => m = m* = floor(-log2(d_unkn), 1-log2(d_unkn))
	// 
	// Now we have d_unkn/bv = (2-1/2^n), where bv = 1/2^(m*).  Let d_sup
	// ("d supplemental") == d_unkn/bv, thus, d_sup = d_unkn*(2^m*)
	// d_sup = 2-1/2^n = [2^(n+1)-1]/[n^2]
	// 2-d_sup = 1/2^n => n = -log2(2-d_sup) (the supplemental duration factor
	// is never > 2, and is == 2 only for an infinite number of dots).  Note 
	// that d_sup can be > or < d_unkn.  
	//
	// n = -1*log2(2-d_unkn/2^floor(1-log2(d_unkn)))
	//
	//
	//

	int m_min = -3; int m_max = 8; //(-2=>quadruple-whole; 8=>256'th)
	int m {0};
	for (m = m_min; m<(m_max+1); ++m) {
		if (m_nv >= std::pow(2,-m)) { break; }
	}
	if (m>m_max) { return {}; }

	double bv {std::pow(2,-m)};
	for (int num_dots = 0; num_dots<=5; ++num_dots) {
		double test_m_nv = bv*(2-std::pow(2,-num_dots));
		if (isapproxeq(m_nv,test_m_nv,6)) {
			return nv_base_dots {bv,num_dots};
		} else if (test_m_nv > m_nv) {
			break;
		}
	}
	double nv_remainder = m_nv - bv;


	return {};
}

std::optional<int> note_value::ndot() const {
	auto o_base_dots = exact();
	if (o_base_dots) {
		return (*o_base_dots).ndot;
	}
	return {};
}

std::optional<double> note_value::undot_value() const {
	auto o_base_dots = exact();
	if (o_base_dots) {
		return (*o_base_dots).base_value;
	}
	return {};
}

std::string note_value::print() const {
	// Large, duplex longa, maxima
	// long, longa
	// double-whole, breve
	// whole, semibreve
	// half, minim
	// ...
	auto o_base_dots = exact();
	std::string s {};
	if (o_base_dots) {
		auto num_dots = (*o_base_dots).ndot;
		auto base_exact = (*o_base_dots).base_value;
		s = "(";
		if (base_exact < 1) {
			s += "1/" + std::to_string(static_cast<int>(1.0/base_exact));
		} else {
			s += std::to_string(static_cast<int>(base_exact));
		}
		s += ")" + std::string(".",num_dots);
	} else {
		s = "(?)";
		//frac rat_approx = rapprox((1.0/m_nv),256);
		//s = "(" + std::to_string(rat_approx.denom) + "/" + std::to_string(rat_approx.num) + ")";
	}

	return s;
}
double note_value::to_double() const {
	return m_nv;
}

note_value& note_value::operator+=(note_value const& rhs) {
	m_nv += rhs.m_nv;
	return *this;
}
note_value& note_value::operator-=(note_value const& rhs) {
	m_nv -= rhs.m_nv;
	au_assert(m_nv>0);  // if rhs.m_nv == 0 we'd still be ok
	return *this;
}

note_value operator+(note_value const& lhs, note_value const& rhs) {
	note_value result {lhs};
	return (result += rhs);
}
note_value operator-(note_value const& lhs, note_value const& rhs) {
	note_value result {lhs};
	return (result -= rhs);  // operator-= checks for a - result
}
double operator/(note_value const& lhs, note_value const& rhs){ 
	// how many rhs-notes fit in the span of an lhs-note?
	return (lhs.to_double())/(rhs.to_double());
}
note_value operator*(note_value const& lhs, double const& rhs) {
	return(note_value{lhs.to_double()*rhs});
}
note_value operator*(double const& lhs, note_value const& rhs) {
	return(note_value{rhs.to_double()*lhs});
}

bool operator==(note_value const& lhs, note_value const& rhs) {
	//return (std::abs((lhs-rhs).to_double()) <= std::pow(10,-6));
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(note_value const& lhs, note_value const& rhs) {
	return !(lhs==rhs);
}
bool operator>(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}























