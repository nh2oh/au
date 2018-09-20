#include "teejee.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_error.h"
#include "..\util\au_util.h"
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>


//
//-----------------------------------------------------------------------------
// teejee::nv_ph
//
teejee::nv_ph::nv_ph(const d_t nv_in, const d_t offset_in) {
	// This constructor does not attempt to enforce nv > d::z, which
	// is required for correct calculation of the phase.  The caller
	// should check validate().  
	nv = nv_in;
	ph = phase(nv_in,offset_in);
}
//  For any nv and offset, the corresponding phase is always >= 0 && < nv. 
d_t teejee::nv_ph::phase(const d_t& nv, const d_t& offset) const {
	return offset-std::floor(offset/nv)*nv;
}

bool teejee::nv_ph::validate() const {
	return (nv > d_t{d::z} && ph >= d_t{d::z} && ph < nv);
}

// Operators <,> are needed for sorting w/ std::sort().  
// Note the weird dependence on the ph.  Elements w/ larger-duration nv_t's
// compare > element's w/ smaller duration nv_t's, however, for elements w/
// the same nv_t, the element w/ the smaller duration phase compares >.  Note
// that a phase value is always >= 0 && < the corresponding nv_t.  
bool teejee::nv_ph::operator<(const teejee::nv_ph& rhs) const {
	if (nv != rhs.nv) {
		return nv < rhs.nv;
	} else {
		return (ph > rhs.ph);  // Note sign flip
	}
}
bool teejee::nv_ph::operator>(const teejee::nv_ph& rhs) const {
	if (nv != rhs.nv) {
		return nv > rhs.nv;
	} else {
		return (ph < rhs.ph);  // Note sign flip
	}
}
bool teejee::nv_ph::operator==(const teejee::nv_ph& rhs) const {
	return (nv==rhs.nv && ph==rhs.ph);
	// A weaker equality condition on ph is: aprx_int((ph-rhs.ph)/nv
	// If the elements validate(), the strict equality must hold.  
}


//
//-----------------------------------------------------------------------------
// teejee
//
teejee::teejee(const ts_t& ts, const std::vector<d_t>& nv, 
		const std::vector<beat_t>& ph, bool tf_barspan) {
	au_assert(ph.size()==nv.size(), "ph.size() != nv.size()");
	
	m_ts = ts;
	m_f_barspan = tf_barspan;

	for (int i=0; i<nv.size(); ++i) {
		insert_level(nv[i],duration(m_ts,ph[i]));
	}

	m_btres = gres();
	m_period = nbeat(m_ts,period());
}


// Adds an element to m_levels and sorts the container.  Returns true if the
// element was successfully added. 
// Does _not_ recalculate the resolution or period:  This  routine is called 
// in loops (for example, when constructing from an rp_t), and there is no
// need to calculate the resolution & period until the end.  
//
// Also checks that nv is > d::z, which is not really an invariant of nv_ph
bool teejee::insert_level(const d_t& nv, const d_t& ph) {
	if (nv <= d_t{d::z}) { return false; }
	nv_ph new_lvl {nv,ph};
	if (std::find(m_levels.begin(),m_levels.end(),new_lvl) == m_levels.end()) {
		m_levels.push_back(new_lvl);
		std::sort(m_levels.begin(),m_levels.end(),
			[](const nv_ph& a, const nv_ph& b){return a>b;});

		return true;
	}
	return false;
}

// Is any member of m_levels allowed at beat_in?
bool teejee::onset_allowed_at(const beat_t beat_in) const {
	for (const auto& e : m_levels) {
		if (onset_allowed_at(nv_ph{m_ts.bar_unit(),d::z},beat_in)) {
			return true;
		}
	}
	return false;
}

// nvph_in need not be a member
bool teejee::onset_allowed_at(const nv_ph nvph, const beat_t beat) const {
	bool tf = aprx_int((beat-nbeat(m_ts,nvph.ph))/nbeat(m_ts,nvph.nv));
	if (m_f_barspan || !tf) {
		return tf;
	}
	// Necessary to check if the element spans a bar
	beat_t btnum_nxt_bar = std::ceil(beat/m_ts.beats_per_bar())*m_ts.beats_per_bar();
	if (btnum_nxt_bar == beat) {btnum_nxt_bar += m_ts.beats_per_bar(); }

	if (btnum_nxt_bar >= beat+nbeat(m_ts,nvph.nv)) {
		return true;
	}

	return false;
}


std::string teejee::print() const {
	std::string s {};
	s += "tmetg_t.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	s += "Grid resolution: " + m_btres.print() + " beats\n";
	s += "Period:          " + m_period.print() + " beats\n";
	s += "Permit Bar-spanning:  ";
	if (m_f_barspan) {  s+= "True\n"; } else { s += "False\n"; }

	s += "\n\n";
	for (auto const& e : m_levels) {
		s += e.nv.print();
		s += " (=> " + nbeat(m_ts,e.nv).print() + " beats ";
		s += "+ " + nbeat(m_ts,e.ph).print() + " beat shift\n";
	}
	s += "\n\n";
	s += print_g();

	return s;
}

std::string teejee::print_g() const {
	std::string s {};
	for (const auto& e : m_levels) {
		for (beat_t cbt {0.0}; cbt<m_period; cbt+=m_btres) {
			if (onset_allowed_at(nv_ph{m_ts.bar_unit(),d::z},cbt)) {
				s += "| ";
			}
			s += bsprintf("%d ", onset_allowed_at(nv_ph{e.nv,e.ph},cbt));
		}
		s += "\n";
	}
	s += "\n";
	return s;
}


// Calculates the minimum grid resolution from m_levels, m_ts
beat_t teejee::gres() const {
	d_t gres = gcd(d_t{0.0}, m_ts.bar_unit());
	for (auto const& e : m_levels) {
		gres = gcd(gres, e.nv);
		gres = gcd(gres, e.ph);
	}
	return nbeat(m_ts,gres);
}


// Calcluates the grid period from m_levels, m_ts, m_btres.  Depends on 
// m_btres being correctly set!
//
// The period is the smallest tg segment which can be concatenated to
// itself repeatedly to generate a tg of any size *AND* which spans an
// integer number of bars.  This is usually larger than the smallest
// repeating unit.  
// 
// 
bar_t teejee::period() const { 
	auto gres_nv = duration(m_ts,m_btres);
	int n_grid_steps = static_cast<int>(m_ts.bar_unit()/gres_nv);
	// If m_ts.bar_unit() is not included in the calculation, 
	// the period will be the smallest repeating unit.  Not all
	// traversals will be the same length.  
	for (const auto& e : m_levels) {
		n_grid_steps = std::lcm(n_grid_steps, static_cast<int>(e.nv/gres_nv));
	}
	return nbar(m_ts,m_btres*n_grid_steps);
}
