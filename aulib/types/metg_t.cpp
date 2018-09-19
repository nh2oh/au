#include "metg_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "rp_t.h"
#include "..\util\au_error.h"
#include "..\util\au_random.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_algs.h"
#include "..\util\au_util.h"  // wait()
#include <vector>
#include <numeric> // lcm, gcd
#include <string>
#include <iostream>
#include <algorithm>


// Constructor from an rp
tmetg_t::tmetg_t(ts_t ts_in, rp_t rp_in) {
	m_ts = ts_in;
	auto vdt = rp_in.to_duration_seq();

	// Construct m_nvsph
	// A phase of 0 means that a given d_t element lies on a beat number that
	// is an integer multiple of its length in beats.  
	beat_t curr_bt {0};
	for (const auto& e : vdt) {
		//double nr = std::round(curr_bt/nbeat(m_ts,e));
		//beat_t ph = curr_bt-nr*nbeat(m_ts,e);
		//nvs_ph curr_nvsph {e, duration(m_ts,ph)};
		d_t offset = duration(m_ts,curr_bt);
		nvs_ph curr_nvsph {e, offset-std::floor(offset/e)*e};
		if (std::find(m_nvsph.begin(),m_nvsph.end(),curr_nvsph) == m_nvsph.end()) {
			m_nvsph.push_back(curr_nvsph);
		}
		curr_bt += nbeat(m_ts,e);
	}  //ph = offset-std::floor(offset/nv)*nv
	std::sort(m_nvsph.begin(),m_nvsph.end(),
		[](const nvs_ph& a, const nvs_ph& b){return a>b;});

	m_btres = gres();
	m_period = period();
	m_btstart = 0_bt;
	m_btend = curr_bt;
	set_pg_zero(curr_bt);

	// Set the probability of each element of the pg that corresponds to
	// a note in the input rp to 1.0.  
	// This should just index the row of the pg 
	// Since a given col curr_spet of the pg, m_pg[curr_step], may have multiple
	// rows corresponding to a given e (for example, if m_nvsph contains the 
	// same e with different phases), the call to levels_allowed() is needed.  
	curr_bt = 0_bt;
	int curr_step = 0;
	for (const auto& e : vdt) {
		auto idx = levels_allowed(curr_bt);
		for (auto i=0; i<idx.size(); ++i) {
			if (m_nvsph[idx[i]].nv == e) {
				m_pg[curr_step][idx[i]].lgp = 1.0;
			}
		}
		curr_bt += nbeat(m_ts,e);
		curr_step += nv2step(e); //stepsz;
	}

	m_f_pg_extends = pg_extends();

	validate();
}


// Constructor from a set of d_t and phases
// TODO:  This should take an options type-arg to set if bar-spanning elements 
// should be allowed... this then changes the gres and period calcs.  
tmetg_t::tmetg_t(ts_t ts_in, std::vector<d_t> nvs_in, std::vector<beat_t> ph_in) {
	if (ph_in.size() != nvs_in.size()) {
		au_error("ph_in.size() != nv_ts_in.size()");
	}

	m_ts = ts_in;

	// TODO:  Need to make sure elements are unique
	for (int i=0; i<nvs_in.size(); ++i) {
		auto curr_ph = duration(m_ts,ph_in[i]);
		nvs_ph curr_nvsph = {nvs_in[i], curr_ph-std::floor(curr_ph/nvs_in[i])*nvs_in[i]};

		if (std::find(m_nvsph.begin(),m_nvsph.end(),curr_nvsph) == m_nvsph.end()) {
			m_nvsph.push_back(curr_nvsph);
		}
	}

	m_btres = gres();
	m_period = period();
	m_btstart = 0_bt;
	m_btend = m_period;

	set_pg_random();
	m_f_pg_extends = pg_extends();  // Expect true...

	validate();
}


