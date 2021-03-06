#pragma once
#include <string>
#include <cstdint>
#include <limits>
#include <array>
#include <type_traits>

// P.134:  
// All MIDI Files should specify tempo and time signature.  If they don't,
// the time signature is assumed to be 4/4, and the tempo 120 beats per 
// minute.
struct midi_timesig_t {
	uint8_t num {4};  // "nn"
	uint8_t log2denom {2};  // denom == std::exp2(log2denom); "dd"
	uint8_t clckspclk {24};  // "Number of MIDI clocks in a metronome tick"; "cc"
	uint8_t ntd32pq {8};  // "Number of notated 32'nd notes per MIDI q nt"; "bb"
};
bool operator==(const midi_timesig_t&, const midi_timesig_t&);

// P.139
// The default-constructed values of si==0, mi==0 => C-major
struct midi_keysig_t {
	int8_t sf {0};  // 0=> key of C;  >0 => n sharps;  <0 => n flats
	int8_t mi {0};  // 0=>major, 1=>minor
};
uint8_t nsharps(const midi_keysig_t&);
uint8_t nflats(const midi_keysig_t&);
bool is_major(const midi_keysig_t&);
bool is_minor(const midi_keysig_t&);

// TODO:  Missing 'select channel mode'
enum : uint8_t {
	note_off = 0x80u,
	note_on = 0x90u,
	key_pressure = 0xA0u,
	ctrl_change = 0xB0u,
	prog_change = 0xC0u,  // 1 data byte
	ch_pressure = 0xD0u,  // 1 data byte
	pitch_bend = 0xE0u
};
struct ch_event_data_t {
	uint8_t status_nybble {0x00u};  // most-significant nybble of the status byte
	uint8_t ch {0x00u};  // least-significant nybble of the status byte
	uint8_t p1 {0x00u};
	uint8_t p2 {0x00u};
	operator bool() const;
};
bool verify(const ch_event_data_t&);
ch_event_data_t make_midi_ch_event_data(int, int, int, int);


// "Forcefully" sets bits in the fields of the input ch_event_data_t such
// that they are valid values.  
// "to_nearest_valid_value()" ?
ch_event_data_t normalize(ch_event_data_t);
bool is_note_on(const ch_event_data_t&);
bool is_note_off(const ch_event_data_t&);
bool is_key_pressure(const ch_event_data_t&);  // 0xAnu
bool is_control_change(const ch_event_data_t&);
bool is_program_change(const ch_event_data_t&);
bool is_channel_pressure(const ch_event_data_t&);  // 0xDnu
bool is_pitch_bend(const ch_event_data_t&); 
bool is_channel_mode(const ch_event_data_t&);



//
// Class time_division_t
//
// Represents a 2-byte time-division field in an MThd chunk.  It is
// impossible for a time_division_t to hold a value that is invalid for a
// time division field.  Ctors silently convert invalid inputs into valid
// values.  
//
// The "support" class smpte_t represents the pair of SMPTE fields; it
// does not enforce any invariants and is only used for as a convenient
// return type for the time_division_t::get_smpte() getter and for the
// SMPTE ctor time_division_t::time_division_t(smpte_t).  
// 
struct smpte_t {
	int32_t time_code;
	int32_t subframes;
};
class time_division_t {
public:
	enum class type : uint8_t {
		ticks_per_quarter,
		smpte
	};

	// 120 tiks-per-quarter; type() == ticks_per_quarter
	time_division_t()=default;
	// Construct a ticks-per-quarter-valued time-division object
	// The input is clamped to [1,32767] (32767==0x7FFF).  
	explicit time_division_t(int32_t);
	// This is declared deleted b/c is is ambiguous; does the user mean for
	// the uint16_t argument to be interpreted as a number of tpq, or as a
	// "raw value," as by make_time_division_from_raw(uint16_t)?
	explicit time_division_t(uint16_t)=delete;
	// Construct a SMPTE-valued time-division object
	// Allowed values for arg 1 (SMPTE-time-code) are -24, -25, -29, -30.  
	// If an invalid value is passed in, -24 is substituted.  The value for
	// arg 2 (subdivisions-per-frame) is clamped to [1,255].  
	explicit time_division_t(int32_t, int32_t);
	explicit time_division_t(smpte_t);
	
