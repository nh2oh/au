#include "rp_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "..\util\au_util.h"  // bsprintf()
#include "..\util\au_algs.h"  // unique_n() in nv_members()
#include "..\util\au_algs_math.h"  // aprx_int()
#include <string>
#include <vector>
#include <cmath> // pow()
#include <algorithm> // find_if(), copy()
#include <chrono>
#include <map>



rp_t::rp_t(ts_t const& ts) {
	m_ts = ts;
}

rp_t::rp_t(ts_t const& ts, std::vector<d_t> const& nv) {
	m_ts = ts;
	for (auto e : nv) {
		push_back(e);
	}
}



// The reverse of member function dt(...)
// Each element of dt is interpreted as the nearest integer multiple of res
// and the d_t closest to this dt is chosen.  
rp_t::rp_t(ts_t const& ts_in, std::vector<std::chrono::milliseconds> const& dt,
	tempo_t const& tempo, std::chrono::milliseconds const& res) {

	m_ts = ts_in;

	std::map<beat_t,d_t> nvset {};
	for (int m=-3; m<8; ++m) { // -3 => 8 (qw) 5 => 1/32
		for (int n=0; n<5; ++n) {
			auto curr_nv = d_t{d_t::mn{m,n}};
			nvset[nbeat(m_ts,curr_nv)]=curr_nv;
		}
	}

	// The t difference between each dt element and the dt of the nv_t
	// chosen to represent said element.  
	std::vector<std::chrono::milliseconds> error {};

	decltype(res/res) one = {1};
	for (auto e : dt) {
		// Find the integer multiple r of res closest to e:
		// e/res is essentially integer division, so r*res will always 
		// be <= e, and (r+1)*res will always be > e.  If e < res, e/res
		// = r == 0.  
		auto r = e/res;	
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);
		// I do not want to skip events < res since this will change the
		// size of the vectors (or i'll have to have 0-duration elements...
		// which don't exist...)
		if (r == 0) { r = one; }

		auto curr_nbts = (r*res)*tempo;

		// Find the element in nvset for which nbeats is closest to curr_nbts
		// TODO:  Generalize to find_closest()
		beat_t nbts_best_nv {};
		d_t best_nv {};
		auto nvset_firstgt = std::find_if(nvset.begin(),nvset.end(),
			[&](std::pair<beat_t,d_t> const& in){return(in.first > curr_nbts);});
		if (nvset_firstgt == nvset.begin()) {
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else if (nvset_firstgt == nvset.end()) {
			--nvset_firstgt;
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else {
			auto first_gt = *nvset_firstgt;  // closest > curr_nbts
			--nvset_firstgt;
			auto first_lteq = *nvset_firstgt;  // closest <= curr_nbts
			if ((first_gt.first-curr_nbts) < (curr_nbts-first_lteq.first)) {
				best_nv = first_gt.second;
				nbts_best_nv = first_gt.first;
			} else {
				best_nv = first_lteq.second;
				nbts_best_nv = first_lteq.first;
			}
		}

		push_back(best_nv);
		//m_tot_nbeats += nbts_best_nv;

		// Error measure
		auto x= nbts_best_nv/tempo;
		error.push_back(x - e);
		
	}
	//m_tot_nbars = nbar(m_ts,m_tot_nbeats);
	wait();

}




void rp_t::push_back(d_t d) {
	int working_bar_n = std::floor(m_nbars);
	int next_bar_n = std::floor(m_nbars)+1;
	auto x = m_ts.bar_unit();
	auto d_to_next_bar = next_bar_n*m_ts.bar_unit()-m_dtot;
	
	
	auto d_singlets = d.to_singlets_partition_max(d_to_next_bar,m_ts.bar_unit());
	for (size_t i=0; i<d_singlets.size(); ++i) {
		m_e.push_back(vgroup{d_singlets[i],m_usridx,d_singlets.size()-1-i,i});
		wait();
	}
	// also need to append rests
	++m_usridx;
	m_dtot += d;
	m_nbars += nbar(m_ts,d).to_double();
	m_nbeats += nbeat(m_ts,d).to_double();
}

std::string rp_t::print() const {
	std::string s {};
	bar_t curr_bar {0};
	for (auto e : m_e) {
		
		//  Want: curr_bar += (curr_d += e)/m_ts
		// if (curr_bar.exact())
		
		if (e.tie_b > 0 && e.tie_f == 0) { s += ")"; }
		s += e.e.print();
		if (e.tie_b==0 && e.tie_f > 0) { s += "("; }

		curr_bar += nbar(m_ts,e.e);
		if (aprx_int(curr_bar/bar_t{1.0})) {
			s += " | ";
		} else {
			s += " ";
		}
		wait();
	}
	return s;
}

bar_t rp_t::nbars() const {
	return bar_t{m_nbars};
}
beat_t rp_t::nbeats() const {
	return beat_t{m_nbeats};
}
size_t rp_t::nelems() const {
	return m_usridx;
}


/*
rp_t::rp_t(ts_t const& ts_in) {
	m_ts = ts_in;
}

rp_t::rp_t(ts_t const& ts_in, std::vector<nv_t> const& nvs_in) {
	m_ts = ts_in;
	m_rp = nvs_in;
	m_tot_nbars = cum_nbar(ts_in,nvs_in).back();
	m_tot_nbeats = nbeat(m_ts,m_tot_nbars);
}

// The reverse of member function dt(...)
// Each element of dt is interpreted as the nearest integer multiple of res
// and the nv_t closest to this dt is chosen.  
rp_t::rp_t(ts_t const& ts_in, std::vector<std::chrono::milliseconds> const& dt,
	tempo_t const& tempo, std::chrono::milliseconds const& res) {

	m_ts = ts_in;

	std::map<beat_t,nv_t> nvset {};
	for (int m=-3; m<8; ++m) { // -3 => 8 (qw) 5 => 1/32
		for (int n=0; n<5; ++n) {
			auto curr_nv = nv_t{std::pow(2,-m),n};
			nvset[nbeat(m_ts,curr_nv)]=curr_nv;
		}
	}

	// The t difference between each dt element and the dt of the nv_t
	// chosen to represent said element.  
	std::vector<std::chrono::milliseconds> error {};

	decltype(res/res) one = {1};
	for (auto e : dt) {
		// Find the integer multiple r of res closest to e:
		// e/res is essentially integer division, so r*res will always 
		// be <= e, and (r+1)*res will always be > e.  If e < res, e/res
		// = r == 0.  
		auto r = e/res;	
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);
		// I do not want to skip events < res since this will change the
		// size of the vectors (or i'll have to have 0-duration elements...
		// which don't exist...)
		if (r == 0) { r = one; }

		auto curr_nbts = (r*res)*tempo;

		// Find the element in nvset for which nbeats is closest to curr_nbts
		// TODO:  Generalize to find_closest()
		beat_t nbts_best_nv {};
		nv_t best_nv {};
		auto nvset_firstgt = std::find_if(nvset.begin(),nvset.end(),
			[&](std::pair<beat_t,nv_t> const& in){return(in.first > curr_nbts);});
		if (nvset_firstgt == nvset.begin()) {
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else if (nvset_firstgt == nvset.end()) {
			--nvset_firstgt;
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else {
			auto first_gt = *nvset_firstgt;  // closest > curr_nbts
			--nvset_firstgt;
			auto first_lteq = *nvset_firstgt;  // closest <= curr_nbts
			if ((first_gt.first-curr_nbts) < (curr_nbts-first_lteq.first)) {
				best_nv = first_gt.second;
				nbts_best_nv = first_gt.first;
			} else {
				best_nv = first_lteq.second;
				nbts_best_nv = first_lteq.first;
			}
		}

		m_rp.push_back(best_nv);
		m_tot_nbeats += nbts_best_nv;

		// Error measure
		auto x= nbts_best_nv/tempo;
		error.push_back(x - e);
		
	}
	m_tot_nbars = nbar(m_ts,m_tot_nbeats);
	wait();

}

bar_t rp_t::nbars() const {
	return m_tot_nbars;
}
beat_t rp_t::nbeats() const {
	return m_tot_nbeats;
}
size_t rp_t::nelems() const {
	return m_rp.size();
}

std::map<nv_t,size_t> rp_t::nv_members() const {
	return unique_n(m_rp);
}

void rp_t::set_ts(ts_t const& ts_in) {
	m_ts = ts_in;
	m_tot_nbars = cum_nbar(ts_in,m_rp).back();
	m_tot_nbeats = nbeat(m_ts,m_tot_nbars);
}

void rp_t::push_back(nv_t const& nv_in) {
	beat_t nbeats_in = nbeat(m_ts,nv_in);
	m_tot_nbars += nbar(m_ts,nbeats_in);
	m_tot_nbeats += nbeats_in;

	m_rp.push_back(nv_in);
}

std::vector<std::chrono::milliseconds> rp_t::dt(tempo_t const& bpm) const {
	std::vector<std::chrono::milliseconds> dt {};
	for (auto e : m_rp) {
		auto dt_ms = nbeat(m_ts,e)/bpm;
		dt.push_back(dt_ms);
	}
	return dt;
}

// TODO:  Check to>from, to not > max(m_rp), etc
// TODO:  Should be a template: from,to could be bar_t, beat_t, seconds...
rp_t rp_t::subrp(bar_t const& from,bar_t const& to) const {
	auto cumrp = cum_nbar(m_ts,m_rp);

	auto cum_its = std::find_if(cumrp.begin(),cumrp.end(),
		[&](bar_t const& cnbar_in){return(cnbar_in >= from);});
	auto cum_ite = std::find_if(cum_its,cumrp.end(),
		[&](bar_t const& cnbar_in){return(cnbar_in >= to);});
	// Note >=:  We want the end iterator to point to one past the
	// final element of interest.  This should be obtained using >,
	// however, the elements in cumrp are indicating the cumulative 
	// nbars at the _end_ of the element (the sum does not start at 0).  
	// Below, the end iterator is incremented by 1; copy then copies
	// exclusive of this final element.  

	auto it_s = cum_its-cumrp.begin();
	auto it_e = cum_ite-cumrp.begin();
	if(cum_ite < cumrp.end()) {
		++it_e; // std::copy => [it_s,it_e)
	}

	std::vector<nv_t> result{};
	std::copy(m_rp.begin()+it_s,m_rp.begin()+it_e,std::back_inserter(result));

	return rp_t{m_ts,result};
}

// Implemented as a separate function the user has to manually call, and not
// implemented durring push_back(), because there are enough situations where
// the whole thing has to be re-computed from scratch that it is more 
// convenient to have a purpose-build function.  For example, if two or
// more rp_t's are spliced together, or if an element is inserted into the
// middle of an rp_t, of if the bar-numbering of rp-element 0 changes, etc
// etc.  
//
// Also, there is no need to carry all this data around w/ every rp_t object;
// it is only used occasionally.  
//
//
// TODO:  Crashes if m_rp is empty??
void rp_t::build_bidx() {
	// Duplicates cum_nbar(ts,std::vector<nv_t>), but i need a vector
	// of doubles, not a vector of bar_t.  
	std::vector<double> rp_cum {}; rp_cum.reserve(m_rp.size());
	bar_t totnb {0.0};  // Only used by the lambda
	std::for_each(m_rp.begin(),m_rp.end(),
		[&](nv_t const& currnv) {
			totnb+=nbar(m_ts,currnv);
			rp_cum.push_back(totnb.to_double());
		});

	std::vector<bidx> baridx {}; //(std::ceil(m_tot_nbars.to_double()),bidx{});
	baridx.push_back({0,true,0,false});

	int curr_bar=0; size_t i=0;
	while (i<rp_cum.size()) {
		if (rp_cum[i] <= (curr_bar+1)) {
			// Note that we only move to the next element in rp_cum if a bar-boundary
			// has been not been crossed in going from element i-1 to i.  The while 
			// loop can run more than once w/ the same value of i if in passing from
			// rp element i-1 -> i more than one bar boundry is crossed.  
			++i;
		} else if (rp_cum[i] > (curr_bar+1)) {
			if (i == 0) {
				// Special case for the first iteration where there is no rp_cum[i-1].
				// This situation arises if rp element 0 is something larger than a 
				// w-note.  
				//
				// If element i=0 is > curr_bar+1 (==0+1 for i==0), curr_bar can not 
				// possibly have end_exact == true.  end_exact==true => rp element i
				// corresponds to exactly 1 bar, but it's clearly > 1 bar.  
				//
				baridx[curr_bar].end_exact = false;

				baridx[curr_bar].end = i+1;
				// Since element i (here, ==0) is spans > 1 bar the first element past 
				// bar 0 must be i+1.  
			} else {
				baridx[curr_bar].end_exact = (rp_cum[i-1] == (curr_bar+1));
				if (rp_cum[i-1] == (curr_bar+1)) {
					// Element i is not needed to complete curr_bar; it is a member
					// of curr_bar+1 and not a member of curr_bar
					baridx[curr_bar].end = i; // 1 past the end of curr_bar
				} else if (rp_cum[i-1] < (curr_bar+1)) {
					// Element i is needed to complete curr_bar; it is a member
					// of curr_bar
					baridx[curr_bar].end = i+1;
				}
			}

			++curr_bar;
			baridx.push_back({i,baridx.back().end_exact,0,false});
			//baridx[curr_bar].start = i;
			//baridx[curr_bar].start_exact = baridx[curr_bar-1].end_exact;
				// Duplicate of baridx[prev_bar].end_exact
		}
	}
	baridx.back().end = rp_cum.size();

	m_bidx = baridx;
}


std::string rp_t::print() {
	build_bidx();

	std::string s {};
	std::string sep {","};
	std::string sep_bar_exact {"|"};
	std::string sep_bar_nexact {"!"};

	size_t n_trim {0};
	bar_t cum_nbar {0};
	for (size_t i=0; i<m_bidx.size(); ++i) {
		for (auto j=m_bidx[i].start; (j<m_bidx[i].end); ++j) {
			if (j == m_bidx[i].start && !m_bidx[i].start_exact) {
				// If the start of bar i is not exact, the rp element indicated
				// by m_bidx[i].start has already been printed as a part of the 
				// previous bar.  If m_bidx[i].start_exact is false, m_bidx[i].start
				// is a member of the previous bar (has an idx j < m_bidx[i-1].end).  
				continue;
			}

			s += m_rp[j].print();
			if (j < (m_bidx[i].end-1)) {
				s += sep; // Not the last iter
			}
			s += " ";
		}  // To next element j of bar i

		// For the very last bar, which is _always_ incomplete, don't print
		// a bar separator char.  
		if (i != (m_bidx.size()-1)) {  // Not the last bar
			if (m_bidx[i].end_exact) {
				s += sep_bar_exact;
			} else {
				s += sep_bar_nexact;
			}
			s += " ";
		}
	}  // To next bar i

	return s;
}

std::string rp_t::printbidx() {
	build_bidx();

	std::string s {};
	for (int i=0; i<m_bidx.size(); ++i) {
		s += bsprintf("%d [%d]  ->  %d [%d]\n",
			m_bidx[i].start, m_bidx[i].start_exact, 
			m_bidx[i].end, m_bidx[i].end_exact);
	}
	return s;
}
*/

//-----------------------------------------------------------------------------
// Support functions

beat_t nbeat(ts_t const& ts_in, d_t const& d_in) {
	return beat_t{d_in/(ts_in.beat_unit())};
}
beat_t nbeat(ts_t const& ts_in, bar_t const& nbars_in) {
	return (ts_in.beats_per_bar())*(nbars_in.to_double());
}

bar_t nbar(ts_t const& ts_in, d_t const& d_in) {
	beat_t nbeats = nbeat(ts_in, d_in);
	return nbar(ts_in,nbeats);
}
bar_t nbar(ts_t const& ts_in, beat_t const& nbts_in) {
	auto nbars_exact = nbts_in/(ts_in.beats_per_bar());
	return bar_t {nbars_exact};
}

std::vector<bar_t> cum_nbar(ts_t const& ts_in, std::vector<d_t> const& d_in) {
	std::vector<bar_t> rp_cum {}; rp_cum.reserve(d_in.size());
	
	bar_t totnb {0.0};  // Only used by the lambda
	std::for_each(d_in.begin(),d_in.end(),
		[&](d_t const& currnv){rp_cum.push_back(totnb+=nbar(ts_in,currnv));});

	return rp_cum;
}

