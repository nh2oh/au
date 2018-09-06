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
	set_rand_pg();
}

void tmetg_t::set_rand_pg() {
	auto N_cols_period = static_cast<int>(m_period/m_btres);
	m_pg = std::vector<std::vector<double>>(N_cols_period,
		std::vector<double>(m_nvsph.size(),0.0));
	
	// For each col, compute levels_allowed().  For each element (row) in 
	// the present col, the value is either == 0 (default) or 1/n where
	// n is the number of elements in the present col allowed to contain a
	// note.  
	// m_pg[col i][row j]
	for (auto i=0; i < m_pg.size(); ++i) { // for each col...
		auto curr_bt = i*m_btres;
		auto curr_allowed = levels_allowed(curr_bt);
		for (auto idx : curr_allowed) {  // for each row were a note is allowed
			m_pg[i][idx] = 1.0/curr_allowed.size();
		}
	}

}


// Generate a random rp from the pg
std::vector<d_t> tmetg_t::draw() const {
	std::vector<d_t> rnts {};
	auto re = new_randeng(true);

	beat_t curr_bt {0};
	while (curr_bt < m_period) {
		auto pg_col_idx = static_cast<int>(curr_bt/m_btres);
		auto pg_col = m_pg[pg_col_idx];
		auto ridx = randset(1,pg_col,re);
		rnts.push_back(m_nvsph[ridx[0]].nv);
		curr_bt += m_nvsph[ridx[0]].nbts;
	}

	return rnts;
}

// Read the note-probability vector at the given beat
std::vector<double> tmetg_t::nt_prob(beat_t) {
	return std::vector<double>(5,0.0);
}

// Enumerate all variations over a single period
void tmetg_t::enumerate() const {

	std::vector<std::vector<int>> g((m_period/m_btres),std::vector<int>{});
	for (int i=0; i<(m_period/m_btres); ++i) {
		g[i]=levels_allowed(i*m_btres);
	}

	int N_max = 10000;
	std::vector<std::vector<int>> rps(N_max,std::vector<int>{});

	int N = 0;
	int x = 0;
	m_enumerator(rps,g,N,x);

	wait();
}

void tmetg_t::m_enumerator(std::vector<std::vector<int>>& rps, 
	std::vector<std::vector<int>> const& g, int& N, int& x) const {
	//...

	bool f_gspan_fatal {false};
	bool f_gspan_complete {false};
	bool f_gspan_continue {false};

	std::vector<int> rp_init = rps[N];

	for (int i=0; i<g[x].size(); ++i) {
		rps[N].push_back(g[x][i]);
		int nx = x + (m_nvsph[g[x][i]].nbts/m_btres);

		if (nx < g.size()) {
			f_gspan_fatal = false; f_gspan_complete = false; f_gspan_continue = true;
		} else if (nx > g.size()) { // overshot the grid
			// For a "well behaved" g this should never happen.  I could implement checks in the
			// initial-call section to check for and drop g elements that will cause this
			// condition.
			f_gspan_fatal = true; f_gspan_complete = false; f_gspan_continue = false;
		} else { // rps[N] spans the grid exactly
			f_gspan_fatal = false; f_gspan_complete = true; f_gspan_continue = false;
		}

		if (f_gspan_continue) {
			// No fatal errors, append next
			m_enumerator(rps,g,N,nx);
		} else if (f_gspan_complete) {
			++N;
		} else if (f_gspan_fatal) {
			rps[N].pop_back();
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
	std::vector<d_t> dt1 {d::h,d::q,d::ed};
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

	wait();
	return s;
}


