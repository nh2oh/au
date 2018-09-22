#include "metg_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "rp_t.h"
#include "teejee.h"
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


// Constructor from an rp_t
tmetg_t::tmetg_t(ts_t ts_in, rp_t rp_in) {
	m_tg = teejee {rp_in};
	auto vdt = rp_in.to_duration_seq();
	m_btstart = 0_bt;
	set_pg_zero(rp_in.nbeats());

	// Set the probability of each element of the pg that corresponds to
	// a note in the input rp to 1.0.  
	ksum<beat_t> curr_beat = {}; int curr_step = 0;
	for (const auto& e : vdt) {
		teejee::nv_ph curr_nvph {e,duration(m_tg.ts(),curr_beat.value)};
		au_assert(m_tg.onset_allowed_at(curr_nvph,curr_beat.value),"what");
		auto r = nvph2level(curr_nvph);
		m_pg[curr_step][r] = 1.0;
		curr_step += level2stride(r);
		curr_beat += nbeat(m_tg.ts(),e);
	}

	m_f_pg_extends = pg_extends();
}


// Constructor from a set of d_t and phases
// TODO:  This should take an options type-arg to set if bar-spanning elements 
// should be allowed.  
tmetg_t::tmetg_t(ts_t ts_in, std::vector<d_t> nvs_in, std::vector<beat_t> ph_in) {
	au_assert(ph_in.size()==nvs_in.size(),"ph_in.size() != nv_ts_in.size()");

	m_tg = teejee {ts_in,nvs_in,ph_in};
	m_btstart = 0_bt;

	set_pg_zero(nbeat(m_tg.ts(),m_tg.period()));
	set_pg_random();
	m_f_pg_extends = pg_extends();  // Expect true...
}

int tmetg_t::nvph2level(const teejee::nv_ph& nvph) const {
	auto lvls = m_tg.levels();
	auto it = std::find(lvls.begin(),lvls.end(),nvph);
	return it-lvls.begin();
}
// Converts an absolute beat-number to a col idx of m_pg
int tmetg_t::bt2step(beat_t beat_number) const {
	//au_assert(aprx_int((beat_number-m_btstart)/m_tg.gres()),"bt2step()");
	return static_cast<int>(std::floor((beat_number-m_btstart)/m_tg.gres()));
}
// Converts an absolute col-number to a beat number
beat_t tmetg_t::step2bt(int col_idx) const {
	return col_idx*m_tg.gres() + m_btstart;
}
int tmetg_t::bt2stride(beat_t nbeats) const {
	//au_assert(aprx_int(nbeats/m_tg.gres()),"bt2stride()");
	return static_cast<int>(std::floor(nbeats/m_tg.gres()));
}
int tmetg_t::nv2stride(d_t nv_in) const {
	return bt2stride(nbeat(m_tg.ts(),nv_in));
}
int tmetg_t::level2stride(int lvl) const {
	return nv2stride(m_tg.levels()[lvl].nv);
}


// True if at least one m_tg.level() that can occur at beat_number.  
bool tmetg_t::onset_allowed_at(beat_t beat_number) const {
	int c = bt2step(beat_number);
	if (c >= m_pg.size() && !m_f_pg_extends) { return false; }

	c = c%m_pg.size();
	for (int r=0; r<m_pg[c%m_pg.size()].size(); ++r) {
		if (m_pg[c][r] > 0.0) {
			return true;
		}
	}
	return false;
}

// True if there is at least one member note-value that can 
// occur at the given beat.  
bool tmetg_t::onset_allowed_at(d_t nv, beat_t beat_number) const {
	int c = bt2step(beat_number);
	if (c >= m_pg.size() && !m_f_pg_extends) { return false; }

	c = c%m_pg.size();
	for (int r=0; r<m_pg[c].size(); ++r) {
		if (m_tg.levels()[r].nv == nv && m_pg[c][r] > 0.0) {
			return true;
		}
	}
	return false;
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
	auto beats_period = nbeat(m_tg.ts(),m_tg.period());  // beats per period
	auto nperiods_curr = std::ceil((step2bt(m_pg.size())-m_btstart)/beats_period);
	auto nperiods_extend = std::max(1.0, nperiods_curr);
	int ncols_extend = bt2stride(beats_period*nperiods_extend);

	for (size_t c=0; c<ncols_extend; ++c) {
		for (size_t r=0; r<m_pg[c%m_pg.size()].size(); ++r) {
			if (m_pg[c%m_pg.size()][r] > 0.0 && 
				!(m_tg.onset_allowed_at(m_tg.levels()[r],step2bt(c)))) {
				return false;
			}
		}
	}
	return true;
}


