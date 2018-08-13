#include "rand_rp.h"
#include "..\types\types_all.h"
#include "..\util\au_util_all.h"
#include <vector>
#include <chrono>
#include <random>
#include <optional>


// rand_rp() ~ rdur()
// The rp returned will always span an integer number of bars (if requested) since the
// algorithm appends random 1-bar "segments" to a growing rp.  
std::optional<std::vector<nv_t>> rand_rp(ts_t ts_in, 
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
				return std::optional<std::vector<nv_t>> {rp};
			}
			// There is no overshooting in mode 1, since in mode 1 the inner loop
			// always adds exactly 1 bar
		} else if (mode == 2) {  // Fixed nnts, floating nbars
			if (rp.size() == nnts_in) { // Success
				return std::optional<std::vector<nv_t>> {rp};
			} else if (rp.size() > nnts_in) { // Overshot
				rp.clear();	nbar_rp = bar_t{0};
			} else if (rp.size() < nnts_in) {
				// Not overshot, but not done yet
			}
		} else if (mode == 3) { // Fixed nbars, fixed nnotes
			if (nbar_rp == nbr_in && rp.size() == nnts_in) { // Success
				return std::optional<std::vector<nv_t>> {rp};
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

std::optional<std::vector<nv_t>> rand_rp(ts_t ts_in, std::vector<nv_t> dp_in,
	std::vector<double> pd_in, int nnts_in, bar_t nbr_in) {
	rand_rp_opts opts {std::chrono::seconds {3}};
	return rand_rp(ts_in, dp_in, pd_in, nnts_in, nbr_in, opts);
}


randrp_uih::randrp_uih() {
	//...
}
randrp_uih::randrp_uih(ts_uih const& ts_uih_in, std::vector<nv_uih> const& nv_uih_set_in, 
	std::vector<double> const& pd_in, int const& n_nts_in, bar_t const& nbars_in) {
	//...
}

void randrp_uih::update(ts_uih const& ts_uih_in, std::vector<nv_uih> const& nv_uih_set_in, 
	std::vector<double> const& pd_in, int const& n_nts_in, bar_t const& nbars_in) {
	if (last_.ts == ts_uih_in && last_.nvset == nv_uih_set_in && 
		last_.n_bars == nbars_in && last_.n_nts == n_nts_in &&
		last_.pd == pd_in) {
		return;
	}
	last_ = {ts_uih_in,nv_uih_set_in, pd_in, n_nts_in, nbars_in};

	// Validity of the indivdual components...
	bool parts_valid = ts_uih_in.is_valid();
	for (auto e : nv_uih_set_in) {
		parts_valid = parts_valid && e.is_valid();
	}
	// Add tests on the rest of the components as soon as the helper classes
	// are implemented...
	if (!parts_valid) {
		msg_ += "rand_rp() requires a valid ts, nv-pd set, and a specification of ";
		msg_ += "one or	both of n_nts, n_bars";
		is_valid_ = false;
		return;
		// Any invalid members invalidate the object.  An empty value => invalid 
		// and should not be confused with 0.  Empty == 0 is a convention that 
		// should be established in the helper for the indivdual component.  
	}
	
	// get() on all members of rrp_uih_in is safe

	// Interaction rules
	if (nbars_in == bar_t{0} && n_nts_in == 0) {
		msg_ += "One or both of n_bars, n_nts must != 0";
		is_valid_ = false;
		flags_ = 1;
		return;
	}

	is_valid_ = true;
	msg_ = "";
	flags_ = 0;

	std::vector<nv_t> nvset_in {};
	for (auto e : nv_uih_set_in) {
		nvset_in.push_back(*(e.get()));
	}
	randrp_input_ = randrp_input {ts_uih_in.get(), nvset_in, pd_in, 
		n_nts_in, nbars_in};
}

bool randrp_uih::is_valid() const {
	return is_valid_;
}

int randrp_uih::flags() const {
	return flags_;
}

randrp_input randrp_uih::get() const {
	au_assert(is_valid_,"Called randrp_uih::get() on a randrp_uih object with is_valid_==false");
	return *randrp_input_;
}



