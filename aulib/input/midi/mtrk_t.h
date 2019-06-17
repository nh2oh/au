#pragma once
#include "midi_raw.h"  // midi_time_t
#include "mtrk_event_t.h"
#include "mtrk_iterator_t.h"
#include <string>
#include <cstdint>
#include <vector>
#include <array>  // For method .get_header()
#include <type_traits>  // std::is_same<>
#include <algorithm>  // std::rotate()
#include <iterator>  // std::back_inserter

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
	using value_type = mtrk_event_t;
	using reference = mtrk_event_t&;
	using const_reference = const mtrk_event_t&;
	using pointer = mtrk_event_t*;
	using const_pointer = const mtrk_event_t*;
	using iterator = mtrk_iterator_t;
	using const_iterator = mtrk_const_iterator_t;
	using difference_type = std::ptrdiff_t;

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
	mtrk_t(mtrk_const_iterator_t,mtrk_const_iterator_t);

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
	std::vector<mtrk_event_t> evnts_ {};
};
std::string print(const mtrk_t&);
// Prints each mtrk event as hexascii (using dbk::print_hexascii()) along
// with its cumtk and onset tick.  The output is valid syntax to brace-init 
// a c++ array.  
std::string print_event_arrays(mtrk_const_iterator_t,mtrk_const_iterator_t);
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
// Returns an iterator to one past the last event with onset tick == that
// of beg.  
// TODO:  Another possible meaning of "simultanious" is all events w/
// tk onset < the tk onset of the off-event matching beg.  
mtrk_iterator_t get_simultanious_events(mtrk_iterator_t, mtrk_iterator_t);

//
// find_linked_off(mtrk_const_iterator_t beg, mtrk_const_iterator_t end,
//						mtrk_event_t on);
// Find the first off event on [beg,end) matching the mtrk_event_t "on" 
// event arg mtrk_event_t on.  
// 
// Returns an event_tk_t<mtrk_const_iterator_t> where member .it is an 
// iterator to the corresponding off mtrk_event_t, and .tk is the cumulative
// number of ticks occuring on the interval [beg,.it).  It is the cumulative 
// number of ticks starting from beg and and continuing to the event
// immediately _prior_ to event .it (the onset tick of event .it is thus
// .tk + .it->delta_time()).  
// If on is not a note-on event, .it==end and .tk==0.  
// If no corresponding off event can be found, .it==end and .tk has the
// same interpretation as before.  
//
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
// Orphan note-on and note-off events are skipped (same behavior as 
// get_linked_onoff_pairs().  
std::string print_linked_onoff_pairs(const mtrk_t&);


//
// OIt split_copy_if(InIt beg, InIt end, OIt dest, UPred pred)
//
// Copy events on [beg,end) for which pred() into dest.  Adjust the delta
// time of each copied event such that in the new track the onset tk of 
// each event is the same as in the original.  Note that the cumtk of the
// events in the new track will in general differ from their values in the
// original track.  
//
// TODO:  This and split_if() should not be templates; they should be regular
// functions of mtrk_iterator_t since only this arg makes sense wrt adjusting
// delta times.  
//
//
template<typename InIt, typename OIt, typename UPred>
OIt split_copy_if(InIt beg, InIt end, OIt dest, UPred pred) {
	uint64_t cumtk_src = 0;
	uint64_t cumtk_dest = 0;
	for (auto curr=beg; curr!=end; ++curr) {
		if (pred(*curr)) {
			auto curr_cpy = *curr;
			curr_cpy.set_delta_time(curr_cpy.delta_time() + (cumtk_src-cumtk_dest));
			*dest++ = curr_cpy;
			cumtk_dest += curr_cpy.delta_time();
		}
		cumtk_src += curr->delta_time();
	}
	return dest;
};
template <typename UPred>
mtrk_t split_copy_if(const mtrk_t& mtrk, UPred pred) {
	auto result = mtrk_t();
	result.reserve(mtrk.size()/2);
	split_copy_if(mtrk.begin(),mtrk.end(),std::back_inserter(result),pred);
	return result;
};

