#include "rand_rp.h"
#include "..\uih.h"
#include "..\types\nv_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\types\rp_t.h"
#include "..\util\au_random.h"
#include "..\util\au_util.h"
#include "..\util\au_error.h"
#include <vector>
#include <chrono>
#include <random>
#include <optional>
#include <set>
#include <string>


// rand_rp() ~ rdur()
// The rp returned will always span an integer number of bars (if requested) since the
// algorithm appends random 1-bar "segments" to a growing rp.  
//
// TODO:  Does this actually make use of the values in pd_in ??? Seems to ignore them...
//
// TODO:  Will this work if nbars = 0.5 ???  Always generates exactly 1 bar at a time?
//
//
std::optional<rp_t> rand_rp(ts_t ts_in, 
	std::vector<nv_t> dp_in, std::vector<double> pd_in, int nnts_in, 
	bar_t nbr_in, rand_rp_opts opts) {

	au_assert(nbr_in >= bar_t{0});
	au_assert(nnts_in >= 0);
	au_assert((nbr_in == bar_t{0.0} && nnts_in >0) || 
		(nbr_in > bar_t{0.0} && nnts_in == 0) || 
		(nbr_in > bar_t{0.0} && nnts_in > 0));
	au_assert(dp_in.size() == pd_in.size());

	// Timeout settings
	std::chrono::seconds maxt {3000};
	//auto tfail {std::chrono::system_clock::now()+opts.maxt};
	auto tfail {std::chrono::system_clock::now()+maxt};

	int mode {0};
	if (nnts_in == 0 && nbr_in >= bar_t{1}) {  // fixed nbr, unconstrained nnts
		mode = 1;
	} else if (nnts_in >= 1 && nbr_in == bar_t{0}) { // fixed nnts, unconstrained nbr
		mode = 2;
	} else if (nnts_in >= 1 && nbr_in >= bar_t{1}) { // nnts, nbr both constrained
		mode = 3;
	}

	std::vector<nv_t> rpseg {};  // "rp segment" always == 1 bar exactly
	std::vector<nv_t> rp {};

	//std::default_random_engine randeng {};
	//randeng.seed(std::chrono::system_clock::now().time_since_epoch().count());
	auto randeng = new_randeng(true);
	std::uniform_int_distribution<int> rdist {0,static_cast<int>(dp_in.size()-1)};
	bar_t nbar_rp {0};  // counter
	while (std::chrono::system_clock::now() < tfail) {
		// Each iteration of the outer loop appends rpseg (generated by the 
		// inner loop) to rp.  If the inner loop passes control to the outer 
		// loop, rpseg will always span exactly 1 bar.  

		bar_t rpseg_nbar {0.0};  // the # of bars spanned by the growing rpseg
		while (true) {
			if (std::chrono::system_clock::now() > tfail) {
				return {};
				// If this condition is used as the conditional for the loop, the
				// outter loop can not assume that rpseg is == 1 bar.  
			}
			//
			// Adds one duration element at a time to rpseg until rpseg is exactly 
			// 1 bar long.  If rpseg overshoots, rpseg is cleared and the process
			// continues.  The loop exits when rpseg == 1 bar or if it times out.  
			// It would be faster to just keep the running total as a double rather
			// than a bar_t.  
			//
			auto ridx {rdist(randeng)};
			nv_t rp_e {dp_in[ridx]};  // "rp element"
			rpseg.push_back(rp_e);
			rpseg_nbar += nbar(ts_in,rp_e);

			if (rpseg_nbar < bar_t {1.0}) {
				//...
			} else if (rpseg_nbar > bar_t {1.0}) {
				rpseg.clear();
				rpseg_nbar = bar_t {0.0};
			} else if (rpseg_nbar == bar_t {1.0}) {
				break;
			}
		}

		// At this point, rpseg is exactly 1 bar in length (modes 1,3), or 
		// exactly nnts_in _notes_ in length (mode 2).  
		rp.insert(rp.end(),rpseg.begin(),rpseg.end());
		rpseg.clear();
		nbar_rp+=bar_t{1};  // rpseg is always exactly 1 bar

		if (mode == 1) { // Fixed nbars, floating nnotes
			if (nbar_rp == nbr_in) { // Success
				//return std::optional<std::vector<nv_t>> {rp};
				return std::optional<rp_t> {rp_t{ts_in,rp}};
			}
			// There is no overshooting in mode 1, since in mode 1 the inner loop
			// always adds exactly 1 bar
		} else if (mode == 2) {  // Fixed nnts, floating nbars
			if (rp.size() == nnts_in) { // Success
				//return std::optional<std::vector<nv_t>> {rp};
				return std::optional<rp_t> {rp_t{ts_in,rp}};
			} else if (rp.size() > nnts_in) { // Overshot
				rp.clear();	nbar_rp = bar_t{0};
			} else if (rp.size() < nnts_in) {
				// Not overshot, but not done yet
			}
		} else if (mode == 3) { // Fixed nbars, fixed nnotes
			if (nbar_rp == nbr_in && rp.size() == nnts_in) { // Success
				//rp_t anrp {ts_in,rp};
				return std::optional<rp_t> {rp_t{ts_in,rp}};
			} else if (nbar_rp > nbr_in || rp.size() > nnts_in) { // Overshot
				rp.clear();	nbar_rp = bar_t{0};
			} else {
				// Not overshot, but not done yet
			}
		}
	} // Build next 1-bar segment rpseg

	// All success paths return inside the loop.  If we're here, the outer loop
	// timed out.  
	return {};
}

// TODO:  Convert rand_rp to take a std::set of nv_t instead of a std::vector
std::optional<rp_t> rand_rp(randrp_input rand_rp_input_in) {
	rand_rp_opts opts{std::chrono::seconds{3}};
	std::vector<nv_t> gross(rand_rp_input_in.nvset.begin(),rand_rp_input_in.nvset.end());

	return rand_rp(rand_rp_input_in.ts,gross,
		rand_rp_input_in.pd,rand_rp_input_in.n_nts,rand_rp_input_in.n_bars,opts);
}

std::optional<rp_t> rand_rp(ts_t ts_in, std::vector<nv_t> dp_in,
	std::vector<double> pd_in, int nnts_in, bar_t nbr_in) {
	rand_rp_opts opts {std::chrono::seconds {3}};
	return rand_rp(ts_in, dp_in, pd_in, nnts_in, nbr_in, opts);
}

au::uih_parser_result<randrp_input>
parse_randrp_input::operator()(randrp_input const& randrp_input_in) const {
	au::uih_parser_result<randrp_input> result {{},""};
		// NB: Clearing the default failmsg
	bool input_is_valid = true;
	if (randrp_input_in.nvset.size() < 1) {
		input_is_valid = false;
		result.failmsg += "Must supply at least one nv.\n";
	}
	if (randrp_input_in.nvset.size() != randrp_input_in.pd.size()) {
		input_is_valid = false;
		result.failmsg += "Must supply a relative probability for each nv.\n";
	}
	// TODO:  At least 2 elements of pd_in should be > 0
	if (randrp_input_in.n_nts < 0) {
		input_is_valid = false;
		result.failmsg += "n_nts must be >= 0.\n";
	}
	if (randrp_input_in.n_nts == 0 && randrp_input_in.n_bars == bar_t{0}) {
		input_is_valid = false;
		result.failmsg += "One of n_nts, n_bars must be constrained.\n";
	}

	if (input_is_valid) { result.o_result = randrp_input_in;}
	return result;
}


