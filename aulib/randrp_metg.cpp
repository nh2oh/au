#include "randrp_metg.h"
#include "types/beat_bar_t.h"
#include "types/metg_t.h"
#include "types/rp_t.h"
#include "util/au_random.h"
#include "util/au_error.h"
#include "util/au_util.h"
#include <vector>
#include <algorithm> // std::max()
#include <numeric> // iota()


// TODO:  Some elements of the split() may be identical
// TODO:  Currently allowing fractional nbars as long as an integer number
// of split() elements can be appended together to get exactly that value
// TODO:  Assumes the pg can be extended
// TODO:  Need to take a more careful look at the method used for dropping
// low-probability rps
// TODO:  Need to truncate the final segment otherwise can't do things like
// 1 bar in 3/4 w/ h,q,e,sx since each segment is 0.3333.... bars




rp_t randrp_metg(tmetg_t mg, int nnts, bar_t nbars) {
	
	if (nbars > 0_br) {
		// Extends, truncates, or extends and truncates the pg to be exactly
		// the size of nbars.  
		mg = mg.slice(0_bt,nbeat(mg.ts(),nbars));
	}

	// Factor the input mg via mg.split(), and store the result in rps
	struct rpset_nbars {
		bar_t nbars {};  
		size_t min_nnts {};  // For the set, the rp w/ the smasllest nnts
		size_t max_nnts {};
		std::vector<tmetg_t::rpp> rpp {};  // Holds the result of enumerate()
	};
	auto mgexact = mg; mgexact.set_length_exact(nbeat(mg.ts(),nbars));
	auto mgsplit = mgexact.factor();

	std::vector<rpset_nbars> rps {};
	for (size_t i=0; i<mgsplit.size(); ++i) {
		std::vector<tmetg_t::rpp> curr_rps = mgsplit[i].enumerate();

		// Discard rp's elements w/ log-probability > the log of e/n
		// Add what remains to rps.back() and normalize the probabilities
		// Since .p is a log(probability), the rp w/ the largest .p is the
		// least probable rp.  
		std::vector<tmetg_t::rpp> curr_rps_highprob {};
		auto curr_max_prob = (*std::max_element(curr_rps.begin(),curr_rps.end(),
			[](const tmetg_t::rpp& lhs, const tmetg_t::rpp& rhs){return lhs.p < rhs.p;})).p;
		double curr_p_tot {0.0};
		size_t curr_max_nnts {0};
		size_t curr_min_nnts {curr_rps[0].n};  // If init w/0, stays 0
		for (const auto& e : curr_rps) {
			auto delta_log_p = e.p-curr_max_prob;
			if (delta_log_p > std::log(0.0000001)-std::log(curr_rps.size())) {
				curr_rps_highprob.push_back(e);
				curr_rps_highprob.back().p = std::exp(delta_log_p);

				curr_p_tot += curr_rps_highprob.back().p;
				curr_max_nnts = std::max(curr_max_nnts,curr_rps_highprob.back().n);
				curr_min_nnts = std::min(curr_min_nnts,curr_rps_highprob.back().n);
			}
		}
		for (auto e : curr_rps_highprob) {
			e.p /= curr_p_tot;
		}

		rps.push_back({mgsplit[i].nbars(),curr_min_nnts,curr_max_nnts,curr_rps_highprob});
	}

	// If the number of bars is constrained, the value of nbars determines 
	// required_n_appends, since each element of rps is a fixed number of 
	// bars.  If nnts is constrained but nbars is not, required_n_appends
	// is an upper bound, but rp's constructed from < required_n_appends steps
	// are ok.  
	size_t required_n_appends {0};
	if (nbars > 0_br) {
		bar_t cum_nbars {0};
		while (cum_nbars < nbars) {
			cum_nbars += rps[required_n_appends%rps.size()].nbars;
			++required_n_appends;
		}
		au_assert(cum_nbars == nbars,
			"The given mg does not split evenly to this nbars :(");

		if (nnts > 0) {
			size_t min_nnts {0}; size_t max_nnts {0};
			for (int i=0; i<required_n_appends; ++i) {
				min_nnts += rps[i%rps.size()].min_nnts;
				max_nnts += rps[i%rps.size()].max_nnts;
			}
			if (!(max_nnts>=nnts && min_nnts<=nnts)) {
				wait();
			}
			au_assert((max_nnts>=nnts && min_nnts<=nnts),
				"Impossible:  No way to do "+std::to_string(nnts)+" notes in " + 
				nbars.print() + " bars.");
		}
	} else if (nbars == 0_br && nnts > 0) {
		// If we're unlucky enough to choose the rp w/the smallest nnts at each
		// step, how many steps would it take to satisfy the nnts constraint?
		int cum_nnts {0};
		while (cum_nnts < nnts) {
			cum_nnts += rps[required_n_appends%rps.size()].min_nnts;
			++required_n_appends;
		}
		if (required_n_appends == 1 && cum_nnts > nnts) {
			au_assert(false,
				"The rp w/the smallest nnts in the first segment of the mg is < nnts.");
		}
	}

	size_t niter_max {100'000};
	std::vector<d_t> curr_rp {};
	size_t n_appends_curr_rp {0}; bar_t curr_nbars {0}; size_t curr_nnts {0};
	auto re = new_randeng(true);
	for (size_t niter=0; niter<niter_max; ++niter) {
		size_t curr_rpset_idx = n_appends_curr_rp%rps.size();
		auto nnts_remaining = nnts-curr_rp.size();

		std::vector<size_t> idx_keep {};
		std::vector<double> probs_keep {};
		if (nnts > 0 && nbars > 0_br) {
			// If, on every iteration _subsequent_ to the present, i happen to pick 
			// the rp with the smallest (or largest) nnts, the final rp will have
			// the nnts of the rp selected in the present iteration (yet 
			// unknown) + min_possible_nnts (or + max_possible_nnts).  
			size_t min_possible_nnts {0}; size_t max_possible_nnts {0};
			auto n_appends_remaining = required_n_appends-n_appends_curr_rp;
			for (size_t i=0; i<(n_appends_remaining-1); ++i) {
				// Note that i start from i+1 because I do not want to include 
				// curr_rpset_idx:  These vars give the min and max number 
				// of notes possible in _subsequent_ iterations.  
				min_possible_nnts += rps[(curr_rpset_idx+i+1)%rps.size()].min_nnts;
				max_possible_nnts += rps[(curr_rpset_idx+i+1)%rps.size()].max_nnts;
			}

			for (size_t i=0; i<rps[curr_rpset_idx].rpp.size(); ++i) {
				// "If i pick rp i AND THEN in all subsequent iterations i pick the rp 
				// with the smallest number of notes, will i end up with too many notes?
				if (rps[curr_rpset_idx].rpp[i].n+min_possible_nnts > nnts_remaining) { continue; }
				// "If i pick rp i AND THEN in all subsequent iterations i pick the rp 
				// with the largest number of notes, will i end up with too few notes?
				if (rps[curr_rpset_idx].rpp[i].n+max_possible_nnts < nnts_remaining) { continue; }
				// For rp i, if the answer to both questions above was "no,"
				idx_keep.push_back(i);
				probs_keep.push_back(rps[curr_rpset_idx].rpp[i].p);
			}
			wait();
		} else if (nnts > 0 && nbars == 0_br) {
			for (size_t i=0; i<rps[curr_rpset_idx].rpp.size(); ++i) {
				if (rps[curr_rpset_idx].rpp[i].n > nnts_remaining) { continue; }
				idx_keep.push_back(i);
				probs_keep.push_back(rps[curr_rpset_idx].rpp[i].p);
			}
		} else {
			for (size_t i=0; i<rps[curr_rpset_idx].rpp.size(); ++i) {
				idx_keep.push_back(i);
				probs_keep.push_back(rps[curr_rpset_idx].rpp[i].p);
			}
		}

		if (idx_keep.size() == 0) {
			curr_rp.clear(); n_appends_curr_rp = 0;
			curr_nbars = 0_br; curr_nnts = 0;
			continue;
		}

		// At this point, any member i of the current rp-set, rps[curr_rpset_idx].rpp[i],
		// where i is a member of idx_keep, will work.  That is, if appended to the
		// curr_rp, it will not overshoot nnts or nbars, nor will it put us in a
		// situation where subsequent iterations will be forced to exceed nnts, or 
		// unable to meet nnts
		auto ridx = randset(1,probs_keep,re);
		for (const auto& e : rps[curr_rpset_idx].rpp[idx_keep[ridx[0]]].rp) {
			curr_rp.push_back(e);
		}
		curr_nbars += rps[curr_rpset_idx].nbars;
		curr_nnts += rps[curr_rpset_idx].rpp[idx_keep[ridx[0]]].n;
		++n_appends_curr_rp;

		if (nnts > 0 && nbars > 0_br) {
			if (nnts == curr_nnts && nbars == curr_nbars) { break; }
		} else if (nnts > 0 && nbars == 0_br) {
			if (nnts == curr_nnts) { break; }
		} else if (nnts == 0 && nbars > 0_br) {
			if (nbars == curr_nbars) { break; }
		}


	}
	
	return rp_t {mg.ts(),curr_rp};

}






