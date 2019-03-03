#pragma once
#include <algorithm>  // std::reverse_copy() in midi_raw_interpret()
#include <string>
#include <vector>
#include <array>
#include <limits> // CHAR_BIT
#include <type_traits> // std::enable_if

//
// TODO:  validate_, parse_, detect_ naming inconsistency
//

// 
// Copies the bytes in the range [p,p+sizeof(T)) into the range occupied by a T such that the
// byte order in the source and destination ranges are the reverse of oneanother.  Hence
// big-endian encoded T in [p,p+sizeof(T)) is corrrectly interpreted on an LE architecture.  
// Obviously this byte order swapping is only needed for interpreting midi files on LE 
// architectures.  
// 
/*template<typename T>
T midi_raw_interpret(const unsigned char* p) {
	T result {};
	unsigned char *p_result = static_cast<unsigned char*>(static_cast<void*>(&result));
	std::reverse_copy(p,p+sizeof(T),p_result);
	return result;
};*/

//
// SMF files are big endian; be_2_native() interprets the bytes in the range
// [p,p+sizeof(T)) such that a BE-encode integer is interpreted correctly on
// both LE and BE architectures.  
//
template<typename T, typename = typename std::enable_if<std::is_integral<T>::value,T>::type>
T be_2_native(const unsigned char *p) {
	T result {0};
	for (int i=0; i<sizeof(T); ++i) {
		result = result << CHAR_BIT;
		result += *(p+i);
		// Note that the endianness of the native architecture is irrelevant; the 
		// MSB is always going to be left-shifted CHAR_BIT*(sizeof(T)-1) times.  
	}
	return result;
};

template<typename T, typename = typename std::enable_if<std::is_integral<T>::value,T>::type>
T le_2_native(const unsigned char *p) {
	T result {0};
	for (int i=0; i<sizeof(T); ++i) {
		result += (*(p+i) << CHAR_BIT);
	}
	return result;
};


// 
// The max size of a vl field is 4 bytes; returns [0,4]
//
struct midi_vl_field_interpreted {
	int8_t N {0};
	int32_t val {0};  // TODO:  uint32_t ???
};
midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char*);


//
// Encodes T in the form of a VL quantity, the maximum size of which, according
// to the MIDI std is 4 bytes.  
// For values requiring less than 4 bytes in encoded form, the rightmost
// bytes of the array will be 0.  
//
template<typename T>  // T ~ integral
std::array<unsigned char,4> midi_encode_vl_field(T val) {
	static_assert(sizeof(T)<=4);  // The max size of a vl field is 4 bytes
	std::array<unsigned char,4> result {0x00,0x00,0x00,0x00};

	int i = 3;
	result[i] = val & 0x7F;
	while (i>0 && (val >>= 7) > 0) {
		--i;
		result[i] |= 0x80;
		result[i] += (val & 0x7F);
	}

	// Shift the elements of result to the left so that result[0] contains 
	// the first nonzero byte and any zero bytes are at the end of result (beyond
	// the first byte w/ bit 7 == 0).  
	for (int j=0; j<4; ++j) { 
		if (i<=3) {
			result[j] = result[i];
			++i;
		} else {
			result[j] = 0x00;
		}
	}

	return result;
};


std::string print_hexascii(const unsigned char*, int, const char = ' ');


//
// There are two types of chunks: the Header chunk, containing data pertaining to the entire file 
// (only one per file), and the Track chunk (possibly >1 per file).  Both have a length field 
// that is is always 4 bytes (is not a vl-type quantity).  From p. 132: "Your programs should 
// expect alien chunks and treat them as if they weren't there."  
//
enum class chunk_type {
	header,  // MThd
	track,  // MTrk
	unknown,  // The std requires that unrecognized chunk types be permitted
	invalid
};
//
// Checks for the 4-char id and the 4-byte size.  Verifies that the id + size field 
// + the reported size does not exceed the max_size supplied as the second argument.  
// Does _not_ inspect anything past the end of the length field.  
//
struct detect_chunk_type_result_t {
	chunk_type type {chunk_type::invalid};
	int32_t size {0};  // byte header + reported length
	int32_t data_length {0};  // reported length (not inclusing the 8 byte header)
	std::string msg {};
};
detect_chunk_type_result_t detect_chunk_type(const unsigned char*, int32_t=0);

struct validate_mthd_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
	int32_t data_length {0};  // the reported length; does not include the "MThd" and length fields
	int32_t size {0};  //  Always == reported size (data_length) + 8
	const unsigned char *p {};  // points at the 'M' of "MThd"...
};
validate_mthd_chunk_result_t validate_mthd_chunk(const unsigned char*, int32_t=0);

struct validate_mtrk_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
	int32_t data_length {0};  // the reported length; does not include the "MTrk" and length fields
	int32_t size {0};  //  Always == reported size (data_length) + 8
	const unsigned char *p {};  // points at the 'M' of "MTrk"...
};
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*, int32_t=0);


