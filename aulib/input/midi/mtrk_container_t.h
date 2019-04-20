#pragma once
#include "midi_raw.h"
#include <string>
#include <cstdint>
#include <vector>


class mtrk_container_t;
class mtrk_event_container_t;
class mtrk_event_container_sbo_t;
class mtrk_view_t;

//
// mtrk_iterator_t
//
// Obtained from the begin() && end() methods of class mtrk_view_t.  
//
// Dereferencing returns an mtrk_event_container_sbo_t, which may allocate
// if the underlying event is large enough.  NB that a range-for automatically
// derefs the iterator.
//
// 
class mtrk_iterator_t {
public:
	mtrk_event_container_sbo_t operator*() const;
	mtrk_iterator_t& operator++();
	//mtrk_iterator_t& operator++(int);
	bool operator==(const mtrk_iterator_t&) const;
	bool operator!=(const mtrk_iterator_t&) const;
	// operator->();
private:
	// Ctor that takes a caller-specified p_ and midi status byte applicipable
	// to the _prior_ event in the sequence (arg 2); only for trusted callers 
	// (ex class mtrk_view_t)!
	mtrk_iterator_t(const unsigned char*, unsigned char);

	// ptr to first byte of the delta-time
	const unsigned char *p_ {nullptr};
	// midi status applicable to the event *prior* to this->p_;  0x00u if
	// that event was a meta or sysex_f0/f7.  See notes in operator++();
	unsigned char s_ {0x00u};

	friend class mtrk_view_t;
};




//
// Obtained from the begin() && end() methods of class mtrk_container_t.  
class mtrk_container_iterator_t {
public:
	mtrk_container_iterator_t(const mtrk_container_t* c, uint32_t o, unsigned char ms)
		: container_(c), container_offset_(o), midi_status_(ms) {};
	mtrk_event_container_t operator*() const;
	mtrk_event_container_sbo_t operator!() const;

	// If you got your iterator from an mtrk_container_t, all points in the
	// stream should have a valid midi status byte.  
	unsigned char midi_status() const;

	mtrk_container_iterator_t& operator++();
	bool operator<(const mtrk_container_iterator_t&) const;
	bool operator==(const mtrk_container_iterator_t&) const;
	bool operator!=(const mtrk_container_iterator_t&) const;
private:
	const mtrk_container_t *container_ {};
	uint32_t container_offset_ {0};  // offset from this->container_.p_

	// All points in a midi stream have an implied (or explicit) midi-status
	unsigned char midi_status_ {0};
};


//
// mtrk_view_t
//
// An mtrk_view_t is a pointer,size pair into a valid MTrk event sequence.  
// p_ indicates the start of the MTrk chunk, so *p=='M', *++p=='T', etc.  It
// should not be possible to construct an mtrk_view_t to an underlying char 
// array that is not a valid MTrk chunk, so posession of an mtrk_view_t is an
// assurance that the underlying range is valid.  Internally, mtrk_view_t 
// takes advantage of this assurance by parsing the sequence using the fast
// but generally unsafe parse_*_usafe(const unsigned char*) family of 
// functions.  
//
// Events within MTrk event sequences are not random-accessible, since 
// events have variable size.  Even if this were not so, at each point in 
// the sequence there may be an implicit "running status" midi byte that 
// can be  determined only by parsing the the preceeding most recent midi 
// channel_voice/mode event containing a status byte.  In the worst case,
// this event may occur hundreds of bytes to the left of the MTrk event of
// interest.  Other styes of implicit state infest the MTrk event sequence: 
// mid-sequence tempo and/or time-signature changes which change the meaning 
// of the delta-time fields are possible, control/program-change events 
// affect the way a given midi event should be sounded.  sysex_f0/f7 
// events may set opaque types of state.  In general, to usefully "process" 
// event n within an mtrk event sequence it is necessary to iterate through 
// the prior n-1 events and accumulate any state relevant to the procesing
// to be done.  
//
// Hence, mtrk_view_t.begin() and .end() return an iterator type defining 
// operators pre/post ++, but not +/- int, and not --.  This iterator keeps
// track of the most recent midi status byte, the smallest amount of 
// information required to determine the size of the pointed at MTrk event,
// and therefore locate the next MTrk event in the sequence.  
//
class mtrk_view_t {
public:
	mtrk_view_t(const validate_mtrk_chunk_result_t&);
	mtrk_view_t(const unsigned char*, uint32_t);
	
	uint32_t size() const;
	uint32_t data_length() const;
	
