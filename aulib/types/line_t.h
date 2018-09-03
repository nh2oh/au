#pragma once
#include "de_t.h"
#include "rp_t.h"
#include "ts_t.h"
#include <vector>
#include <string>

//
// Represents a sequence of musical elements each of which has duration,
// ie.  Behaves as, but is not, a sequence of de_t's.  
//
//
//
//
//
//
//
//
//
//
//
//


template<typename T>
class line_t {
public:
	line_t()=default;
	line_t(ts_t ts) {
		// Have to call w/ explicit template argument, ie:
		// line_t<scd_t> my_line_var {my_ts_var};
		m_rp = rp_t{ts};
	};

	explicit line_t(ts_t ts, std::vector<de_t<T>> des) {
		// At the time i write this, rp_t is less effecient w/a bunch of
		// calls to push_back than it is being created in one shot from a
		// std::vector<nv_t>.  
		std::vector<d_t> d {};
		for (auto e : des) {
			d.push_back(e.dv());
			m_e.push_back(e.nts());
		}
		m_rp = rp_t {ts,d};
	};

	explicit line_t(rp_t rp, std::vector<T> nts) {
		// Impossible for arg2 to represent rests, but if the user wanted to
		// repreesent rests, he wouldn't have been working w/ a std::vector<T>,
		// he'd have made a std::vector<de_t<T>>.  
		if (rp.nelems() != nts.size()) { return; };
		m_rp = rp;
		for (auto e : nts) {
			m_e.push_back(std::vector<T>{e});
		}
	};

	void push_back(T nt, d_t nv) {
		m_e.push_back(nt);
		m_rp.push_back(nv);
	};

	void push_back(de_t<T> de) {
		m_rp.push_back(de.dv());
		m_e.push_back(de.nts());
	};
	void push_back(de_t<rest_dummy_t> de) {  // Lest i not solve this problem in de_t...
		m_rp.push_back(de.dv());
		m_e.push_back(de.nts());
	};
	void push_back(std::vector<de_t<T>> des) {
		for (auto e : des) {
			m_rp.push_back(e.dv());
			m_e.push_back(e.nts());
		}
	};

	// Getters
	bar_t nbars() const { return m_rp.nbars(); };
	beat_t nbeats() const { return m_rp.nbeats(); };
	size_t nelems() const { return m_rp.nelems(); };
	std::string print() {
		return std::string{"line_t printing function"};
	};
private:
	// std::vector<de_t<T>> m_line {};
	// If i simply use this to store a vector of de_t<T>, i will have to
	// duplicate any rp_t functionality.  
	// Also, users can make vector's of de_t<T>'s themselves; they don't
	// need some opaque object.  The only point of the opaque object is to
	// make it easier to obtain some functionality... in this case i think
	// it involves stripping the input.  
	rp_t m_rp;
	std::vector<std::vector<T>> m_e;  // "elements"
};




