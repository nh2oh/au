#pragma once
#include "randmel_gens.h"
#include "..\types\line_t.h"
#include "..\util\au_random.h"
#include "..\util\au_algs.h"  // unique_n() in score()
#include "..\util\au_algs_math.h"  // aprx_eq() in score()
#include <vector>
#include <numeric>  // std::iota() to generate scdpool
#include <cmath>  // std::abs() (not in <numeric>)

//
// melody_a()
//
// Generates a melody (parameterized by scds) p.nnts in length on [p.min,p.max]
// by "evolving" a completely random initial melody m for p.npass "generations"
// as follows:
// At each generation i:
// 1) A random note is chosen from the melody m and replaced by a random note 
//    on [p.min,p.max] to generate a new melody m_new.  
// 2) m_new is scored according to p.sc_* (see "scoring function" below).  
// 3) If the score computed for m_new is greater than that for m, m is replaced
//    by m_new, and the process repeats from step (1).  If the score for m_new
//    is less than or equal to that for m, m_new is discarded and the process 
//    repeats from step (1).  
//
// This algorithm is written only in terms of scd_t (all the p members are expressed 
// in terms of scd_t).  Hence there is no dependence on any sort of scale, and the
// line_t returned will always be a line_t<scd_t>.  It is of course possible to 
// generalize the algorithm to arbitrary "param" structs and score functions, but the
// origional "melody_a()" from the matlab implementation of automus was strictly an
// scd function, so I want to keep melody_a() that way as well.  
//
//
// Scoring function
// The scoring function looks at adjacent pairs {i,i+1} of scds in the melody vector.
// - Every adjacent pair of scds which are the same contributes p.sc_adjnts.
// - Every instance of an scd appearing more than once (even if the occurences are 
//   non-adjacent pairs) contributes p.sc_rptnts.  
// - Every adjacent pair of scds which differ by > than p_optimstep contribute 
//   (abs(m[i]-m[i-1])-p.optimstep)*p.sc_stepsize.  p.optimstep should be >= 0.  
//
// An interesting feature of this scoring function is that it is independent of 
// absolute scd_t.  That is, for any melody vector m and any scd n, m+n has the 
// same score as m.  
//
//
// Notes
// - Since the offspring melody is only kept if its score is > than its parent,
//   the score for the evolving melody is monotonically increasing & is never
//   flat.  
// - Settings ma_params p {}:
//   p.npass=100; p.sc_adjnts=-0.5; p.sc_rptnts=-0.5; p.sc_stepsize=-1; p.optimstep=1.5;
//

std::vector<scd_t> melody_a(ma_params p) {
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
			// e.first => the scd; e.second => number of occurences
			if (e.second > 1) {
				s += (e.second-1)*p.sc_rptnts;
			}
		}
		return s;
	};

	// The scdpool is the "domain" of notes from which the melody is drawn
	std::vector<scd_t> scdpool((p.max-p.min).to_int(),scd_t{0});
	std::iota(scdpool.begin(),scdpool.end(),p.min);

	// rn_spidx => "random number scd pool idx;" rn_midx => "random number
	// melody idx"
	// Note the -1: Random numbers are generated on [min,max]
	auto re = new_randeng(true);
	std::uniform_int_distribution<size_t> rn_spidx {0,scdpool.size()-1};
	std::uniform_int_distribution<size_t> rn_midx {0,static_cast<size_t>(p.nnts-1)};

	// Completely random initial melody
	std::vector<scd_t> m = urandelems(scdpool,p.nnts,re);

	// Evolution
	double s = score(m);
	for (int i=0; i<p.npass; ++i) {
		std::vector<scd_t> m_new = m;
		m_new[rn_midx(re)] = scdpool[rn_spidx(re)];

		double new_s = score(m_new);
		if (new_s > s) {
			m = m_new;
			s = new_s;
		}
	}

	return m;
}