std::vector<std::vector<int>> tmetg_t::zero_pointers() const {
	std::vector<std::vector<int>> zps {};

	for (int c=0; c<m_pg.size(); ++c) {
		for (int r=0; r<m_pg[c].size(); ++r) {
			if (m_pg[c][r] == 0.0) { continue; }

			int nxtc = c + level2stride(r);
			if (nxtc >= m_pg.size()) { continue; }

			if (!onset_allowed_at(step2bt(nxtc))) {//(m_tg.gres()*nxtc)) {
				zps.push_back({c,r});
			}
		} // to next row r in c
	} // to next c

	return zps;
}


// Replaces m_pg w/ a new one spanning nbts beats with all entries == 0.0.
void tmetg_t::set_pg_zero(beat_t nbts) {
	m_pg.clear();
	auto pg_nsteps = bt2stride(nbts);
	for (int i=0; i<pg_nsteps; ++i) {
		m_pg.push_back(std::vector<double>(m_tg.levels().size(),0.0));
	}
}


// At each beat bt, sets each element of the pg returned by 
// m_tg.onset_allowed_at(bt) to a random number.  First zero's the pg w/
// set_pg_zero():  All data in m_pg is lost.  Does _not_ resize m_pg.  
void tmetg_t::set_pg_random(int mode) {
	set_pg_zero(step2bt(m_pg.size()));

	// For each col c in m_pg, compute a random probability value for the 
	// elements returned by m_tg.onset_allowed()
	auto re = new_randeng(true);
	for (auto c=0; c<m_pg.size(); ++c) {
		auto curr_allowed = m_tg.which_allowed_at(step2bt(c));
		std::vector<double> curr_probs {};
		if (mode == 0) { // Probabilty of all allowed elements is the same
			curr_probs = normalize_probvec(
				std::vector<double>(curr_allowed.size(),1.0));
		} else { // Probabilty of all allowed elements is random
			curr_probs = normalize_probvec(
				urandd(curr_allowed.size(),0.0,1.0,re));
		}

		for (auto r=0; r<curr_allowed.size(); ++r) { 
			m_pg[c][nvph2level(curr_allowed[r])] = curr_probs[r];
			// Note that only elements r of m_pg[c] are being assigned; all
			// other elements are == 0 by the call to set_pg_zero().
		}
	}

}


// Generate a random rp from the pg
// TODO:  Return an rpp?
std::vector<d_t> tmetg_t::draw() const {
	auto re = new_randeng(true);
	auto levels = m_tg.levels();
	std::vector<d_t> rnts {};
	int curr_step=0;
	while (curr_step<m_pg.size()) {
		auto ridx = randset(1,m_pg[curr_step],re);
		rnts.push_back(levels[ridx[0]].nv);
		curr_step += level2stride(ridx[0]);
	}
	return rnts;
}


// Factors the pg, if possible.  
// To be factorable, the pg must contain one or more cols which no prior
// elements point *over.*  rp's generated from the members of the result 
// can be concatenated to produce rp's valid under the parent.  
std::vector<tmetg_t> tmetg_t::factor() const {
	std::vector<std::vector<int>> col_ptrs {};  // col idxs pointed at by all elements of row r. 
	std::vector<int> cmn_ptrs {}; // "common pointers;" col_ptrs common between all rows
	for (int r=0; r<m_tg.levels().size(); ++r) {
		col_ptrs.push_back(std::vector<int>{});
		for (int c=0; c<m_pg.size(); ++c) {
			if (m_pg[c][r] > 0.0) {
				col_ptrs[r].push_back(c+level2stride(r));
			}
		}
		if (r==0) {
			cmn_ptrs = col_ptrs[0];
		} else {
			auto temp_cmn_ptrs = cmn_ptrs; cmn_ptrs.clear();
			std::set_intersection(col_ptrs[r].begin(),col_ptrs[r].end(),
				temp_cmn_ptrs.begin(),temp_cmn_ptrs.end(), std::back_inserter(cmn_ptrs));
		}
	}

	std::vector<tmetg_t> slices {};
	beat_t curr_bt = m_btstart;
	for (auto e : cmn_ptrs) {
		if (e > m_pg.size()) { break; }
		slices.push_back(slice(curr_bt,step2bt(e)));
		curr_bt = step2bt(e);
	}
	if (cmn_ptrs.back() < m_pg.size()) {
		slices.push_back(slice(curr_bt,m_pg.size()*m_tg.gres()));
	}

	return slices;
}


