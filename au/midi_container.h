#include "..\aulib\input\midi_util.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

namespace mc {

void midi_example();



//
// TODO:  validate_, parse_, detect_ naming inconsistency
//



std::string print_hexascii(const unsigned char*, int, const char = ' ');


//
// There are two types of chunks: the Header chunk, containing data pertaining to the entire file 
// (only one per file), and the Track chunk (possibly >1 per file).  Both chunk types have the 
// layout implied by struct midi_chunk.  Note that the length field is always 4 bytes and is not a
// vl-type quantity.  From p. 132: "Your programs should expect alien chunks and treat them as if
// they weren't there."  
//
enum class chunk_type {
	header,  // MThd
	track,  // MTrk
	unknown  // The std requires that unrecognized chunk types be permitted
};
enum event_type {  // MTrk events
	midi,
	sysex,
	meta
};
std::string print(const event_type&);

struct parse_meta_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	// FF <type> <length> <bytes>N+
	//uint8_t size {0};  // does not include delta_time; is 2+length.N+length.val
	//uint8_t id {0};  // FF
	uint8_t type {0};
	int32_t data_size {};  // Everything not delta_time
};
parse_meta_event_result_t parse_meta_event(const unsigned char*);

struct parse_sysex_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	// F0 (sometimes F7 ...see std) <length> <bytes to be transmitted after F0||F7>
	//uint8_t size {0}; // does not include delta_time; is 1+length.N+length.val
	uint8_t type {0};  // F0 or F7
	int32_t data_size {};  // Everything not delta_time
};
parse_sysex_event_result_t parse_sysex_event(const unsigned char*);

struct parse_midi_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	bool has_status_byte {false};
	uint8_t status_byte {0};
	uint8_t n_data_bytes {0};  // 0, 1, 2
	// does not include delta_time;  0 || 1 (status) + 1 || 2 (data) => 1 || 2 || 3
	int32_t data_size {};  // Everything not delta_time
};
parse_midi_event_result_t parse_midi_event(const unsigned char*, unsigned char=0);
bool midi_event_has_status_byte(const unsigned char*);
unsigned char midi_event_get_status_byte(const unsigned char*);





//
// Checks for the 4-char id and the 4-byte size.  Sanity check on the value of the
// size.  
//
struct detect_chunk_type_result_t {
	chunk_type type {chunk_type::unknown};
	int32_t size {0};
	std::string msg {};
	bool is_valid {false};
		// False => Does not begin w/a 4 char ASCII id & a 4-byte length, or possibly
		// the length => a buffer overrun.  
	const unsigned char* p {};
};
detect_chunk_type_result_t detect_chunk_type(const unsigned char*, int32_t);

// Why not a generic midi_chunk_container_t<T> template?  Because MThd and MTrk containers
// are radically different and it really does not make sense to write generic functions to 
// operate on either type.  
class mthd_container_t {
public:
	mthd_container_t(const detect_chunk_type_result_t&);

	// NB:  Second arg is the _exact_ size, not a max size
	mthd_container_t(const unsigned char *p, int32_t sz) 
		: p_(p),size_(sz) {};

	int16_t format() const;
	int16_t ntrks() const;
	uint16_t division() const;

	// Does not include the 4 byte "MThd" and 4 byte data-length fields
	int32_t data_size() const;
	// Includes the "MThd" and data-length fields
	int32_t size() const;	
private:
	const unsigned char *p_ {};
	int32_t size_ {0};
};

enum class midi_time_division_field_type_t {
	ticks_per_quarter,
	SMPTE
};
midi_time_division_field_type_t detect_midi_time_division_type(uint16_t);
uint16_t interpret_tpq_field(uint16_t);  // assumes midi_time_division_field_type_t::ticks_per_quarter
struct midi_smpte_field {
	int8_t time_code_fmt {0};
	uint8_t units_per_frame {0};
};
midi_smpte_field interpret_smpte_field(uint16_t);  // assumes midi_time_division_field_type_t::SMPTE
std::string print(const mthd_container_t&);



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