	// In one possible design, this returns a ptr to the first byte of the 
	// delta-time of the first mtrk event.  I reject this b/c for an empty 
	// mtrk (data_length()==0, this ptr is invalid.  
	// ctor mtrk_iterator_t(mtrk_view_t) relies on this behavior.  
	// TODO:  unsigned char or some sort of mtrk_event_t???
	const unsigned char *data() const;
	mtrk_iterator_t begin() const;
	mtrk_iterator_t end() const;

	bool validate() const;
private:
	const unsigned char *p_ {};  // Points at the 'M' of "MTrk..."
	uint32_t size_ {};
	//friend class mtrk_iterator_t;
};
std::string print(const mtrk_view_t&);




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
class mtrk_container_t {
public:
	mtrk_container_t(const validate_mtrk_chunk_result_t&);
	mtrk_container_t(const unsigned char*, uint32_t);

	int32_t data_size() const;
	uint32_t size() const;
	mtrk_container_iterator_t begin() const;
	mtrk_container_iterator_t end() const;
private:
	const unsigned char *p_ {};  // Points at the 'M' of "MTrk..."
	uint32_t size_ {0};
	friend class mtrk_container_iterator_t;
};
std::string print(const mtrk_container_t&);


//
// mtrk_event_container_t
//
// The std defines 3 types of MTrk events (enum class smf_event_type):  
// sysex_f0/f7, (midi) channel_voice/mode, meta
//
// All MTrk events consist of a vl delta-time field followed by a sequence
// of bytes, the number of which can only be determined by parsing said 
// sequence to (1) get the number of bytes in the delta-time, (2) get the
// number of bytes of the "payload" following the delta-time.  For (midi)
// channel_voice/mode events, the number of bytes in the payload can only be
// determined in the context of a status byte, which may or may not be the 
// first byte of the payload.  Consider a midi event not containing a local 
// (contained-within-payload) status byte, and a pointer p to the first byte
// following this event's delta-time, thus, !(*p>>7).  Does ++p belong to the
// event or is it the first byte of the delta-time for the next event?  If 
// ((*++p)>>7), ++p is the first byte of the delta-time for the next event, 
// however, if !((*++p)>>7), ++p may either be the first (and only) byte of 
// the delta-time for the next event, or it may be the second midi data byte
// of a 2-byte midi message.  It is impossible to tell.   To parse an mtrk 
// event sequence, it is required that the most recent midi-status byte be 
// known at all times.  
//
// Sysex and meta events explictly encode their length as a vl field within
// the payload; midi events are 1 (running status), 2 (local status or running
// status), or 3 (local status), bytes (following delta-time field).  
//
// The standard considers the delta-time to be part of the definition of 
// an "MTrk event," but not a part of the event proper.  From the std:
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
// this 12-byte pointer,size class will mostly reference 5-byte (2-byte-delta-t+status+p1+p2) 
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
	smf_event_type type() const;  // enum event_type:: midi || sysex || meta
	int32_t data_size() const;  // Does not include the delta-time
	int32_t size() const;  // Includes the delta-time

	// TODO:  Replace with some sort of "safer" iterator type
	const unsigned char *begin() const;  // Starts at the delta-time
	const unsigned char *data_begin() const;  // Starts just after the delta-time
	const unsigned char *end() const;

	const unsigned char *raw_begin() const;
private:
	const unsigned char *p_ {};  // points at the first byte of the delta-time
	int32_t size_ {0};  // delta_t + payload
};
std::string print(const mtrk_event_container_t&);








//
// TODO:
// - Should rename the present "container" types to "view;" these are still useful and 
//   can form the basis for the internal implementation.  
//
class mtrk_event_container_sbo_t {  // sizeof() == 24
private:
	//
	// Of the smf_event_types, only sysex_f0/f7 & meta events can be large enough to
	// overflow the small buffer.  Hence, any midi message (channel_voice or 
	// channel_mode) will fit in the small buffer.  These messages may or may not be
	// in running_status so may or may not contain an explicit midi status byte.  
	//
	// For sysex_f0/f7 or meta events that overflow the small buffer, some of the
	// fields presently in big_t make no sense; these messges overwrite any 
	// running status, so the midi_status field makes no sense.  But, OTOH, see
	// the MIDI Channel Prefix meta event (p.138)
	// ->  big.capacity >= big.size
	//
	struct big_t {  // sizeof() == 24
		unsigned char *p;
		uint32_t size;
		uint32_t capacity;
		uint32_t dt_fixed;
		smf_event_type sevt;  // "smf_event_type"
		unsigned char midi_status;
		uint8_t unused;
		unsigned char bigsmall_flag;  // ==1u => small; ==0u => big
	};
	struct small_t {
		std::array<unsigned char, sizeof(big_t)> arry;
	};
	union bigsmall_t {
		big_t b;
		small_t s;
	};
	
