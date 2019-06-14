#pragma once
#include "midi_raw.h"  // midi_time_t
#include "mtrk_event_t.h"
#include "mtrk_iterator_t.h"
#include <string>
#include <cstdint>
#include <vector>
#include <array>  // For method .get_header()
#include <type_traits>

struct maybe_mtrk_t;

// Intended to represent an event occuring in an mtrk at a tk value given
// by .tk.  The onset tk of the event is .tk + .it->delta_time().  The onset
// tk of the event(s) prior to .it is tk.  
template <typename It>
struct event_tk_t {
	static_assert(std::is_same<It,mtrk_iterator_t>::value
		|| std::is_same<It,mtrk_const_iterator_t>::value);
	It it;
	uint64_t tk;
};

//
// mtrk_t
//
// Holds a sequence of mtrk_event_t's as a std::vector<mtrk_event_t>; owns 
// the underlying data.  Provides certain convienience functions for 
// obtaining iterators into the sequence (at a specific tick number, etc).  
//
// Because the conditions on a valid MTrk event sequence are complex and 
// expensive to maintain for operations such as push_back(), insert() etc,
// an mtrk_t object may not be serializable to a valid SMF MTrk chunk.  For 
// example, the sequence may contain multiple EOT events (or a terminating
// EOT event may be missing), there may be orphan note-on events, etc.  
// The method validate() returns a summary of the problems w/ the object.  
// Disallowing "invalid" states would make it prohibitively cumbersome to
// provide straightforward, low-overhead editing functionality to users.
// A user _has_ to be able to, for example, call push_back() to add note-on
// and note_off events one at a time.  If I were to require the object to
// always be serializable to a valid MTrk, this would be impossible since 
// adding a note-on event before the corresponding note-off creates an
// orphan on-event situation (as well as a "missing EOT" error).  
//
// Note that a default-constructed mtrk_t is empty (nevents()==0, 
// data_size()==0) and .validate() will return false (no EOT event);
// empty mtrk_t's are not serializable to valid MTrk chunks.  
//
//
// TODO:  Check for max_size() type of overflow?  Maximum data_size
// == 0xFFFFFFFFu (?)
//
class mtrk_t {
public:
	// Creates an empty MTrk event sequence:
	// nbytes() == 8, data_nbytes() == 0;
	// This will classify as invalid (!.validate()), because an MTrk 
	// sequence must terminate w/ an EOT meta event.  
	mtrk_t()=default;
	// Calls validate_chunk_header(p,max_sz) on the input, then iterates 
	// through the array building mtrk_event_t's by calling 
	// validate_mtrk_event_dtstart(curr_p,rs,curr_max_sz) and 
	// the ctor mtrk_event_t(const unsigned char*, unsigned char, uint32_t)
	// Iteration stops when the number of bytes reported in the chunk
	// header (required to be <= max_size) have been processed or when an 
	// invalid smf event is encountered.  No error checking other than
	// that provided by validate_mtrk_event_dtstart() is implemented.  
	// The resulting mtrk_t may be invalid as an MTrk event sequence.  For
	// example, it may contain multiple internal EOT events, orphan 
	// note-on events, etc.  Note that the sequence may be only partially
	// read-in if an error is encountered before the number of bytes 
	// indicated by the chunk header have been read.  
	mtrk_t(const unsigned char*, uint32_t);

	// The number of events in the track
	uint32_t size() const;
	uint32_t capacity() const;
	// The size in bytes occupied by the container written out (serialized)
	// as an MTrk chunk, including the 8-bytes occupied by the header.  
	// This is an O(n) operation
	uint32_t nbytes() const;
	// Same as .nbytes(), but excluding the chunk header
	uint32_t data_nbytes() const;
	// Cumulative number of midi ticks occupied by the entire sequence
	uint64_t nticks() const;

	// Writes out the literal chunk header:
	// {'M','T','r','k',_,_,_,_}
	std::array<unsigned char,8> get_header() const;

