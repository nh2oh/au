#include "rp_t.h"
#include "nv_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "..\util\au_util.h"  // bsprintf()
#include <string>
#include <vector>
#include <cmath> // pow()
#include <algorithm> // find_if(), copy()
#include <chrono>
#include <map>
#include <exception>



rp_t::rp_t(ts_t const& ts) {
	m_ts = ts;
}

rp_t::rp_t(ts_t const& ts, std::vector<d_t> const& nv) {
	m_ts = ts;
	for (auto e : nv) {
		push_back(e);
	}
}

// The reverse of member function dt(...)
// Each element of dt is interpreted as the nearest integer multiple of res
// and the d_t closest to this dt is chosen.  
rp_t::rp_t(ts_t const& ts_in, std::vector<std::chrono::milliseconds> const& dt,
	tempo_t const& tempo, std::chrono::milliseconds const& res) {

	m_ts = ts_in;

	std::map<beat_t,d_t> nvset {};
	for (int m=-3; m<8; ++m) { // -3 => 8 (qw) 5 => 1/32
		for (int n=0; n<5; ++n) {
			auto curr_nv = d_t{d_t::mn{m,n}};
			nvset[nbeat(m_ts,curr_nv)]=curr_nv;
		}
	}

	// The t difference between each dt element and the dt of the nv_t
	// chosen to represent said element.  
	std::vector<std::chrono::milliseconds> error {};

	decltype(res/res) one = {1};
	for (auto e : dt) {
		// Find the integer multiple r of res closest to e:
		// e/res is essentially integer division, so r*res will always 
		// be <= e, and (r+1)*res will always be > e.  If e < res, e/res
		// = r == 0.  
		auto r = e/res;	
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);
		// I do not want to skip events < res since this will change the
		// size of the vectors (or i'll have to have 0-duration elements...
		// which don't exist...)
		if (r == 0) { r = one; }

		auto curr_nbts = (r*res)*tempo;

		// Find the element in nvset for which nbeats is closest to curr_nbts
		// TODO:  Generalize to find_closest()
		beat_t nbts_best_nv {};
		d_t best_nv {};
		auto nvset_firstgt = std::find_if(nvset.begin(),nvset.end(),
			[&](std::pair<beat_t,d_t> const& in){return(in.first > curr_nbts);});
		if (nvset_firstgt == nvset.begin()) {
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else if (nvset_firstgt == nvset.end()) {
			--nvset_firstgt;
			best_nv = (*nvset_firstgt).second;
			nbts_best_nv = (*nvset_firstgt).first;
		} else {
			auto first_gt = *nvset_firstgt;  // closest > curr_nbts
			--nvset_firstgt;
			auto first_lteq = *nvset_firstgt;  // closest <= curr_nbts
			if ((first_gt.first-curr_nbts) < (curr_nbts-first_lteq.first)) {
				best_nv = first_gt.second;
				nbts_best_nv = first_gt.first;
			} else {
				best_nv = first_lteq.second;
				nbts_best_nv = first_lteq.first;
			}
		}

		push_back(best_nv);
		//m_tot_nbeats += nbts_best_nv;

		// Error measure
		auto x= nbts_best_nv/tempo;
		error.push_back(x - e);
		
	}
	//m_tot_nbars = nbar(m_ts,m_tot_nbeats);
	//wait();

}

void rp_t::push_back(d_t d) {
	//auto fremain = m_nbars.fremain();
	//auto d_to_next_bar = (m_nbars.fremain())*(m_ts.bar_unit());

	int n_full_bars = std::floor(m_nbeats/m_ts.beats_per_bar());
	auto bts_till_next_bar = (n_full_bars+1)*m_ts.beats_per_bar() - m_nbeats;
	auto d_to_next_bar = duration(m_ts,bts_till_next_bar);

	if (d_to_next_bar.weird()) {
		//wait();
	}

	auto d_singlets = d.to_singlets_partition_max(d_to_next_bar,m_ts.bar_unit());
	for (size_t i=0; i<d_singlets.size(); ++i) {
		m_e.push_back(vgroup{d_singlets[i],m_usridx,d_singlets.size()-1-i,i});
	}
	// also need to append rests
	++m_usridx;
	m_dtot += d;
	m_nbars += nbar(m_ts,d);
	m_nbeats += nbeat(m_ts,d);
	//wait();
}

std::string rp_t::print() const {
	std::string s {};
	bar_t curr_bar {0};
	for (auto e : m_e) {
		if (e.tie_b > 0) { s += ")"; }
		s += e.e.print();
		if (e.tie_f > 0) { s += "("; }

		curr_bar += nbar(m_ts,e.e);
		if (curr_bar.isexact()) {
			s += " | ";
		} else {
			s += " ";
		}
	}
	return s;
}

