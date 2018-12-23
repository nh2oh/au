#include "ts_t.h"
#include "beat_bar_t.h"
#include "nv_t.h"
#include <string>
#include <numeric>  // std::accumulate()
#include <vector>
#include <regex>
#include <iostream>  // operator<<


ts_t::ts_t(const beat_t& num, const d_t& denom, bool is_compound) {
	from_parts(num,denom,is_compound);
}

ts_t::ts_t(const std::string& str_in) {
	ts_str_parsed ts_parsed = parse_ts_string(str_in);
	if (!ts_parsed.is_valid) {
		std::abort();
	}

	from_parts(beat_t{ts_parsed.num}, d_t{ts_parsed.denom}, ts_parsed.is_compound);
}

//
// Called by the constructor ts_t(beat_t, d_t, bool) && from_string()
// 
// If compound:
// -> The numerator is the "number of into-3 subdivisions of the beat"-per bar.  Thus, 
//    dividing the input beat_t 'num' by 3 yields the number of beats-per bar.  
// -> The denominator is the "note-value per (3-part) beat subdivision."  That is, 3 notes 
//    of value denom == 1 beat.  A group of three identical note-values d is equivalent to a
//    single nv of duration of 2*d, dotted once:  (2d).
//
void ts_t::from_parts(beat_t num, d_t denom, bool is_compound) {
	if (num <= 0_bt || denom <= d_t{d::z}) { std::abort(); }

	m_compound = is_compound;
	if (!is_compound) {  // "Simple" time signature
		m_bpb = num;
		m_beat_unit = denom;
	} else {  // Compound time signature
		m_bpb = num/3.0;
		d_t beat_unit = 2*denom;
		if (!beat_unit.add_dots(1)) {
			std::abort();
		}
		m_beat_unit = beat_unit;
	}
}


ts_t operator""_ts(const char *literal_in, size_t length) {
	return ts_t {std::string {literal_in}};
}

d_t ts_t::beat_unit() const {
	return m_beat_unit;
}
d_t ts_t::bar_unit() const {
	return (m_bpb/(1_bt))*(m_beat_unit);
}
beat_t ts_t::beats_per_bar() const {
	return m_bpb;
}

std::string ts_t::print() const {
	std::string s {};
	d_t::opts dt_propts {};
	dt_propts.denom_only = true;

	if (!m_compound) {
		s = m_bpb.print() + "/" + m_beat_unit.print(dt_propts);
	} else {
		beat_t bpb {m_bpb*3};
		d_t beat_unit = m_beat_unit; 
		beat_unit.rm_dots(1);  beat_unit/=2.0;

		s = bpb.print() + "/" + beat_unit.print(dt_propts) + "c";
	}

	return s;
}
std::ostream& operator<<(std::ostream& os, const ts_t& ts) {
    os << ts.print();
    return os;
}

bool ts_t::operator==(const ts_t& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}
bool operator!=(const ts_t& lhs, const ts_t& rhs) {
	return !(lhs == rhs);
}

// 
// Delegated to by ts_t(std::string)
//
ts_str_parsed parse_ts_string(const std::string& str_in) {
	ts_str_parsed res {};
	std::regex rx {"(\\d+)/(\\d+)(c)?"};
	std::smatch rx_matches {};  // std::smatch == std::match_results<std::string::const_interator>
	if (!std::regex_match(str_in,rx_matches,rx)) {
		res.is_valid = false;
		return res;
	}
	double bt_per_bar {std::stod(rx_matches[1].str())};
	double inv_dv_per_bt = std::stod(rx_matches[2].str());
	if (bt_per_bar <= 0.0 || inv_dv_per_bt <= 0.0) {
		// 0 is forbidden for num or denom (values < 0 will not match the regex).  
		res.is_valid = false;
		return res;
	}

	res.is_valid = true;
	res.num = bt_per_bar;
	res.denom = 1.0/inv_dv_per_bt;
	res.is_compound = rx_matches[3].matched;

	return res;
}


//-----------------------------------------------------------------------------
// beat_t - bar_t - d_t conversion functions

beat_t nbeat(const ts_t& ts_in, const d_t& d_in) {
	return beat_t{d_in/(ts_in.beat_unit())};
}
beat_t nbeat(const ts_t& ts_in, const bar_t& nbars_in) {
	return (ts_in.beats_per_bar())*(nbars_in.to_double());
}
// Return the _total_ number of beats spanned by the rp
beat_t nbeat(const ts_t& ts_in, const std::vector<d_t>& vdt_in) {
	d_t tot_duration {};
	tot_duration = std::accumulate(vdt_in.begin(),vdt_in.end(),d_t{0.0});
	return nbeat(ts_in,tot_duration);
}

d_t duration(const ts_t& ts_in, beat_t nbeats_in) {
	return d_t {ts_in.beat_unit()*(nbeats_in/(1_bt))};
}

bar_t nbar(const ts_t& ts_in, const d_t& d_in) {
	return bar_t {d_in/ts_in.bar_unit()};
}
bar_t nbar(const ts_t& ts_in, const beat_t& nbts_in) {
	return bar_t {nbts_in/ts_in.beats_per_bar()};
}
// Return the _total_ number of bars spanned by the rp
bar_t nbar(const ts_t& ts_in, const std::vector<d_t>& vdt_in) {
	d_t tot_duration {};
	tot_duration = std::accumulate(vdt_in.begin(),vdt_in.end(),d_t{0.0});
	return nbar(ts_in,tot_duration);
}

std::vector<bar_t> cum_nbar(const ts_t& ts_in, const std::vector<d_t>& vdt_in) {
	std::vector<bar_t> nbar_cum {};  nbar_cum.reserve(vdt_in.size());
	d_t curr_tot_duration {0};
	// The assumption here is that d_t's sum more accurately than bar_t's.  Otherwise,
	// I could: curr += nbar(ts,d_t); push_back(curr);
	for (const auto& e : vdt_in) {
		curr_tot_duration += e;
		nbar_cum.push_back(nbar(ts_in,curr_tot_duration));
	}
	return nbar_cum;
}

//
// Demonstrates the accumulation of "error" for repeated summing of a single d_t nv in calculating the
// number-of-bars spanned by niter nv's.  This type of sum sometimes has to be be carried out when  
// working with rp's, ie, sequences of d_t's.  
// The error is calculated every record_every iterations.  nbars_every is the number of bars added 
// to the total every record_every interations (the number of bars spanned by record_every nv's).  
// nv and record_every should be chosen so that nbars_every is exact.  
//
err_accum err_nbeat_accum(ts_t ts, d_t nv, int niter, int record_every, bar_t nbars_every) {
	std::vector<double> err_nbar {};
	//std::vector<double> err_nbeat {};

	// ksum<bar_t> cum_nbar_ks {};
	bar_t cum_nbar {0};
	//beat_t cum_nbeat {0};
	for (int i=1; i<niter; ++i) {
		cum_nbar += nbar(ts,nv);
		// cum_nbar_ks += nbar(ts,nv);
		// cum_nbeat += nbeat(ts,nv);

		if (i%record_every == 0) {
			double cum_nbar_exact = ((i/record_every)*nbars_every)/1_br;
			//double cum_nbeat_exact = (i/record_every)*nbeat(ts,nbars_every)/1_bt;
			
			err_nbar.push_back(cum_nbar_exact-cum_nbar/1_br);
			//err_nbeat.push_back(cum_nbeat_exact-cum_nbeat/1_bt);
		}
	}
	//return {err_nbar,err_nbeat};
	return {err_nbar};
}



