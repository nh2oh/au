#include "randrp_metg.h"
#include "types/beat_bar_t.h"
#include "types/metg_t.h"
#include "types/rp_t.h"
#include "dbklib\stats.h"
#include "util/au_util.h"
#include "dbklib\algs.h"
#include <vector>
#include <algorithm> // std::max()
#include <numeric> // iota()
#include <exception>
#include <random>

// TODO:  Assumes the pg can be extended
// TODO:  Need to take a more careful look at the method used for dropping
// low-probability rps

rp_t randrp_metg(tmetg_t mg, int nnts, bar_t nbars) {
	auto mgexact = mg;
	if (nbars > 0_br) {
		// Extends, truncates, or extends /and/ truncates the pg such that all rps will be
		// exactly nbars.  
		bool success_setting_length = mgexact.set_length_exact(nbeat(mg.ts(),nbars));
		if (!success_setting_length) {
			std::abort();
		}
	}
	auto mgsplit = mgexact.factor();

	// To generate the sequence the caller is requesting, one needs to enumerate() all the
	// entries in mgsplit in order and random append selections from these rp sets in order, 
	// passing over the enumerate()d mgsplit entries however many times is needed to satisfy
	// the constraints.  Since the entries in mgsplit may not be unique, in general only some
	// need to be enumerate()d into an rp set.  idx_uqmgsplit maps the entries in
	// mgsplit to unique tmetg_t's in mgsplit returned by factor.  We hit the elements in
	// uq_mgsplit in the order given by idx_uqmgsplit.  The first entry in rps corresponds to
	// uq_mgsplit[idx_uqmgsplit[0]], the second is uq_mgsplit[idx_uqmgsplit[1]], ...
	auto uq_mgsplit = unique_nosort(mgsplit);
	std::vector<int> idx_uqmgsplit {};
	for (int i=0; i<mgsplit.size(); ++i) {
		for (int j=0; j<uq_mgsplit.size(); ++j) {
			if (uq_mgsplit[j] == mgsplit[i]) {
				idx_uqmgsplit.push_back(j);  // mgsplit[i] == uq_mgsplit[idx_uqmgsplit[i]]
			}
		}
	}

	// Factor the input mg via mg.split(), and store the result in rps
	struct rpset_nbars {
		bar_t nbars {};  
		size_t min_nnts {};  // For the set, the rp w/ the smasllest nnts
		size_t max_nnts {};
		std::vector<tmetg_t::rpp> rpp {};  // Holds the result of enumerate()
	};
	
	// rps[i] corresponds to uq_mgsplit[i]
	std::vector<rpset_nbars> rps {};
	for (size_t i=0; i<uq_mgsplit.size(); ++i) { // NB: uq_mgsplit.size() <= mgsplit.size()
		std::vector<tmetg_t::rpp> curr_rps = uq_mgsplit[i].enumerate();

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

		rps.push_back({uq_mgsplit[i].nbars()[0],curr_min_nnts,curr_max_nnts,curr_rps_highprob});
		// Note the 0 idx on uq_mgsplit[i].nbars()[0]:  The call to factor() ensures all members of
		// uq_mgsplit[i].nbars() are the same.  TODO:  Unit tests!
	}

	// If the number of bars is constrained, exactly 1 pass over idx_uqmgsplit is
	// required, since the input mg was extended to nbars prior to factoring, and
	// since each element of rps is a fixed number of  bars.  If, OTOH, nnts is 
	// constrained but nbars is not, modify idx_uqmgsplit by concatenating it to itself
	// some number of times such that idx_uqmgsplit.size() is the maximum possible number
	// of steps capable of building an rp w/ nnts notes.  
	if (nbars > 0_br && nnts > 0) { // Constraining nbars _and_ nnts
		size_t min_nnts {0}; size_t max_nnts {0};
		for (auto i : idx_uqmgsplit) {
			min_nnts += rps[i].min_nnts;
			max_nnts += rps[i].max_nnts;
		}
		if (max_nnts<nnts || min_nnts>nnts) {
			std::abort();
		}
		//au_assert((max_nnts>=nnts && min_nnts<=nnts),
		//	"Impossible:  No way to do "+std::to_string(nnts)+" notes in " + 
		//	nbars.print() + " bars.");
	} else if (nbars == 0_br && nnts > 0) {  // nbars is unconstrained
		// If we're unlucky enough to choose the rp w/the smallest nnts at each
		// step, how many steps would it take to satisfy the nnts constraint?
		size_t max_required_n_appends {0}; int cum_nnts {0};
		while (cum_nnts < nnts) {
			cum_nnts += rps[idx_uqmgsplit[max_required_n_appends%idx_uqmgsplit.size()]].min_nnts;
			++max_required_n_appends;
		}
		if (max_required_n_appends == 1 && cum_nnts > nnts) {
			std::abort();
			//  The rp w/the smallest nnts in the first segment of the mg is < nnts
		}
		size_t init_n_uq = idx_uqmgsplit.size();
		for (int i=idx_uqmgsplit.size(); i<max_required_n_appends; ++i) {
			idx_uqmgsplit.push_back(idx_uqmgsplit[i%init_n_uq]);
			// Appending to idx_uqmgsplit in this way in general makes it impossible to
			// make more than one pass over it.  Ex, if this loop appends 1.5 copies to 
			// itself, passing over this extended idx_uqmgsplit twice does not generate 
			// the same sequence of idxs as three passes over the initial, non-extended 
			// version.  max_required_n_appends really must be an upper bound.  
		}
	}

	size_t niter_max {100'000};
	std::vector<d_t> curr_rp {};
	size_t n_appends_curr_rp {0}; bar_t curr_nbars {0}; size_t curr_nnts {0};
	auto re = new_randeng(true);
	for (size_t niter=0; niter<niter_max; ++niter) {
		size_t curr_rpset_idx = idx_uqmgsplit[n_appends_curr_rp%idx_uqmgsplit.size()];
		auto nnts_remaining = nnts-curr_rp.size();

		std::vector<size_t> idx_keep {};
		std::vector<double> probs_keep {};
		if (nnts > 0 && nbars > 0_br) {
			// If, on every iteration _subsequent_ to the present, i happen to pick 
			// the rp with the smallest (or largest) nnts, the final rp will have
			// the nnts of the rp selected in the present iteration (yet 
			// unknown) + min_possible_nnts (or + max_possible_nnts).  
			size_t min_possible_nnts {0}; size_t max_possible_nnts {0};
			auto n_appends_remaining = idx_uqmgsplit.size()-n_appends_curr_rp;
			for (size_t i=0; i<(n_appends_remaining-1); ++i) {
				// Note that i start from i+1 because I do not want to include 
				// curr_rpset_idx:  These vars give the min and max number 
				// of notes possible in _subsequent_ iterations.  

				min_possible_nnts += rps[curr_rpset_idx].min_nnts;
				max_possible_nnts += rps[curr_rpset_idx].max_nnts;
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
		std::discrete_distribution rd {probs_keep.begin(),probs_keep.end()};
		auto ridx = rd(re);
		for (const auto& e : rps[curr_rpset_idx].rpp[idx_keep[ridx]].rp) {
			curr_rp.push_back(e);
		}

		curr_nbars += rps[curr_rpset_idx].nbars;
		curr_nnts += rps[curr_rpset_idx].rpp[idx_keep[ridx]].n;
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

