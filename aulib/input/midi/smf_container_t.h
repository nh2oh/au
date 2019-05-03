#pragma once
#include "midi_raw.h"
#include "mthd_container_t.h"
#include "mtrk_container_t.h"
#include <string>
#include <cstdint>
#include <vector>


//
// smf_t
//
// Holds an smf; owns the underlying data.  Splits each chunk into its
// own std::vector<unsigned char>
//
//
// Alternate design:  Can hold mthd_view_t, mtrk_view_t objects as members.
//
class smf_chrono_iterator_t;

class smf_t {
public:
	smf_t(const validate_smf_result_t&);

	uint32_t size() const;
	uint16_t nchunks() const;
	uint16_t format() const;  // mthd alias
	uint16_t division() const;  // mthd alias
	uint32_t mthd_size() const;  // mthd alias
	uint32_t mthd_data_length() const;  // mthd alias
	uint16_t ntrks() const;
	std::string fname() const;

	mtrk_view_t get_track_view(int) const;
	mthd_view_t get_header_view() const;

	// Returns mtrk events in order by tonset
	smf_chrono_iterator_t event_iterator_begin() const;  // int track number
	smf_chrono_iterator_t event_iterator_end() const;  // int track number
private:
	std::string fname_ {};
	std::vector<std::vector<unsigned char>> d_ {};  // All chunks, incl ::unknown
};
std::string print(const smf_t&);

// Draft of the the chrono_iterator
std::string print_events_chrono(const smf_t&);
// Uses the iterator
std::string print_events_chrono2(const smf_t&);





//
// smf_chrono_iterator_t
//
// Returned from smf_t.event_iterator_begin().  Returns all the mtrk_events
// in the smf_t in chronological order, ie, w/o regard to the track number
// in which the event is encoded (format 1 files).  Internally, holds a 
// std::vector<track_state_t>, which, for each track in the collection, 
// holds an end iterator, an iterator to the _next_ event scheduled on that
// track, and the cumulative tick value, which is the onset time (tick) 
// corresponding to the _previous_ event in the track (one event prior to 
// the next-event iterator).  The event indicated by an iterator object, ie,
// the mtrk event that the iterator is currently "pointing to" is encoded 
// implictly:  it is the event for which the quantity 
// tonset = trks_[i].cumtk + (*trks_[i].next).delta_time()
// is minimum.  
// This has to me calculated for all i on every call to operator++() and
// operator*().  
//
// In an alternate design, which is probably simpler, trks_[i].cumtk holds 
// the tonset for trks_[i].next.  When trks_[i].next==trks_[i].end, however,
// trks_[i].cumtk has a different meaning.  
//
// TODO:  MIDI tracks are indeiendent midi streams; events on diff mtrks do
// not crosstalk.  
//
// TODO:  Need to incorporate an understanding of tick units and respond
// to meta events that change those units.  
//
// TODO:  Don't intersperse events for format 2 files
//
// How "attached" should this be to class smf_t?  As written, will work to 
// intersperse events from any collection of mtrk_view_t's.  
//
//
//


struct track_state_t {
	mtrk_iterator_t next;
	mtrk_iterator_t end;
	int32_t cumtk;  // The time at which the _previous_ event on the track initiated
};
struct smf_event_t {
	uint16_t trackn;
	int32_t tick_onset;  // cumulative (TODO: ?)
	mtrk_event_container_sbo_t event;
};
class smf_chrono_iterator_t {
public:
	smf_event_t operator*() const;
	smf_chrono_iterator_t& operator++();
	//smf_chrono_iterator_t& operator++(int);
	bool operator==(const smf_chrono_iterator_t&) const;
	bool operator!=(const smf_chrono_iterator_t&) const;
private:
	smf_chrono_iterator_t(const std::vector<track_state_t>&);  // used by smf_t

	struct present_event_t {
		uint16_t trackn;
		int32_t tick_onset;
	};
	present_event_t present_event() const;
	std::vector<track_state_t> trks_;

	friend class smf_t;
};









//
// Operations
//
// Print a list of on and off times for each note in each track in the file 
// in order of tick-onset:
//
//
//
std::string print_tied_events(const smf_t&);



