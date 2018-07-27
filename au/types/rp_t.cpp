#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <optional>
#include <algorithm>
#include "rp_t.h"
#include "..\util\au_util_all.h"



//-----------------------------------------------------------------------------
// Class beat_t

beat_t::beat_t(double beats_in) {
	m_beats = beats_in;
}

double beat_t::to_double() const {
	return m_beats;
}

beat_t& beat_t::operator+=(beat_t const& rhs) {
	m_beats += rhs.m_beats;
	return *this;
}
beat_t& beat_t::operator-=(beat_t const& rhs) {
	m_beats -= rhs.m_beats;
	return *this;
}

beat_t operator+(beat_t const& lhs, beat_t const& rhs) {
	beat_t result {lhs};
	return (result+=rhs);
}
beat_t operator-(beat_t const& lhs, beat_t const& rhs) {
	beat_t result {lhs};
	return (result-=rhs);
}
beat_t operator-(beat_t const& rhs) {
	return beat_t{-1.0*(rhs.to_double())};
}

double operator/(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double())/(rhs.to_double());
}
double operator/(double const& lhs, beat_t const& rhs) {
	return (lhs/rhs.to_double());
}
beat_t operator*(double const& lhs, beat_t const& rhs) {
	return beat_t{lhs*rhs.to_double()};
}
beat_t operator/(beat_t const& lhs, double const& rhs) {
	return beat_t{lhs.to_double()/rhs};
}
beat_t operator*(beat_t const& lhs, double const& rhs) {
	return beat_t{rhs*lhs.to_double()};
}

