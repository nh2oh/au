#include "rp_t.h"
#include "note_value_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "..\util\au_util_all.h"
#include <string>
#include <vector>
#include <algorithm> // for is_sorted()






//-----------------------------------------------------------------------------
// Support functions

beat_t nbeat(ts_t const& ts_in, note_value const& nv_in) {
	return beat_t{nv_in/(ts_in.beat_unit())};
}

bar_t nbar(ts_t const& ts_in, note_value const& nv_in) {
	beat_t nbeats = nbeat(ts_in, nv_in);
	auto nbars_exact = nbeats/(ts_in.beats_per_bar());
	return bar_t {nbars_exact};
}

std::string printrp(ts_t const& ts_in, std::vector<note_value> const& nv_in) {
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
std::vector<note_value> deltat2rp(std::vector<double> const& delta_t, 
	ts_t const& ts_in, double const& bpm, double const& s_resolution) {
	au_assert(delta_t.size()>=2,"A delta-t vector must contain >= 2 events.");
	au_assert(bpm>0,"bpm>0");
	au_assert(s_resolution>0,"s_resolution>0");
	auto bps = bpm/60.0;

	std::vector<note_value> nts {}; nts.reserve(delta_t.size());
	for (auto curr_dt: delta_t) {
		// NOTE:  best_nv is 1 smaller than sec_onset, so sec_onset[i+1] does
		// not overshoot.  
		auto curr_dt_round = roundquant(curr_dt,s_resolution);
		if (isapproxeq(curr_dt_round,0,6)) {
			// Intervals less than s_resolution/2 are rounded to 0 by roundquant();
			// round them up to s_resolution, since note_values == 0 are impossible.
			curr_dt_round = s_resolution;
		}
		nts.push_back(note_value{ts_in,beat_t{curr_dt_round*bps}});
		wait();
	}
	return nts;
}


//
// Does the reverse of deltat2rp().
// Units of the delta_t vector is seconds.  
//  
std::vector<double> rp2deltat(std::vector<note_value> const& rp_in, 
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


std::string deltat2rp_demo() {
	std::vector<note_value> nts {note_value{1.0/1.0},note_value{1.0/2.0},
		note_value{1.0/4.0},note_value{1.0/8.0}};
	auto ts = "4/4"_ts;
	double bpm = 60; auto bps = bpm/60;
	int n = 15;

	auto ridx_nts = urandi(n,0,nts.size()-1);
	auto rand_frac_delta_t = urandd(n,-0.075,0.075);

	std::vector<note_value> note_seq {}; // Random seq of note_value's
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

	auto nv_resolution = note_value{1.0/8.0};
	double sec_resolution = nbeat(ts,nv_resolution).to_double()/(bps+1);
	auto rp_backcalc = deltat2rp(delta_t,ts,bpm,sec_resolution);

	std::string s {};
	s += "Input seq: \n" + printrp(ts,note_seq) + "\n\n" + 
		"deltat2rp(): \n" + printrp(ts,rp_backcalc) + "\n\n";

	return s;
}



//
//The off-time of note i is the on-time of note i+1.  
// Intervals between successive notes are rounded to the nearest multiple
// of s_resolution.  For an n-element vector of onset times, the note_value
// vector returned will be n-1 -element.  The input time-onset vector must
// be sorted and may not contain any simultanious events.  
// 
// This algorithm takes the difference of each pair of timepoints in 
// the input then rounds each difference to the closest value of
// n*s_resolution, for integer n.  This is different from a method of first
// rounding the values in sec_onset to the nearest n*s_resolution before 
// taking the differences.  Consider:
// s_resolution = 0.1
// sec_onset =    {...,0.10,0.24,0.36,...}
// Round-first:   {...,0.10,0.20,0.40,...} => {...,0.10,0.20,...}     
// Diff-first:    {...,..., 0.14,0.12,...} => {...,0.10,0.10,...}
//
//tonset2rp()
//for (auto i=0; i<best_nv.size(); ++i) {
//	// NOTE:  best_nv is 1 smaller than sec_onset, so sec_onset[i+1] does
//	// not overshoot.  
//	auto delta_t = roundquant((sec[i+1]-sec[i]),s_resolution);
//	if (isapproxeq(delta_t,0.0,1)) {
//		au_error("No simultaneous events in a t_onset vector.");
//	}
//	best_nv[i] = note_value{ts_in,beat_t{delta_t*bps}};
//}



