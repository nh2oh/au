#pragma once
//#include "ts_t.h"
//#include "beat_bar_t.h"
#include <string>
#include <optional>
class ts_t;
class beat_t;
class bar_t;




//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative amount of time spanned by a single note.  Storage is
// in decimal, not "rational" form.  A "quarter note" has m_nv == 0.25, a
// double-whole note has m_nv == 2, etc.  
struct nv_base_dots {
	double base_value;
	int ndot;
};

class note_value {
public:
	explicit note_value() = default;
	explicit note_value(double);
	explicit note_value(double,int);  // base_value, ndots
	explicit note_value(ts_t,beat_t);

	//std::optional<nv_base_dots> exact2() const;
	std::optional<int> ndot() const;
	std::optional<double> undot_value() const;
	
	std::string print() const;
	double to_double() const;

	note_value& operator+=(note_value const&);
	note_value& operator-=(note_value const&);
private:
	double m_nv {1.0};

	std::optional<nv_base_dots> exact() const;
};

note_value operator""_nv(const char *literal_in, size_t length);

note_value operator+(note_value const&, note_value const&);
note_value operator-(note_value const&, note_value const&);
double operator/(note_value const&, note_value const&);
	// how many rhs-notes fit in the span of an lhs-note?

note_value operator*(note_value const&, double const&);
note_value operator*(double const&, note_value const&);
bool operator==(note_value const&, note_value const&);
bool operator!=(note_value const&, note_value const&);
bool operator>(note_value const&, note_value const&);
bool operator<(note_value const&, note_value const&);
bool operator>=(note_value const&, note_value const&);
bool operator<=(note_value const&, note_value const&);














