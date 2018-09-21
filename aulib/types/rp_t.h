#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "de_t.h"
#include <string>
#include <vector>


//
// Class rp_t
// Represents a sequence of d_t's under some ts_t
//
// There are two ways this might be designed
// 1)  The internal representation is a container where each element is a
//     d_t (which may or may not correspond to a representable nv_t), the 
//     value of which was set by the caller, either upon construction, or
//     via sequential .push_back().  Each entry in the container represents
//     something tangible to the caller, ie, a note onset or a rest.  
//     For this case:
//     1)  There is likely to be a straightforward mapping between related
//         sequences and objects posessed by the caller.  For example,  
//         a caller that has assigned a sequence of N d_t's likely has a
//         related sequence of N note_t's.  It will be very easy for the to 
//         rp_t object to return data relevant to the caller, count the
//         number of "soundable events" in the sequence, etc.  
//
//     2)  It is much less straightforward to compute bar boundaries and 
//         legal nv_t's.  Each element in the container, is an internal 
//         data structure which decomposes the user's d_t into a sequence of 
//         nv_t's where none will span a bar line.  Note that the vast 
//         majority of the caller's d_t elements are < 1 bar and do not span.  
//         This is therefore a bit wastefull.  
//         Maybe the container looks like this:
//         
//         struct rp_elem_t {
//            d_t user {};
//            std::vector<d_t> e {};  // durations are valid for nv_t's
//            std::vector<size_t> b {};  // a barline _follows_ e[b[i]]
//         };
//         
//         For most elements in the rp_t container .e.size()==1 and and 
//         .b.size() == 0.  
//     Also note that because the outter container holds the soundable
//     events, iterating over the outter and inner containers, say, to print
//     the sequence, won't end us w/a sequence spanning an integer number of
//     bars.  The algorithm suggested above could be modified to left-pad 
//     each new entry with rests, but then you'd have to edit entry i-1 when
//     adding entry i.  
// 2)  The internal representation is a container where each element may or
//     may not exist in the world of the caller.  The terminal element or  
//     elements of the container is always a rest or sequence thereof which 
//     exactly completes the final bar.  When a caller appends an element that
//     spans a bar boundry it is broken into into multiple, as necessary.  
//     The object keeps track of which elements are "virtual," ie, added just
//     to keep the rp_t even.  **There is no "nesting" of containers as in
//     option (1) above.  
//
//     std::vector<vgroup> e;  
//     struct vgroup {
//        d_t e {};  // valid nv_t that does not span a barline
//        size_t usrix {};  // The # of preceeding vgroups in e w/ tie_b = 0;
//        size_t tie_b {};  // tie e[i] _back_ to e[i-e[i].tie_b]
//        size_t tie_f {};  // tie e[i] _forward_ to e[i+e[i].tie_f]
//     };
//     size_t n_vgroups {};
//
//     Because e.size() is totally unrelated to what the user may expect, the
//     mapping back to the user's reality is pulled off through usrix.  Any
//     sort of routine that associates d_t elements in the rp_t with a user-
//     owned/created sequence has to be able to return d_t "chunks" 
//     w/ the same userix.  
//
//
// Operations that need to be supported:
// -Return the sequence of de_t's initially supplied by the caller
// -Print
// -Replace element i, where i is the indexing of the caller's de_t sequence
//  with something else.
// -Replace element i, where i is the indexing of the internal container
//  with somethign else.  Say the user selects a "virtual" tied element 
//  from the output of print().  
//


class rp_t {
public:
	rp_t()=default;
	explicit rp_t(ts_t const&);
	explicit rp_t(ts_t const&, std::vector<d_t> const&);
	//TODO:  Split the quantizer out of this
	explicit rp_t(ts_t const& ts_in, 
		std::vector<std::chrono::milliseconds> const& dt, tempo_t const& tempo,
		std::chrono::milliseconds const& res);

	void push_back(d_t);

	std::string print() const;
	bar_t nbars() const;
	beat_t nbeats() const;
	size_t nelems() const;
	ts_t ts() const;
	
	std::vector<d_t> to_duration_seq() const;
	// TODO:  Should take options, for example, maybe:  
	// - Return a sequence of durations corresponding to the "sounded events,"
	// ie, the sequence as push'ed_back() int construct the present rp_t
	// - Return a sequence of singlets (obv some may span bar-boundries)
	// - ...
	//

	d_t operator[](int) const;
		// Returns the d_t elements as initially passed in
private:
	struct vgroup {
		d_t e {};
		size_t usrix {0};
		size_t tie_f {0};
		size_t tie_b {0};
	};
	std::vector<vgroup> m_e {};
	beat_t m_nbeats {};
	bar_t m_nbars {};
	d_t m_dtot {};
	size_t m_usridx {0};
	ts_t m_ts;
};