	mtrk_iterator_t begin();
	mtrk_iterator_t end();
	mtrk_const_iterator_t begin() const;
	mtrk_const_iterator_t end() const;
	mtrk_event_t& operator[](uint32_t);
	const mtrk_event_t& operator[](uint32_t) const;
	mtrk_event_t& back();
	const mtrk_event_t& back() const;
	mtrk_event_t& front();
	const mtrk_event_t& front() const;
	// at_cumtk() returns an iterator to the first event with
	// cumtk >= the number provided, and the exact cumtk for that event.  
	// The onset tk for the event pointed to by .it is:
	// .tk + .it->delta_time();
	event_tk_t<mtrk_iterator_t> at_cumtk(uint64_t);
	event_tk_t<mtrk_const_iterator_t> at_cumtk(uint64_t) const;
	// at_tkonset() returns an iterator to the first event with onset
	// tk >= the number provided, and the exact onset tk for that event.  
	// The cumtk for the event pointed to by .it is:
	// .tk - .it->delta_time();
	event_tk_t<mtrk_iterator_t> at_tkonset(uint64_t);
	event_tk_t<mtrk_const_iterator_t> at_tkonset(uint64_t) const;

	// Returns a ref to the event just added
	mtrk_event_t& push_back(const mtrk_event_t&);
	void pop_back();
	// Inserts arg2 _before_ arg1 and returns an iterator to the newly
	// inserted event.  Note that if the new event has a nonzero delta_time
	// insertion will timeshift all downstream events by that number of 
	// ticks.  
	mtrk_iterator_t insert(mtrk_iterator_t, const mtrk_event_t&);
	// Insert the provided event into the sequence such that its onset tick
	// is == the cumtk at arg1 + arg2.delta_time() and such that the onset 
	// tick of all downstream events is unchanged.  The delta time of the
	// event and/or of the event immediately downstream (after insertion) 
	// may be changed (reduced).  
	// If the onset tick intended for the new event (cumtk at arg1 + 
	// arg2.delta_time()) is <= nticks() prior to insertion, nticks() is 
	// unchanged following insertion.  
	mtrk_iterator_t insert_no_tkshift(mtrk_iterator_t, mtrk_event_t);
	// Insert the provided event into the sequence such that its onset tick
	// is == arg1 + arg2.delta_time()
	// TODO:  Unit tests
	mtrk_iterator_t insert(uint64_t, mtrk_event_t);	
	
	// Erase the event pointed to by the iterator.  Returns an iterator to 
	// the event immediately following the erased event.  
	mtrk_iterator_t erase(mtrk_iterator_t);
	mtrk_const_iterator_t erase(mtrk_const_iterator_t);
	// Erase the event indicated by the iterator.  If the event has a delta
	// time > 0, it is added to the event immediately following the deleted
	// event.  
	mtrk_iterator_t erase_no_tkshift(mtrk_iterator_t);


	// Note that calling clear will cause !this.validate(), since there is
	// no longer an EOT meta event at the end of the sequence.  
	void clear();
	void resize(uint32_t);

	// TODO:  This substantially duplicates the functionality of 
	// make_mtrk(const unsigned char*, uint32_t);
	// Could have make_mtrk() just call push_back() "blindly" on the
	// sequence then call validate() on the object.  
	struct validate_t {
		std::string error {};
		std::string warning {};
		operator bool() const;
	};
	validate_t validate() const;

	friend maybe_mtrk_t make_mtrk(const unsigned char*, uint32_t);
private:
	mtrk_iterator_t from_vec_iterator(const std::vector<mtrk_event_t>::iterator&);
	mtrk_const_iterator_t from_vec_iterator(const std::vector<mtrk_event_t>::iterator&) const;
	