// Calculates the minimum grid resolution from m_nvsph, m_ts
beat_t tmetg_t::gres() const {
	//  TODO:  Overload of gcd for beat_t ?
	d_t gres = gcd(d_t{0.0}, m_ts.bar_unit());
	for (auto const& e : m_nvsph) {
		gres = gcd(gres, e.nv);
		gres = gcd(gres, e.ph);
	}
	return nbeat(m_ts,gres);
}


// Calcluates the grid period from m_nvsph, m_ts, m_btres.  Depends on 
// m_btres being correctly set!
//
// The period is the smallest tg segment which can be concatenated to
// itself repeatedly to generate a tg of any size *AND* which spans an
// integer number of bars.  This is usually larger than the smallest
// repeating unit.  
//
// 
beat_t tmetg_t::period() const { 
	auto gres_nv = duration(m_ts,m_btres);
	int n_grid_steps = static_cast<int>(m_ts.bar_unit()/gres_nv);
	// If m_ts.bar_unit() is not included in the calculation, 
	// the period will be the smallest repeating unit.  Not all
	// traversals will be the same length.  
	for (const auto& e : m_nvsph) {
		n_grid_steps = std::lcm(n_grid_steps, static_cast<int>(e.nv/gres_nv));
	}
	return m_btres*n_grid_steps;
}


// Converts a number-of-beats to a number-of-grid-steps.  Note that this
// is different from converting a "beat number" to a "step number" if
// If m_btstart != 0.  
int tmetg_t::bt2step(beat_t bt_in) const {
	//au_assert(aprx_int(bt_in/m_btres),"bt2step()");
	//return static_cast<int>(bt_in/m_btres);
	return static_cast<int>(std::floor(bt_in/m_btres));
}


int tmetg_t::nv2step(d_t nv_in) const {
	//au_assert(aprx_int(nbeat(m_ts,nv_in)/m_btres),"nv2step()");
	//return static_cast<int>(nbeat(m_ts,nv_in)/m_btres);
	return static_cast<int>(std::floor(nbeat(m_ts,nv_in)/m_btres));
}


// True if there is at least one note of m_nvs that can occur at the 
// given (beat + the given nv)
// Used by validate() to check for zero-pointers
bool tmetg_t::allowed_next(beat_t beat, d_t nv) const {
	auto nxt_bt = beat+nbeat(m_ts,nv); // Beat-number of the next beat
	return allowed_at(nxt_bt);
}


// True if there is at least one m_nvsph member note-value that can 
// occur at the given beat.  
bool tmetg_t::allowed_at(beat_t beat) const {
	// aprx_int((cbt-e.ph)/e.nbts)  // TODO:  better
	for (const auto& e : m_nvsph) {
		//if (beat == (std::round(beat/e.nbts)*e.nbts + e.ph)) {
		if (beat == (std::round(beat/nbeat(m_ts,e.nv))*nbeat(m_ts,e.nv) + nbeat(m_ts,e.ph))) {
			return true;
		}
	}
	return false;
}


// True if nv can occur at the given beat
// TODO:  This essentially queries the "tg," not the pg... if the object
// was made from a pg much more restricted than the tg, want to be able to
// query the pg.  
bool tmetg_t::allowed_at(d_t nv, beat_t beat) const {
	for (const auto& e : m_nvsph) {
		if (e.nv != nv) { continue; }
		if (aprx_int((beat-nbeat(m_ts,e.ph))/nbeat(m_ts,e.nv))) {
			return true;
		}
	}
	return false;
}


// Which levels of the grid are allowed at the given beat?
// TODO:  This essentially queries the "tg," not the pg... if the object
// was made from a pg much more restricted than the tg, want to be able to
// query the pg.  
// This is used to construct a random pg when not creating from an rp;
// it can not rely on being able to read the pg.  
std::vector<int> tmetg_t::levels_allowed(beat_t beat) const {
	std::vector<int> allowed {}; allowed.reserve(m_nvsph.size());
	for (int i=0; i<m_nvsph.size(); ++i) {
		if (aprx_int((beat-nbeat(m_ts,m_nvsph[i].ph))/nbeat(m_ts,m_nvsph[i].nv))) {
			allowed.push_back(i);
		}
	}
	return allowed;
}


