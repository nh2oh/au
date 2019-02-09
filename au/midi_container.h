#include "..\aulib\input\midi_util.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

namespace mc {


//
// Separates the unsafe functions of unsigned char* from safer user-facing classes.  
//

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
	unknown
};
enum event_type {
	midi,
	sysex,
	meta
};

struct parse_meta_event_result_t {
	bool is_valid {false};
	// FF <type> <length> <bytes>N+
	uint8_t size {0};  // does not include delta_time; is 2+length.N+length.val
	uint8_t id {0};  // FF
	uint8_t type {0};
	midi_vl_field_interpreted length {};
};
parse_meta_event_result_t parse_meta_event(const unsigned char*);

struct parse_sysex_event_result_t {
	bool is_valid {false};
	// F0 (sometimes F7 ...see std) <length> <bytes to be transmitted after F0||F7>
	uint8_t size {0}; // does not include delta_time; is 1+length.N+length.val
	uint8_t id {0};  // F0 or F7
	midi_vl_field_interpreted length {};
};
parse_sysex_event_result_t parse_sysex_event(const unsigned char*);

struct parse_midi_event_result_t {
	bool is_valid {false};
	bool running_status {false};
	uint8_t size {0};  // does not include delta_time;  0 || 1 (status) + 1 || 2 (data) => 1 || 2 || 3
};
parse_midi_event_result_t parse_midi_event(const unsigned char*, unsigned char=0);
bool midi_event_has_status_byte(const unsigned char*);
unsigned char midi_event_get_status_byte(const unsigned char*);







struct detect_chunk_type_result_t {
	chunk_type type {chunk_type::unknown};
	std::string msg {};
	bool is_valid {};
		// False => Does not begin w/a 4 char ASCII id & a 4-byte length, or possibly
		// the length => a buffer overrun.  
};
detect_chunk_type_result_t detect_chunk_type(const unsigned char*);

// Why not a generic midi_chunk_container_t<T> template?  Because MThd and MTrk containers
// are radically different and it really does not make sense to write generic functions to 
// operate on either type.  
class mthd_container_t {
public:
	mthd_container_t()=default;
	mthd_container_t(const unsigned char *p) : p_(p) {};

	std::array<char,4> id() const {
		std::array<char,4> result {};
		std::copy(p_,p_+4,result.begin());
		return result;
	};

	int32_t data_length() const {
		return midi_raw_interpret<int32_t>(p_+4);
	};

	// Includes the "MThd" and data-length fields
	int32_t size() const {
		return this->data_length() + 8;
	};

	// begin(), end() allow me to hide the member p_
	// iterators... ?
	const unsigned char *begin() const {
		return this->p_+8;
	};
	const unsigned char *end() const {
		return this->p_+this->size();
	};
	
private:
	const unsigned char *p_ {};
	// const uint64_t size ??
};


int16_t fmt_type(const mthd_container_t&);
int16_t num_trks(const mthd_container_t&);

enum class midi_time_division_field_type_t {
	ticks_per_quarter,
	SMPTE
};
midi_time_division_field_type_t detect_midi_time_division_type(const mthd_container_t&);
uint16_t interpret_tpq_field(const mthd_container_t&);  // assumes midi_time_division_field_type_t::ticks_per_quarter
struct midi_smpte_field {
	int8_t time_code_fmt {0};
	uint8_t units_per_frame {0};
};
midi_smpte_field interpret_smpte_field(const mthd_container_t&);  // assumes midi_time_division_field_type_t::SMPTE
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
	unsigned char *begin() const;  // Starts at the delta-time
	unsigned char *data_begin() const;  // Starts just after the delta-time
	unsigned char *end() const;
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
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*);

// Obtained from the begin() && end() methods of class mtrk_container_t.  
class mtrk_container_iterator {
public:
	mtrk_container_iterator(const mtrk_container_t* c, int32_t o, unsigned char ms)
		: container_(c), container_offset_(o), midi_status_(ms) {};
	mtrk_event_container_t operator*() const;
	mtrk_container_iterator& operator++();
private:
	const mtrk_container_t *container_ {};
	int32_t container_offset_ {0};  // offset from this->container_.beg_
	unsigned char midi_status_ {0};
};

class mtrk_container_t {
public:
	mtrk_container_t(const validate_mtrk_chunk_result_t&);

	int32_t data_size() const;
	int32_t size() const;
	mtrk_container_iterator begin() const;
	mtrk_container_iterator end() const;
private:
	const unsigned char *p_ {};  // Points at "MTrk..."
	int32_t size_ {0};
	friend struct mtrk_container_iterator;
};









class smf_container_t {
public:
	smf_container_t()=default;
	smf_container_t(const std::vector<int32_t>& o, const std::vector<unsigned char>& d)
		: chunk_offset_(o), data_(d) {};
	mthd_container_t get_header() const;
	mtrk_container_t get_track(int) const;
private:
	std::vector<int32_t> chunk_offset_ {};  // MThd, Mtrk, unknown
	std::vector<unsigned char> data_ {};
};

struct read_smf_result_t {
	bool is_valid {};
	std::string msg {};
	smf_container_t smf {};
};
read_smf_result_t read_smf(const std::filesystem::path&);


std::string print(const smf_container_t&);
bool play(const smf_container_t&);









};  // namespace mc