bool operator==(beat_t const& lhs, beat_t const& rhs) {
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(beat_t const& lhs, beat_t const& rhs) {
	return !(lhs==rhs);
}
bool operator>(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(beat_t const& lhs, beat_t const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}

//-----------------------------------------------------------------------------
// Class bar_t
// Represents some number of bars
bar_t::bar_t(double bars_in) {
	m_bars = bars_in;
}

double bar_t::to_double() const {
	return m_bars;
}

bar_t& bar_t::operator+=(bar_t const& rhs) {
	m_bars += rhs.m_bars;
	return *this;
}
bar_t& bar_t::operator-=(bar_t const& rhs) {
	m_bars -= rhs.m_bars;
	return *this;
}
bar_t operator+(bar_t const& lhs, bar_t const& rhs) {
	bar_t result {lhs};
	return (result+=rhs);
}
bar_t operator-(bar_t const& lhs, bar_t const& rhs) {
	bar_t result {lhs};
	return (result-=rhs);
}
double operator/(bar_t const& lhs, bar_t const& rhs) {
	return ((lhs.to_double())/(rhs.to_double()));
}
bar_t operator/(bar_t const& lhs, double const& rhs) {
	return bar_t{lhs.to_double()/rhs};
}
double operator/(double const& lhs, bar_t const& rhs) {
	return (lhs/rhs.to_double());
}
bar_t operator*(bar_t const& lhs, double const& rhs) {
	return(bar_t{lhs.to_double()*rhs});
}
bar_t operator*(double const& lhs, bar_t const& rhs) {
	return(bar_t{rhs.to_double()*lhs});
}

bool operator==(bar_t const& lhs, bar_t const& rhs) {
	//return (std::abs((lhs-rhs).to_double()) <= std::pow(10,-6));
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(bar_t const& lhs, bar_t const& rhs) {
	return !(lhs==rhs);
}
bool operator>(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(bar_t const& lhs, bar_t const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}

//-----------------------------------------------------------------------------
// Class note_value
// Represents a relative timespan

note_value::note_value(double nv_in) {
	m_nv = nv_in;
}

note_value::note_value(double base_value_in, int num_dots_in) {
	// nv = rv*(2-1/(2^n))
	m_nv = base_value_in*(2.0 - 1.0/std::pow(2,num_dots_in));
}

note_value::note_value(ts_t ts_in, beat_t nbeats) {
	m_nv = (ts_in.beat_unit().to_double())*(nbeats.to_double());
}

std::optional<nv_base_dots> note_value::exact() const {
	// nv = bv*(2-1/(2^n))
	double bv {0}; int num_dots {0};  // relative-value, number-of-dots
	for (true; num_dots<=5; ++num_dots) {
		bv = m_nv/(2-1/std::pow(2,num_dots));
		if (isapproxint(1.0/bv,6) || isapproxint(bv,6)) {
			return nv_base_dots {bv,num_dots};
		}
	}
	return {};
}

std::optional<nv_base_dots> note_value::exact2() const {
	// nv = bv*(2-1/(2^n)), for integer n >= 0
	// For the standard notes, bv = 1/(2^m), for integer m
	// nv = 1/2^(m-1) - 1/2^(n+m)

	int m_min = -3; int m_max = 8; //(-2=>quadruple-whole; 8=>256'th)
	int m {0};
	for (m = m_min; m<(m_max+1); ++m) {
		if (m_nv >= std::pow(2,-m)) { break; }
	}
	if (m>m_max) { return {}; }

	double bv {std::pow(2,-m)};
	for (int num_dots = 0; num_dots<=5; ++num_dots) {
		double test_m_nv = bv*(2-std::pow(2,-num_dots));
		if (isapproxeq(m_nv,test_m_nv,6)) {
			return nv_base_dots {bv,num_dots};
		} else if (test_m_nv > m_nv) {
			break;
		}
	}
	double nv_remainder = m_nv - bv;

	for (m = m_min; m<(m_max+1); ++m) {
		if (nv_remainder >= std::pow(2,-m)) { break; }
	}
	if (m>m_max) { return {}; }

	double bv_remainder {std::pow(2,-m)};
	for (int num_dots = 0; num_dots<=5; ++num_dots) {
		double test_m_nv = bv_remainder*(2-std::pow(2,-num_dots));
		if (isapproxeq(nv_remainder,test_m_nv,6)) {
			// {bv + {bv_remainder,num_dots}, 0}
			return nv_base_dots {bv,num_dots};
		}
	}

	return {};
}

std::optional<int> note_value::ndot() const {
	auto o_base_dots = exact();
	if (o_base_dots) {
		return (*o_base_dots).ndot;
	}
	return {};
}

std::optional<double> note_value::undot_value() const {
	auto o_base_dots = exact();
	if (o_base_dots) {
		return (*o_base_dots).base_value;
	}
	return {};
}

std::string note_value::print() const {
	// Large, duplex longa, maxima
	// long, longa
	// double-whole, breve
	// whole, semibreve
	// half, minim
	// ...
	auto o_base_dots = exact();
	std::string s {};
	if (o_base_dots) {
		auto num_dots = (*o_base_dots).ndot;
		auto base_exact = (*o_base_dots).base_value;
		s = "(";
		if (base_exact < 1) {
			s += "1/" + std::to_string(static_cast<int>(1.0/base_exact));
		} else {
			s += std::to_string(static_cast<int>(base_exact));
		}
		s += ")" + std::string(".",num_dots);
	} else {
		s = "(?)";
		//frac rat_approx = rapprox((1.0/m_nv),256);
		//s = "(" + std::to_string(rat_approx.denom) + "/" + std::to_string(rat_approx.num) + ")";
	}

	return s;
}
double note_value::to_double() const {
	return m_nv;
}

note_value& note_value::operator+=(note_value const& rhs) {
	m_nv += rhs.m_nv;
	return *this;
}
note_value& note_value::operator-=(note_value const& rhs) {
	m_nv -= rhs.m_nv;
	return *this;
}

note_value operator+(note_value const& lhs, note_value const& rhs) {
	note_value result {lhs};
	return (result += rhs);
}
note_value operator-(note_value const& lhs, note_value const& rhs) {
	note_value result {lhs};
	return (result -= rhs);
}
double operator/(note_value const& lhs, note_value const& rhs){ 
	// how many rhs-notes fit in the span of an lhs-note?
	return (lhs.to_double())/(rhs.to_double());
}
note_value operator*(note_value const& lhs, double const& rhs) {
	return(note_value{lhs.to_double()*rhs});
}
note_value operator*(double const& lhs, note_value const& rhs) {
	return(note_value{rhs.to_double()*lhs});
}

bool operator==(note_value const& lhs, note_value const& rhs) {
	//return (std::abs((lhs-rhs).to_double()) <= std::pow(10,-6));
	return (isapproxeq(lhs.to_double(),rhs.to_double(),6));
}
bool operator!=(note_value const& lhs, note_value const& rhs) {
	return !(lhs==rhs);
}
bool operator>(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() > rhs.to_double());
}
bool operator<(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() < rhs.to_double());
}
bool operator>=(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() >= rhs.to_double());
}
bool operator<=(note_value const& lhs, note_value const& rhs) {
	return (lhs.to_double() <= rhs.to_double());
}


//-----------------------------------------------------------------------------
// The ts_t class

ts_t::ts_t(beat_t bt_per_br_in, note_value nv_per_bt, bool is_compound_in) {
	m_compound = is_compound_in;
	if (!is_compound_in) {
		// Simple meter
		m_bpb = bt_per_br_in;
		m_beat_unit = nv_per_bt;
	} else {
		// Compound meter
		// bt_per_br_in is really the "number of 3-part divisions of the beat" per bar
		m_bpb = beat_t {bt_per_br_in.to_double()/3.0};
		// nv_per_bt is really the "note-value per 3-part beat subdivision"
		m_beat_unit = note_value{(nv_per_bt+nv_per_bt).to_double(),1};
	}
}

ts_t operator""_ts(const char *literal_in, size_t length) {
	ts_t default_ts {beat_t {4.0}, note_value {1.0/4.0}, false};

	auto o_matches = rx_match_captures("(\\d+)/(\\d+)(c)?",std::string{literal_in});
	if (!o_matches) {
		au_error("Could not parse ts string literal");
		return default_ts;
	}

	auto matches = *o_matches;
	if (matches.size() != 3 && matches.size() != 4) {
		au_error("Could not parse ts string literal");
		return default_ts;
	}

	double bt_per_bar {std::stod(*(matches[1]))};
	double inv_nv_per_bt {std::stod(*(matches[2]))};
	if (bt_per_bar <= 0 || inv_nv_per_bt <= 0) {
		au_error("Could not parse ts string literal");
		return default_ts;
	}
	
	bool is_compound {false};
	if (matches.size() == 4 && *(matches[3]) == "c") {
		is_compound = true;
	}

	return ts_t {beat_t {bt_per_bar}, note_value {1.0/inv_nv_per_bt}, is_compound};
}

note_value ts_t::beat_unit() const {
	return m_beat_unit;
}
note_value ts_t::bar_unit() const {
	return note_value{(m_beat_unit.to_double())*(m_bpb.to_double())};
}
beat_t ts_t::beats_per_bar() const {
	return m_bpb;
}
std::string ts_t::print() const {
	int numerator {0};
	int denominator {0};
	std::string compound_indicator {""};
	if (!m_compound) {
		numerator = static_cast<int>(m_bpb.to_double());
		denominator = static_cast<int>(1/(m_beat_unit.to_double()));
	} else {
		auto o_denominator = m_beat_unit.undot_value();
		if (o_denominator) {
			numerator = 3*static_cast<int>(m_bpb.to_double());
			denominator = static_cast<int>(2.0/(*o_denominator));
			compound_indicator = "c";
		} else {
			return ("?/?");
		}
	}
	std::string s = std::to_string(numerator) + "/" 
		+ std::to_string(denominator) + 
		compound_indicator;
	return s;
}

bool ts_t::operator==(ts_t const& rhs) const {
	return ((m_bpb == rhs.m_bpb) && 
		(m_beat_unit == rhs.m_beat_unit) &&
		(m_compound == rhs.m_compound));
}


//-----------------------------------------------------------------------------
// The tmetg_t class

const int tmetg_t::m_bt_quantization {1024};

tmetg_t::tmetg_t(ts_t ts_in, std::vector<note_value> note_values_in, std::vector<beat_t> ph_in) {
	if (ph_in.size() != note_values_in.size()) {
		au_error("ph_in.size() != note_values_in.size()");
	}
	m_ts = ts_in;
	m_note_values = note_values_in;
	m_ph = ph_in;
	for (auto curr_nv : note_values_in) {
		m_beat_values.push_back(nbeat(ts_in,curr_nv));
	}

	//
	// Each note value measured in beats (m_beat_values) spans some number of 
	// some hitherto unknown minimum "beat resolution" beats btres.  
	// For all the m_beat_values[i]'s, as well as the full bar
	// (ts_in.beats_per_bar()), to be exactly representable on the grid, this
	// "resolution" number-of-beats must be chosen small enough.  
	// For all i, Ni/m_beat_values[i] = Nbar/bt_br (= 1/btres)
	// where Ni is the integer number of btres beats spanned by m_beat_values[i],
	// and Nbar is the number spanned by the full bar.  
	//     => Ni = (m_beat_values[i]/btres), Nbar = (bt_br/btres)
	//     => Ni/Nbar = m_beat_values[i]/bt_br
	//     => Ni = (m_beat_values[i]/bt_br)*Nbar
	//     
	// The smallest value of Nbar for which all (m_beat_values[i]/bt_br)
	// are integers yields the largest btres as:
	// btres = Nbar/ts_in.beats_per_bar()
	//
	frac bt_br = rapprox(ts_in.beats_per_bar().to_double(),m_bt_quantization).reduce();
	int Nbar = std::lcm(bt_br.denom,1);
	for (auto curr_bt : m_beat_values) {
		auto curr_bt_quant = rapprox(curr_bt.to_double(),m_bt_quantization);
		Nbar = std::lcm((curr_bt_quant/bt_br).reduce().denom,Nbar);
	}

	// ... The same argument applies to the phase offsets...
	for (auto curr_ph : m_ph) {
		auto curr_ph_quant = rapprox(curr_ph.to_double(),m_bt_quantization);
		Nbar = std::lcm((curr_ph_quant/bt_br).reduce().denom,Nbar);
	}
	m_btres = beat_t{(bt_br/Nbar).to_double()};
	m_period = *std::max_element(m_beat_values.begin(),m_beat_values.end());
}

std::vector<note_value> tmetg_t::which_allowed(beat_t beat, 
	std::vector<note_value> nvs, int mode) const {
	// mode == 1 => at && next (default)
	// mode == 2 => at only
	// mode == 3 => next only
	std::vector<note_value> allowed_nvs {};

	if (mode == 1 || mode == 2) {
		if (!allowed_at(beat)) {
			return allowed_nvs; // Empty vector
		}
	}

	if (mode == 1 || mode == 3) {
		for (auto const& cnv : nvs) {
			if (allowed_next(beat,cnv)) {
				allowed_nvs.push_back(cnv);
			}
		}
	}

	return allowed_nvs;
}

bool tmetg_t::allowed_next(beat_t beat, note_value nv) const {
	auto nxt_bt = beat+nbeat(m_ts,nv); // Beat-number of the next beat
	return allowed_at(nxt_bt);
}

bool tmetg_t::allowed_at(beat_t beat) const {
	for (auto i=0; i<m_beat_values.size(); ++i) {
		if (beat == (std::round(beat/m_beat_values[i])*m_beat_values[i] + m_ph[i])) {
			return true;
		}
	}
	return false;
}

std::string tmetg_t::print() const {

	std::string s {};
	s += "tmetg.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	for (auto i=0; i<m_note_values.size(); ++i) {
		s += m_note_values[i].print();
		s += " (=> " + std::to_string(m_beat_values[i].to_double()) + " beats);  ";
		s += "+ " + std::to_string(m_ph[i].to_double()) + " beat shift\n";
	}
	s += "\n\n";
	s += "Grid resolution: " + std::to_string(m_btres.to_double()) + " beats\n";
	s += "Period:          " + std::to_string(m_period.to_double()) + " beats\n";
	s += "\n\n";

	std::string grid_sep {" "};
	std::string level_sep {"\n"};
	for (auto i=0; i<m_beat_values.size(); ++i) {
		for (beat_t cbt {0.0}; cbt<m_period; cbt += m_btres) {
			if (ismultiple(cbt.to_double(),m_ts.beats_per_bar().to_double(),6)) {
				s += "|" + grid_sep;
			}

			if (cbt == (std::round(cbt/m_beat_values[i])*m_beat_values[i] + m_ph[i])) {
				s += "1" + grid_sep;
			} else {
				s += "0" + grid_sep;
			}
		}
		s += level_sep;
	}

	return s;
}