// Can the pg be concatenated repeatedly to itself to generate rp's always
// aligning to the tg?
// Even though for all valid metg's all nonzero m_pg elements align with the
// corresponding tg, the pg may nevertheless be non-extendable.  For example,
// consider a 3/4 metg containing a half-note w/ ph=0 and containing an m_pg
// spanning exactly 1 bar w/ a nonzero probability for the half-note at beats
// 0 and 2.  Naive extension of the m_pg would place a third half-note at 
// beat 3, too close to the half-note at beat 2.  
// 
bool tmetg_t::pg_extends() const {
	// If m_pg is shorter than a period, extend it to exactly 1 period; if
	// longer by a factor N, extend to ceil(N).  
	auto nperiods_extend = std::max(1.0, std::ceil((m_btend-m_btstart)/m_period));
	int ncols_extend = bt2step(m_period*nperiods_extend);

	for (size_t c=0; c<ncols_extend; ++c) {
		for (size_t r=0; r<m_pg[c%m_pg.size()].size(); ++r) {
			if (m_pg[c%m_pg.size()][r].lgp > 0.0 && 
				!allowed_at(m_nvsph[r].nv,c*m_btres+m_btstart)) {
				return false;
			}
		}
	}

	return true;
}


// Clears the existing pg and creates a new one nbts long.  If nbts is
// not specified or is ==0 , sets the size to correspond to 
// m_btstart-m_btend.  
void tmetg_t::set_pg_zero(beat_t nbts) {
	m_pg.clear();
	if (nbts == 0_bt) {
		nbts = m_btend-m_btstart;
	}

	std::vector<pgcell> def_pgcol {};  // "Default pg col"
	for (int i=0; i<m_nvsph.size(); ++i) {
		//def_pgcol.push_back({i,bt2step(m_nvsph[i].nbts),0.0});
		def_pgcol.push_back({i,nv2step(m_nvsph[i].nv),0.0});
	}

	auto pg_nsteps = bt2step(nbts);
	for (int i=0; i<pg_nsteps; ++i) {
		m_pg.push_back(def_pgcol);
	}

	validate();
}


// At each beat, sets each element of the pg returned by levels_sllowed()
// to a random number.  The sum of all probabilities for each beat is
// == 0 (if no nv is allowed at that beat) or == 1.  
// First zero's the pg with a call to set_pg_zero():  All data in m_pg
// is lost.  
// Does _not_ resize m_pg.  
void tmetg_t::set_pg_random(int mode) {
	set_pg_zero(m_pg.size()*m_btres);

	// For each col i in g, compute a probability value for the elements 
	// returned by levels_allowed() and assign to the corresponding element
	// of m_pg.  For col i in g, the entry in m_pg corresponding to 
	// note-value k (m_nvsph[k]) is m_pg[i][k].  
	// m_pg[i].size() == m_nvsph.size() for all i.  
	auto re = new_randeng(true);
	for (auto i=0; i < m_pg.size(); ++i) { // for each col in g...
		auto curr_allowed = levels_allowed(i*m_btres);
		std::vector<double> curr_probs {};
		if (mode == 0) {
			// Probabilty of all allowed elements is the same
			curr_probs = normalize_probvec(
				std::vector<double>(curr_allowed.size(),1.0));
		} else {
			// Probabilty of all allowed elements is random
			curr_probs = normalize_probvec(
				urandd(curr_allowed.size(),0.0,1.0,re));
		}

		for (auto j=0; j<curr_allowed.size(); ++j) { 
			// for each row were a note is allowed...
			m_pg[i][curr_allowed[j]].lgp = curr_probs[j];
			// Note that the only elements of m_pg[i] being assigned are those
			// which are allowed at position i in g.  For the other elements
			// in m_pg[i] the default .lgp == 0, set by the call to set_pg_zero().  
		}
	}

	validate();
}


