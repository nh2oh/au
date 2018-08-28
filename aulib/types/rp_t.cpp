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
#include <algorithm>
#include <numeric>




rp_t::rp_t() {}

rp_t::rp_t(ts_t const& ts_in) {
	m_ts = ts_in;
}

rp_t::rp_t(ts_t const& ts_in, std::vector<nv_t> const& nvs_in) {
	m_ts = ts_in;
	for (auto const& e : nvs_in) {
		this->push_back(e);
	}
}

bar_t rp_t::nbars() const {
	return m_tot_nbars;
}
beat_t rp_t::nbeats() const {
	return m_tot_nbeats;
}

rp_t rp_t::subrp(bar_t const& from, bar_t const& to) const {
	auto from_startbar = m_rp.begin() + m_bidx[std::floor(from.to_double())].start;	
	auto to_endbar = m_rp.begin() + m_bidx[std::floor(to.to_double())].end;

	int start_offset = 0;
	if (!isapproxint(from.to_double(),6)) {
		std::vector<bar_t> rp_cum {};
		auto from_endbar = m_rp.begin() + m_bidx[std::floor(from.to_double())].end;
		bar_t totnb {0.0};
		std::for_each(from_startbar,from_endbar,
			[&](nv_t const& currnv) {rp_cum.push_back(totnb+=nbar(m_ts,currnv));});
		start_offset = std::count_if(rp_cum.begin(),rp_cum.end(),
			[&](bar_t const& currbar){return (currbar<from);});
	}
	int end_offset = 0;
	if (!isapproxint(to.to_double(),6)) {
		std::vector<bar_t> rp_cum {};
		bar_t totnb {0.0};
		auto to_startbar = m_rp.begin() + m_bidx[std::floor(to.to_double())].start;
		std::for_each(to_startbar,to_endbar,
			[&](nv_t const& currnv) {rp_cum.push_back(totnb+=nbar(m_ts,currnv));});
		end_offset = std::count_if(rp_cum.begin(),rp_cum.end(),
			[&](bar_t const& currbar){return (currbar<=to);});
	}

	std::vector<nv_t> result {};
	std::copy(from_startbar+start_offset,to_endbar-end_offset,std::back_inserter(result));

	return rp_t{m_ts,result};
}

void rp_t::push_back(nv_t const& nv_in) {
	beat_t nbeats_in = nbeat(m_ts,nv_in);
	bar_t nbars_in = nbar(m_ts,nbeats_in);
	m_tot_nbeats += nbeats_in;

	m_rp.push_back(nv_in);

	double nnewbars = std::floor((m_tot_nbars+nbars_in).to_double()) - m_tot_nbars.to_double();
		// The number of bar boundaries crossed upon addition of nv_in,
		// not the number of bars spanned by nv_in.  
	m_tot_nbars += nbars_in;
	bool isexact = isapproxint((m_tot_nbars).to_double(),6);
	// TODO:  Hardcoded precision

	// Note that for each entry in m_bidx, .start is _inclusive_, and
	// .end is _exclusive_.  That is, the bar corresponding to m_bidx[i]
	// always includes the element indicated by m_bidx[i].start
	// and excludes element m_bidx[i].end (m_bidx[i].end is the first 
	// element belonging to the next bar).  
	//
	// Also note that m_bidx.size() is always 1+ceil(total-number-of-bars-in-rp),
	// and m_bidx.back() always refers to an incomplete bar and has:
	// m_bidx.back().end == 0
	// m_bidx.end_Exact == false
	//
	
	for (int i=0; i<std::floor(nnewbars); ++i) {
		wait();
		bool end_is_exact = isexact && (i == (std::floor(nnewbars)-1));
		// Finish the current entry in m_bidx:
		m_bidx.back().end = m_rp.size();
		m_bidx.back().end_exact = end_is_exact;

		// Append the next working entry:
		if ((i+1) < nnewbars) {  // Not the last iter

			m_bidx.back().end = m_rp.size();
			m_bidx.back().end_exact = end_is_exact;

			// If this is _not_ the last bar we're appending, it should begin on
			// the final element in the sequence (nv_in, the element just appended).
			// Said element evidently spans more than one bar and m_bidx[i].start 
			// is inclusive of the first element on bar i.  
			m_bidx.push_back({m_rp.size()-1,end_is_exact,0,false});
		} else {  // Last iter

			m_bidx.back().end = m_rp.size();
			m_bidx.back().end_exact = isexact;

			// If this _is_ the last bar we're appending, it should begin on
			// one _past_ the final element in the sequence (nv_in).
			m_bidx.push_back({m_rp.size(),isexact,0,false});
		}
		wait();
	}
}

std::string rp_t::print() const {
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
		// a bar serepator char.  
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

std::string rp_t::printbidx() const {
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

bar_t nbar(ts_t const& ts_in, nv_t const& nv_in) {
	beat_t nbeats = nbeat(ts_in, nv_in);
	//auto nbars_exact = nbeats/(ts_in.beats_per_bar());
	//return bar_t {nbars_exact};
	return nbar(ts_in,nbeats);
}
bar_t nbar(ts_t const& ts_in, beat_t const& nbts_in) {
	auto nbars_exact = nbts_in/(ts_in.beats_per_bar());
	return bar_t {nbars_exact};
}

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
}

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
	s += "Input seq: \n" + printrp(ts,note_seq) + "\n\n" + 
		"deltat2rp(): \n" + printrp(ts,rp_backcalc) + "\n\n";

	return s;
}

