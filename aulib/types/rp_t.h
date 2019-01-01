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
// .nbeats(), etc) under some ts_t.  The sequence may have an initial offset, considered part of
// the span of the sequence.  Ex: For a single q-note, if start_ == 0.5_bt, and ts_ == "4/4", 
// rp_ will == {0.5,q}, and rp.nbeats() will return 1.5_bt.  
//
// TODO:
// -> Insert()
// -> begin(), end() iterator generators
//

class rp_t {
public:
	rp_t()=default;
	explicit rp_t(const ts_t&);
	explicit rp_t(const ts_t&, const beat_t&);  // start_
	explicit rp_t(const ts_t&, const std::vector<d_t>&);
	explicit rp_t(const ts_t&, const beat_t&, const std::vector<d_t>&);
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
	// T_out operator[](T_in pos)
	// Returns the first element <= T_in and the onset position of said element.  
	// Callers can request pos at any arbitrary position in the rp provided that pos >= rp_front().on
	// && pos < rp_.back().on+nbeat(tp_.ts_,rp_.back().e), including positions that fall between onset
	// events.  Ex, for 
	// rp.ts_ == "4/4"_ts; rp.rp_ == q q q q | q q e e e e |
	// rp[0.5_bt] returns { q, 0_bt };
	// Beat 0.5 occurs after the first quarter note (onset at beat 0) but before the second quarter
	// note (onset at beat 1).  
	// Illegal inputs are rp[-0.5_bt], rp[8_bt]
	//
	// rp[p].on + nbeat(rp.ts(),rp[p].e) => idx of the next element
	// p - rp[p].on => distance to start of _present_ element
	// p - (rp[p].on + nbeat(rp.ts(),rp[p].e)) => distance to start of _next_ element
	//
	struct rp_element_t {
		d_t e {d::z};  // element presently within
		beat_t on {0_bt};  // (cumulative) onset beat of present element
	};
	rp_element_t operator[](int) const;  // Returns the d_t elements corresponding to sounded event i
	rp_element_t operator[](const d_t&) const;
	rp_element_t operator[](const beat_t&) const;
	rp_element_t operator[](const bar_t&) const;
private:
	ts_t ts_ {4_bt,d::q,false};

	// Beat number corresponding to the onset of the first rp element.  Redundant with rp_[0].on
	// but needed so an empty rp_t can be constructed w/a nonzero start pos.  
	beat_t start_ {0};  
	
	// .on for each entry is the _cumulative_ onset position in the rp, including the effect
	// of a nonzero start_.  Once rp_size() > 0, start_ is redundant with rp_[0].on and is no
	// longer needed.  
	std::vector<rp_element_t> rp_ {};

};

