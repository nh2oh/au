#pragma once
#include "randmel_gens.h"
#include "..\scale\scale_12tet.h"
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\types\metg_t.h"
#include "..\randrp_metg.h"
#include "..\util\au_random.h"
#include "..\util\au_algs.h"
#include "..\util\au_algs_math.h"
#include <string>
#include <vector>
#include <numeric>


//
//
//
//
// Notes
// The matlab version also scores the deviation from a "perfect" score on 
// kk_key(), but a consequence of doing this is that the alg will depend on 
// a scale and puurely scd intervals.  
//
//

std::vector<ntstr_t> melody_a(ma_params p) {
	scale_12tet sc {};

	// The scdpool is the "domain" of notes from which the melody is drawn
	std::vector<scd_t> scdpool((p.max-p.min).to_int(),scd_t{0});
	std::iota(scdpool.begin(),scdpool.end(),p.min);
	double prob_each {1.0/static_cast<double>(scdpool.size())};
	std::vector<double> scdpool_probs(scdpool.size(),prob_each);

	// The score actually isn't a function of scd_t - it can be entirely
	// represented as a function of scdpool idx values provided all elements
	// of scdpool are uq.  
	auto score = [&](const std::vector<scd_t>& m) {
		double s {0};

		for (int i=1; i<m.size(); ++i) {  // NB: starts @ 1
			double curr_delta = std::abs((m[i]-m[i-1]).to_double());
			if (aprx_eq(curr_delta,0.0)) {
				s += p.sc_adjnts;
			}
			s += std::abs(curr_delta-p.optimstep)*p.sc_stepsize;
		}
		auto uqs = unique_n(m);
		for (const auto& e : uqs) {
			s += (e.second)*p.sc_rptnts;
		}

		return s;
	};

	auto re = new_randeng(true);
	std::uniform_int_distribution<int> rn_spidx {0,scdpool.size()};

	// Initial completely random melody
	// TODO: range of urandi: [,) or [,] ??
	std::vector<scd_t> m = randelems(scdpool,p.nnts,scdpool_probs,re);
	double s = score(m);
	std::uniform_int_distribution<int> rn_midx {0,scdpool.size()};
	for (int i=0; i<p.npass; ++i) {
		std::vector<scd_t> m_new = m;
		m_new[rn_midx(re)] = scdpool[rn_spidx(re)];

		double new_s = score(m_new);
		if (new_s > s) {
			m = m_new;
			s = new_s;
		}
	}

	std::vector<ntstr_t> ntstrs(m.size(),ntstr_t{});
	for (int i=0; i<m.size(); ++i) {
		ntstrs[i] = sc.to_ntstr(m[i]);
	}

	return ntstrs;
}


