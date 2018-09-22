#include "teejee.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_algs.h"
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
bool teejee::nv_ph::operator!=(const teejee::nv_ph& rhs) const {
	return (nv!=rhs.nv || ph!=rhs.ph);
	// See the notes in operator==()
}


//
//-----------------------------------------------------------------------------
// teejee
//
teejee::teejee(const ts_t& ts, const std::vector<d_t>& nv, 
		const std::vector<beat_t>& ph) {
	au_assert(ph.size()==nv.size(), "ph.size() != nv.size()");
	
	m_ts = ts;

	for (int i=0; i<nv.size(); ++i) {
		insert_level(nv[i],duration(m_ts,ph[i]));
	}

	m_btres = calc_gres();
	m_period = calc_period();
}

teejee::teejee(const rp_t& rp) {
	m_ts = rp.ts();
	auto vdt = rp.to_duration_seq();

	// insert_level() ignores d_t, ph elements that are already members of 
	// m_levels.  
	// Note the use of Kahan summation to keep track of the cumulative offset.  
	ksum<beat_t> curr_bt {};
	for (const auto& e : vdt) {
		d_t offset = duration(m_ts,curr_bt.value);
		insert_level(e,offset);
		curr_bt += nbeat(m_ts,e);
	}
	m_btres = calc_gres();
	m_period = calc_period();
}

// Adds an element to m_levels and sorts the container.  Returns true if the
// element was successfully added. 
// Does _not_ recalculate the resolution or period:  This  routine is called 
// in loops (for example, when constructing from an rp_t), and there is no
// need to calculate the resolution & period until the end.  
//
// Also checks that nv is > d::z, which is not an invariant of nv_ph.  
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

ts_t teejee::ts() const {
	return m_ts;
}
std::vector<teejee::nv_ph> teejee::levels() const {
	return m_levels;
}
int teejee::bt2step(const beat_t beat) const {
	return static_cast<int>(beat/m_btres);
}
beat_t teejee::step2bt(const int step) const {
	return beat_t {step*m_btres};
}

// True => the grid can be sliced [some_initial_beat, beat)
bool teejee::factors_at(const beat_t beat) const {
	int n = 0;
	for (const auto& e : m_levels) {
		n += onset_allowed_at(bt2reducedbt(beat));
	}
	return (n == m_levels.size());
}

// Note that if the input does not lie on the grid (that is, is not a
// multiple of m_btres), neither will the return value.  
beat_t teejee::bt2reducedbt(const beat_t beat) const {
	return (beat - std::floor(beat/m_period)*m_period);
}

// The number of possible rp's spanning 1 period
// TODO:  Wrong, probably.  
int teejee::count() const {
	int ncols = m_period/m_btres;
	int n_tot = 0;
	for (int i=0; i<ncols; ++i) {
		int n = 0;
		for (int j=0; j<m_levels.size(); ++j) {
			if (onset_allowed_at(m_levels[j],i*m_btres)) { n +=1; }
		}
		n_tot *= std::max(n,1);
	}

	return 0;
}

