#include "metg_t.h"
#include "ts_t.h"
#include "nv_t.h";
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
#include <map>
#include <algorithm>

//-----------------------------------------------------------------------------
// The tmetg_t class


tmetg_t::tmetg_t(ts_t ts_in, rp_t rp_in) {
	m_ts = ts_in;
	auto vdt = rp_in.to_duration_seq();

	beat_t curr_bt {0};
	for (const auto& e : vdt) {
		double nr = std::round(curr_bt/nbeat(m_ts,e));
		beat_t ph = curr_bt-nr*nbeat(m_ts,e);
		nvs_ph curr_nvsph {e, nbeat(m_ts,e), ph};
		if (std::find(m_nvsph.begin(),m_nvsph.end(),curr_nvsph) == m_nvsph.end()) {
			m_nvsph.push_back(curr_nvsph);
		}
		curr_bt += nbeat(m_ts,e);
	}
	std::sort(m_nvsph.begin(),m_nvsph.end(),
		[](const nvs_ph& a, const nvs_ph& b){return a.nv>b.nv;});

	m_btres = gres();
	m_period = period();

	// The rp passed in may be > or < 1 period.  If < or if 
	// !aprx_integer(curr_bt/m_period), it is not possible to sample longer
	// rp's by extending m_pg...  What did i do in the matlab version?
	// Flag such situations
	if (!aprx_int(m_period/curr_bt)) {
		m_f_pg_extends = false;  // Default is true
	}

	set_pg_zero(curr_bt);

	curr_bt = 0_bt;
	int curr_step = 0;
	for (const auto& e : vdt) {
		auto idx = levels_allowed(curr_bt);
		int stepsz = static_cast<int>(nbeat(m_ts,e)/m_btres);
		for (auto i=0; i<idx.size(); ++i) {
			if (m_nvsph[idx[i]].nv == e) {
				m_pg[curr_step][idx[i]].lgp = 1.0;
			}
		}
		curr_bt += nbeat(m_ts,e);
		curr_step += stepsz;
	}
}


// TODO:  This should take an options type-arg to set if bar-spanning elements 
// should be allowed... this then changes the gres and period calcs.  
tmetg_t::tmetg_t(ts_t ts_in, std::vector<d_t> nvs_in, std::vector<beat_t> ph_in) {
	if (ph_in.size() != nvs_in.size()) {
		au_error("ph_in.size() != nv_ts_in.size()");
	}

	m_ts = ts_in;
	for (int i=0; i<nvs_in.size(); ++i) {
		m_nvsph.push_back({nvs_in[i], nbeat(ts_in,nvs_in[i]), ph_in[i]});
	}

	m_btres = gres();
	m_period = period();

	set_pg_random();
	split();
}


// Calculates the minimum grid resolution from m_nvsph, m_ts
beat_t tmetg_t::gres() const {
	//  TODO:  Overload of gcd for beat_t ?
	d_t gres = gcd(d_t{0.0}, m_ts.bar_unit());
	for (auto const& e : m_nvsph) {
		gres = gcd(gres, e.nv);
		gres = gcd(gres, duration(m_ts,e.ph));
	}
	return nbeat(m_ts,gres);
}


// Cancluates the minimum grid period from m_nvsph, m_ts, m_btres.  
// NB: Depends on m_btres being correctly set!
beat_t tmetg_t::period() const { 
	// TODO:  Just do the whole thing in beat_t ?
	auto nvgres = duration(m_ts,m_btres);
	int n_gres_units = m_ts.bar_unit()/nvgres;
	int ce = 0;
	for (auto e : m_nvsph) {
		ce = e.nv/nvgres;
		n_gres_units = std::lcm(n_gres_units, ce);

		ce = e.ph/nbeat(m_ts,nvgres);
		if (ce > 0) {
			// TODO:  should take the abs() of a -ph ?
			n_gres_units = std::lcm(n_gres_units, ce);
		}
	}
	auto gperiod = n_gres_units*nvgres;
	return beat_t{gperiod/m_ts.beat_unit()};
}