// Slices the pg and returns a metg_t w/ a pg corresponding to cols
// [from, to) of the parent.  
// Callers should check span_possible() first.  
tmetg_t tmetg_t::slice(beat_t bt_from, beat_t bt_to) const {
	int idx_from = bt2step(bt_from);
	int idx_to = bt2step(bt_to);
	au_assert(aprx_int((bt_from-m_btstart)/m_tg.gres()),
		"slice(): !aprx_int(bt_from-m_btstart)/m_tg.gres()");
	au_assert(aprx_int((bt_to-m_btstart)/m_tg.gres()),
		"slice(): !aprx_int(bt_to-m_btstart)/m_tg.gres()");
	if (idx_from >= m_pg.size() || idx_from < 0 || idx_to > m_pg.size() || idx_to < 0) {
		au_assert(m_f_pg_extends,"!m_f_pg_extends but range exceeds m_pg");
		// extend_pg() does not check m_f_pg_extends
	}

	auto result = *this;
	result.m_pg = extend_pg(bt_from,bt_to);
	result.m_btstart = bt_from;
	result.m_f_pg_extends = result.pg_extends();

	return result;
}


// Extend the pg to span [from,to) where to >= from.  
// Does _not_ check m_f_pg_extends; external users access this through
// slice(), which does make all the necessary checks.  
std::vector<std::vector<double>> tmetg_t::extend_pg(beat_t from, beat_t to) const {
	int idx_from = bt2step(from);
	int idx_to = bt2step(to);
	auto nsteps = idx_to-idx_from;
	if (idx_from < 0) {
		idx_from = std::abs(idx_from)%m_pg.size();
		idx_to = idx_from+nsteps;
	}
	au_assert((idx_from >= 0 && idx_to >= idx_from),"!(idx_from >= 0 && idx_to >= idx_from)");
	if (idx_from >= m_pg.size() || idx_to > m_pg.size()) {
		au_assert(m_f_pg_extends,"!m_f_pg_extends but range exceeds m_pg");
	}

	std::vector<std::vector<double>> new_pg {};
	for (size_t c=idx_from; c<idx_to; ++c) {
		new_pg.push_back(m_pg[c%m_pg.size()]);
	}

	return new_pg;
}


// Enumerate all variations represented by the pg.  
std::vector<tmetg_t::rpp> tmetg_t::enumerate() const {
	// g is derrived from m_pg:  
	// 1)  Each col of g contains only the entries in the corresponding 
	// col of m_pg where the probability is > 0.  Hence, each col of g may
	// contain a different number of rows.  
	// 2)  Entries appear in each col of g appear in order of _decreasing_ 
	// probability, not in order of m_tg.levels()
	// 3)  g[i][j].lgp is the std::log() of the probability of the  
	// corresponding element in m_pg.  
	int N_tot_guess = 1;  // A crude guess @ the tot. # of rp's
	std::vector<std::vector<enumerator_pgcell>> g {};
	for (auto e : m_pg) {
		std::vector<enumerator_pgcell> c_nz {};  // "col nonzero"
		for (int i=0; i<e.size(); ++i) { //auto ee : e) {
			if (e[i] == 0.0) { continue; }
			c_nz.push_back({i,level2stride(i),std::log(e[i])});
		}
		std::sort(c_nz.begin(),c_nz.end(),
			[](const enumerator_pgcell& a, const enumerator_pgcell& b){return a.lgp>b.lgp;});
		g.push_back(c_nz);
		N_tot_guess *= std::max(g.back().size(),static_cast<size_t>(1));
	}

	int N_prealloc = std::min(100000,N_tot_guess)+1;  // TODO:  Gross + 1
	nvp_p rpp_prototype {std::vector<int>(g.size(),-1),0.0};
	std::vector<nvp_p> rps_all(N_prealloc,rpp_prototype);

	int N = 0;  // The # of rps actually generated
	int x = 0;
	m_enumerator(rps_all,g,N,x);

	auto levels = m_tg.levels();
	std::vector<rpp> rps {};
	for (int i=0; i<N; ++i) {
		std::vector<d_t> curr_rp {};
		for (int j=0; j<rps_all[i].rp.size(); ++j) {
			if (rps_all[i].rp[j] != -1) {
				curr_rp.push_back(levels[rps_all[i].rp[j]].nv);
			}
		}
		rps.push_back({curr_rp, rps_all[i].p, curr_rp.size()});
	}

	return rps;
}


