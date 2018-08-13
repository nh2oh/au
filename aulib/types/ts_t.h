#pragma once
#include "beat_bar_t.h"
#include "nv_t.h"
#include <string>
#include <optional>

//
// Class ts_t
// Represents either a simple or compound time signature.  
// Simple:
// (note-value == 1 beat) / (num-beats-per-bar)
// Compound:
//   Each beat of the bar divides into 3 equal parts
//   Numerator => 3*(the number of into-3 divisions of the beat) in 
//				  each bar.  So if the number is 6, there are 2 "into-3"
//				  divisions; if the number is 12, there are 4 "into-3"
//				  divisions.
//				  6/8 => 2 bpb
//				  9/8 => 3 bpb
//				  etc...
//   Denominator => The note value for which a group of 3 correspond to 
//					one beat.  
//
class ts_t {
public:
	explicit ts_t();
	explicit ts_t(beat_t const&, nv_t const&, bool const& =false); // num, denom, is-compound
	explicit ts_t(std::string const&);  // Calls ts_t.from_string()

	nv_t beat_unit() const;
		// The nv_t corresponding to one beat (== m_beat_unit).  
		// If the ts is simple, this is the denominator.  If compound,
		// it's the nv_t w/ twice the duration of the denominator, then
		// dotted once.  

	nv_t bar_unit() const;  // The note value that would span 1 bar
	beat_t beats_per_bar() const;  // Numerator=>beats-per-bar
	std::string print() const;

	bool operator==(ts_t const&) const;
private:
	void from_string(std::string const&);  // Delegated constructor

	nv_t m_beat_unit {1.0/4.0};  // The nv_t corresponding to one beat
	beat_t m_bpb {4.0};  // Beats per bar (numerator)
	bool m_compound {false};  // False => simple
};

ts_t operator""_ts(const char*, size_t);

bool operator!=(ts_t const&, ts_t const&);


// Time signature user input helper
//
// The purpose of this class is to provide a container to hold std::string
// input entered by a user, say, on a gui form, and to make decisions about
// whether or not it is valid and can be parsed to a ts_t.  
//
//
class ts_uih {
public:
	ts_uih();
	ts_uih(std::string const&);
	// ts_uih(std::string const&, predicates...);  Future...
	// Also possibly some way to set custom messages, flags dfns...

	void update(std::string const&);
	bool is_valid() const;
	int flags() const;
	ts_t get() const;

	bool operator==(ts_uih const&) const;
private:
	struct ts_str_parts {
		double bt_per_bar {0.0};
		double nv_per_bt {0.0};
		bool is_compound {false};
	};
	std::optional<ts_str_parts> parse_ts_str_() const;
		// Parses str_last_; does not set any internal variables (note the const
		// declaration).  Called by the constructor and by update().  

	bool is_valid_ {false};
	std::string msg_ {""};
	int flags_ {0};
	std::string str_last_ {""};
		// The std::string passed to the most recent call to update(), or that
		// passed to the constructor if update() has not yet been called.
		// Allows the object to decide if any change has been made and make a
		// decision if further processing is needed.  

	std::optional<ts_t> ts_ {};
		// If str_last_ can be parsed to create a ts_t, ts_ is that ts_t.  
		// std::optional is used to prevent imposing some sort of "default" ts
		// on a user of the ts_uih class.  Since the constructor takes a 
		// string, not a ts_t object, there's a chance that the ts_uih is
		// created with an "invalid" input.  This is ok where the argument to
		// the constructor comes from a user.  
};