// Generate a random rp from the pg
// TODO:  Return an rpp?
std::vector<d_t> tmetg_t::draw() const {
	std::vector<d_t> rnts {};
	auto re = new_randeng(true);
	beat_t curr_bt {0};
	int curr_step {0};
	while (curr_step < m_pg.size()) {
		auto ridx = randset(1,nt_prob(curr_bt),re);
		// nt_prob(curr_bt).size() == m_pg[i] where i corresponds to curr_step.  
		// Since m_pg[i].size() == m_nvsph.size() for all i, ridx[0] indexes
		// both.  
		rnts.push_back(m_nvsph[ridx[0]].nv);
		//curr_bt += m_nvsph[ridx[0]].nbts;
		curr_bt += nbeat(m_ts,m_nvsph[ridx[0]].nv);
		curr_step += m_pg[curr_step][ridx[0]].stepsz;
	}
	return rnts;
}


// Read the note-probability vector at the given beat
// TODO:  bt may exceed m_pg2.size()... in this case, should "wrap"
// bt as if m_pg extended to infinity
std::vector<double> tmetg_t::nt_prob(beat_t bt) const {
	auto pg_col_idx = bt2step(bt);
	std::vector<double> probs {};
	for (auto const& e : m_pg[pg_col_idx]) {
		probs.push_back(e.lgp);
	}
	return probs;
}


// Factors the pg, if possible.  
// To be factorable, the pg must contain one or more cols which no prior
// elements point *over.*  
// rp's generated from the members of the result can be concatenated to
// produce rp's valid under the parent.  
// TODO: Does not respect m_btstart
std::vector<tmetg_t> tmetg_t::factor() const {
	// For each _row_ i in m_pg, col_ptrs[i] is a vector of col idxs
	// pointed at by the elements of row i.  
	std::vector<std::vector<int>> col_ptrs {};
	for (int i=0; i<m_nvsph.size(); ++i) {
		col_ptrs.push_back(std::vector<int>{});
		for (int j=0; j<m_pg.size(); ++j) {  // Each m_pg element j on row i
			if (m_pg[j][i].lgp > 0.0) {
				col_ptrs[i].push_back(j+m_pg[j][i].stepsz);
			}
		}
	}

	// cmn ptrs => "common pointers"
	// Col idxs that each row of m_pg contains an element pointing into.  Note 
	// that the elements of std::vector<std::vector<int>> col_ptrs are the 
	// _rows_ of the pg.  
	std::vector<int> cmn_ptrs = col_ptrs[0];
	for (const auto& e : col_ptrs) {  // For pg _row_ e...
		auto temp_cmn_ptrs = cmn_ptrs;
		cmn_ptrs.clear();
		std::set_intersection(e.begin(),e.end(),temp_cmn_ptrs.begin(),temp_cmn_ptrs.end(),
			std::back_inserter(cmn_ptrs));
	}

	std::vector<tmetg_t> slices {};
	beat_t curr_bt {0.0};
	for (auto e : cmn_ptrs) {
		if (e > m_pg.size()) { break; }
		slices.push_back(slice(curr_bt,e*m_btres));
		curr_bt = e*m_btres;
	}
	if (cmn_ptrs.back() < m_pg.size()) {
		slices.push_back(slice(curr_bt,m_pg.size()*m_btres));
	}

	return slices;
}


