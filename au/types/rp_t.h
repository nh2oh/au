#pragma once
#include <string>
#include <vector>
#include <optional>


//-----------------------------------------------------------------------------
// Class beat_t
// Represents some number of beats

class beat_t {
public:
	explicit beat_t() = default;
	explicit beat_t(double);

	double to_double() const;

	beat_t& operator+=(beat_t const&);
	beat_t& operator-=(beat_t const&);
private:
	double m_beats {0.0};
};

beat_t operator+(beat_t const&, beat_t const&);
beat_t operator-(beat_t const&, beat_t const&);
double operator/(beat_t const&, beat_t const&);
double operator/(double const&, beat_t const&);
beat_t operator/(beat_t const&, double const&);
beat_t operator*(double const&, beat_t const&);
beat_t operator*(beat_t const&, double const&);
bool operator==(beat_t const&, beat_t const&);
bool operator!=(beat_t const&, beat_t const&);
bool operator>(beat_t const&, beat_t const&);
bool operator<(beat_t const&, beat_t const&);
bool operator>=(beat_t const&, beat_t const&);
bool operator<=(beat_t const&, beat_t const&);

//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars
class bar_t {
public:
	explicit bar_t() = default;
	explicit bar_t(double);

	double to_double() const;

	bar_t& operator+=(bar_t const&);
	bar_t& operator-=(bar_t const&);
private:
	double m_bars {0.0};
};

bar_t operator+(bar_t const&, bar_t const&);
bar_t operator-(bar_t const&, bar_t const&);
double operator/(bar_t const&, bar_t const&);
double operator/(double const&, bar_t const&);
bar_t operator/(bar_t const&, double const&);
bar_t operator*(bar_t const&, double const&);
bar_t operator*(double const&, bar_t const&);
bool operator==(bar_t const&, bar_t const&);
bool operator!=(bar_t const&, bar_t const&);
bool operator>(bar_t const&, bar_t const&);
bool operator<(bar_t const&, bar_t const&);
bool operator>=(bar_t const&, bar_t const&);
bool operator<=(bar_t const&, bar_t const&);

//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative amount of time spanned by a single note.  Storage is
// in decimal, not "rational" form.  A "quarter note" has m_nv == 0.25, a
// double-whole note has m_nv == 2, etc.  
struct nv_base_dots {
	double base_value;
	int ndot;
};

class ts_t;

class note_value {
public:
	explicit note_value() = default;
	explicit note_value(double);
	explicit note_value(double,int);  // base_value, ndots
	explicit note_value(ts_t,beat_t);

	std::optional<nv_base_dots> exact() const;
	std::optional<nv_base_dots> exact2() const;
	std::optional<int> ndot() const;
	std::optional<double> undot_value() const;
	
	std::string print() const;
	double to_double() const;

	note_value& operator+=(note_value const&);
	note_value& operator-=(note_value const&);
private:
	double m_nv {0.0};
};

note_value operator+(note_value const&, note_value const&);
note_value operator-(note_value const&, note_value const&);
double operator/(note_value const&, note_value const&);  // how many rhs-notes fit in the span of an lhs-note?
note_value operator*(note_value const&, double const&);
note_value operator*(double const&, note_value const&);
bool operator==(note_value const&, note_value const&);
bool operator!=(note_value const&, note_value const&);
bool operator>(note_value const&, note_value const&);
bool operator<(note_value const&, note_value const&);
bool operator>=(note_value const&, note_value const&);
bool operator<=(note_value const&, note_value const&);

//-----------------------------------------------------------------------------
// Class ts_t
// Simple:
// (note-value == 1 beat) / (num-beats-per-bar)
// Compound:
//   Each beat of the bar divides into 3 equal parts
//   Numerator => 3*(the number of into-3 divisions of the beat) in 
//				  each bar.  So if the number is 6, there are 2 "into-3"
//				  divisions; if the number is 12, there are 4 "into-3"
//				  divisions.  
//   Denominator => The note value for which a group of 3 correspond to 
//					one beat.  
class ts_t {
public:
	ts_t() = delete;
	explicit ts_t(beat_t,note_value,bool=false); // num, denom, is-compound

	note_value beat_unit() const;  // Denominator=>note_value==1 beat
	note_value bar_unit() const;  // The note value that would span 1 bar
	beat_t beats_per_bar() const;  // Numerator=>beats-per-bar
	std::string print() const;

	bool operator==(ts_t const&) const;
private:
	// Simple:
	// The note-value representing one beat (denominator); the length of time 
	// spanned by a single beat in the relative-time units of the note_value 
	// type.  
	note_value m_beat_unit {1.0/4.0};
	// Beats per bar (numerator)
	beat_t m_bpb {4.0};

	// Compound:
	// the m_beat_unit note value is the note having twice the timespan
	// of what it would be if the ts were simple, then with one dot added.
	// Thus, if 9/8, the simple m_beat_unit == 1/8, thus the compound
	// m_beat_unit == (1/4).
	// If 6/8, the simple m_beat_unit == 1/8, thus the compound
	// m_beat_unit == (1/4).
	//               m_beat_unit from a simple ts...
	// The number of beats-per-bar is 1/3 of the simple-meter version, thus:
	// m_bpb = m_bpb/3; thus,
	// 6/8 => 2 bpb
	// 9/8 => 3 bpb
	bool m_compound {false};
};

ts_t operator""_ts(const char*, size_t);

//-----------------------------------------------------------------------------
// Class tmetg
//
class tmetg_t {
public:
	tmetg_t() = delete;

	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<note_value>,std::vector<beat_t>);

	// ts, dp, phases, tactus-level
	//explicit tmetg_t(ts_t,std::vector<note_value>,std::vector<beat_t>, note_value);
	
	// list of allowed note_values @ given beat
	std::vector<note_value> which_allowed(beat_t, std::vector<note_value>, int=1) const;

	// Is the input note_value allowed @ the input beat?
	bool allowed_at(beat_t) const;

	// If I put note_value @ beat, is any note value allowed at the next beat?
	bool allowed_next(beat_t,note_value) const;

	std::string print() const;
private:
	ts_t m_ts {beat_t{4.0},note_value{1.0/4.0}};
	std::vector<note_value> m_note_values {};

	// "beat-pool" => number of beats spanned by each element of m_note_values
	std::vector<beat_t> m_beat_values {}; 

	// phase-shift of each note-value level (units == beats)
	std::vector<beat_t> m_ph {};  

	// Grid resolution:  The largest number of beats such that all beat-numbers
	// corresponding to a bar or a note-value can be reached as an integer number
	// increments.  
	beat_t m_btres {0.0};  // grid resolution
	beat_t m_period {0.0}; // The shortest repeating unit

	// The maximum number of subdivisions of the beat used in calculating btres
	static const int m_bt_quantization; 
};

//-----------------------------------------------------------------------------
// non-ts_t class functions
//
beat_t nbeat(ts_t const&, note_value const&);
bar_t nbar(ts_t const&, note_value const&);
std::string printrp(ts_t const&, std::vector<note_value> const&);

std::string rp_t_info();

std::vector<std::vector<int>> tmetg(ts_t, std::vector<note_value>, std::vector<beat_t>);  // ts, dp, phase
//std::string print_tg(std::vector<std::vector<int>> tg);

//  t_start, ts, bpm
std::vector<note_value> tonset2rp(std::vector<double> const&, ts_t const&, 
	double const&, double const&);

std::vector<note_value> tonset2rp_demo();




