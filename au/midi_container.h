#pragma once
#include "..\aulib\input\midi_util.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>

namespace mdata {
struct byte_desc_t {
	const uint8_t type {};
	const std::array<char,30> d {};
};
inline const std::array<const byte_desc_t,17> meta {
	byte_desc_t {0x00,"Sequence-Number"},
	byte_desc_t {0x01,"Text-Event"},
	byte_desc_t {0x02,"Copyright Notice"},
	byte_desc_t {0x03,"Sequence/Track Name"},
	byte_desc_t {0x04,"Instrument Name"},
	byte_desc_t {0x05,"Lyric"},
	byte_desc_t {0x06,"Marker"},
	byte_desc_t {0x07,"Cue Point"},
	byte_desc_t {0x20,"MIDI Channel Prefix"},
	byte_desc_t {0x2F,"End-of-Track"},
	byte_desc_t {0x51,"Set-Tempo"},
	byte_desc_t {0x54,"SMPTE Offset"},
	byte_desc_t {0x58,"Time-Signature"},
	byte_desc_t {0x59,"Key-Signature"},
	byte_desc_t {0x7F,"Sequencer-Specific"}
};

// For midi-status bytes of type "channel-voice," mask w/ 0xF0.  See Std p. 100.  
// Note that They byte must be channel-voice, not channel-mode, since for channel-mode, 
// 0xB0 => "Select channel mode"  Ex:
// (status_type & 0xF0) ==
inline const std::array<const byte_desc_t,7> midi_status_ch_voice {
	byte_desc_t {0x80,"Note-off"},
	byte_desc_t {0x90,"Note-on"},
	byte_desc_t {0xA0,"Aftertouch/Key-pressure"},
	byte_desc_t {0xB0,"Control-change"},
	byte_desc_t {0xC0,"Program-change"},
	byte_desc_t {0xD0,"Aftertouch/Channel-pressure"},
	byte_desc_t {0xE0,"Pitch-bend-change"}
};
inline const std::array<const byte_desc_t,3> midi_status_ch_mode {
	byte_desc_t {0xB0,"Select-channel-mode"}
};

};  // namespace mdata

namespace mc {

int midi_example();



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
	uint8_t type {0};
	int32_t data_size {};  // Everything not delta_time
};
parse_meta_event_result_t parse_meta_event(const unsigned char*);

struct parse_sysex_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
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
	int32_t data_size {};  // Everything not delta_time
};
parse_midi_event_result_t parse_midi_event(const unsigned char*, unsigned char=0);
bool midi_event_has_status_byte(const unsigned char*);
unsigned char midi_event_get_status_byte(const unsigned char*);





//
// Checks for the 4-char id and the 4-byte size.  Verifies that the id + size field 
// + the reported size does not exceed the max_size suppled as the second argument.  
// Does _not_ inspect anything past the end of the length field.  
//
struct detect_chunk_type_result_t {
	chunk_type type {chunk_type::unknown};
	int32_t size {0};
	std::string msg {};
	bool is_valid {false};
	const unsigned char* p {};
};
detect_chunk_type_result_t detect_chunk_type(const unsigned char*, int32_t);

