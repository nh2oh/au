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


//
// What's the point of this?  This is a convienience class for working with mtrk events
// originating as part of a sequence where each member had a delta_time, midi_status, etc.  
// If you have one of these you have a ptr to a legal, validated mtrk event of type midi, sysex,
// or meta.  
//
// The four methods are applicable to _all_ mtrk events are:
// (1) delta_time(mtrk_event_container_t)
// (2) event_type(mtrk_event_container_t) => sysex, midi, ...
// (3) non-interpreted print:  <delta_t>-<type>-hex-ascii (note that the size is needed for this)
// (4) data_size() && size()
//
// Note that for a byte sequence starting at p_ corresponding to a midi event, it is not in
// general possible to determine the length of the event since it may have been specified in the
// status byte of a previous midi event.  
// For example, for an event without a status byte, if the byte after the first data byte 
// begins w/ 0, it can not be distingished from the first byte of a single-byte delta_time vl
// quantity for the next event.  
//

class mtrk_event_container_t {
public:
	mtrk_event_container_t(const unsigned char*, int32_t);  // beg_, size

	// size of the event not including the delta-t field (but including the length field 
	// in the case of sysex & meta events)
	int32_t delta_time() const;
	event_type type() const;
	int32_t data_length() const;
	int32_t size() const;

private:
	unsigned char *beg_ {};
	int32_t size_ {0};  // delta_t + payload
};

struct parse_mtrk_event_result_t {
	bool is_valid {false};  // False if the delta-t vl field does not validate
	midi_vl_field_interpreted delta_t {};
	event_type type {event_type::midi};
};
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char*);
std::string print(const mtrk_event_container_t&);


struct validate_mtrk_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
};
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*);

//
//
//
class mtrk_container_iterator;

class mtrk_container_t {
public:
	mtrk_container_t()=default;
	mtrk_container_t(const unsigned char*);

	std::array<char,4> id() const {
		std::array<char,4> result {};
		std::copy(beg_,beg_+4,result.begin());
		return result;
	};

	int32_t data_length() const {
		return midi_raw_interpret<int32_t>(beg_+4);
	};
	int32_t size() const {
		return this->data_length()+4;
	};

	mtrk_container_iterator begin() const {
		// From the std p.135:  "The first event in each MTrk chunk must specify status"
		return mtrk_container_iterator { this, 8, midi_event_get_status_byte(this->beg_+8) };
	};
	mtrk_container_iterator end() const {
		return mtrk_container_iterator { this, this->size()};
	};
	
private:
	const unsigned char *beg_ {};  // Points at "MTrk..."
	friend struct mtrk_container_iterator;
};


// Obtained from the begin() && end() methods of class mtrk_container_t.  
struct mtrk_container_iterator {
	mtrk_event_container_t operator*() const {
		const unsigned char *p = *this->container_.beg_+this->container_offset_;
		auto sz = parse_midi_event(p, this->midi_status_).size;
		parse_midi_event_result_t parse_midi_event(, unsigned char=0);
		return mtrk_event_container_t {p,sz};
	};

	mtrk_container_iterator& operator++() {
		this->container_offset_ += 1;
		const unsigned char *p = *this->container_.beg_+this->container_offset_;
		if (detect_mtrk_event_type(p)==mtrk_event_container_t::event_type::midi 
			&& midi_event_has_status_byte(p)) {
			this->midi_status_ = midi_event_get_status_byte(p);
		}
		return *this;
	};

	mtrk_container_t *container_ {};
	int32_t container_offset_ {0};  // offset from this->container_.beg_
	unsigned char midi_status_ {0};
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