// Slices the pg and returns a metg_t w/ a pg corresponding to cols
// [from, to) of the parent.  
// bt_from, bt_to are beat _numbers_ 
tmetg_t tmetg_t::slice(beat_t bt_from, beat_t bt_to) const {
	int idx_from = bt2step(bt_from-m_btstart);
	int idx_to = bt2step(bt_to-m_btstart);
	if (idx_from >= m_pg.size() || idx_from < 0 || idx_to > m_pg.size() || idx_to < 0) {
		au_assert(m_f_pg_extends,"!m_f_pg_extends but range exceeds m_pg");
		// extend_pg() does not check m_f_pg_extends
	}

	auto result = *this;
	//result.m_pg.clear();
	//std::copy(m_pg.begin()+idx_from,m_pg.begin()+idx_to,std::back_inserter(result.m_pg));
	result.m_pg = extend_pg(bt_from,bt_to);
	result.m_btstart = bt_from;
	result.m_btend = bt_to;
	result.m_f_pg_extends = result.pg_extends();

	return result;
}


// Extend the pg to span [from,to) where to >= from.  
// Does _not_ check m_f_pg_extends; external users access this through
// slice(), which does make all the necessary checks.  
std::vector<std::vector<tmetg_t::pgcell>> tmetg_t::extend_pg(beat_t from, beat_t to) const {
	int idx_from = bt2step(from-m_btstart);
	int idx_to = bt2step(to-m_btstart);
	auto nsteps = idx_to-idx_from;
	if (idx_from < 0) {
		idx_from = std::abs(idx_from)%m_pg.size();
		idx_to = idx_from+nsteps;
	}
	au_assert((idx_from >= 0 && idx_to >= idx_from),"!(idx_from >= 0 && idx_to >= idx_from)");
	if (idx_from >= m_pg.size() || idx_to > m_pg.size()) {
		au_assert(m_f_pg_extends,"!m_f_pg_extends but range exceeds m_pg");
	}

	std::vector<std::vector<tmetg_t::pgcell>> new_pg {};
	for (size_t c=idx_from; c<idx_to; ++c) {
		new_pg.push_back(m_pg[c%m_pg.size()]);
	}

	return new_pg;
}


// Enumerate all variations represented by the pg.  
std::vector<tmetg_t::rpp> tmetg_t::enumerate() const {
	// g is derrived from m_pg:  
	// 1)  Each col of g contains only the entries in the corresponding col of
	// m_pg where the probability is > 0.  Hence, each col of g in principle
	// contains a different number of rows.  
	// 2)  Entries appear in each col of g appear in order of _decreasing_ 
	// probability, not in order of idx in m_nvsph.  
	// 3)  g[i][j].lgp is the std::log() of the probability of the corresponding
	// element in m_pg.  
	int N_tot_guess = 1;  // A crude guess @ the tot. # of rp's
	std::vector<std::vector<pgcell>> g {};
	for (auto e : m_pg) {
		std::sort(e.begin(),e.end(),
			[](const pgcell& a, const pgcell& b){return a.lgp>b.lgp;});
		std::vector<pgcell> e_nz {};  // "elements nonzero"
		for (auto ee : e) {
			if (ee.lgp == 0.0) { break; }
			e_nz.push_back(ee);
			e_nz.back().lgp = std::log(e_nz.back().lgp);
		}
		g.push_back(e_nz);
		N_tot_guess *= std::max(g.back().size(),static_cast<size_t>(1));
	}

	int N_prealloc = std::min(100000,N_tot_guess)+1;  // TODO:  Gross + 1
	nvp_p rpp_prototype {std::vector<int>(g.size(),-1),0.0};
	std::vector<nvp_p> rps_all(N_prealloc,rpp_prototype);

	int N = 0;  // The # of rps actually generated
	int x = 0;
	m_enumerator(rps_all,g,N,x);

	std::vector<rpp> rps {};
	for (int i=0; i<N; ++i) {
		std::vector<d_t> curr_rp {};
		for (int j=0; j<rps_all[i].rp.size(); ++j) {
			if (rps_all[i].rp[j] != -1) {
				curr_rp.push_back(m_nvsph[rps_all[i].rp[j]].nv);
			}
		}
		rps.push_back({curr_rp, rps_all[i].p, curr_rp.size()});
	}

	return rps;
}