//
// mtrk_event_container_t
//
// This is a convienience class for working with mtrk events event_type:: midi, sysex 
// && meta.  If you have one of these you have a ptr to a legal, validated mtrk event.  
// The event in question may or may not be part of a sequence of events as in an MTrk section
// of an smf.  In the case of midi events occuring as part of such a sequence, "running 
// status" may be in effect, in which case it is not possible to detect and assign midi 
// events at arbitrary locations internal to the sequence; the relevant status byte may have
// been set in a previous midi event, possibly distantly removed from the present event.  
// For example, for a midi event without a status byte, if the byte after the first data byte 
// begins w/ 0, it can not be distingished from the first byte of a single-byte delta_time vl
// quantity for the next event.  
//
// An mtrk_event_container_t associates the size (number of bytes) with the pointer to the
// event, hence even if running status is in effect, a pointer into an MTrk sequence in the
// form of a valid mtrk_event_container_t can be used to extract an entire complete event
// (though in the case of midi events you still may not be able to determine the value of the
// applicible status byte).  This allows an mtrk_event_container_t to act as a handle to any
// concrete MTrk event in memory, since if it exists in memory it _must_ have a size, 
// regardless of whether or not that size can be determined by examining *p and *(p+n) alone.  
// Midi status is not a data member because it must be possible to create and work with 
// events to be used in a running-status context which are completely valid but obviously will
// lack an explicit status - for example, if writing a program to write out a novel smf.  
// Nevertheless, to exist at all these events must ipso facto have a concrete size.  
//
//
// The four methods are applicable to _all_ mtrk events are:
// (1) delta_time(mtrk_event_container_t)
// (2) event_type(mtrk_event_container_t) => sysex, midi, ...
// (3) non-interpreted print:  <delta_t>-<type>-hex-ascii (note that the size is needed for this)
// (4) data_size() && size()
//
//
class mtrk_event_container_t {
public:
	mtrk_event_container_t(const unsigned char *p, int32_t sz)
		: p_(p), size_(sz) {};

	// size of the event not including the delta-t field (but including the length field 
	// in the case of sysex & meta events)
	int32_t delta_time() const;
	event_type type() const;  // enum event_type:: midi || sysex || meta
	int32_t data_size() const;  // Does not include the delta-time
	int32_t size() const;  // Includes the delta-time

	// TODO:  Replace with some sort of "safer" iterator type
	const unsigned char *begin() const;  // Starts at the delta-time
	const unsigned char *data_begin() const;  // Starts just after the delta-time
	const unsigned char *end() const;
private:
	const unsigned char *p_ {};
	int32_t size_ {0};  // delta_t + payload
};

struct parse_mtrk_event_result_t {
	bool is_valid {false};  // False if the delta-t vl field does not validate
	midi_vl_field_interpreted delta_t {};
	event_type type {event_type::midi};
};
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char*);
std::string print(const mtrk_event_container_t&);
event_type detect_mtrk_event_type_dtstart_unsafe(const unsigned char*);
event_type detect_mtrk_event_type_unsafe(const unsigned char*);

//
// mtrk_container_t & friends
//
// As with the mtrk_event_container_t above, an mtrk_container_t is little more than an
// assurance that the pointed to range is a valid MTrk chunk.  This is still useful, however:
// The event sequence pointed to by an mtrk_container_t object has already been validated 
// and can therefore be parsed using the fast parse_*_usafe(const unsigned char*) family 
// of functions.  
//
//
class mtrk_container_iterator;
class mtrk_container_t;

struct validate_mtrk_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
	const unsigned char *p;
	int32_t size {0};
};
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*, int32_t);

// Obtained from the begin() && end() methods of class mtrk_container_t.  
class mtrk_container_iterator {
public:
	mtrk_container_iterator(const mtrk_container_t* c, int32_t o, unsigned char ms)
		: container_(c), container_offset_(o), midi_status_(ms) {};
	mtrk_event_container_t operator*() const;
	mtrk_container_iterator& operator++();
	bool operator<(const mtrk_container_iterator&) const;
	bool operator==(const mtrk_container_iterator&) const;
	bool operator!=(const mtrk_container_iterator&) const;
private:
	const mtrk_container_t *container_ {};
	int32_t container_offset_ {0};  // offset from this->container_.beg_
	unsigned char midi_status_ {0};
};

