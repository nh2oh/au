#include "rp_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "..\util\au_error.h"
#include "..\util\au_util.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_random.h"
#include <string>
#include <vector>
#include <cmath> // pow()
#include <algorithm> // find_if(), copy()

rp_t::rp_t() {}

rp_t::rp_t(ts_t const& ts_in) {
	m_ts = ts_in;
}

rp_t::rp_t(ts_t const& ts_in, std::vector<nv_t> const& nvs_in) {
	m_ts = ts_in;
	m_rp = nvs_in;
	m_tot_nbars = cum_nbar(ts_in,nvs_in).back();
	m_tot_nbeats = nbeat(m_ts,m_tot_nbars);
}

bar_t rp_t::nbars() const {
	return m_tot_nbars;
}
beat_t rp_t::nbeats() const {
	return m_tot_nbeats;
}
int rp_t::nelems() const {
	return m_rp.size();
}

void rp_t::push_back(nv_t const& nv_in) {
	beat_t nbeats_in = nbeat(m_ts,nv_in);
	m_tot_nbars += nbar(m_ts,nbeats_in);
	m_tot_nbeats += nbeats_in;

	m_rp.push_back(nv_in);
}

// TODO:  Check to>from, to not > max(m_rp), etc
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

	std::vector<bidx> baridx(std::ceil(m_tot_nbars.to_double()),bidx{});
	baridx[0] = {0,true,0,false};

	int curr_bar=0; int i=0;
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
			baridx[curr_bar].start = i;
			baridx[curr_bar].start_exact = baridx[curr_bar-1].end_exact;
				// Duplicate of baridx[prev_bar].end_exact
		}
	}
	baridx.back().end = rp_cum.size()-1;

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
	for (auto i=0; i<m_bidx.size(); ++i) {
		// For the very last entry k in m_bidx, m_bidx[k].start indicates
		// the first element, but m_bidx[k].end == 0; m_bidx.size() always
		// == the total number of bars in the sequence + 1.  Thus, for the
		// last bar, the loop should terminate on m_rp.size()-1, not
		// m_bidx[k].end.  
		bool lastbar = (i == (m_bidx.size()-1));
		int fin_j = lastbar ? m_rp.size() : m_bidx[i].end;
		for (auto j=m_bidx[i].start; (j<fin_j); ++j) {
			if (j == m_bidx[i].start && !m_bidx[i].start_exact && i != 0) {
				// If the start of bar i is not exact, the rp element indicated
				// by m_bidx[i].start has already been printed as a part of the 
				// previous bar.  If m_bidx[i].start_exact is false, m_bidx[i].start
				// is a member of the previous bar (has an idx j < m_bidx[i-1].end).  
				continue;
			}

			s += m_rp[j].print();
			if (j < (fin_j-1)) {
				s += sep; // Not the last iter
			}
			s += " ";
		}  // To next element j of bar i

		// For the very last bar, which is _always_ incomplete, don't print
		// a bar separator char.  
		if (!lastbar) {
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








//-----------------------------------------------------------------------------
// Support functions

beat_t nbeat(ts_t const& ts_in, nv_t const& nv_in) {
	return beat_t{nv_in/(ts_in.beat_unit())};
}
beat_t nbeat(ts_t const& ts_in, bar_t const& nbars_in) {
	return (ts_in.beats_per_bar())*(nbars_in.to_double());
}

bar_t nbar(ts_t const& ts_in, nv_t const& nv_in) {
	beat_t nbeats = nbeat(ts_in, nv_in);
	return nbar(ts_in,nbeats);
}
bar_t nbar(ts_t const& ts_in, beat_t const& nbts_in) {
	auto nbars_exact = nbts_in/(ts_in.beats_per_bar());
	return bar_t {nbars_exact};
}

std::vector<bar_t> cum_nbar(ts_t const& ts_in, std::vector<nv_t> const& nvs_in) {
	std::vector<bar_t> rp_cum {}; rp_cum.reserve(nvs_in.size());
	
	bar_t totnb {0.0};  // Only used by the lambda
	std::for_each(nvs_in.begin(),nvs_in.end(),
		[&](nv_t const& currnv){rp_cum.push_back(totnb+=nbar(ts_in,currnv));});

	return rp_cum;
}


// TODO:  Deprecate
// Old, non-rp_t-member version 
// 
/*
std::string printrp(ts_t const& ts_in, std::vector<nv_t> const& nv_in) {
	std::string s {};
	std::string sep {", "};
	std::string sep_bar {" | "};

	size_t n_trim {0};
	bar_t cum_nbar {0};
	for (auto i=0; i<nv_in.size(); ++i) {
		s += nv_in[i].print();
		cum_nbar += nbar(ts_in, nv_in[i]);
		if (isapproxint(cum_nbar.to_double(),6)) {
			s += sep_bar;
		} else {
			s += sep;
			n_trim = sep.size();
		}
	}
	s.erase(s.begin()+(s.size()-n_trim),s.end());
	return s;
}*/

std::string rp_t_info() {
	std::string s {};
	std::vector<ts_t> ts_simple {"2/2"_ts,"3/2"_ts,"4/4"_ts,"2/4"_ts,
		"3/4"_ts,"3/8"_ts,"6/8"_ts,"9/8"_ts,"12/8"_ts};
	std::vector<ts_t> ts_compound {"3/4c"_ts,"3/8c"_ts,"6/8c"_ts,"9/8c"_ts,"12/8c"_ts};
	std::vector<ts_t> ts_weird {"5/4"_ts,"5/8"_ts,"7/4"_ts,"11/8"_ts,"11/4"_ts};

	s += "SIMPLE TIME SIGNATURES\n";
	for (auto e : ts_simple) {
		s += e.print() + "   =>   ";
		s += std::to_string(e.beats_per_bar().to_double()) + " ";
		s += e.beat_unit().print() + " -note beats per bar;\n";
		s+= "\tThe bar-equivalent note value is " + e.bar_unit().print() + "\n";
	}
	s += "\n\n";

	s += "COMPOUND TIME SIGNATURES\n";
	for (auto e : ts_compound) {
		s += e.print() + "   =>   ";
		s += std::to_string(e.beats_per_bar().to_double()) + " ";
		s += e.beat_unit().print() + " -note beats per bar;\n";
		s += "\tThe bar-equivalent note value is " + e.bar_unit().print() + "\n";
	}
	s += "\n\n";

	s += "WEIRD TIME SIGNATURES\n";
	for (auto e : ts_weird) {
		s += e.print() + "   =>   ";
		s += std::to_string(e.beats_per_bar().to_double()) + " ";
		s += e.beat_unit().print() + " -note beats per bar;\n";
		s += "\tThe bar-equivalent note value is " + e.bar_unit().print() + "\n";
	}
	s += "\n\n";

	return s;
}


//
// Convert a sequence of note durations (in seconds) to a sequence of 
// note-values.  
//
//
// TODO:  Algorithm based on clustering dt's, not nv_t's
//
std::vector<nv_t> deltat2rp(std::vector<double> const& delta_t, 
	ts_t const& ts_in, double const& bpm, double const& s_resolution) {
	au_assert(delta_t.size()>=2,"A delta-t vector must contain >= 2 events.");
	au_assert(bpm>0,"bpm>0");
	au_assert(s_resolution>0,"s_resolution>0");
	auto bps = bpm/60.0;
	auto bt_resolution = beat_t{bps*s_resolution};

	std::vector<nv_t> ntset {};
	std::vector<double> dtset {};
	for (int m = 0; m<5; ++m) { // 5 => 1/32
		for (int n = 0; n<2; ++n) {
			auto curr_nv = nv_t{std::pow(2,-m),n};
			if (nbeat(ts_in,curr_nv) >= bt_resolution) {
				ntset.push_back(curr_nv);
				dtset.push_back(nv2dt(curr_nv,ts_in,bpm));
			}
		}
	}

	std::vector<nv_t> nts {}; nts.reserve(delta_t.size());
	for (auto curr_dt: delta_t) {
		auto i = nearest_idx(curr_dt,dtset);
		nts.push_back(ntset[i]);
	}
	return nts;
}


//
// Does the reverse of deltat2rp().
// Units of the delta_t vector is seconds.  
//
std::vector<double> rp2deltat(std::vector<nv_t> const& rp_in, 
	ts_t const& ts_in, double const& bpm) {
	au_assert(rp_in.size()>=1);
	au_assert(bpm>0);

	std::vector<double> delta_t(rp_in.size(), 0.0);
	auto bps = bpm/60;
	for (auto curr_nt : rp_in) {
		delta_t.push_back((nbeat(ts_in,curr_nt).to_double())/bps);
	}
	return delta_t;
}

//
// Units of dt is seconds.  
//  
double nv2dt(nv_t const& nv_in, 
	ts_t const& ts_in, double const& bpm) {
	au_assert(bpm>0);

	auto bps = bpm/60;
	auto delta_t = (nbeat(ts_in,nv_in).to_double())/bps;
	return delta_t;
}

std::string deltat2rp_demo() {
	std::vector<nv_t> nts {nv_t{1.0/1.0},nv_t{1.0/2.0},
		nv_t{1.0/4.0},nv_t{1.0/8.0}};
	auto ts = "4/4"_ts;
	double bpm = 60; auto bps = bpm/60;
	int n = 15;

	auto ridx_nts = urandi(n,0,nts.size()-1);
	auto rand_frac_delta_t = urandd(n,-0.075,0.075);

	std::vector<nv_t> note_seq {}; // Random seq of nv_t's
	std::vector<double> delta_t {}; // dt for note_seq +/- some random offset
	double t_total {0.0};
	for (auto i=0; i<n; ++i) {
		auto curr_nt = nts[ridx_nts[i]];
		note_seq.push_back(curr_nt);
		
		auto dt_exact = nbeat(ts,curr_nt).to_double()/bps;
		auto dt_fuzz = dt_exact + dt_exact*(rand_frac_delta_t[i]);
		t_total += dt_fuzz;
		delta_t.push_back(dt_fuzz);
	}

	auto nv_resolution = nv_t{1.0/8.0};
	double sec_resolution = nbeat(ts,nv_resolution).to_double()/(bps+1);
	auto rp_backcalc = deltat2rp(delta_t,ts,bpm,sec_resolution);

	std::string s {};
	// TODO:  FIXME
	//s += "Input seq: \n" + printrp(ts,note_seq) + "\n\n" + 
	//	"deltat2rp(): \n" + printrp(ts,rp_backcalc) + "\n\n";

	return s;
}

