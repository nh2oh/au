#pragma once
#include "beat_bar_t.h"
#include "nv_t.h"
#include <string>
#include <vector>  // declaration of cum_nbar(...)

//
// Class ts_t
// Represents a time signature.  The default constructed value is equivalent
// to ts_t {"4/4"}.  
//
// There are two types of time signature, "simple" and "compound."  
// Simple:
// (note-value == 1 beat) / (num-beats-per-bar)
//
// Compound:
// Each beat of the bar divides into 3 equal parts.  
// Numerator => 3*(the number of into-3 divisions of the beat)-per-bar.  
//	 So if the numerator is 6, there are 2 "into-3" divisions; if the
//   numerator is 12, there are 4 "into-3" divisions of the beat in each
//   bar.  
//   6/8 => 2 beats-per-bar
//   9/8 => 3 beats-per-bar
//   etc...
// Denominator => The note value for which a group of 3 corresponds to a
//   beat.  So if the denominator is 8, 3 eighth-notes span 1 beat.  
//   Since a group of 3 notes, call them d, of the same note-value has 
//   the same duration as a single note of duration twice d, then dotted
//   once.  That is, three eithth notes (1/8) (1/8) (1/8) == (1/4). and
//   three (1/6). notes == (1/8).. (an eighth note w/ 2 dots).  
//
//
class ts_t {
public:
	ts_t() = default;
	explicit ts_t(beat_t const&, d_t const&, bool const& =false); // num, denom, is-compound
	explicit ts_t(std::string const&);  // Calls ts_t.from_string()

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

	bool operator==(ts_t const&) const;
private:
	void from_string(std::string const&);
		// Called by ts_t(std::string const&);

	d_t m_beat_unit {1.0/4.0};  // The d_t corresponding to one beat
	beat_t m_bpb {4.0};  // Beats per bar (numerator)
	bool m_compound {false};  // False => simple
};

ts_t operator""_ts(const char*, size_t);

bool operator!=(ts_t const&, ts_t const&);



// Number of beats spanned by a given nv_t or bar_t.  
beat_t nbeat(ts_t const&, d_t const&);
beat_t nbeat(ts_t const&, bar_t const&);
// Number of bars spanned by a given nv_t or d_t.  
bar_t nbar(ts_t const&, d_t const&);
bar_t nbar(ts_t const&, beat_t const&);
std::vector<bar_t> cum_nbar(ts_t const&, std::vector<d_t> const&);