	// Getters
	// If called on an object of the wrong type, the value returned 
	// will be valid as the requested type of quantity, but will not 
	// correspond to the value held in the object.  A time_division_t 
	// /never/ holds or returns invalid data.  
	// See also the non-member T get_*(...) family of functions
	time_division_t::type get_type() const;
	uint16_t get_raw_value() const;
	smpte_t get_smpte() const;
	int32_t get_tpq() const;

	bool operator==(const time_division_t&) const;
	bool operator!=(const time_division_t&) const;
private:
	// SMPTE => Society of Motion Picture and Television Engineers 
	// 16-bit:  [[1] frames-per-second] [resolution-within-frame]
	// Number of subframes per frame ("units per frame," "ticks per frame")
	// Legal values (std p.133): "4 (MIDI time code resolution),
	// 8, 10, 80 (bit resolution), or 100"
	std::array<unsigned char,2> d_ {0x00u,0x78u};  // 120 ticks-per-quarter
};

//
// maybe_time_division_t make_*() "Factory" functions
//
// For invalid input, .is_valid == false, however, the time_division_t 
// object in .value will always contain a valid value (a consequence of the
// hard invariants enfoeced by the time_divison_t class).  Where is_valid
// == false, the object in .value obviously does not correspond to the 
// values passed to the make_*() function.  
//
struct maybe_time_division_t {
	time_division_t value;
	bool is_valid {false};
};
// maybe_time_division_t
// make_time_division_smpte(int32_t SMPTE-time-code, int32_t subdivs);
// maybe_time_division_t
// make_time_division_tpq(int32_t ticks-per-quarter-note);
maybe_time_division_t make_time_division_smpte(smpte_t);
maybe_time_division_t make_time_division_smpte(int32_t,int32_t);
maybe_time_division_t make_time_division_tpq(int32_t);
// maybe_time_division_t
// make_time_division_from_raw(int16_t raw_val);
// raw_val is the value of the d_ array "deserialized" as a BE-encoded 
// quantity; same as returned by the time_division_t::raw_value() getter.  
maybe_time_division_t make_time_division_from_raw(uint16_t);
bool is_smpte(time_division_t);
bool is_tpq(time_division_t);
bool is_valid_time_division_raw_value(uint16_t);
//
// T get_*(time_division_t tdiv) functions
//
// These are provided alternatives to the time_division_t-member getter  
// methods T time_division_t::get_*().  The member getters always return
// a valid value, even where a getter is called on an object of incorrect 
// type (ex, if time_division_t::get_smpte() is called on a time_division_t
// with type()==time_division_t::type::ticpq_per_quarter).  These non-member 
// "getters" return a caller-provided default value if the time_division_t
// has type() not corresponding to the getter.  
// 
smpte_t get_smpte(time_division_t, smpte_t={0,0});
int32_t get_tpq(time_division_t, int32_t=0);


// double ticks2sec(const int32_t& ticks, const time_division_t& tdiv, 
//					int32_t tempo=500000);
// For a smpte tdiv, the tempo argument is ignored, and 
// seconds = ticks/(smpte.time_code*smpte_subframes)
// For a tpq tdiv, 
// seconds = ticks*(tempo/tpq)
double ticks2sec(int32_t, const time_division_t&, int32_t=500000);
// int32_t sec2ticks(const double& sec, const time_division_t& tdiv,
//						int32_t tempo=500000);
// For a smpte tdiv, the tempo argument is ignored, and 
// ticks = sec/(smpte.time_code*smpte_subframes)
// For a tpq tdiv, 
// ticks = sec*(tpq/tempo)
int32_t sec2ticks(const double&, const time_division_t&, int32_t=500000);



