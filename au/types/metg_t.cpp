#include "metg_t.h"
#include "ts_t.h"
#include "beat_bar_t.h"
#include "rp_t.h"
#include "..\util\au_util_all.h"
#include <vector>
#include <numeric> // lcm
#include <string>



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



//
// Old version that explictly constructs a discrete grid, like i did in
// the Matlab version.  May be useful in the future for explicit grid-walking
// algorithms.  
//
//
//std::vector<std::vector<int>> tmetg(ts_t ts_in, 
//	std::vector<note_value> dp_in, std::vector<beat_t> ph_in) {
//	//
//	// Each duration element di in dp_in spans some number of grid spaces NGi.  
//	// For all the di's, as well as the full bar, dbr (which  spans NGbr grid
//	// spaces), and the beat, dbt (which spans NGbt grid spaces) to be 
//	// exactly representable on the grid, the grid resolution dg (the duration
//	// spanned by a single grid step) must be chosen small enough.  
//	// For all i, NGi/di = NGbr/dbr (= 1/gres)
//	//     => NGi = (di/dbr)*NGbr  (also, NGi = di/dg)
//	//     
//	// The smallest value of NGbr for which all (di/dbr) are integers is the
//	// optimal solution.  
//	//     => NGbr = lcm((di/dbr).denom) (where all (di/dbr) are reduced)
//	//
//	// (I should probably also consider the "beat" a duration element...)
//	//
//	frac dbr = rapprox(ts_in.bar_unit().to_double(),256);
//	frac dbt = rapprox(ts_in.beat_unit().to_double(),256);
//
//	int NGbr = std::lcm((dbt/dbr).reduce().denom,1);
//	std::vector<frac> dp {};
//	for (auto e : dp_in) {
//		auto di = rapprox(e.to_double(),256);
//		dp.push_back(di);
//		NGbr = std::lcm((di/dbr).reduce().denom,NGbr);
//	}
//	auto dg = (NGbr/dbr).reduce(); // expect same result for all NGi,di
//
//	std::vector<int> NGi {};  
//	for (auto e : dp) {
//		frac tempfrac = (NGbr*(e/dbr)).reduce(); // for testing only; all should be x/1
//		if (tempfrac.denom != 1) {
//			au_error("shit");
//		}
//		NGi.push_back(tempfrac.num);
//	}
//	int NGbt = (NGbr*(dbt/dbr)).reduce().num;
//
//	//
//	// What is the minimum grid length containing an integer number of all
//	// di, as well as dbr and dbt?  
//	// For a grid with NGtot cols, Ni = NGtot/NGi, Nbr = NGtot/NGbr, 
//	// Nbt = NGtot/NGbt, Ng = NGtot.
//	// Thus the value of NGtot for which NGbr, NGbt, and all of the NGi are
//	// integers is the solution.  
//	//     => Ntot = lcm(NGall)
//	//
//	std::vector<int> NG_all = NGi;
//	NG_all.push_back(NGbr);
//	NG_all.push_back(NGbt);
//	auto Ntot = lcm(NG_all);
//
//	//
//	// Explicit construction of the total grid:
//	// tg[0] => dp_all[0] = dp_in[0], ... tg[n] => dp_all[n] = db
//	// dp_all is dp_in with db appended to the end, and "indexes" tg.  
//	//
//	std::vector<note_value> dp_all = dp_in;
//	dp_all.push_back(note_value{dbr.to_double()});
//	dp_all.push_back(note_value{dbt.to_double()});
//
//	std::vector<std::vector<int>> tg {};
//	for (auto i=0; i<dp_all.size(); ++i) {
//		tg.push_back(std::vector<int>(Ntot,0));
//		for (auto j=0; j<=(tg[i].size()-NG_all[i]); j+=NG_all[i]) {
//			tg[i][j] = 1;
//		}
//	}
//
//	return tg;
//}
//












