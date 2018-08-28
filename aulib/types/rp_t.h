#pragma once
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include <string>
#include <vector>


class rp_t {
public:
	explicit rp_t();
	explicit rp_t(ts_t const&);
	explicit rp_t(ts_t const&, std::vector<nv_t> const&); 

	// Getters
	std::string print() const;
	std::string printbidx() const;

	bar_t nbars() const;
	beat_t nbeats() const;

	rp_t get_subrp(int const&) const;
	rp_t get_subrp(int const&, int const&) const;  // [From, to)
	rp_t get_subrp(bar_t const&) const;
	rp_t subrp(bar_t const&, bar_t const&) const;  // [From, to)
	rp_t get_subrp(beat_t const&) const;
	rp_t get_subrp(beat_t const&, beat_t const&) const;  // [From, to)


	// Setters
	void push_back(nv_t const&);
private:
	ts_t m_ts {};  // Default ts_t == 4/4
	std::vector<nv_t> m_rp {};

	// m_bidx
	//
	// Bar indices... may or may not want to keep this as a continuously
	// updated member, however, the algorithm is useful in general.  It is
	// needed for rp_t::print() and many other external functions.  There
	// needs to be a central implementation of it, and it is ok for it to
	// live here at least for now.  
	//
	// Bar i of m_rp contains elements:
	// [m_rp[m_bidx[i].start], m_rp[m_bidx[i].end);   NB:  [start,end)
	// 
	// If bar i of m_rp does not end evenly, but instead, an element
	// within bar i spans the bar i->(i+1) boundry, but then bar i+2
	// starts evenly,
	//       e           e+4
	// ... | q  q  q  h  h  q  |  q  q  q  q  |...
	//     i                   i+2
	//
	// m_bidx[i] =   {e,   true,  e+4, false}  // bar i does _not_ include e+4 
	// m_bidx[i+1] = {e+3, false, e+6, true }  // bar (i+1) _does_ include e+3
	// m_bidx[i+2] = {e+6, true,  e+10, true}
	//
	// If an element of m_rp should span > 1 bar,
	//
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
	std::vector<bidx> m_bidx {bidx {}};

	beat_t m_tot_nbeats {0};  // Exact
	bar_t m_tot_nbars {0};  // Exact; std::ceil(m_tot_nbars) == m_bidx.size()

	beat_t m_nbeats_pickup {0};
	bar_t m_nbars_pickup {0};
};




// Number of beats spanned by a given nv_t for a given ts.  
beat_t nbeat(ts_t const&, nv_t const&);

// Number of bars spanned by a given nv_t for a given ts.  
// Overload for a given nbeats
bar_t nbar(ts_t const&, nv_t const&);
bar_t nbar(ts_t const&, beat_t const&);

// Print a vector of nv_ts
std::string printrp(ts_t const&, std::vector<nv_t> const&);

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




