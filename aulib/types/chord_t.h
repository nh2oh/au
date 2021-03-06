#pragma once
#include "dbklib\algs.h"
#include <vector>
#include <string>
#include <algorithm>

//
// chord_t<T>
//
// Represents a chord: a vertical note-group
// 1)  All members are unique
//     => A chord_t represents a chord sounded by a single voice and not an "effective" chord
//     resulting from many voices sounding together.  
//     => What of insturments where the same note can be sounded > once simultaneously?  Ex, tune
//     two strings of a guitar to the saem frq.  
// 2)  There is no associated duration
// 
// - What methods does a chord need that other containers-of-notes do not?
// - What level of genericity is needed to implement those methods for _all_ T?
//
// Methods unique to chords:
// - Special printing format: <...>
// - Naming, but not possible unless associated w/ scale, diatonic key
// - Identification of bass, high nt (not nec. as usefull for a hosizontal nt group; also
//   maybe not possible for scales w/ crazy scd ordering)
// - Iterator from low->high instead of t=0 -> t=end, as for an h-line
// - Equality independent of note order (ie, notes are always sorted)
// - Interval calculation for all pairs of intervals
// - Invert method
//
//
//
//
// (1) => that an == operator exists for note-type T.  For scd_t, ntl_t,
// ntstr_t, and frq_t, equality is defined independent of scale, despite the
// fact that a scale may map different note-type elements to the same frq.  
// Since in au the "scale" concept is so abstract and unconstrained it's 
// difficult to incorporate scale-dependent behavior into static types...
//
// Since all the "interesting" problems related to chords such as naming,
// computing manor/minor "quality," finding the bass note etc depend on the
// scale, a chord_t is little mpore than a container for a set of unique 
// note elements.  More advanced functionality has to be implemented as
// external functions taking a scale argument.  Hence the notes() getter.  
//
// A possible alternative design associates a particular scale w/ a chord, ie,
// chord_t<scale-type,note-type> (actually such a design probably makes the 
// note-type unnecessary since i can just hold scd_t and use the scale to 
// convert as necessary).  I reject this since it loses the scale-agnosticism
// that i currently have w/ note-types.  
//
//

template<typename T>
class chord_t {
public:
	explicit chord_t() =default;
	explicit chord_t(std::vector<T> vnts) {
		m_nts = unique_nosort(vnts);
	};

	bool insert(T new_nt) {
		if (!ismember(new_nt,m_nts)) {
			m_nts.push_back(new_nt);
			return true;
		}
		return false;
	};

	bool erase(T drop_nt) {
		auto p_drop_nt = std::find(m_nts.begin(),m_nts.end(),drop_nt);
		if (p_drop_nt != m_nts.end()) {
			m_nts.erase(p_drop_nt);
			return true;
		}
		return false;
	};

	std::string print(const std::string& sep=",") const {
		std::string s {"<"};
		for (int i=0; i<m_nts.size(); ++i) {
			s += m_nts[i].print();
			if (i<m_nts.size()-1) {
				s += sep;
			}
		}
		s += ">";

		return s;
	};

	std::vector<T> notes() const {
		return m_nts;
	};

	int n() const {
		return m_nts.size();
	};

	bool operator==(const chord_t& rhs) const {
		return m_nts == rhs.m_nts;
	};
	bool operator!=(const chord_t& rhs) const {
		return !(m_nts == rhs.m_nts);
	};

	T operator[](size_t idx) const {
		return m_nts[idx];
	};

private:
	std::vector<T> m_nts {};
};