class mtrk_container_t {
public:
	mtrk_container_t(const validate_mtrk_chunk_result_t&);
	mtrk_container_t(const unsigned char*, int32_t);

	int32_t data_size() const;
	int32_t size() const;
	mtrk_container_iterator begin() const;
	mtrk_container_iterator end() const;
private:
	const unsigned char *p_ {};  // Points at "MTrk..."
	int32_t size_ {0};
	friend class mtrk_container_iterator;
};

std::string print(const mtrk_container_t&);

//
// smf_container_t & friends
//
// As was the case for the mtrk_event_container_t and mtrk_container_t above, an 
// smf_container_t is nothing more than an assurance that the pointed to range is a valid 
// smf.  It contains exactly one MThd chunk and one or more unknown or MTrk chunks.  The
// sizes of all chunks are validated, so the chunk_container_t returned from the appropriate
// methods are guaranteed correct.  Further, for the MThd and Mtrk chunk types, all sub-data
// is validated (ie, all events in an MTrk are valid).
//
//
struct chunk_idx_t {
	chunk_type type {};
	int32_t offset {0};
	int32_t size {0};
};
struct validate_smf_result_t {
	bool is_valid {};
	std::string msg {};
	const unsigned char *p {};
	int32_t size {0};
	int n_mtrk {0};  // Number of MTrk chunks
	int n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};
};
//validate_smf_result_t validate_smf(const std::filesystem::path&);
validate_smf_result_t validate_smf(const unsigned char*, int32_t);

class smf_container_t {
public:
	smf_container_t(const validate_smf_result_t&);

	mthd_container_t get_header() const;
	mtrk_container_t get_track(int) const;
	bool get_chunk(int) const;  // unknown chunks allowed...
	
	int n_tracks() const;
	int n_chunks() const;  // Includes MThd
private:
	std::vector<chunk_idx_t> chunk_idxs_ {};

	// Total number of chunks is n_unknown + n_mtrk + 1 (MThd)
	int8_t n_mtrk_ {0};
	int8_t n_unknown_ {0};
	const unsigned char *p_ {};
	int32_t size_ {0};
};

std::string print(const smf_container_t&);
bool play(const smf_container_t&);



//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//
// An MTrk event stream has global state from at least 4 sources:
// 1) Time - the onset time for event n depends on the delta-t vl field for all prior
//    events.  
// 2) Midi status - the midi status byte for event n may not be specified; it may
//    be inherited from some prior event.  
// 3) The interpretation of event n depends in general on all prior control/program-
//    change events.  These may occur mid-stream any number of times (ex, the 
//    instrument could change in the middle of a stream).  
// 4) Status from opaque sysex or meta events not understood by the parser.  
//
// Without taking a huge amount of memory, it is probably not possible to obtain
// random, O(1) access to the values of all of the state variables in an MTrk event
// stream.  
//
//


//
// For Format 1 files, combines the events of all tracks into a single array where
// events are ordered first by onset time, then by track number.  
//
class smf_t {
public:
private:
	std::vector<unsigned char> data_ {};  // raw file data; whole file
};

















/*
class indexed_smf_t {
public:
	struct yikes_t {
		// The largest vl-quantity allowed is 0x0FFFFFFF, hence the MS-nybble can be used as
		// a flag.  0 => payload is the message, 1 => payload is a "ptr" to the message.  
		std::array<unsigned char,4> dt_flag {};
		std::array<unsigned char,8> payload {};
	};
private:
	// Format 1 => Tracks are played simultaneously
	// All midi events contain their status byte.  Since here i am storing different tracks 
	// interleaved, need to deal with the fact that different tracks can have different 
	// running-status at the same time.  
	struct event_idx_t {
		int32_t t_on {0};  // Cumulative
		int32_t offset {0};  // into member "augmented" MTrk chunk vector
		int8_t track_num {0};  // 0 => MThd, ...
	};

	struct event_idx2_t {
		int8_t track {0};
		yikes_t d {};
	};

	std::vector<std::vector<>> didx_ {};  // in-file order of the chunks

	std::vector<unsigned char> data_ {};  // raw file data; whole file
};
*/

};  // namespace mc


