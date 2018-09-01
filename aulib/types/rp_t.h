#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "de_t.h"
#include <map>
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

/*
class rp_t2 {
public:
	rp_t2()=default;
	explicit rp_t2(ts_t const&);
	explicit rp_t2(ts_t const&, std::vector<nv_t> const&);

	void push_back(nv_t);

	std::string print() const;
	std::string print_bidx() const;
private:
	struct vgroup {
		nv_t e {};
		size_t usrix {0};
		size_t tie_f {0};
		size_t tie_b {0};
	};
	std::vector<vgroup> m_e {};
	double m_nbeats {};
	double m_nbars {};
	size_t m_nusrelems {0};
	ts_t m_ts;
};
*/
// TODO
// 1)  Some sort of interospection/analysis functions...  Are there 
//     bar-spanning elements?  What is the nv_t dist?  the ts?
// 
// 2)  Ability to change the ts (if ts is going to be a member, need to
//     be able to set it).   
//
// 3)  Bar fragments???
//
// 4)  More elaborate printing... different nv_t's at different vertical 
//     positions, etc
//
// 5)  Editing ability?  Extend by multiple??  Replace bar-spanning elements
//
// 6)  Set bar number of element 0 (ie set pickup bar) ???
//
//
class rp_t {
public:
	rp_t() = default;
	explicit rp_t(ts_t const&);
	explicit rp_t(ts_t const&, std::vector<nv_t> const&);
	explicit rp_t(ts_t const&, std::vector<std::chrono::milliseconds> const&,
		tempo_t const&, std::chrono::milliseconds const&);

	// Getters
	std::string print();  // Calls build_bidx(), hence not const
	std::string printbidx();  // Calls build_bidx(), hence not const
	rp_t subrp(bar_t const&, bar_t const&) const;  
		// [From, to)
		// To get all the elements belonging to only bar n, call
		// subrp(bar_t{n}, bar_t{n+1})
	bar_t nbars() const;
	beat_t nbeats() const;
	size_t nelems() const;
	std::map<nv_t,size_t> nv_members() const;

	std::vector<std::chrono::milliseconds> dt(tempo_t const&) const;

	// Setters
	void set_ts(ts_t const&);
	void push_back(nv_t const&);
private:
	//----------------------------------------------------------------------------
	// Data members
	ts_t m_ts {};  // Default ts_t == 4/4
	std::vector<nv_t> m_rp {};
	
	// m_bidx:  "Bar indices"
	// Only computed upon a call to build_bidx() (private function)
	//
	// Bar i of m_rp contains elements:
	// [m_rp[m_bidx[i].start], m_rp[m_bidx[i].end);   NB:  [start,end)
	// 
	// If bar i of m_rp does not end evenly, but instead an element
	// within bar i spans the bar i->(i+1) boundry, but then bar i+2
	// starts evenly, ex:
	//       e           e+4
	// ... | q  q  q  h  h  q  |  q  q  q  q  |...
	//     i                   i+2
	//
	// m_bidx[i] =   {e,   true,  e+4, false}  // bar i does _not_ include e+4 
	// m_bidx[i+1] = {e+3, false, e+6, true }  // bar (i+1) _does_ include e+3
	// m_bidx[i+2] = {e+6, true,  e+10, true}
	//
	// start_exact and end_exact are redundant:
	// m_bidx[i].end_exact == m_bidx[i+1].start_exact
	//
	struct bidx {
		std::vector<nv_t>::size_type start {0};  // The first element of the bar
		bool start_exact {true};  
			// Is the start element of bar i the end element of bar i-1 ?
		std::vector<nv_t>::size_type end {0};  // The first element of the _next_ bar
		bool end_exact {false};
			// Is the end element of bar i the start element of bar i+1 ?
	};
	std::vector<bidx> m_bidx {bidx {}};  // Does this need to be a member???

	beat_t m_tot_nbeats {0};  // Exact
	bar_t m_tot_nbars {0};  // Exact; std::ceil(m_tot_nbars) == m_bidx.size()

	//----------------------------------------------------------------------------
	// Private functions
	void build_bidx();
};

// TODO:  Some (all?) of these belong in either ts_t.h or beat_bar_t.h
// or nv_t.h ...
// Number of beats spanned by a given nv_t for a given ts.  
beat_t nbeat(ts_t const&, nv_t const&);
beat_t nbeat(ts_t const&, bar_t const&);
// Number of bars spanned by a given nv_t for a given ts.  
// Overload for a given nbeats
bar_t nbar(ts_t const&, nv_t const&);
bar_t nbar(ts_t const&, beat_t const&);
std::vector<bar_t> cum_nbar(ts_t const&, std::vector<nv_t> const&);

//nv_t closest_nv(ts_t const&, beat_t const&, std::set<nv_t>)