//
// FwIt split_if(FwIt beg, FwIt end, UPred pred)
//
// Partitions the mtrk events in [beg,end) such that events matching pred
// appear at the beginning of the range, preserving relative order of the 
// events.  
// Returns an iterator to one-past the last event matching pred.  The delta 
// times for events on both of the resulting ranges [beg,result_it), 
// [result_it,end) are adjusted so that each event has the same onset tk as 
// in the original range.  
//
// This is similar to std::remove_if(), but adjusts event delta times.  
//
template<typename FwIt, typename UPred>
FwIt split_if(FwIt beg, FwIt end, UPred pred) {
	uint64_t cumtk_src = 0;
	uint64_t cumtk_dest = 0;
	for (auto curr=beg; curr!=end; ++curr) {
		auto curr_dt = curr->delta_time();
		if (pred(*curr)) {
			curr->set_delta_time(curr_dt + (cumtk_src-cumtk_dest));
			cumtk_dest += curr->delta_time();

			auto next = curr+1;
			beg = std::rotate(beg,curr,next);
			if (next != end) {
				next->set_delta_time(next->delta_time()+curr_dt);
			}
		} else {
			// Event *curr has been removed, so no longer contributes to the 
			// src cumtk; curr_dt was added to the delta time for the next 
			// (curr+1) event & will be accounted for in the next iteration.  
			cumtk_src += curr_dt;
		}
	}
	return beg;
};
template <typename UPred>
mtrk_t split_if(mtrk_t& mtrk, UPred pred) {
	auto it = split_if(mtrk.begin(),mtrk.end(),pred);

	auto result = mtrk_t(mtrk.begin(),it);
	mtrk = mtrk_t(it,mtrk.end());
	return result;
};

//
// OIt merge(InIt beg1, InIt end1, InIt beg2, InIt end2, OIt dest)
//
// Merges the sequence of mtrk events on [beg1,end1) with the sequence on
// [beg2,end2) into dest.  Delta times are adjusted so that the onset tk for 
// each event in the merged sequence is the same as in the original 
// sequence.  
//
//
template<typename InIt, typename OIt>
OIt merge(InIt beg1, InIt end1, InIt beg2, InIt end2, OIt dest) {
	uint64_t ontk_1 = 0;
	if (beg1 != end1) {
		ontk_1 = beg1->delta_time();
	}
	uint64_t ontk_2 = 0;
	if (beg2 != end2) {
		ontk_2 = beg2->delta_time();
	}
	uint64_t cumtk_dest = 0;
	auto curr1 = beg1;  auto curr2 = beg2;
	uint64_t ontk_curr = 0;  InIt curr = beg1;
	while (curr1!=end1 || curr2!=end2) {
		// Set curr, ontk_curr
		// curr points at the next event for dest; its desired onset tk is
		// ontk_curr.  After this conditional block, curr1,2 and ontk_1,2 have
		// been advanced/incremented to the next event in the respective
		// stream.
		if (curr1!=end1 && (ontk_1<=ontk_2 || curr2==end2)) {
			ontk_curr = ontk_1;
			curr = curr1++;
			if (curr1 != end1) {
				ontk_1 += curr1->delta_time();
			}
		} else if (curr2!=end2 && (ontk_1>ontk_2 || curr1==end1)) {
			ontk_curr = ontk_2;
			curr = curr2++;
			if (curr2 != end2) {
				ontk_2 += curr2->delta_time();
			}
		}
		auto curr_ev_cpy = *curr;
		curr_ev_cpy.set_delta_time(ontk_curr - cumtk_dest);
		*dest = curr_ev;
		cumtk_dest += (ontk_curr - cumtk_dest);
		++dest;
	}
	return dest;
};