std::vector<std::vector<int>> tmetg(ts_t ts_in, std::vector<note_value> dp_in, std::vector<beat_t> ph_in) {
	//
	// Each duration element di in dp_in spans some number of grid spaces NGi.  
	// For all the di's, as well as the full bar, dbr (which  spans NGbr grid
	// spaces), and the beat, dbt (which spans NGbt grid spaces) to be 
	// exactly representable on the grid, the grid resolution dg (the duration
	// spanned by a single grid step) must be chosen small enough.  
	// For all i, NGi/di = NGbr/dbr (= 1/gres)
	//     => NGi = (di/dbr)*NGbr  (also, NGi = di/dg)
	//     
	// The smallest value of NGbr for which all (di/dbr) are integers is the
	// optimal solution.  
	//     => NGbr = lcm((di/dbr).denom) (where all (di/dbr) are reduced)
	//
	// (I should probably also consider the "beat" a duration element...)
	//
	frac dbr = rapprox(ts_in.bar_unit().to_double(),256);
	frac dbt = rapprox(ts_in.beat_unit().to_double(),256);

	int NGbr = std::lcm((dbt/dbr).reduce().denom,1);
	std::vector<frac> dp {};
	for (auto e : dp_in) {
		auto di = rapprox(e.to_double(),256);
		dp.push_back(di);
		NGbr = std::lcm((di/dbr).reduce().denom,NGbr);
	}
	auto dg = (NGbr/dbr).reduce(); // expect same result for all NGi,di

	std::vector<int> NGi {};  
	for (auto e : dp) {
		frac tempfrac = (NGbr*(e/dbr)).reduce(); // for testing only; all should be x/1
		if (tempfrac.denom != 1) {
			au_error("shit");
		}
		NGi.push_back(tempfrac.num);
	}
	int NGbt = (NGbr*(dbt/dbr)).reduce().num;

	//
	// What is the minimum grid length containing an integer number of all
	// di, as well as dbr and dbt?  
	// For a grid with NGtot cols, Ni = NGtot/NGi, Nbr = NGtot/NGbr, 
	// Nbt = NGtot/NGbt, Ng = NGtot.
	// Thus the value of NGtot for which NGbr, NGbt, and all of the NGi are
	// integers is the solution.  
	//     => Ntot = lcm(NGall)
	//
	std::vector<int> NG_all = NGi;
	NG_all.push_back(NGbr);
	NG_all.push_back(NGbt);
	auto Ntot = lcm(NG_all);

	//
	// Explicit construction of the total grid:
	// tg[0] => dp_all[0] = dp_in[0], ... tg[n] => dp_all[n] = db
	// dp_all is dp_in with db appended to the end, and "indexes" tg.  
	//
	std::vector<note_value> dp_all = dp_in;
	dp_all.push_back(note_value{dbr.to_double()});
	dp_all.push_back(note_value{dbt.to_double()});

	std::vector<std::vector<int>> tg {};
	for (auto i=0; i<dp_all.size(); ++i) {
		tg.push_back(std::vector<int>(Ntot,0));
		for (auto j=0; j<=(tg[i].size()-NG_all[i]); j+=NG_all[i]) {
			tg[i][j] = 1;
		}
	}

	return tg;
}







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

	int n_trim {0};
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