std::vector<teejee::nv_ph> teejee::which_allowed_at(const beat_t beat) const {
	std::vector<teejee::nv_ph> members_allowed {};
	for (const auto& e : m_levels) {
		if (onset_allowed_at(e,beat)) {
			members_allowed.push_back(e);
		}
	}
	return members_allowed;
}
// Is any member of m_levels allowed at beat_in?
bool teejee::onset_allowed_at(const beat_t beat_in) const {
	for (const auto& e : m_levels) {
		if (onset_allowed_at(e,bt2reducedbt(beat_in))) {
			return true;
		}
	}
	return false;
}
// nvph need not be a member
bool teejee::onset_allowed_at(const nv_ph nvph, const beat_t beat) const {
	return aprx_int((beat-nbeat(m_ts,nvph.ph))/nbeat(m_ts,nvph.nv));
}
// Does an nv located at the given beat span a bar break?
// nv need not be a member of m_levels
bool teejee::spans_bar(const beat_t beat, const d_t nv) const {
	beat_t btnum_nxt_bar = std::ceil(beat/m_ts.beats_per_bar())*m_ts.beats_per_bar();
	if (btnum_nxt_bar == beat) {btnum_nxt_bar += m_ts.beats_per_bar(); }

	return (btnum_nxt_bar >= beat+nbeat(m_ts,nv));
}
bool teejee::ismember(const d_t nv) const {
	for (const auto& e : m_levels) {
		if (e.nv == nv) { 
			return true;
		} else if (e.nv < nv) {
			// Since m_levels is sorted from large to small nv_t's, as soon as the 
			// m_levels member e becomes smaller than nv, we know nv is not a member
			return false;
		}
	}
	return false;
}
bool teejee::ismember(const nv_ph nvph) const {
	for (const auto& e : m_levels) {
		if (e == nvph) { 
			return true;
		} else if (e.nv < nvph.nv) {
			// Since m_levels is sorted from large to small nv_t's, as soon as the 
			// m_levels member e becomes smaller than nv, we know nv is not a member
			return false;
		}
	}
	return false;
}


std::string teejee::print() const {
	std::string s {};
	s += "tmetg_t.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	s += "Grid resolution: " + m_btres.print() + " beats\n";
	s += "Period:          " + m_period.print() + " beats\n";
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


// It is critical that the elements in m_levels be sorted.  
bool teejee::operator==(const teejee& rhs) const {
	if (m_ts != rhs.m_ts) { return false; }

	if (m_levels.size() != rhs.m_levels.size()) { return false; }
	for (int i=0; i<m_levels.size(); ++i) {
		if (m_levels[i] != rhs.m_levels[i]) {
			return false;
		}
	}

	return true;
}


// Calculates the grid resolution from m_levels, m_ts.
// The grid resolution is the coarsest possible step size such that all
// members of m_levels _and_ m_ts.bar_unit() fall exactly on a grid point.  
beat_t teejee::calc_gres() const {
	d_t gres = gcd(d_t{0.0}, m_ts.bar_unit());
	for (auto const& e : m_levels) {
		gres = gcd(gres, e.nv);
		gres = gcd(gres, e.ph);
	}
	return nbeat(m_ts,gres);
}
beat_t teejee::gres() const {
	return m_btres;
}


// Calcluates the grid period from m_levels, m_ts, m_btres.  Depends on 
// m_btres being correctly set!
//
// The period is the smallest number of beats able to contain an integer 
// number of all emmbers of m_levels as well as m_ts.bar_unit().  This 
// will usually larger than the smallest repeating subunit of the grid.  
// For example, for m_levels containing d::h and d::q both w/ zero phase,
// the smallest repeating unit is 2 beats, but m_period == 4 beats, since
// 2 beats do not span a full bar.  
//
beat_t teejee::calc_period() const { 
	auto gres_nv = duration(m_ts,m_btres);
	int n_grid_steps = static_cast<int>(m_ts.bar_unit()/gres_nv);
	// If m_ts.bar_unit() is not included in the calculation, 
	// the period will be the smallest repeating unit.  Not all
	// traversals will be the same length.  
	for (const auto& e : m_levels) {
		n_grid_steps = std::lcm(n_grid_steps, static_cast<int>(e.nv/gres_nv));
	}
	return m_btres*n_grid_steps;
}
bar_t teejee::period() const { 
	return nbar(m_ts,m_period);
}



bool teejee::validate() const {
	// m_levels can't be empty; all members must be unique
	if (m_levels.size() == 0) {
		return false;
	}
	auto uq_nvsph = unique(m_levels);
	if (uq_nvsph.size() != m_levels.size()) {
		return false;
	}
	for (const auto& e : m_levels) {
		if (e.ph-std::floor(e.ph/e.nv)*(e.nv) != e.ph) {
			return false; // e.ph is not in "reduced" form
		}
		if (e.ph < d_t{0} || e.ph > e.nv) {
			// This check *should* be redundant w/ the check above
			return false;
		}
	}

	if (!aprx_int(m_period/m_btres)) {
		return false;
	}
}


