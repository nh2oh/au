#include "ts_t.h"
#include "beat_bar_t.h"
#include "nv_t.h"
#include <string>
#include <numeric>  // std::accumulate()
#include <vector>  // cum_nbar()
#include <regex>
#include "..\util\au_algs_math.h"  // ksum<T> in err_nbeat_accum()


ts_t::ts_t(const beat_t& num, const d_t& denom, bool is_compound_in) {
	if (num <= 0_bt || denom <= d_t{d::z}) {
		std::abort();
	}

	m_compound = is_compound_in;
	if (!is_compound_in) {  // "Simple" time signature
		m_bpb = num;
		m_beat_unit = denom;
	} else {  // Compound time signature
		// The numerator is the "number of into-3 subdivisions of the beat"-per
		// bar.  Thus, dividing by 3 yields the number of beats-per bar.  
		m_bpb = num/3.0;

		// The denominator is the "note-value per (3-part) beat subdivision."
		// That is, 3 notes of value denom == 1 beat.  
		// A group of three identical note-values x is equivalent a single nv
		// having twice the duration of x, 2x, dotted once: (2x).
		d_t beat_unit = 2*denom;
		if (!beat_unit.add_dots(1)) {
			std::abort();
		}
		m_beat_unit = beat_unit;
	}
}

ts_t::ts_t(const std::string& str_in) {
	from_string(str_in);
}

//
// Called by the constructor ts_t(const std::string&)
// 
void ts_t::from_string(const std::string& str_in) {
	std::regex rx {"(\\d+)/(\\d+)(c)?"};
	std::smatch rx_matches {};  // std::match_results<std::string::const_interator>
	if (!std::regex_match(str_in,rx_matches,rx)) {
		std::abort();
	}
	double bt_per_bar {std::stod(rx_matches[1].str())};
	double inv_dv_per_bt = std::stod(rx_matches[2].str());
	if (bt_per_bar <= 0.0 || inv_dv_per_bt <= 0.0) {
		// 0 is forbidden for num or denom (values < 0 will not match the regex).  
		std::abort();  
	}
	double dv_per_bt = 1.0/inv_dv_per_bt;
	bool is_compound {rx_matches[3].matched};

	m_beat_unit = d_t {dv_per_bt};
	m_bpb = beat_t {bt_per_bar};
	m_compound = is_compound;
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
	std::string s = m_bpb.print() + "/(" + m_beat_unit.print() + ")";
	if (m_compound) { s += "c"; }
	return s;
}

bool ts_t::operator==(const ts_t& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}
bool operator!=(const ts_t& lhs, const ts_t& rhs) {
	return !(lhs == rhs);
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


// Looks at the accumulation of "error" for repeated summing of a single nv_t
// in calculating a number-of-bars.  This type of thing sometimes has to be
// be done when working with rp's, ie, sequences of nv_t's.  
// Calculte the error every record_every iterations.  Each record_every
// iterations should add exactly nbars_every bars to the total.  
err_accum err_nbeat_accum(ts_t ts, d_t nv, int niter, int record_every, bar_t nbars_every) {
	std::vector<double> err_nbar {};
	//std::vector<double> err_nbeat {};

	ksum<bar_t> cum_nbar_ks {};
	bar_t cum_nbar {0};
	//beat_t cum_nbeat {0};
	for (int i=1; i<niter; ++i) {
		cum_nbar += nbar(ts,nv);
		cum_nbar_ks += nbar(ts,nv);
		//cum_nbeat += nbeat(ts,nv);

		if (i%record_every == 0) {
			double cum_nbar_exact = ((i/record_every)*nbars_every)/1_br;
			//double cum_nbeat_exact = (i/record_every)*nbeat(ts,nbars_every)/1_bt;
			err_nbar.push_back(cum_nbar_exact-(cum_nbar_ks.value)/1_br);
			//err_nbeat.push_back(cum_nbeat_exact-cum_nbeat/1_bt);
		}
	}
	//return {err_nbar,err_nbeat};
	return {err_nbar};
}