void tmetg_t::m_enumerator(std::vector<nvp_p>& rps, 
	std::vector<std::vector<pgcell>> const& g, int& N, int x) const {
	if (N >= (rps.size()-1)) { return; }

	bool f_gspan_fatal {false};
	bool f_gspan_complete {false};
	bool f_gspan_continue {false};

	auto rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		rps[N].rp[x] = g[x][i].ix_nvsph;
		rps[N].p += g[x][i].lgp;
		int nx = x + g[x][i].stepsz;

		if (nx < g.size()) {
			f_gspan_fatal = false; f_gspan_complete = false; f_gspan_continue = true;
			// Re-enter, passing nx in place of x.  Note x does not change; when the
			// call returns, it will have the same value as before the call.  
			// On return, set rps[N] to rp_init
			// To next loop iter
		} else if (nx > g.size()) { // overshot the grid
			// Note that g.size() is one past the final col of the grid.  
			// For a "well behaved" g this should never happen.  I could implement checks in the
			// initial-call section to check for and drop g elements that will cause this
			// condition.
			f_gspan_fatal = true; f_gspan_complete = false; f_gspan_continue = false;
			// Remove the final element of rps[N]
			// Set rps[N] to rp_init (redundant?)
			// To next loop iter
		} else { // rps[N] spans the grid exactly
			f_gspan_fatal = false; f_gspan_complete = true; f_gspan_continue = false;
			// Increment N
			// Set rps[N] to rp_init
			// To next loop iter
		}

		if (f_gspan_continue) {
			// No fatal errors, append next
			m_enumerator(rps,g,N,nx);
		} else if (f_gspan_complete) {
			++N;
		} else if (f_gspan_fatal) {
			//rps[N].rp.pop_back();
			//rps[N].p /= g[x][i].p;
			// Redundant w/ rps[N] = rp_init below
		}

		// If f_gspan_fatal, N is not incremented
		rps[N] = rp_init;
	}
}


void tmetg_t::m_enumerator2(std::vector<nvp_p>& rps, 
	std::vector<std::vector<pgcell>> const& g, int& N, int x) const {
	if (N >= (rps.size()-1)) { return; }
	auto rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		rps[N].rp[x] = g[x][i].ix_nvsph;
		rps[N].p += g[x][i].lgp;
		int nx = x + g[x][i].stepsz;

		if (nx < g.size()) {
			// Re-enter, passing nx in place of x.  Note x does not change; when the
			// call returns, it will have the same value as before the call.  
			m_enumerator(rps,g,N,nx);
		} else if (nx == g.size()) { // rps[N] spans the grid exactly
			++N;
		}

		// If f_gspan_fatal, N is not incremented
		rps[N] = rp_init;
	}
}


bar_t tmetg_t::nbars() const {
	return nbar(m_ts,m_btres*m_pg.size());
}
ts_t tmetg_t::ts() const {
	return m_ts;
}
std::vector<tmetg_t::nvs_ph> tmetg_t::levels() const {
	return m_nvsph;
}

std::string tmetg_t::print() const {
	std::string s {};
	s += "tmetg.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	s += "Grid resolution: " + m_btres.print() + " beats\n";
	s += "Period:          " + m_period.print() + " beats\n";
	s += "\n\n";
	for (auto const& e : m_nvsph) {
		s += e.nv.print();
		s += " (=> " + nbeat(m_ts,e.nv).print() + " beats => " + std::to_string(nv2step(e.nv)) + " grid steps);  ";
		s += "+ " + nbeat(m_ts,e.ph).print() + " beat shift\n";
	}

	s += print_tg();

	return s;
}


std::string tmetg_t::print_tg() const {
	std::string s {};
	for (auto const& e : m_nvsph) {
		for (beat_t cbt {0.0}; cbt<m_period; cbt += m_btres) {
			if (aprx_int(cbt/m_ts.beats_per_bar())) {
				s += "| ";
			}
			s += bsprintf("%d ",aprx_int((cbt-nbeat(m_ts,e.ph))/nbeat(m_ts,e.nv)));
		}
		s += "\n";
	}

	return s;
}


