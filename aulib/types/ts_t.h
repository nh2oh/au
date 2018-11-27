#pragma once
#include "beat_bar_t.h"
#include "nv_t.h"
#include <string>
#include <vector>  // declaration of cum_nbar(...)

//
// Class ts_t
// Represents a time signature.  The default constructed value is equivalent to ts_t {"4/4"}.  
//
// There are two types of time signature, "simple" and "compound."  
// Simple:
// (note-value == 1 beat) / (num-beats-per-bar)
//
// Compound:
// Each beat of the bar divides into 3 equal parts.  
// Numerator => 3*(the number of into-3 divisions of the beat)-per-bar.  Thus if the numerator
//   is 6, there are 2 "into-3" divisions of the beat in each bar; if the numerator is 12, 
//   there are 4 "into-3" divisions of the beat in each bar.  
//   6/8 => 2 beats-per-bar
//   9/8 => 3 beats-per-bar
//   etc...
// Denominator => The note value for which a group of 3 corresponds to a single beat.  Thus if 
//   the denominator is 8, 3 eighth-notes span 1 beat.  
//   A group of 3 notes of equal note-value d has the same duration as a single note of 
//   duration 2*(d + (1/2)*d) (a 2*d note with a single dot).  For example:
//   Three eithth notes:  (1/8) (1/8) (1/8) == (1/4). (a quarter note w/ 2 dots).  
//   Three (1/6). notes == (1/8).. (an eighth note w/ 2 dots).  
//
//

//
// TODO:  Why can i not introspect is_compound() ?
// TODO:  Unit tests for parse_ts_string().  
//
class ts_t {
public:
	ts_t() = default;
	explicit ts_t(const beat_t&, const d_t&, bool =false);  // num, denom, is-compound
	explicit ts_t(const std::string&);  // Calls ts_t.from_string()

	d_t beat_unit() const;
		// The d_t corresponding to one beat (== m_beat_unit).  
		// If the ts is simple, this is the denominator.  If compound,
		// it's the d_t w/ twice the duration of the denominator, then
		// dotted once.  

	d_t bar_unit() const;
		// The note value that spans exactly 1 bar
	
	beat_t beats_per_bar() const;
		// The number of beats per bar:  For a simple ts, this is the
		// numerator; for a compound ts, it's (1/3)*(the numerator).  

	std::string print() const;

	bool operator==(const ts_t&) const;
private:
	void from_string(const std::string&);  // Called by ts_t(std::string const&);

	d_t m_beat_unit {1.0/4.0};  // The d_t corresponding to one beat
	beat_t m_bpb {4.0};  // Beats per bar (numerator)
	bool m_compound {false};  // False => simple
};

ts_t operator""_ts(const char*, size_t);

bool operator!=(const ts_t&, const ts_t&);

//
// If !ts_str_parsed.is_valid, the values of all other fields should be regarded as "undefined."  
//
// Note that if the input string is valid, parse_ts_string() calls the ts_t(beat_t,d_t,bool)
// ctor to populate ts_str_parsed.ts.  Thus, the ts_t(std::string) ctor can not (or rather
// _should_ not) delegate to this function.  
//
// The numerator and denominator fields of the input str are returned as doubles in 
// ts_str_parsed.  If the caller wants these as a beat_t, d_t, they can just call ts.beat_unit,
// ts.bar_unit(), etc.  
//
struct ts_str_parsed {
	bool is_valid {false};
	bool is_compound {false};
	double num {0};  
	double denom {0};
	ts_t ts {};
};
ts_str_parsed parse_ts_string(const std::string&);


// Number of beats spanned by a given nv_t or bar_t.  
beat_t nbeat(const ts_t&, const d_t&);
beat_t nbeat(const ts_t&, const bar_t&);
beat_t nbeat(const ts_t&, const std::vector<d_t>&);  // _Total_ nbts
d_t duration(const ts_t&, beat_t);

bar_t nbar(const ts_t&, const d_t&);  // _Total_ nbars
bar_t nbar(const ts_t&, const beat_t&);
bar_t nbar(const ts_t&, const std::vector<d_t>&);
std::vector<bar_t> cum_nbar(const ts_t&, const std::vector<d_t>&);


struct err_accum {
	std::vector<double> nbar_err {};
	//std::vector<double> nbeat_err {};
};
err_accum err_nbeat_accum(ts_t,d_t,int,int,bar_t);
