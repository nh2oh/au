#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include <string>
#include <vector>
#include <chrono>

//
// Class rp_t
// Represents a continuous sequence of d_t's spanning some distance (queryable with .nbars(), 
// .nbeats(), etc) under some ts_t.  The sequence may have an initial offset considered part of
// the span of the sequence.  
//
// Operations (that need to be) supported:
// -Return the sequence of de_t's initially supplied by the caller
// -Print w/ barlines, appropriate ties, etc
// -Replace/access element i, where i is an index coresponding to the caller's de_t sequence
//  used to construct the container.  
// -Replace/access arbitrary locations within the sequence not nec. == to an event in the caller's
//  de_t sequence used to construct the container.  Ex, access a beat between two rhythm elements.  
//
//

class rp_t {
public:
	rp_t()=default;
	explicit rp_t(const ts_t&);
	explicit rp_t(const ts_t&, const beat_t&);  // start_
	explicit rp_t(const ts_t&, const std::vector<d_t>&);
	explicit rp_t(const ts_t&, const std::vector<std::chrono::milliseconds>&, const tempo_t&,
		const std::chrono::milliseconds&);

	// Callers push_back() d_t's only, hence the beat-number of each added element (modulo the
	// overall "phase shift") is not under the control of the caller.  A caller can not create 
	// an rp_t with a "gap" between adjacent elements.  
	// The container can not represent caller-annotated ties, ex:  ... | e e( )e e| ...
	void push_back(d_t);

	std::string print() const;
	bar_t nbars() const;  // Total length
	beat_t nbeats() const;  // Total length
	int nevents() const;  // Total number of note-onset events
	ts_t ts() const;
	
	std::vector<d_t> to_duration_seq() const;

	//
	// T_out operator[](T_in)
	// Caller's can request the element at any arbitrary position in the rp, including positions
	// falling between note-onset events.  Ex, for 
	// rp.ts_ == "4/4"_ts; rp.rp_ == q q q q | q q e e e e |
	// what does rp[0.5_bt] return?
	// -> { q, 0_bt }  // Element currently within, onset position of said element
	// -> { q, 0.5_bt }  // Element currently within, time remaining until next said element
	// -> { q, 0.5_bt }  // Element currently within, time since onset of said element
	//
	struct rp_element_t {
		d_t e {d::z};  // element presently within
		beat_t on {0_bt};  // cumulative onset beat of present element
	};
	rp_element_t operator[](int) const;  // Returns the d_t elements corresponding to sounded event i
	rp_element_t operator[](const d_t&) const;
	rp_element_t operator[](const beat_t&) const;
	rp_element_t operator[](const bar_t&) const;
private:
	ts_t ts_ {4_bt,d::q,false};
	beat_t start_ {0};  // Beat number corresponding to the first element
	// beat_t tot_nbeats_ {0};  // == rp_.back().on - start_
	std::vector<rp_element_t> rp_ {};

};

