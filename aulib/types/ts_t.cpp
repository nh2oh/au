#include "ts_t.h"
#include "beat_bar_t.h"
#include "nv_t.h"
#include "..\util\au_error.h"
#include "..\util\au_util.h"
#include <string>
#include <algorithm>
#include <vector>  // cum_nbar()
#include "..\util\au_algs_math.h"


ts_t::ts_t(beat_t const& num, d_t const& denom, bool const& is_compound_in) {
	au_assert(num > 0_bt);

	m_compound = is_compound_in;
	if (!is_compound_in) { // "Simple" time signature
		m_bpb = num;
		m_beat_unit = denom;
	} else { // Compound time signature
		// The numerator is the "number of into-3 subdivisions of the beat"-per
		// bar.  Thus, dividing by 3 yields the number of beats-per bar.  
		m_bpb = num/3.0;

		// The denominator is the "note-value per (3-part) beat subdivision."
		// That is, 3 notes of value denom == 1 beat.  
		// A group of three identical note-values x is equivalent a single nv
		// having twice the duration of x, 2x, dotted once: (2x).

		auto beat_unit = 2*denom; au_assert(beat_unit.add_dots(1));
		m_beat_unit = beat_unit;
	}
}

ts_t::ts_t(std::string const& str_in) {
	from_string(str_in);
}

// TODO:  this is essentially replicated in the ts_t uih
// Called by the constructor ts_t(std::string const&)
// Defined as its own function because ... ???
void ts_t::from_string(std::string const& str_in) {  
	auto o_matches = rx_match_captures("(\\d+)/(\\d+)(c)?",str_in);
	if (!o_matches || (*o_matches).size() != 4) {
		au_error("Could not parse ts string literal");
	}
	auto matches = *o_matches;

	double bt_per_bar {std::stod(*(matches[1]))};
	double dv_per_bt = 1.0/std::stod(*(matches[2]));
	au_assert((bt_per_bar > 0 && dv_per_bt > 0), "No (-) values in a ts");
	
	bool is_compound {false};
	if (matches[3]) { is_compound = true; }

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
	std::string s = m_bpb.print() + "/" + m_beat_unit.print();
	if (m_compound) { s += "c"; }
	return s;
}

bool ts_t::operator==(ts_t const& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}
bool operator!=(ts_t const& lhs, ts_t const& rhs) {
	return !(lhs == rhs);
}


//-----------------------------------------------------------------------------
// beat_t - bar_t - d_t conversion functions

beat_t nbeat(ts_t const& ts_in, d_t const& d_in) {
	return beat_t{d_in/(ts_in.beat_unit())};
}
beat_t nbeat(ts_t const& ts_in, bar_t const& nbars_in) {
	return (ts_in.beats_per_bar())*(nbars_in.to_double());
}

d_t duration(const ts_t& ts_in, beat_t nbeats_in) {
	return d_t {ts_in.beat_unit()*(nbeats_in/(1_bt))};
}

bar_t nbar(ts_t const& ts_in, d_t const& d_in) {
	//return nbar(ts_in, nbeat(ts_in,d_in));
	return bar_t {d_in/ts_in.bar_unit()};
}
bar_t nbar(ts_t const& ts_in, beat_t const& nbts_in) {
	return bar_t {nbts_in/ts_in.beats_per_bar()};
}

std::vector<bar_t> cum_nbar(ts_t const& ts_in, std::vector<d_t> const& d_in) {
	std::vector<bar_t> rp_cum {}; rp_cum.reserve(d_in.size());
	bar_t totnb {0.0};  // Only used by the lambda
	std::for_each(d_in.begin(),d_in.end(),
		[&](d_t const& currnv){rp_cum.push_back(totnb+=nbar(ts_in,currnv));});

	return rp_cum;
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