#include "metg_t.h"
#include "ts_t.h"
#include "beat_bar_t.h"
#include "rp_t.h"
#include "..\util\au_error.h"
#include "..\util\au_random.h"
#include "..\util\au_algs_math.h"
#include "..\util\au_util.h"  // wait()
#include <vector>
#include <numeric> // lcm, gcd
#include <string>
#include <iostream>
#include <map>
#include <algorithm>

//-----------------------------------------------------------------------------
// The tmetg_t class

// Constructors

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

	// Minimum grid resolution
	//  TODO:  Overload of gcd for beat_t
	d_t gres = gcd(d_t{0.0}, m_ts.bar_unit());
	for (auto const& e : m_nvsph) {
		gres = gcd(gres, e.nv);
		gres = gcd(gres, duration(m_ts,e.ph));
	}
	
	// Minimum grid period
	// TODO:  Just do the whole thing in beat_t
	int n_gres_units = m_ts.bar_unit()/gres;
	int ce = 0;
	for (auto e : m_nvsph) {
		ce = e.nv/gres;
		n_gres_units = std::lcm(n_gres_units, ce);

		ce = e.ph/nbeat(m_ts,gres);
		if (ce > 0) {
			// TODO:  should take the abs() of a -ph
			n_gres_units = std::lcm(n_gres_units, ce);
		}
	}
	auto gperiod = n_gres_units*gres;
	m_btres = beat_t{gres/m_ts.beat_unit()};
	m_period = beat_t{gperiod/m_ts.beat_unit()};

	// The pg is always initialized to this dummy value to obviate the
	// need to add a bool is_pg_set check on every method that reads the
	// pg.  
	std::vector<pgcell> pgcol_default {};
	for (int i=0; i<m_nvsph.size(); ++i) {
		pgcol_default.push_back({i,static_cast<int>(m_nvsph[i].nbts/m_btres),0.0});
	}
	for (int i=0; i<(m_period/m_btres); ++i) {
		m_pg.push_back(pgcol_default);
	}

	set_rand_pg();
}

void tmetg_t::set_rand_pg() {
	int mode = 0;

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
			// in m_pg[i] the default .lgp == 0.  
		}
	}

}


// Generate a random rp from the pg
std::vector<d_t> tmetg_t::draw() const {
	std::vector<d_t> rnts {};
	auto re = new_randeng(true);
	beat_t curr_bt {0};
	while (curr_bt < m_period) {
		auto ridx = randset(1,nt_prob(curr_bt),re);
		rnts.push_back(m_nvsph[ridx[0]].nv);
		curr_bt += m_nvsph[ridx[0]].nbts;
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

// Enumerate all variations over a single period
void tmetg_t::enumerate() const {
	int ncols = static_cast<int>(m_period/m_btres);

	// g is essentially a copy of m_pg, however, instead of each col having
	// m_nvsph rows, each col has between 0 and m_nvsph rows.  For each col, 
	// entries appear in order of _decreasing_ probability.  Entries in m_pg 
	// with p == 0.0 do not appear in g.  	
	int N_tot_guess =1;
	std::vector<std::vector<pgcell>> g(ncols,std::vector<pgcell>{});
	for (int i=0; i<ncols; ++i) {

		std::vector<double> curr_col_probs;// = m_pg2[i];
		for (auto const& e : m_pg[i]) {
			curr_col_probs.push_back(e.lgp);
		}
		while (std::any_of(curr_col_probs.begin(),curr_col_probs.end(),[](double d){return d>0.0;})) {
			auto it = std::max_element(curr_col_probs.begin(),curr_col_probs.end());
			auto idx = std::distance(curr_col_probs.begin(),it);
			if (idx < curr_col_probs.size()) {  // Shouldn't need this test
				g[i].push_back(m_pg[i][idx]);
				g[i].back().lgp = std::log(g[i].back().lgp);
				curr_col_probs[idx] = 0.0;
			}
		}

		// Just a crude guess at the total number of rps possible
		if (g[i].size() > 0) {
			N_tot_guess *= g[i].size();
		}
	}

	int N_prealloc = std::min(10000,N_tot_guess);
	rpp rpp_prototype {std::vector<int>(g.size(),-1),0.0};
	std::vector<rpp> rps(N_prealloc,rpp_prototype);

	int N = 0;
	int x = 0;
	m_enumerator(rps,g,N,x);

	// Crude print
	std::vector<double> p {};
	for (int i=0; i<N; ++i) {
		p.push_back(rps[i].p);
		std::string s {};
		s += bsprintf("i=%d:  p=%.3f  :  ",i,rps[i].p);
		for (auto& e : rps[i].rp) {
			if (e == -1) { continue; }
			s += m_nvsph[e].nv.print();
			s += " ";
		}
		std::cout << s << std::endl;
	}

	wait();
}

void tmetg_t::m_enumerator(std::vector<rpp>& rps, 
	std::vector<std::vector<pgcell>> const& g, int& N, int x) const {
	//...

	bool f_gspan_fatal {false};
	bool f_gspan_complete {false};
	bool f_gspan_continue {false};

	auto rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		//rps[N].rp.push_back(g[x][i].ix_nvsph);
		//rps[N].p += g[x][i].lgp;
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
			wait();
			m_enumerator(rps,g,N,nx);
			wait();
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

//  Which levels of the grid are allowed at the given beat?
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


std::string tmetg_t::print() const {
	std::string s {};
	s += "tmetg.print():\n";
	s += "ts == " + m_ts.print() + "\n";
	for (auto const& e : m_nvsph) {
		s += e.nv.print();
		s += " (=> " + e.nbts.print() + " beats);  ";
		s += "+ " + e.ph.print() + " beat shift\n";
	}
	s += "\n\n";
	s += "Grid resolution: " + m_btres.print() + " beats\n";
	s += "Period:          " + m_period.print() + " beats\n";
	s += "\n\n";

	std::string grid_sep {" "};
	std::string level_sep {"\n"};
	for (auto const& e : m_nvsph) {
		for (beat_t cbt {0.0}; cbt<m_period; cbt += m_btres) {
			if (aprx_int(cbt/m_ts.beats_per_bar())) {
			//if (ismultiple(cbt.to_double(),m_ts.beats_per_bar().to_double(),6)) {
				s += "|" + grid_sep;
			}

			if (aprx_int((cbt-e.ph)/e.nbts)) {
				s += "1" + grid_sep;
			} else {
				s += "0" + grid_sep;
			}
			// TODO:  Better:
			// s += bsprintf("%d%s",(cbt-e.ph)/e.nbts, grid_sep);
		}
		s += level_sep;
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

std::string autest::metg::tests1() {
	std::vector<d_t> dt1 {d::h,d::q,d::ed,d::e};
	std::vector<beat_t> ph1(dt1.size(),beat_t{0});
	auto mg = tmetg_t(ts_t{beat_t{3},d::q},dt1,ph1);

	auto s = mg.print();
	std::cout <<s <<std::endl<<std::endl<<std::endl;

	mg.set_rand_pg();
	for (int i=0; i < 5; ++i) {
		auto vdt = mg.draw();
		rp_t rp {ts_t{3_bt,d::q},vdt};
		std::cout <<rp.print() <<std::endl<<std::endl<<std::endl;
	}

	mg.enumerate();

	wait();
	return s;
}