	bigsmall_t d_;

	static_assert(sizeof(small_t)==sizeof(big_t),"sizeof(small_t)!=sizeof(big_t)");
	static_assert(sizeof(bigsmall_t)==sizeof(big_t),"sizeof(bigsmall_t)!=sizeof(big_t)");
	static_assert(sizeof(bigsmall_t)==24,"sizeof(bigsmall_t)!=24");

	void set_flag_big() {
		if (this->is_small()) {
			this->d_.b.bigsmall_flag = 0x00u;
		}
	};
	void set_flag_small() {
		if (this->is_big()) {
			this->d_.b.bigsmall_flag = 0x01u;
		}
	};

public:
	//
	// is_small() && is_big() are only public so that the function
	// print(mtrk_event_container_sbo_t,mtrk_sbo_print_opts::debug) can report 
	// on the state of the object.  
	//
	// Note that b/c my flag is not external to the big/small_t structs, i have
	// to make a possibly invalid read here.  I arbitrarily choose the convention
	// of reading from d_.b (see also this->raw_flag()).  
	//
	bool is_small() const {
		return (this->d_.b.bigsmall_flag == 0x01u);
	};
	bool is_big() const {
		return !is_small();
	};

	//
	// Default ctor
	//
	mtrk_event_container_sbo_t()=delete;

	//
	// Copy ctor
	//
	mtrk_event_container_sbo_t(const mtrk_event_container_sbo_t& rhs) {
		this->d_ = rhs.d_;
		if (this->is_big()) {
			// Deep copy the pointed-at range
			unsigned char *new_p = new unsigned char[this->d_.b.capacity];
			std::copy(this->d_.b.p,this->d_.b.p+this->d_.b.capacity,new_p);
			this->d_.b.p = new_p;
		}
	};
	//
	// Copy assignment; overwrites a pre-existing lhs 'this' w/ rhs
	//
	mtrk_event_container_sbo_t& operator=(const mtrk_event_container_sbo_t& rhs) {
		if (this->is_big()) {
			delete this->d_.b.p;
		}
		this->d_ = rhs.d_;
		
		if (this->is_big()) {
			// Deep copy the pointed-at range
			unsigned char *new_p = new unsigned char[this->d_.b.capacity];
			std::copy(this->d_.b.p,this->d_.b.p+this->d_.b.capacity,new_p);
			this->d_.b.p = new_p;
		}

		return *this;
	};
	//
	// Move ctor
	//
	mtrk_event_container_sbo_t(mtrk_event_container_sbo_t&& rhs) {
		this->d_ = rhs.d_;
		if (this->is_big()) {
			rhs.set_flag_small();  // prevents ~rhs() from freeing its memory
			// Note that this state of rhs is invalid as it almost certinally does
			// not contain a valid "small" mtrk event
		}
	};
	//
	// Move assignment
	//
	mtrk_event_container_sbo_t& operator=(mtrk_event_container_sbo_t&& rhs) {
		if (this->is_big()) {
			delete this->d_.b.p;
		}
		this->d_ = rhs.d_;
		if (rhs.is_big()) {
			rhs.set_flag_small();  // prevents ~rhs() from freeing its memory
			// Note that this state of rhs is invalid as it almost certinally does
			// not contain a valid "small" mtrk event
		}

		return *this;
	};