//
// There are no sys_realtime messages in an mtrk event stream.  The set of valid sys_realtime 
// status bytes includes 0xFF, which is the leading byte for a "meta" event.  
//
// Why do i include "invalid," which is clearly not a member of the "class" of things-that-are-
// smf-events?  Because users switch behavior on functions that return a value of this type (ex,
// while iterating through an mtrk chunk, the type of event at the present position in the chunk
// is detected by parse_mtrk_event()).  I want to force users to deal with the error case rather
// than relying on the convention that some kind of parse_mtrk_event_result.is_valid field be 
// checked before moving forward with parse_mtrk_event_result.detected_type.  
//
// Sysex events and meta-events cancel any running status which was in effect.  Running status
// does not apply to and may not be used for these messages (p.136).  
//
enum class smf_event_type {  // MTrk events
	channel_voice,
	channel_mode,
	sysex_f0,
	sysex_f7,
	meta,
	invalid
};
std::string print(const smf_event_type&);
//
// parse_mtrk_event_type() will parse & validate the delta-t field and evaluate the status
// byte immediately following.  It will _not_ validate other quantities internal to the event
// (ie, beyond the status byte) such as the <length> field in sysex_f0/f7 events.  Will return
// smf_event_type::invalid under the following circumstances:
// -> the delta_t field is invalid (ex: > 4 bytes) or > max_size
// -> the size of the delta_t field + the calculated num-data/status-bytes > max_size
// -> the byte immediately following the delta_t field is not a status byte AND (s is not
//    a status byte, or is a disallowed running_status status byte such as FF, F0, F7).  
// -> an 0xB0 status byte is not followed by a valid data byte
// Note that except in the one limited case above, the data bytes are not validated as plausible
// data bytes.  This is up to the relevant parse_() function.  
//
// It would be really nice if this could return the size of the event, however, the size 
// calculation is different for sysex, meta, and channel events, and in the latter case can not
// even be made without knowing the present midi_status (for running_status streams).  Although
// I am trying to treat mtrk events "polymorphically," since all events have a size, that size
// can only be calculated once the event has been classified/parsed.  
//
//
struct parse_mtrk_event_result_t {
	midi_vl_field_interpreted delta_t {};
	smf_event_type type {smf_event_type::invalid};
};
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char*, unsigned char, int32_t=0);
//
// Pointer to the first data byte following the delta-time, _not_ to the start of the delta-time.  
// This is the most lightweight status-byte classifier that i have in this lib.  The pointer may
// have to be incremented by 1 byte to classify an 0xB0 status byte as channel_voice or 
// channel_mode.  In this case, will return smf_event_type::invalid if said data byte is an  
// invalid data byte (has its high bit set).  
//
// For meta and sysex_f0 messages (status byte == 0xFF, 0xF0 or 0xF7) the length field subsequent to
// the status byte is not validated.  It *only* classifies the status byte (and if neessary to make
// the classification, reads the first data byte).  
//
smf_event_type detect_mtrk_event_type_unsafe(const unsigned char*, unsigned char=0);
smf_event_type detect_mtrk_event_type_dtstart_unsafe(const unsigned char*, unsigned char=0);

struct parse_meta_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	midi_vl_field_interpreted length {};
	unsigned char type {0};
	int32_t size {0};
	int32_t data_length {};  // Everything not delta_time
};
parse_meta_event_result_t parse_meta_event(const unsigned char*,int32_t=0);
struct parse_sysex_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	midi_vl_field_interpreted length {};
	uint8_t type {0};  // F0 or F7
	int32_t size {0};
	int32_t data_length {};  // Everything not delta_time
	bool has_terminating_f7 {false};
};
parse_sysex_event_result_t parse_sysex_event(const unsigned char*,int32_t=0);
enum class channel_msg_type {
	note_on,
	note_off,
	key_pressure,
	control_change,
	program_change,
	channel_pressure,
	pitch_bend,
	channel_mode,
	invalid
};
// NB:  validate_mtrk_chunk() keeps track of the most recent status byte by assigning from the 
// status_byte field of this struct.  Change this before adopting any sort of sign convention for
// implied running_status.  
struct parse_channel_event_result_t {
	bool is_valid {false};
	channel_msg_type type {channel_msg_type::invalid};
	midi_vl_field_interpreted delta_t {};
	bool has_status_byte {false};
	unsigned char status_byte {0};
	uint8_t n_data_bytes {0};  // 0, 1, 2
	int32_t size {0};
	int32_t data_length {0};  // Everything not delta_time
};
parse_channel_event_result_t parse_channel_event(const unsigned char*, unsigned char=0, int32_t=0);
bool midi_event_has_status_byte(const unsigned char*);
unsigned char midi_event_get_status_byte(const unsigned char*);
// Result is only valid for channel_voice or channel_mode status bytes:  Does not 
// verify that the input is a legit channel_voice or _mode status byte.  
int8_t channel_number_from_status_byte_unsafe(unsigned char);
// arg 1 => status byte, arg 2 => first data byte, needed iff arg1 & 0xF0 == 0xB0.  
// Not really "_unsafe"-worthy since will return channel_msg_type::invalid if the
// status byte is not legit.  
channel_msg_type channel_msg_type_from_status_byte(unsigned char, unsigned char=0);


struct chunk_idx_t {
	chunk_type type {};
	int32_t offset {0};
	int32_t size {0};
};
struct validate_smf_result_t {
	bool is_valid {};
	std::string msg {};

	// Needed by the smf_container_t ctor
	std::string fname {};
	const unsigned char *p {};
	int32_t size {0};
	int32_t n_mtrk {0};  // Number of MTrk chunks
	int32_t n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};
};
validate_smf_result_t validate_smf(const unsigned char*, int32_t, const std::string&);