// True if there is at least one note of m_nvs that can occur at the 
// given (beat + the given nv)
bool tmetg_t::allowed_next(beat_t beat, d_t nv) const {
	auto nxt_bt = beat+nbeat(m_ts,nv); // Beat-number of the next beat
	return allowed_at(nxt_bt);
}


// True if there is at least one member note-value that can occur at the 
// given beat
bool tmetg_t::allowed_at(beat_t beat) const {
	// aprx_int((cbt-e.ph)/e.nbts)  // TODO:  better
	for (auto const& e : m_nvsph) {
		if (beat == (std::round(beat/e.nbts)*e.nbts + e.ph)) {
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
	for (auto const& e : m_nvsph) {
		if (e.nv != nv) {continue;}
		if (aprx_int((beat-e.ph)/e.nbts)) {
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
		// aprx_int((cbt-e.ph)/e.nbts)  // TODO:  better
		if (beat==(std::round(beat/m_nvsph[i].nbts)*m_nvsph[i].nbts + m_nvsph[i].ph)) {
			allowed.push_back(i);
		}
	}
	return allowed;
}


// Sets a pg nbts long with all probabilities == 0.
// If nbts is unspecified or is == 0, the pg is m_period beats long
void tmetg_t::set_pg_zero(beat_t nbts) {
	m_pg.clear();
	if (nbts == 0_bt) {
		nbts = m_period;
	}

	std::vector<pgcell> def_pgcol {};
	for (int i=0; i<m_nvsph.size(); ++i) {
		def_pgcol.push_back({i,static_cast<int>(m_nvsph[i].nbts/m_btres),0.0});
	}

	auto pg_nsteps = static_cast<int>(nbts/m_btres);
	for (int i=0; i<pg_nsteps; ++i) {
		m_pg.push_back(def_pgcol);
	}
}


// At each beat, sets each element of the pg returned by levels_sllowed()
// to a random number.  The sum of all probabilities for each beat is
// == 0 (if no nv is allowed at that beat) or == 1.  
// First zero's the pg with a call to set_pg_zero():  All data in m_pg
// is lost.  
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
		curr_bt += m_nvsph[ridx[0]].nbts;
		curr_step += m_pg[curr_step][ridx[0]].stepsz;
	}
	return rnts;
}


// Read the note-probability vector at the given beat
// TODO:  bt may exceed m_pg2.size()... in this case, should "wrap"
// bt as if m_pg extended to infinity
std::vector<double> tmetg_t::nt_prob(beat_t bt) const {
	auto pg_col_idx = static_cast<int>(bt/m_btres);
	std::vector<double> probs {};
	for (auto const& e : m_pg[pg_col_idx]) {
		probs.push_back(e.lgp);
	}
	return probs;
}


// Factors the pg, if possible.  
// rp's generated from the members of the result can be concatenated to
// produce rp's valid under the parent.  
std::vector<tmetg_t> tmetg_t::split() const {
	
	std::vector<std::vector<int>> col_ptrs {};
	for (int i=0; i<m_nvsph.size(); ++i) {
		col_ptrs.push_back(std::vector<int>{});
		for (int j = 0; i<m_pg.size(); ++j) {
			if (aprx_eq(m_pg[j][i].lgp,0.0)) { continue; }
			if (j+m_pg[j][i].stepsz >= m_pg.size()) {break;}
			col_ptrs[i].push_back(j+m_pg[j][i].stepsz);
		}
	}

	std::vector<int> cmn_ptrs = col_ptrs[0];
	for (auto e : col_ptrs) {
		auto temp_cmn_ptrs = cmn_ptrs;
		cmn_ptrs.clear();
		std::set_intersection(e.begin(),e.end(),temp_cmn_ptrs.begin(),temp_cmn_ptrs.end(),
			std::back_inserter(cmn_ptrs));
	}

	std::vector<tmetg_t> slices {};
	beat_t curr_bt {0.0};
	for (auto e : cmn_ptrs) {
		slices.push_back(get_slice(curr_bt,curr_bt+e*m_btres));
		curr_bt += e*m_btres;
	}
	slices.push_back(get_slice(curr_bt,m_period));
	//auto s1 = get_slice(0_bt,cmn_ptrs[0]*m_btres);
	//auto s2 = get_slice(cmn_ptrs[0]*m_btres,m_period);

	wait();

	return slices;
}


// TODO:  range may exceed m_pg limits
// TODO:  Extend the pg???
// 
tmetg_t tmetg_t::get_slice(beat_t bt_from, beat_t bt_to) const {
	int idx_from = bt_from/m_btres;
	int idx_to = bt_to/m_btres;

	auto result = *this;

	result.m_pg.clear();
	std::copy(m_pg.begin()+idx_from,m_pg.begin()+idx_to,std::back_inserter(result.m_pg));
	result.m_btinit = bt_from;


	return result;
}


// Enumerate all variations over a single period
std::vector<tmetg_t::rpp> tmetg_t::enumerate() const {
	int ncols = static_cast<int>(m_period/m_btres);

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
			if (ee.lgp == 0.0) { break;}
			e_nz.push_back(ee);
			e_nz.back().lgp = std::log(e_nz.back().lgp);
		}
		g.push_back(e_nz);
		N_tot_guess *= std::max(g.back().size(),static_cast<size_t>(1));
	}

	int N_prealloc = std::min(100000,N_tot_guess);
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
		rps.push_back({curr_rp, rps_all[i].p});
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