	std::vector<mtrk_event_t> evnts_ {};
};
std::string print(const mtrk_t&);
// Prints each mtrk event as hexascii (using dbk::print_hexascii()) in a
// format valid to brace-init a c++ array.  
std::string print_event_arrays(const mtrk_t&);

// Returns true if the track qualifies as a tempo map; only a certain
// subset of meta events are permitted in a tempo_map.  Does not 
// validate the mtrk.  
bool is_tempo_map(const mtrk_t&);

// Get the duration in seconds.  A midi_time_t _must_ be provided, since
// a naked MTrk object does not inherit the tpq field from the MThd chunk
// of an smf_t, and there is no standardized default value for this 
// quantity.  The value midi_time_t.uspq is updated as meta tempo events 
// are encountered in the mtrk_t.  
double duration(const mtrk_t&, const midi_time_t&);
double duration(mtrk_const_iterator_t&, mtrk_const_iterator_t&, const midi_time_t&);

// Declaration matches the in-class friend declaration to make the 
// name visible for lookup outside the class.  
maybe_mtrk_t make_mtrk(const unsigned char*, uint32_t);
struct maybe_mtrk_t {
	std::string error {"No error"};
	mtrk_t mtrk;
	operator bool() const;
};



//
// get_simultanious_events(mtrk_iterator_t beg, mtrk_iterator_t end)
//
// Returns an iterator to one past the last event simultanious with
// beg.  
mtrk_iterator_t get_simultanious_events(mtrk_iterator_t, mtrk_iterator_t);

//
// find_linked_off(mtrk_const_iterator_t beg, mtrk_const_iterator_t end,
//						mtrk_event_t on);
// Find an off event on [beg,end) matching the mtrk_event_t "on" event 
// given by arg mtrk_event_t on.  
// 
// Returns an mtrk_event_cumtk_t where member .ev is an iterator to the
// corresponding off mtrk_event_t, and .cumtk is the cumulative number
// of ticks occuring on the interval [beg,.ev).  It is the cumulative 
// number of ticks starting from beg and and continuing to the event
// immediately _prior_ to event .ev (the onset tick of event .ev is thus
// .cumtk + .ev->delta_time()).  
// If on is not a note-on event, .ev==end and .cumtk==0.  
// If no corresponding off event can be found, .ev==end and .cumtk has the
// same interpretation as before.  
//
struct mtrk_event_cumtk_t {
	uint32_t cumtk;  // cumtk _before_ event ev
	mtrk_const_iterator_t ev;
};
event_tk_t<mtrk_const_iterator_t> find_linked_off(mtrk_const_iterator_t, 
					mtrk_const_iterator_t, const mtrk_event_t&);

//
// get_linked_onoff_pairs()
// Find all the linked note-on/off event pairs on [beg,end)
//
// For each linked pair {on,off}, the field cumtk (on.cumtk, off.cumtk) is 
// the cumulative number of ticks immediately prior to the event pointed 
// at by the iterator in field ev (on.ev, off.ev).  For a 
// linked_onoff_pair_t p,
// the onset tick for the on event is p.on.cumtk + p.on.ev->delta_time(),
// and similarly for the onset tick of the off event.  The duration of
// the note is:
// uint32_t duration = (p.on.cumtk+p.on.ev->delta_time()) 
//                     - (p.off.cumtk+p.off.ev->delta_time());
//
// Orphan note-on events are not included in the results.  
//
struct linked_onoff_pair_t {
	event_tk_t<mtrk_const_iterator_t> on;
	event_tk_t<mtrk_const_iterator_t> off;
};
std::vector<linked_onoff_pair_t>
	get_linked_onoff_pairs(mtrk_const_iterator_t,mtrk_const_iterator_t);

//
// Print a table of linked note-on/off event pairs in the input mtrk_t.  
// On events w/o a matching off event are output to the table with a
// "not found" message in place of the data for the off event.  Orphan
// off events are skipped (they are not detected at all).  
std::string print_linked_onoff_pairs(const mtrk_t&);

