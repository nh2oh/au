#include "note_value_t.h"
#include "ts_t.h"
#include "beat_bar_t.h"
#include "..\util\au_util_all.h"
#include <numeric> // pow()
#include <cmath> // floor(), log2()
#include <optional>
#include <string>
/*
//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative timespan

note_value::note_value(double nv_in) {
	m_nv = nv_in;
	au_assert(m_nv>0.0);
}

note_value::note_value(double base_value_in, int num_dots_in) {
	// nv = bv*(2-1/(2^n))
	m_nv = base_value_in*(2.0 - 1.0/std::pow(2,num_dots_in));
	au_assert(m_nv>0.0);
}

note_value::note_value(ts_t ts_in, beat_t nbeats) {
	m_nv = (ts_in.beat_unit().to_double())*(nbeats.to_double());
	au_assert(m_nv>0.0,"Note-value's must be >0");
}

note_value operator""_nv(const char *literal_in, size_t length) {
	auto o_matches = rx_match_captures("(\\d+)(?:/(\\d+))?(\\.+)?",
		std::string{literal_in});
	if (!o_matches || (*o_matches).size() != 4) {
		au_error("Could not parse note-value string literal");
	}
	auto matches = *o_matches;

	double numerator = str2int(*(matches[1])); // regex gaurantees existence

	double denominator {1};
	if (matches[2]) { denominator = str2int(*(matches[2])); }

	int ndots {0};
	if (matches[3]) { 
		ndots = static_cast<int>((*(matches[3])).size());
	}

	return note_value {numerator/denominator,ndots};
}


std::optional<nv_base_dots> note_value::exact() const {
	// For "note-value" nv, "base-value" bv
	// nv = bv*(2-1/(2^n)), integer n >= 0
	// For the standard notes, bv = 1/(2^m), integer m
	// => nv = 1/2^(m-1) - 1/2^(n+m)
	//
	// For any given m, n=0 => nv = 1/2^m, and n-> infinity => nv = 2/2^m
	// => nv = 1/2^(m-1); an infinite number of dots doubles the duration 
	// of the note.  
	//
	// For a d_unkn w/an unknown m,n, possible values of m fall between:
	//     m = 1-log2(d_unkn), corresponding to the case where d_unkn has 
	//     n = infinity:  d_unkn = 1/2^(m_max)*(2-1/2^infty).  
	// and,
	//     m = -log2(d_unkn), corresponding to the case were d_unkn has
	//     n = 0:  d_unkn = (1/2^m_min)(2-1/2^0).  
	// Since n = infinity is unlikely, m is the first integer smaller than
	// 1-log2(d_unkn).  Thus,
	//     m* = floor(1-epsilon-log2(d_unkn))
	//     n* = -1*log2(2-d_unkn*(2^m*))
	//
	double epsilon = 0.00000009;
	int m = std::floor(1-epsilon-std::log2(m_nv));
	double n = -1*std::log2(2-m_nv*(std::pow(2,m)));
	if (!isapproxint(n,6) || n < 0) {
		return {};
	}

	return nv_base_dots {std::pow(2,-m),static_cast<int>(n)};
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

std::string note_value::print(int flags) const {
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
		s += ")" + std::string(num_dots,'.');
	} else {
		if (flags & notevalueopts::printapproxifnotexact) {
			s = "(? ~" + std::to_string(m_nv);
		} else {
			s = "(?)";
		}
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
	au_assert(m_nv>0, "note_value::operator-=(note_value const& rhs):  m_nv>0");  // if rhs.m_nv == 0 we'd still be ok
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






//-----------------------------------------------------------------------------
// The nv_uih class

nv_uih::nv_uih() {
	//...
}

nv_uih::nv_uih(std::string const& str_in) {
	// Do not set str_last_ before calling update()... update() checkes 
	// str_last_ before doing anything and will not proceed if 
	// str_in == str_last_.  
	update(str_in);
}

void nv_uih::update(std::string const& str_in) {
	if (str_in == str_last_) {
		return;
	}
	str_last_ = str_in;

	auto str_in_parts = parse_nv_str_();
	if (!str_in_parts) {
		is_valid_ = false;
		msg_ += "An nv is of the form n/d[.] where n and d are integers and [.]";
		msg_ += " is 0 or more \".\" characters; Ex: \"1/4\", \"1/4..\".  ";
		return;
	}

	auto nv_parts = *str_in_parts;
	// Addnl tests... weird magnitudes, large number of dots...

	is_valid_ = true;
	nv_ = note_value{(nv_parts.num)/(nv_parts.denom), nv_parts.ndots};
}

bool nv_uih::is_valid() const {
	return is_valid_;
}

int nv_uih::flags() const {
	return flags_;
}

note_value nv_uih::get() const {
	au_assert(is_valid_,"Called nv_uih::get() on a nv_uih object with is_valid_==false");
	return *nv_;
}

// Parses str_last_; does not read any other internal state, does not alter
// any internal state.  
// Called by the constructor.  
std::optional<nv_uih::nv_str_parts> nv_uih::parse_nv_str_() const {
	auto o_matches = rx_match_captures("^\\s*([1-9]+)/([1-9]+)(\\.*)\\s*$",str_last_);
	if (!o_matches || (*o_matches).size() != 4) {
		return {};
	}

	auto matches = *o_matches;

	double n = std::stod(*(matches[1]));
	double d = std::stod(*(matches[2]));
	int ndots = static_cast<int>((*matches[3]).size());
	
	return nv_str_parts {n, d, ndots};
}


bool nv_uih::operator==(nv_uih const& rhs) const {
	return (str_last_ == rhs.str_last_);
}


*/





