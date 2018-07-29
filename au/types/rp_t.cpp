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
// Convert a sequence of note on-times (in seconds) to a sequence of 
// note-values.  The off-time of note i is the on-time of note i+1.  
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
//
std::vector<note_value> tonset2rp(std::vector<double> const& sec_onset, 
	ts_t const& ts_in, double const& bpm, double const& s_resolution) {
	au_assert(sec_onset.size()>=2 && std::is_sorted(sec_onset.begin(),sec_onset.end()),
		"A t_onset vector must be sorted and contain >= 2 events.  ");
	au_assert(bpm>0,"bpm>0");
	au_assert(s_resolution>0,"s_resolution>0");
	auto bps = bpm/60.0;

	std::vector<note_value> best_nv(sec_onset.size()-1,note_value{});
	for (auto i=0; i<best_nv.size(); ++i) {
		// NOTE:  best_nv is 1 smaller than sec_onset, so sec_onset[i+1] does
		// not overshoot.  
		auto delta_t = roundquant((sec_onset[i+1]-sec_onset[i]),s_resolution);
		if (isapproxeq(delta_t,0.0,1)) {
			au_error("No simultaneous events in a t_onset vector.");
		}
		best_nv[i] = note_value{ts_in,beat_t{delta_t*bps}};
	}
	return best_nv;
}

//
// Does the reverse of tonset2rp().
// For an n-element vector of note_values, the vector of onset times will 
// contain n+1 elements, with the first element, t_onset[0] == 0.0.  Units
// of the t_onset vector is seconds.  
//  
std::vector<double> rp2tonset(std::vector<note_value> const& rp_in, 
	ts_t const& ts_in, double const& bpm) {
	au_assert(rp_in.size()>=1);
	au_assert(bpm>0);

	std::vector<double> t_onset(rp_in.size()+1, 0.0);
	auto bps = bpm/60;
	for (auto i=0; i<rp_in.size(); ++i) {
		t_onset[i+1] = t_onset[i] + (nbeat(ts_in,rp_in[i]).to_double())/bps;
	}
	return t_onset;
}

std::string tonset2rp_demo() {
	std::vector<note_value> nts {note_value{1.0/1.0},note_value{1.0/2.0},
		note_value{1.0/4.0},note_value{1.0/8.0}};
	auto nv_resolution = note_value{1.0/8.0};
	auto ts = "4/4"_ts;
	double bpm = 60; auto bps = bpm/60;
	double sec_resolution = nbeat(ts,nv_resolution).to_double()/bps;
	int n = 15;

	auto ridx_nts = urandi(n,0,nts.size()-1);
	auto rand_frac_delta_t = urandd(n,-0.075,0.075);

	std::vector<note_value> note_seq {};
	std::vector<double> sec_onset {0.0};  // +/- some random offset
	double t_total {0.0};
	for (auto i=0; i<n; ++i) {
		auto curr_nt = nts[ridx_nts[i]];
		note_seq.push_back(curr_nt);
		
		auto delta_s = nbeat(ts,curr_nt).to_double()/bps;
		t_total += delta_s + delta_s*rand_frac_delta_t[i];
		sec_onset.push_back(t_total);
	}

	auto best_nvs = tonset2rp(sec_onset,ts,bpm,sec_resolution);

	std::string s {};
	s += "Input seq: \n" + printrp(ts,note_seq) + "\n\n" + 
		"tonset2rp(): \n" + printrp(ts,best_nvs) + "\n\n";

	return s;
}