std::string tmetg_t::print_pg() const {
	std::vector<std::vector<double>> pg_full {};
	for (const auto& e : m_pg) {
		std::vector<double> curr_col(m_nvsph.size(),0.0);
		for (const auto& ee : e) {
			curr_col[ee.ix_nvsph] = ee.lgp;
		}
		pg_full.push_back(curr_col);
	}

	std::string s {};
	int ridx {0};
	for (int ridx=0; ridx<m_nvsph.size(); ++ridx) {
		for (const auto& c : pg_full) {
			s += bsprintf("%5.3f   ",c[ridx]);
		}
		s += "\n";
	}

	return s;
}


bool tmetg_t::validate() const {
	// m_nvsph can't be empty
	// All members are unique
	if (m_nvsph.size() == 0) {
		return false;
	}
	auto uq_nvsph = unique(m_nvsph);
	if (uq_nvsph.size() != m_nvsph.size()) {
		return false;
	}
	for (const auto& e : m_nvsph) {
		if (e.ph-std::floor(e.ph/e.nv)*(e.nv) != e.ph) {
			// e.ph is not in "reduced" form
			return false;
		}
		if (e.ph < d_t{0} || e.ph > e.nv) {
			// This check *should* be redundant w/ the check above
			return false;
		}
	}

	// m_btres, m_period, m_btstart, m_btend are correctly calculated & have 
	// been updated for any additions or deletions to m_nvsph or the pg.  
	if (m_btres != gres() || m_period != period()) {
		return false;
	}
	if ((m_btend-m_btstart) != m_btres*m_pg.size()) {
		return false;
	}
	if (!aprx_int(m_period/m_btres)) {
		return false;
	}
	for (int i=0; i<m_nvsph.size(); ++i) {
		if (!aprx_int(nbeat(m_ts,m_nvsph[i].nv)/m_btres)) {
			// A check on m_btres, not m_nvsph
			return false;
		}
	}

	// Each col of m_pg must have the same size() as m_nvsph
	// Probabilities must be >= 0
	//    All probability entries in a given col must sum to 1.0 or 0.0
	// Nonzero elements in m_pg align to the tg
	for (int c=0; c<m_pg.size(); ++c) {
		if (m_pg[c].size() != m_nvsph.size()) {
			return false;
		}
		double curr_prob_sum {0.0};
		for (int r=0; r<m_pg[c].size(); ++r) {
			if ((m_pg[c][r].stepsz)*m_btres != nbeat(m_ts,m_nvsph[r].nv)) {
				// Stepsize is not calculated correctly...
				return false;
			}
			if (m_pg[c][r].lgp < 0.0) {
				return false;
			}
			if (m_pg[c][r].lgp > 0.0 && !allowed_at(m_nvsph[r].nv,c*m_btres+m_btstart)) {
				return false;
			}

			if (!allowed_next(c*m_btres+m_btstart,m_nvsph[r].nv)) {
				// Element c,r is a zero-pointer.
				// TODO:  Not really: allowed_next() checks the tg, but c,r could
				// point into a col where all probabilities are 0 in the pg.  It 
				// could also point off the end of my "extended pg" ... I need
				// to extend the pg for at least 2 cols.  

				//return false;
			}

			curr_prob_sum += m_pg[c][r].lgp;
		}
		if (!(aprx_eq(curr_prob_sum,0.0) || aprx_eq(curr_prob_sum,1.0))) {
			// Probabilities for the col are not correctly normalized
			return false;
		}
	}

	// m_f_pg_extends is set correctly
	if (pg_extends() != m_f_pg_extends) {
		return false;
	}

	return true;
}


