#pragma once
#include "midi_raw.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>


namespace mc {

int midi_example();

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

std::string print(const mtrk_event_container_t&);

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
class mtrk_container_iterator_t;
class mtrk_container_t;


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

};  // namespace mc


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/*
//
// Statically-sized containers
//
// TODO:  Packing rules probably => room for one more 8-bit member
//
class midi_event_t {
public:

	unsigned char status() const;
	bool explicit_status() const;


	bool set_status(unsigned char);  // False if failed
	void set_status_explicit();  // False if failed
	void set_status_implicit();  // False if failed
private:
	unsigned char status;  // < 0 => implicit (running-status)
	unsigned char p1;
	unsigned char p2;
};
*/





