#pragma once
#include "de_t.h"
#include "chord_t.h"
#include "rp_t.h"
#include "ts_t.h"
#include <vector>
#include <string>

//
// line_t<T>
//
// Represents a sequence of musical elements each of which has duration.  
// A "musical element" w/ an associated duration is a de_t<T>, which may be 
// either a single note of type T (scd_t, frq_t, ...), a rest, or a chord 
// chord_t<T>.  
//
// Associates with this sequence a ts_t.  
//
// There are two possible designs for line_t:
// 1)  Hold a std::vector T where T is a note-type.  Also hold additional 
//     datastructures to dynamically indicate groups of T's corresponding to
//     chords, rests, etc.  (Alternatively, perhaps, hold several std::vector<T>'s:
//     T1=note-type, T2=chord_t<T1>, T3=rest_t).  IOW, hold note-types _directly_
//     and hold their durations separately.  Dynamically re-associate as needed.  
//     Insertion/deletion etc is very complex.  See, for example, rp_t.  
// 2)  Hold a std::vector<de_t<T>>.  
//
//
//


template<typename T>
class line_t {
public:
	line_t()=default;

	explicit line_t(ts_t ts, std::vector<de_t<T>> des) {
		std::vector<d_t> d {};

		for (int i=0; i<des.size(); ++i) {
			d.push_back(des[i].dv());

			if (des[i].isrest()) { 
				m_eidxs.push_back({m_eidxs.size()+i,m_eidxs.size()+i,false,true});
				continue;
			}

			int j=0;
			for (true; j<des[i].n(); ++j) {
				m_e.push_back(des[i][j]);
			}
			m_eidxs.push_back({m_e.size()-j,m_e.size(),true,false});
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
			m_e.push_back(e);
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
	void push_back(de_t<rest_t> de) {  // Lest i not solve this problem in de_t...
		m_rp.push_back(de.dv());
		m_e.push_back(de[0]);
	};
	void push_back(std::vector<de_t<T>> des) {
		for (auto e : des) {
			for (int i=0; i<e.num_notes(); ++i) {
				m_e.push_back(e[i]);
			}
			m_rp.push_back(e.dv());
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
	struct idxs {
		size_t begin {0};
		size_t end {0};
		bool ischord {false};
		bool isrest {false};
	};
	// std::vector<de_t<T>> m_line {};
	// If i simply use this to store a vector of de_t<T>, i will have to
	// duplicate any rp_t functionality.  
	// Also, users can make vector's of de_t<T>'s themselves; they don't
	// need some opaque object.  The only point of the opaque object is to
	// make it easier to obtain some functionality... in this case i think
	// it involves stripping the input.  
	rp_t m_rp;
	//std::vector<std::vector<T>> m_e;  // "elements"
	std::vector<T> m_e {};
	std::vector<idxs> m_eidxs {};
};