// Convert a sequence of note on-times (in seconds) to a sequence of 
// note-values.  The off-time of note i is the on-time of note i+1.  
// If you have a vector of ontimes _and_ a vector of offtimes (say, 
// from a "notefile," there is an overload below.  
// 
std::vector<note_value> tonset2rp(std::vector<double> const& sec_onset, 
	ts_t const& ts_in, double const& bpm, double const& s_resolution) {
	auto bps = bpm/60.0;
	std::vector<note_value> best_nv(sec_onset.size()-1,note_value{0.0});
	for (auto i=1; i<sec_onset.size(); ++i) { // NOTE:  Begins with _second_ element
		auto delta_t = roundquant((sec_onset[i]-sec_onset[i-1]),s_resolution);
		best_nv[i-1] = note_value{ts_in,beat_t{delta_t*bps}};
	}
	return best_nv;
}

// Overload for a vector of ontimes and a vector of offtimes
std::vector<note_value> tonset2rp(std::vector<double> const& sec_on, 
	std::vector<double> const& sec_off, ts_t const& ts_in, double const& bpm, 
	double const& s_resolution) {

	auto bps = bpm/60.0;
	std::vector<note_value> best_nv(sec_on.size()-1,note_value{0.0});
	for (auto i=0; i<sec_on.size(); ++i) {
		auto delta_t = roundquant((sec_off[i]-sec_on[i]),s_resolution);
		best_nv[i-1] = note_value{ts_in,beat_t{delta_t*bps}};
	}
	return best_nv;
}

//  rp, ts, bpm
std::vector<double> rp2tonset(std::vector<note_value> const& rp_in, 
	ts_t const& ts_in, double const& bpm) {
	std::vector<double> t_on(rp_in.size()+1, 0.0);

	auto bps = bpm/60;
	for (auto i=0; i<rp_in.size(); ++i) {
		t_on[i+1] = t_on[i] + (nbeat(ts_in,rp_in[i]).to_double())/bps;
	}
	return t_on;
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