bar_t rp_t::nbars() const {
	return bar_t{m_nbars};
}
beat_t rp_t::nbeats() const {
	return beat_t{m_nbeats};
}
size_t rp_t::nelems() const {
	return m_usridx;
}
ts_t rp_t::ts() const {
	return m_ts;
}

std::vector<d_t> rp_t::to_duration_seq() const {
	// TODO:  Possibly there is some overlap here w/ operator[]
	std::vector<d_t> result {};
	int curr_usridx = 0;
	d_t curr_sum {};
	for (auto const& e : m_e) {
		if (e.usrix > curr_usridx) {
			++curr_usridx;
			result.push_back(curr_sum);
			curr_sum = d_t {};
		}
		curr_sum += e.e;
	}
	result.push_back(curr_sum);

	return result;
}

d_t rp_t::operator[](int i) const {
	if (i>m_usridx) { std::abort(); }  // out of range
	
	std::vector<rp_t::vgroup> e_i {};
	auto junk = std::copy_if(m_e.begin(),m_e.end(),std::back_inserter(e_i),
		[&](const vgroup& curr_e){return curr_e.usrix==i; });

	d_t sum {};
	for (auto const& e : e_i) {
		sum += e.e;
	}

	return sum;
}

rp_t::rp_element rp_t::operator[](const d_t& pos) const {
	if (pos>m_dtot) { std::abort(); }  // out of range
	
	rp_t::rp_element result {};

	d_t curr_pos {d::z};
	int idx {0};
	for (int i=0; i<m_e.size(); ++i) {
		curr_pos += m_e[i].e;
		if (curr_pos == pos) {
			idx = i;
			break;
		} else if (curr_pos > pos) {
			idx = std::max(0,i-1);  // TODO:  Danger: signed-unsigned subtraction
		}
	}

	int idx_tie_start = idx - m_e[idx].tie_b;
	d_t dist_tie_back {d::z};
	for (int i=idx_tie_start; i<idx; ++i) {
		dist_tie_back += m_e[i].e;
	}
	int idx_tie_end = idx + m_e[idx].tie_f;
	d_t dist_tie_fwd {d::z};
	for (int i=idx; i<idx_tie_end; ++i) {
		dist_tie_fwd += m_e[i].e;
	}

	result.tieback = dist_tie_back;
	result.tiefwd = dist_tie_fwd;
	if (m_e[idx].e == pos) { 
		result.e = m_e[idx].e;
	} else {
		result.e = d_t {d::z};
	}

	return result;
}












rp2_t::rp2_t(const ts_t& ts) {
	ts_ = ts;
}

rp2_t::rp2_t(const ts_t& ts, const beat_t& start) {
	ts_ = ts;
	start_ = start;
}

rp2_t::rp2_t(const ts_t& ts, const std::vector<d_t>& nv) {
	ts_ = ts;
	for (auto e : nv) {
		this->push_back(e);
	}
}

void rp2_t::push_back(d_t d) {
	// Note that start_ only matters if inserting into an empty container since after the first 
	// insertion its effect will be reflected in the cumulative beat-onset field of each rp_ 
	// element.  
	// A consequence of this is that nbars() nbeats() etc can return + or - results even if rp_ 
	// is empty.  
	beat_t bt_onset = rp_.size() == 0 ? start_ : rp_.back().on + nbeat(ts_,d);
	rp_.push_back({d, bt_onset});
}


bar_t rp2_t::nbars() const {
	if (rp_.size() == 0) {
		return nbar(ts_,start_);
	}
	return nbar(ts_,rp_.back().on) + nbar(ts_,rp_.back().e);
}
beat_t rp2_t::nbeats() const {
	if (rp_.size() == 0) {
		return start_;
	}
	return rp_.back().on + nbeat(ts_,rp_.back().e);
}
int rp2_t::nevents() const {
	return rp_.size();
}
ts_t rp2_t::ts() const {
	return ts_;
}

std::vector<d_t> rp2_t::to_duration_seq() const {
	std::vector<d_t> result {};  result.reserve(rp_.size());
	for (const auto& e : rp_) {
		result.push_back(e.e);
	}
	return result;
}


std::string rp2_t::print() const {
	std::string s {};

	for (const auto& e : rp_) {
		int n_full_bars = std::floor(e.on/ts_.beats_per_bar());
		beat_t bts_till_next_bar = (n_full_bars+1)*ts_.beats_per_bar() - e.on;
		d_t d_to_next_bar = duration(ts_,bts_till_next_bar);

		auto d_singlets = (e.e).to_singlets_partition_max(d_to_next_bar,ts_.bar_unit());
		for (size_t i=0; i<d_singlets.size(); ++i) {
			if (i > 0) { s += ')'; }
			s += d_singlets[i].print();
			if ((d_singlets.size()-1-i) > 0) { s += '('; }
		}

		bar_t curr_bar = nbar(ts_,e.on);
		if (curr_bar.isexact()) {
			s += " | ";
		} else {
			s += " ";
		}
	}

	return s;
}


