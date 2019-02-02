#include "..\aulib\input\midi_util.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>

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
enum class midi_chunk_t {
	header,  // MThd
	track,  // MTrk
	unknown,
	invalid  // Does not begin w/a 4 char ASCII id & a 4-byte length
};
struct detect_chunk_type_result_t {
	mc::midi_chunk_t type {mc::midi_chunk_t::unknown};
	bool is_valid {};
	std::string msg {};
};
mc::detect_chunk_type_result_t detect_midi_chunk_type(const unsigned char*);

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
// What's the point of this?  This is a convienience class for _viewing_/inspecting mtrk events
// originating as part of a sequence where each member had a delta_time, midi_status, etc.  
// If you have one of these you have a ptr to a legal, validated mtrk event of type midi, sysex,
// or meta event.  
//
// The four methods are applicable to _all_ mtrk events are:
// (1) delta_time(mtrk_event_container_t)
// (2) event_type(mtrk_event_container_t) => sysex, midi, ...
// (3) non-interpreted print:  <delta_t>-<type>-hex-ascii (note that the size is needed for this)
// (4) data_size() && size()
//
// Note that midi event lengths cannot in general be determined by examining *p_ alone, since
// *p_ may get its status byte from a previous message.  For events of type midi, the field 
// const char midi_status_ is the applicable midi status byte, either contained in *p_ or within
// some previous (probably unknown) midi event.  
// ...Without a status byte, can you not get the size from the first (possibly only) data byte?
// No, b/c if the byte after the first data byte begins w/ 0 you can not distingish it from 
// a single-byte delta_time vl-quantity for the next event.  
//
// TODO:  Conversion functions for specialized containers for midi, sysex, meta ??
//
class mtrk_event_container_t {
public:
	enum event_type {
		midi,
		sysex,
		meta,
		unknown,
		invalid
	};
	// Does not need to be recorded as a data member
	// const mtrk_event_container_t::event_type type {mtrk_event_container_t::event_type::unknown};

	// size of the event not including the delta-t field (but including the length field 
	// in the case of sysex & meta events)
	int32_t data_length() const;
	int32_t size() const;
	// Used by the free func delta_time(const mtrk_event_container_t&)
	const unsigned char *begin() const;
private:
	const unsigned char *beg_ {};

	// Meaningless for sysex and meta events.  For midi events, holds the status byte, needed
	// to compute the event size.  For creation of midi events using this container, use a (-)
	// value to indicate running status (ie, that the status byte for the present event should
	// not be written.  
	const char midi_status_ {0};
};

mtrk_event_container_t::event_type detect_mtrk_event_type(const unsigned char*);
midi_vl_field_interpreted delta_time(const mtrk_event_container_t&);
std::string print(const mtrk_event_container_t&);




class mtrk_container_t;

// Obtained from the begin() && end() methods of class mtrk_container_t.  
struct mtrk_container_iterator {
	const mtrk_container_t *container_ {};
	int32_t container_event_offset_ {0};  // offset into the container's event_idx vector

	mtrk_event_container_t operator*() const {
		auto context = (*(this->container_)).mtrk_events_[this->container_event_offset_];
		const unsigned char *p = (*(this->container_)).mtrk_events_.begin + context.offset;
		return mtrk_event_container_t {p,context.midi_status};
	};

	mtrk_container_iterator& operator++() {
		this->container_event_offset_ += 1;
		return *this;
	};
	mtrk_container_iterator& operator--() {
		this->container_event_offset_ -= 1;
		return *this;
	};
};

class mtrk_container_t {
public:
	mtrk_container_t()=default;
	mtrk_container_t(const unsigned char *p) : beg_(p) {};

	std::array<char,4> id() const {
		std::array<char,4> result {};
		std::copy(beg_,beg_+4,result.begin());
		return result;
	};

	int32_t data_length() const {
		return midi_raw_interpret<int32_t>(beg_+4);
	};

	int32_t size() const {
		return this->data_length();
	};

	mtrk_container_iterator begin() const {
		return mtrk_container_iterator { this, 0 };
	};
	mtrk_container_iterator end() const {
		return mtrk_container_iterator {this, this->mtrk_events_.size()};
	};
	
private:
	struct mtrk_event_idx_t {
		int32_t offset {0};  // Offset from this->beg_
		unsigned char midi_status {};
		// Probably also want t_onset ?
	};
	const unsigned char *beg_ {};  // Points at "MTrk..."
	const int32_t size_ {0};
	std::vector<mtrk_event_idx_t> mtrk_events_ {};

	friend struct mtrk_container_iterator;
};




struct validate_smf_result_t {
	bool is_valid {};
	std::string msg {};
	std::vector<const unsigned char*> chunk_offsets_ {};
};
validate_smf_result_t validate_smf(const unsigned char*, int32_t);

class smf_container_t {
public:
	smf_container_t(const unsigned char *p, const std::vector<const unsigned char*>&) : beg_(p) {};
	mtrk_container_t get_header(int) const;
	mtrk_container_t get_track(int) const;
private:
	std::vector<const unsigned char*> chunk_offset_ {};  // MThd, Mtrk, unknown
	const unsigned char *beg_ {};
	const int32_t size_ {};
};

std::string print(const smf_container_t&);
bool play(const smf_container_t&);






midi_vl_field_interpreted sysex_event_length(const unsigned char*);
midi_vl_field_interpreted midi_event_length(const unsigned char*);
midi_vl_field_interpreted meta_event_length(const unsigned char*);


};  // namespace mc

/*
int64_t somehow_calc_event_size(const unsigned char*);
int64_t somehow_detn_event_type(const unsigned char*);
struct mtrk_container_t;
struct mtrk_event_container_t;



struct mtrk_container_t {
	const unsigned char *begin_ {};

	// size of the whole chunk including the id and length fields
	const int64_t size {0};

	mtrk_container_iterator begin() const;
};



*/