std::string tmetg_t::print() const {
	std::string s {};
	s += "tmetg.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	s += "Grid resolution: " + m_btres.print() + " beats\n";
	s += "Period:          " + m_period.print() + " beats\n";
	s += "\n\n";
	for (auto const& e : m_nvsph) {
		s += e.nv.print();
		s += " (=> " + e.nbts.print() + " beats => " + std::to_string(e.nbts/m_btres) + " grid steps);  ";
		s += "+ " + e.ph.print() + " beat shift\n";
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
			s += bsprintf("%d ",aprx_int((cbt-e.ph)/e.nbts));
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

// TODO:   Generalize this to any T
beat_t tmetg_t::round(beat_t bt) const {
	return beat_t{std::round(bt/(1_bt))};
}

std::string autests::tests1() {
	auto s = std::string();
	std::vector<d_t> dt1 {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph1(dt1.size(),0_bt);
	std::vector<beat_t> ph2 {0_bt,0_bt,0_bt};

	auto ts = ts_t{4_bt,d::q};
	auto mg = tmetg_t(ts,dt1,ph1);
	std::cout << mg.print() <<std::endl<<std::endl<<std::endl;
	std::cout << mg.print_pg() <<std::endl<<std::endl<<std::endl;

	mg.set_pg_random();
	auto all_rps = mg.enumerate();
	
	for (auto e : all_rps) {
		//rp_t curr_rp {ts,e.rp};
		//std::cout << bsprintf("%6.3f:  ", e.p) << curr_rp.print() <<std::endl;
	}
	
	wait();
	std::vector<d_t> big_vdt {};
	for (int i=0; i < 10; ++i) {
		auto vdt = mg.draw();
		for (auto e : vdt) {
			big_vdt.push_back(e);
		}
		rp_t rp {ts_t{3_bt,d::q},vdt};
		std::cout <<rp.print() <<std::endl<<std::endl;
	}
	//mg.enumerate();
	//wait();
	//auto vdt = mg.draw();
	rp_t big_rp {ts_t{3_bt,d::q},big_vdt};
	std::cout << big_rp.print() <<std::endl<<std::endl<<std::endl;

	wait();

	tmetg_t mg2 {ts_t{3_bt,d::q},big_rp};
	std::cout << mg2.print_pg() <<std::endl<<std::endl<<std::endl;
	std::cout << mg2.print() <<std::endl<<std::endl<<std::endl;
	

	wait();
	return s;
}