//
// Why not a generic midi_chunk_container_t<T> template?  Because MThd and MTrk containers
// are radically different and it really does not make sense to write generic functions to 
// operate on either type.  
//
// int16_t mthd_container_t::format():  Returns 0, 1, 2
//  -> 0 => File contains a single multi-channel track
//  -> 1 => File contains one or more simultaneous tracks
//  -> 3 =>  File contains one or more sequentially independent single-track patterns.  
// int16_t mthd_container_t::ntrks() is always == 1 for a fmt-type 0 file.  
//
//
// Need:
// validate_mthd_result_t validate_mthd(detect_chunk_type_result_t)
// Then change the mthd_container_t ctor to accept this.  
//
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
// The std defines 3 types of MTrk events:  sysex, midi, meta
//
// All MTrk events consist of a vl delta-time field followed by a sequence of bytes, the number
// of which can only be determined by parsing the sequence to determine its type; in the
// case of midi events, the size may only be determinable by parsing _prior_ midi events occuring
// in the same MTrk container to determine the value of the running-status midi status byte.  
// Sysex and meta events explictly encode their length as a vl field; midi events are 2, 3, 
// or 4, bytes (following delta-time field), depending on the presence or absense and value of
// a status byte.  
//
// The standard considers the delta-time to be part of the definition of an "MTrk event," but
// not a part of the event proper.  From the std:
// <MTrk event> = <delta-time> <event>
// <event> = <MIDI event> | <sysex event> | <meta-event> 
//
// Here, i group the <delta-time> and <event> bytes into a single entity, the mtrk_event_t.  
// mtrk_event_t identifies an MTrk event internally as a pointer and size into a byte array.  
// The mtrk_event_t ctor validates the byte sequence; an mtrk_event_t can not be constructed
// from a byte sequence that is not a valid midi, meta, or sysex event.  For midi events 
// without a status byte (ie, occuring as an internal member of a sequence of MTrk events where
// running-status is in effect), whoever calls the ctor is responsible for keeping track of
// the value of the running-status to determine the size of the message.  Further, because
// an mtrk_event_t is just a pointer and a size, the value of the status byte corresponding to
// such a midi mtrk_event_t is not recoverable from the mtrk_event_t alone.  Midi events 
// lacking explicit status bytes are valid midi events.  
//
// Note that for the majority of simple smf files comprised of a large number of midi events,
// this 96-byte pointer,size class will mostly be into 5-byte (2-byte-delta-t+status+p1+p2) 
// or 4-byte arrays.  mtrk_event_t is useful therefore as a generic return type from the MTrk
// container iterator.  Do not try to "index" each event in an n-thousand event midi file with 
// a vector of mtrk_event_t.  
// TODO:  SSO-style union for midi messages?
//
//
// The four methods are applicable to _all_ mtrk events are:
// (1) delta_time(mtrk_event_container_t)
// (2) event_type(mtrk_event_container_t) => sysex, midi, meta 
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
// The event sequence pointed to by an mtrk_container_t object has been validated 
// and can therefore be parsed internally using the fast but generally unsafe 
// parse_*_usafe(const unsigned char*) family of functions.  
//
// MTrk event sequqences are not random-accessible, since (1) midi, sysex, and meta events are
// of variable length, and (2) since at each point in the sequence there is an implicit 
// "running status" that can be determined only by parsing the the preceeding most recent midi
// status byte.  This byte may occur hundreds of bytes to the left of the MTrk event of interest.  
// To read MTrk event n in the container, it is necessary to iterate through events 0->(n-1).  
// For this purpose, the begin() and end() methods of mtrk_container_t return an
// mtrk_container_iterator_t, a special iterator dereferencing to an mtrk_event_container_t.  
// An mtrk_container_iterator_t can only be forward-incremented, but keeps track of the most
// recent midi status byte.  This is the smallest amount of information required to determine
// the size of an MTrk event from a pointer to the first byte of its delta-time field.  
//
// It should be noted that midi status is not the only type of state implicit to an MTrk event
// stream.  At any point in a stream, tempo, time-signature, etc can change.  A user interested 
// in these data is responsible for keeping track of their values.  The midi-status byte is 
// stored in the iterator because it is the only way it is possible to determine the size of the
// next midi message in a running-status sequence.  
//
class mtrk_container_iterator_t;
class mtrk_container_t;

struct validate_mtrk_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
	const unsigned char *p;
	int32_t size {0};
};
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*, int32_t);

// Obtained from the begin() && end() methods of class mtrk_container_t.  
class mtrk_container_iterator_t {
public:
	mtrk_container_iterator_t(const mtrk_container_t* c, int32_t o, unsigned char ms)
		: container_(c), container_offset_(o), midi_status_(ms) {};
	mtrk_event_container_t operator*() const;
	mtrk_container_iterator_t& operator++();
	bool operator<(const mtrk_container_iterator_t&) const;
	bool operator==(const mtrk_container_iterator_t&) const;
	bool operator!=(const mtrk_container_iterator_t&) const;
private:
	const mtrk_container_t *container_ {};
	int32_t container_offset_ {0};  // offset from this->container_.beg_

	// All points in a midi stream have an implied (or explicit) midi-status
	unsigned char midi_status_ {0};
};

//
//
//
class mtrk_container_t {
public:
	mtrk_container_t(const validate_mtrk_chunk_result_t&);
	mtrk_container_t(const unsigned char*, int32_t);

	int32_t data_size() const;
	int32_t size() const;
	mtrk_container_iterator_t begin() const;
	mtrk_container_iterator_t end() const;
private:
	const unsigned char *p_ {};  // Points at "MTrk..."
	int32_t size_ {0};
	friend class mtrk_container_iterator_t;
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

	// Needed by the smf_container_t ctor
	std::string fname {};
	const unsigned char *p {};
	int32_t size {0};
	int n_mtrk {0};  // Number of MTrk chunks
	int n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};
};
//validate_smf_result_t validate_smf(const std::filesystem::path&);
validate_smf_result_t validate_smf(const unsigned char*, int32_t, const std::string&);

class smf_container_t {
public:
	smf_container_t(const validate_smf_result_t&);

	mthd_container_t get_header() const;
	mtrk_container_t get_track(int) const;
	bool get_chunk(int) const;  // unknown chunks allowed...
	std::string fname() const;

	int n_tracks() const;
	int n_chunks() const;  // Includes MThd
private:
	std::string fname_ {};
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


};  // namespace mc


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//
// Owning containers
//
// TODO:  Packing rules probably => room for one more 8-bit member
//
class midi_event_t {
public:

	unsigned char status() const;
	bool explicit_status() const;


	bool set_status(unsigned char);  // False if failed
	void set_status_explicit();  // False if failed
	void set_status_implicit(u);  // False if failed
private:
	unsigned char status;  // < 0 => implicit (running-status)
	unsigned char p1;
	unsigned char p2;
};