	//
	// For callers who have pre-computed the exact size of the event and who
	// can also supply a midi status byte if applicible, ex, an mtrk_container_iterator_t.  
	// For values of sz => bigsmall_t => big_t, d_.b.tag.midi_status == 0, since
	// the indicated event can not be channel_voice or channel_mode.  
	// For values of sz => bigsmall_t => small_t, and where the indicated event is 
	// channel_voice or channel_mode, the last byte of the local data array,
	// d_.s.arry[sizeof(small_t)-1] == the status byte.  
	//
	// TODO:  parse_mtrk_event_type can return smf_event_type::invalid in field .type;
	// could replace w/ detect_mtrk_event_type_unsafe()...
	//
	mtrk_event_container_sbo_t(const unsigned char *p, uint32_t sz, unsigned char s=0) {
		if (sz > sizeof(small_t)) {
			this->d_.b.p = new unsigned char[sz];
			this->d_.b.size = sz;
			this->d_.b.capacity = sz;
			std::copy(p,p+sz,this->d_.b.p);

			auto ev = parse_mtrk_event_type(p,s,sz);
			this->d_.b.dt_fixed = ev.delta_t.val;
			this->d_.b.sevt = ev.type;
			this->d_.b.midi_status = 0x00;
			// Since all smf_event_type::channel_voice || channel_mode messages have
			// a max size of 4 + 1 + 2 == 7, ev.type will always be sysex_f*, meta, or
			// invalid.  The former two cancel any running status so the midi_status
			// byte does not apply.  

			this->set_flag_big();
		} else {
			auto end = std::copy(p,p+sz,this->d_.s.arry.begin());
			while (end!=this->d_.s.arry.end()) {
				*end++ = 0x00;
			}
			
			// If this is a meta or sysex_f0/f7 event, chev.is_valid == false;
			// but == true if this is a channel_voice/mode event.  Write the 
			// status byte to the field d_.s.arry[sizeof(small_t)-3], which is
			// the same offset as d_.b.midi_status.  
			// TODO:  This is some **really**nasty** hardcoding here
			auto chev = parse_channel_event(&(this->d_.s.arry[0]),s,sizeof(small_t));
			if (chev.is_valid) {
				this->d_.s.arry[sizeof(small_t)-3] = chev.status_byte;
			}
			this->set_flag_small();
		}
	};

	~mtrk_event_container_sbo_t() {
		if (this->is_big()) {
			delete this->d_.b.p;
		}
	};

	unsigned char operator[](int32_t i) const {
		if (this->is_small()) {
			return this->d_.s.arry[i];
		} else {
			return this->d_.b.p[i];
		}
	};

	// TODO:  If small, this returns a ptr to a stack-allocated object...  Bad?
	const unsigned char *data() const {
		if (this->is_small()) {
			return &(this->d_.s.arry[0]);
		} else {
			return this->d_.b.p;
		}
	};

	// Pointer directly to start of this->data_, without regard to 
	// this->is_small()
	const unsigned char *raw_data() const {
		return &(this->d_.s.arry[0]);
	};
	// Pointer directly to start of this->data_, without regard to 
	// this->is_small()
	const unsigned char *raw_flag() const {
		//return &(this->bigsmall_flag);
		return &(this->d_.b.bigsmall_flag);
	};


	int32_t delta_time() const {
		if (this->is_small()) {
			return midi_interpret_vl_field(&(this->d_.s.arry[0])).val;
		} else {
			return midi_interpret_vl_field(this->d_.b.p).val;
		}
	};

	smf_event_type type() const {  // channel_{voice,mode},sysex_{f0,f7},meta,invalid
		if (this->is_small()) {
			return detect_mtrk_event_type_dtstart_unsafe(&(this->d_.s.arry[0]),
				this->d_.s.arry[sizeof(small_t)-1]);
		} else {
			return this->d_.b.sevt;
		}
	};

	// size of the event not including the delta-t field (but including the length field 
	// and type-byte in the case of sysex & meta events)
	int32_t data_size() const {  // Does not include the delta-time
		if (this->is_small()) {
			// 041219
			//detect_mtrk_event_type_dtstart_unsafe

			// TODO:  this->d_.s.arry[sizeof(small_t)-1] is not the running_status...
			auto evt = parse_mtrk_event_type(&(this->d_.s.arry[0]),
				this->d_.s.arry[sizeof(small_t)-1],sizeof(small_t));
			if (evt.type == smf_event_type::invalid) {
				std::abort();
			}
			return evt.data_length;
		} else {
			return this->d_.b.size - midi_interpret_vl_field(this->d_.b.p).N;
			//return this->d_.b.size - midi_vl_field_size(this->d_.b.dt_fixed);
			// TODO:  This assumes that the dt field does not contain a leading sequence
			// of 0x80's
		}
	};

	int32_t size() const {  // Includes the delta-time
		if (this->is_small()) {
			auto evt = parse_mtrk_event_type(&(this->d_.s.arry[0]),
				this->d_.s.arry[sizeof(small_t)-1],sizeof(small_t));
			if (evt.type == smf_event_type::invalid) {
				std::abort();
			}
			if (evt.size <= 0) {
				std::abort();
			}
			return evt.size;
		} else {
			return this->d_.b.size;
		}
	};
};

enum class mtrk_sbo_print_opts {
	normal,
	debug
};
std::string print(const mtrk_event_container_sbo_t&,
			mtrk_sbo_print_opts=mtrk_sbo_print_opts::normal);



