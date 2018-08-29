#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include <string>
#include <vector>


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
class rp_t {
public:
	explicit rp_t();
	explicit rp_t(ts_t const&);
	explicit rp_t(ts_t const&, std::vector<nv_t> const&); 

	// Getters
	std::string print();  // Calls build_bidx(), hence not const
	std::string printbidx();  // Calls build_bidx(), hence not const

	bar_t nbars() const;
	beat_t nbeats() const;
	int nelems() const;

	rp_t subrp(bar_t const&, bar_t const&) const;  
		// [From, to)
		// To get all the elements belonging to only bar n, call
		// subrp(bar_t{n}, bar_t{n+1})

	// Setters
	void push_back(nv_t const&);
private:
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

	beat_t m_nbeats_pickup {0};
	bar_t m_nbars_pickup {0};

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


// Print a vector of nv_ts
// Old, non-rp_t-member version... should delete
//std::string printrp(ts_t const&, std::vector<nv_t> const&);

std::string rp_t_info();

// Convert a vector of durations (units ~ time) to an rp
//  delta_t, ts, bpm, resolution-in-seconds
std::vector<nv_t> deltat2rp(std::vector<double> const&, ts_t const&, 
	double const&, double const&);

//  rp, ts, bpm
std::vector<double> rp2deltat(std::vector<nv_t> const&, ts_t const&, 
	double const&);

double nv2dt(nv_t const&, ts_t const&, double const&);

std::string deltat2rp_demo();