// Note that this does not check m_btstart == rhs.m_btstart.  
// All that matters for equality is that enumerate() returns the same
// set of rp's.  
// There is a big here:
// If one tmetg_t object contains an entry in m_nvsph for which the 
// corresponding row in m_pg is all 0, the two objects would compare != but
// would generate the same rp's by enumerate().  
//
// It is critical that the elements in m_nvsph be sorted by nv, then by ph,
// since the comparison loop just goes through both structures in order.  
//
bool tmetg_t::operator==(const tmetg_t& rhs) const {
	if (m_ts != rhs.m_ts) { return false; }

	if (m_nvsph.size() != rhs.m_nvsph.size()) { return false; }
	for (int i=0; i<m_nvsph.size(); ++i) {
		if (m_nvsph[i].nv != rhs.m_nvsph[i].nv ||
			aprx_int((m_nvsph[i].ph-rhs.m_nvsph[i].ph)/m_nvsph[i].nv)) {
			// If the nv is the same, the corresponding phases must be such that 
			// the two entries place their generate elements at the same position
			// on the grid.  
			// m_nvsph is sorted first by nv then by ph
			return false;
		}
	}

	if (m_pg.size() != rhs.m_pg.size()) { return false; }
	for (int c=0; c<m_pg.size(); ++c) {
		for (int r=0; r<m_pg[c].size(); ++r) {
			if (m_pg[c][r].lgp != rhs.m_pg[c][r].lgp) {
				return false;
			}
		}
	}

	return true;
}


d_t tmetg_t::gcd(const std::vector<d_t>& dset) const {
	long long sfctr = 100'000'000'000;
	long long cgcd = 0;
	std::vector<long long> d_scaled {};
	for (const auto& e : dset) {
		long long curr = std::round(sfctr*(e/d_t{d::w}));
		d_scaled.push_back(curr);
		cgcd = std::gcd(cgcd, curr);
	}
	auto res = d_t {static_cast<double>(cgcd)/sfctr};
	return res;
}


// TODO:  Inspect m, n of a,b and work out the proper scaling factor
d_t tmetg_t::gcd(const d_t& a, const d_t& b) const {
	long sfctr = 100'000'000'000;
	long as = sfctr*(a/d_t{d::w});
	long bs = sfctr*(b/d_t{d::w});
	double cgcd = std::gcd(as,bs);
	return d_t {cgcd/sfctr};
}




// Operators <,> are needed for sorting m_mvsph.  
// Note the weird dependence on the ph.  Elements w/ larger-duration nv_t's
// compare > element's w/ smaller duration nv_t's, however, for elements w/
// the same nv_t, the element w/ the smaller duration phase compares >.  Note
// that a phase value is always >= 0 && < the paired nv_t.  
// 
// I do not define a proper constructor for the nvs_ph.  See 
// nvs_ph::validate() for the invariant.  
bool tmetg_t::nvs_ph::operator<(const tmetg_t::nvs_ph& rhs) const {
	if (nv != rhs.nv) {
		return nv < rhs.nv;
	} else {
		return (ph > rhs.ph);
	}
}
bool tmetg_t::nvs_ph::operator>(const tmetg_t::nvs_ph& rhs) const {
	if (nv != rhs.nv) {
		return nv > rhs.nv;
	} else {
		return (ph < rhs.ph);
	}
}
bool tmetg_t::nvs_ph::operator==(const tmetg_t::nvs_ph& rhs) const {
	return (nv==rhs.nv && aprx_int((ph-rhs.ph)/nv));
	// If the element validate()s, it should be possible to compare 
	// the phases directly w/ ==.  
}
bool tmetg_t::nvs_ph::validate() const {
	return (nv > d_t{0.0} && ph >= d_t{0.0} && ph < nv);
}


std::string autests::tests1() {
	auto s = std::string();
	auto ts1 = ts_t{4_bt,d::q};
	//tmetg_t mg1 {ts1,big_rp};
	//std::cout << mg1.print_pg() <<std::endl<<std::endl<<std::endl;
	//std::cout << mg1.print() <<std::endl<<std::endl<<std::endl;

	return s;
}


