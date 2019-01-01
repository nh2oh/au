#include "rp_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include <string>
#include <vector>
#include <algorithm> // find_if(), copy()
#include <chrono>
#include <exception>
#include <iostream>


rp_t::rp_t(const ts_t& ts) {
	ts_ = ts;
}

rp_t::rp_t(const ts_t& ts, const beat_t& start) {
	ts_ = ts;
	start_ = start;
}

rp_t::rp_t(const ts_t& ts, const std::vector<d_t>& nv) {
	ts_ = ts;
	for (auto e : nv) {
		this->push_back(e);
	}
}

// TODO:  This is horrible; generalize the internals to some sort of find_closest()
// Each element of dt is interpreted as the nearest integer multiple of res
// and the d_t closest to this dt is chosen.  
rp_t::rp_t(const ts_t& ts_in, const std::vector<std::chrono::milliseconds>& dt, 
	const tempo_t& tempo, const std::chrono::milliseconds& res) {
	ts_ = ts_in;

	struct beat_duration {
		d_t d {d::z};
		beat_t bt {0};
	};
	std::vector<beat_duration> nvset {};
	for (int m=-3; m<8; ++m) { // -3 => 8 (qw) 5 => 1/32
		for (int n=0; n<5; ++n) {
			d_t curr_nv {d_t::mn{m,n}};
			nvset.push_back({curr_nv, nbeat(ts_,curr_nv)});
		}
	}

	// The t difference between each dt element and that of the chosen nv_t.
	std::vector<std::chrono::milliseconds> error {};  error.reserve(dt.size());

	for (const auto& e : dt) {
		// Find the integer multiple r of res closest to e:
		// e/res is integer division, so r*res will always be <= e, and (r+1)*res 
		// will always be > e.  If e < res, e/res = r == 0.  
		auto r = e/res;	
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);
		// I do not want to skip events < res since this will change the size of the vectors (or 
		// i'll need to deal with 0-duration elements)
		if (r == 0) { r = 1; }

		auto curr_nbts = (r*res)*tempo;

		// Find the element in nvset for which nbeats is closest to curr_nbts
		auto nvset_firstgt = std::find_if(nvset.begin(),nvset.end(),
			[curr_nbts](const beat_duration& in){ return(in.bt > curr_nbts); });
		if (nvset_firstgt == nvset.end()) {
			--nvset_firstgt;
		} else if (nvset_firstgt != nvset.begin()) {
			if (((*nvset_firstgt).bt-curr_nbts) >= (curr_nbts-(*(nvset_firstgt-1)).bt)) {
				--nvset_firstgt;
			}
		}
		this->push_back((*nvset_firstgt).d);

		// Error measure
		auto x = ((*nvset_firstgt).bt)/tempo;
		error.push_back(x - e);
	}
}


void rp_t::push_back(d_t d) {
	// Note that start_ only matters if inserting into an empty container since after the first 
	// insertion its effect will be reflected in the cumulative beat-onset field of each rp_ 
	// element.  
	// A consequence of this is that nbars() nbeats() etc can return + or - results even if rp_ 
	// is empty.  
	beat_t bt_onset = rp_.size() == 0 ? start_ : rp_.back().on + nbeat(ts_,rp_.back().e);
	rp_.push_back({d, bt_onset});
}


bar_t rp_t::nbars() const {
	if (rp_.size() == 0) {
		return nbar(ts_,start_);
	}
	return nbar(ts_,rp_.back().on) + nbar(ts_,rp_.back().e);
}
beat_t rp_t::nbeats() const {
	if (rp_.size() == 0) {
		return start_;
	}
	return rp_.back().on + nbeat(ts_,rp_.back().e);
}
int rp_t::nevents() const {
	return rp_.size();
}
ts_t rp_t::ts() const {
	return ts_;
}

std::vector<d_t> rp_t::to_duration_seq() const {
	std::vector<d_t> result {};  result.reserve(rp_.size());
	for (const auto& e : rp_) {
		result.push_back(e.e);
	}
	return result;
}


std::string rp_t::print() const {
	std::string s {};

	for (const auto& e : rp_) {
		int n_full_bars = std::floor(e.on/ts_.beats_per_bar());
		beat_t bts_till_next_bar = (n_full_bars+1)*ts_.beats_per_bar() - e.on;
		d_t d_to_next_bar = duration(ts_,bts_till_next_bar);
		
		bar_t curr_bar = nbar(ts_,e.on);
		auto d_singlets = (e.e).to_singlets_partition_max(d_to_next_bar,ts_.bar_unit());
		for (size_t i=0; i<d_singlets.size(); ++i) {
			
			if (curr_bar.isexact()) {
				s += " | ";
			} else {
				s += " ";
			}

			if (i > 0) { s += ')'; }
			s += d_singlets[i].print();
			if ((d_singlets.size()-1-i) > 0) { s += '('; }

			curr_bar += nbar(ts_,d_singlets[i]);
		}

	}

	return s;
}


// Returns the d_t element corresponding to sounded event i
rp_t::rp_element_t rp_t::operator[](int i) const {
	if (i > rp_.size() || i < 0) {
		std::cout << "rp_t::operator[](int i) const : i out of range.\n";
		std::abort();
	}

	return rp_t::rp_element_t {rp_[i].e,rp_[i].on};
}

rp_t::rp_element_t rp_t::operator[](const d_t& pos) const {
	beat_t nbt = nbeat(ts_,pos);
	return this->operator[](nbt);
}
rp_t::rp_element_t rp_t::operator[](const bar_t& pos) const {
	beat_t nbt = nbeat(ts_,pos);
	return this->operator[](nbt);
}
rp_t::rp_element_t rp_t::operator[](const beat_t& pos) const {
	if (rp_.size() == 0 
		|| pos < rp_.front().on 
		|| (pos >= rp_.back().on+nbeat(ts_,rp_.back().e))) { 
		// Caller requested a beat subsequent to the span of the final rp element, or possibly prior
		// to the initial element.  
		std::cout << "rp_t::operator[](const beat_t& pos) const : " 
			<< "Out of range.\n ";
		std::abort();
	}

	for (int i=0; i<rp_.size(); ++i) {
		if (pos >= rp_[i].on && (pos < rp_[i].on+nbeat(ts_,rp_[i].e))) {
			return rp_[i];
		}
	}

	std::cout << "rp_t::operator[](const beat_t& pos) const : " 
		<< "This should never happen.\n ";
	std::abort();
}


