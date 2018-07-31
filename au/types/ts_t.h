#pragma once
#include "beat_bar_t.h"
#include "note_value_t.h"
#include <string>




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