void tmetg_t::m_enumerator(std::vector<nvp_p>& rps, 
	std::vector<std::vector<enumerator_pgcell>> const& g, int& N, int x) const {
	if (N >= (rps.size()-1)) { return; }

	bool f_gspan_fatal {false};
	bool f_gspan_complete {false};
	bool f_gspan_continue {false};

	auto rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		rps[N].rp[x] = g[x][i].level;
		rps[N].p += g[x][i].lgp;
		int nx = x + g[x][i].stride;

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
	std::vector<std::vector<enumerator_pgcell>> const& g, int& N, int x) const {
	if (N >= (rps.size()-1)) { return; }
	auto rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		rps[N].rp[x] = g[x][i].level;
		rps[N].p += g[x][i].lgp;
		int nx = x + g[x][i].stride;

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
	return nbar(m_tg.ts(),m_tg.gres()*m_pg.size());
}
ts_t tmetg_t::ts() const {
	return m_tg.ts();
}
std::vector<tmetg_t::nvs_ph> tmetg_t::levels() const {
	return m_tg.levels();
}
// A number-of-beats, not a beat-number.  
bool tmetg_t::span_possible(beat_t nbeats) const {
	if (!aprx_int(nbeats/m_tg.gres())) {
		return false;
	}

	int c = bt2stride(nbeats);
	if (c > m_pg.size() && !m_f_pg_extends) {
		return false;
	}
	c %= m_pg.size();

	// If, in the m_pg col corresponding to nbeats (=> nsteps), there is at 
	// least 1 nonzero p, the present object can generate rp's spanning nbeats.  
	// This assumes that m_pg has no zero pointers.  I am assuming that a nonzero
	// p => pervious cols pointing into nbars_target.  TODO:  !!
	for (int r=0; r<m_tg.levels().size(); ++r) {
		if (m_pg[c][r] > 0.0) { 
			return true; 
		}
	}

	return false;
}
bool tmetg_t::span_possible(bar_t nbars_target) const {
	return span_possible(nbeat(m_tg.ts(),nbars_target));
}

std::string tmetg_t::print() const {
	std::string s {};
	s += m_tg.print();
	s += print_pg();
	return s;
}
std::string tmetg_t::print_tg() const {
	return m_tg.print_g();
}
std::string tmetg_t::print_pg() const {
	std::string s {};
	for (int r=0; r<m_tg.levels().size(); ++r) {
		for (int c=0; c<m_pg.size(); ++c) {
			if (m_tg.onset_allowed_at(teejee::nv_ph{m_tg.ts().bar_unit(),d::z},step2bt(c))) {
				s += "| ";
			}
			s += bsprintf("%5.3f   ",m_pg[c][r]);
		}
		s += "\n";
	}
	s += "\n";
	return s;
}


bool tmetg_t::validate() const {
	if (m_tg.levels().size() == 0) {
		return false;
	}

	if ((step2bt(m_pg.size())-m_btstart) != m_tg.gres()*m_pg.size()) {
		return false; // m_btstart, m_pg.size() are inconsistent.  
	}

	// Each col of m_pg must have the same size() as m_tg.levels()
	// -Probabilities must be >= 0
	// -All probability entries in a given col must sum to 1.0 or 0.0
	// -Nonzero elements in m_pg align to the tg
	for (int c=0; c<m_pg.size(); ++c) {
		if (m_pg[c].size() != m_tg.levels().size()) {
			return false;
		}
		double curr_prob_sum {0.0};
		for (int r=0; r<m_pg[c].size(); ++r) {
			if (m_pg[c][r] < 0.0) {
				return false;
			}
			if (m_pg[c][r] > 0.0 && !m_tg.onset_allowed_at(m_tg.levels()[r],step2bt(c))) {
				return false;
			}
			curr_prob_sum += m_pg[c][r];
		}
		if (!(aprx_eq(curr_prob_sum,0.0) || aprx_eq(curr_prob_sum,1.0))) {
			return false;
		}
	}

	if (zero_pointers().size() > 0) { return false; }

	// m_f_pg_extends is set correctly
	if (pg_extends() != m_f_pg_extends) { return false; }

	return true;
}


bool tmetg_t::operator==(const tmetg_t& rhs) const {
	if (!(m_tg == rhs.m_tg)) { return false; }
	if (m_btstart != rhs.m_btstart) { return false; }
	if (m_pg.size() != rhs.m_pg.size()) { return false; }
	for (int c=0; c<m_pg.size(); ++c) {
		for (int r=0; r<m_pg[c].size(); ++r) {
			if (m_pg[c][r] != rhs.m_pg[c][r]) {
				return false;
			}
		}
	}

	return true;
}


std::string autests::tests1() {
	auto s = std::string();
	auto ts1 = ts_t{4_bt,d::q};
	//tmetg_t mg1 {ts1,big_rp};
	//std::cout << mg1.print_pg() <<std::endl<<std::endl<<std::endl;
	//std::cout << mg1.print() <<std::endl<<std::endl<<std::endl;

	return s;
}

