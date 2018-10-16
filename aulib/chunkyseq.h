#pragma once 
#include <vector>
#include <string>
#include "util/au_util.h";

//
// chunkyseq<T>
// Kind of like a std::vector<std::vector<T>> except that the elements are 
// contiguous.  
//

//
// TODO:  Instead of start,end, start,n
//  - one less operation when inserting/deleting
//
// TODO:  m_idx should hold vector iterator types not size_t
//
// TODO:  ctor
//
template<typename T>
class chunkyseq {
public:
	// The number of chunks
	size_t size() const {
		return m_idx.size();
	};

	void insert(size_t i, T d) {
		m_d.insert(m_d.begin()+m_idx[i].start,1,d);
		m_idx.insert(m_idx.begin()+i,1,{m_idx[i].start, 1});
		for (size_t j=i; j<m_idx.size(); ++j) {
			m_idx[j].start += 1;
		}
	};

	void erase(size_t i) {
		size_t n = m_idx[i].n;
		m_d.erase(m_idx[i].start,m_idx[i].start+m_idx[i].n);
		m_idx.erase(i);
		for (size_t j=i; j<m_idx.size(); ++j) {
			m_idx[j].start -= n;
		}
	};

	void push_back(T d) {
		m_idx.push_back({m_d.size(), 1});
		m_d.push_back(d);
	};

	void push_back(std::vector<T> d) {
		m_idx.push_back({m_d.size(),d.size()});
		for (size_t i=0; i<d.size(); ++i) {
			m_d.push_back(d[i]);
		}
	};

	// Get single chunk i
	std::vector<T> get(size_t i) const {
		std::vector<T> res(m_idx[i].n, T {});
		for (size_t j=0; j<m_idx[i].n; ++j) {
			res[j] = m_d[j+m_idx[i].start];
		}
		return res;
	};
	// Get a range of chunks [from,to)
	chunkyseq<T> get(size_t idx_from, size_t idx_to) const {
		std::vector<T> res_d(m_d.begin()+m_idx[idx_from].start,
			m_d.begin()+m_idx[idx_to].start);

		std::vector<T> res_idx(m_idx.begin()+idx_from,m_idx.begin()+idx_to);
		for (size_t i=0; i<res_idx.size(); ++i) {
			res_idx[i].start -= m_idx[idx_from].start;
		}
		chunkyseq<T> res {};
		res.m_d = res_d; res.m_idx = res_idx;
		return res;
	};

	std::vector<T> get_flat() const {
		return m_d;
	};

	std::string print() const {
		std::string s {};

		// Print as a linear sequence
		/*for (size_t i=0; i<m_idx.size(); ++i) { s += "[";
			for (size_t j=m_idx[i].start; j<m_idx[i].end; ++j) {
				s += bsprintf("%2.1f",m_d[j]);
				if ((j+1)<m_idx[i].end) { s += ","; }
			}
			s += "]";
			if ((i+1)<m_idx.size()) { s += ", "; }
		}
		return s;*/

		// Print vertically as a list w/ idxs, etc
		for (size_t i=0; i<m_idx.size(); ++i) {
			auto eidx = m_idx[i].n + m_idx[i].start;
			s += bsprintf("chunk %.3d: [%d,%d) %d elements: [",
				i,m_idx[i].start,eidx,m_idx[i].n);
			for (size_t j=0; j<m_idx[i].n; ++j) {
				s += bsprintf("%2.1f", m_d[j+m_idx[i].start]);
				if ((j+1)<m_idx[i].n) { s += ","; }
			}
			s += "]\n";
		} // to next chunk i
		return s;
	}

	bool operator==(const chunkyseq<T>& rhs) const {
		if (m_d.size() != rhs.m_d.size() ||
			m_idx.size() || rhs.m_idx.size()) {
			return false;
		}
		for (size_t i=0; i<m_idx.size(); ++i) {
			if (m_idx[i] != rhs.m_idx[i]) { return false; }

			for (size_t j=0; j<m_idx[i].n; ++j) {
				if (m_d[j+m_idx[i].start] != rhs.m_d[j+m_idx[i].start]) {
					return false;
				}
			}
		}
		return true;
	}

	bool validate() const {
		// m_idx.front().start == 0, m_idx.back().end == m_d.size(), and
		// each element i in m_idx follows element i-1 with no gaps and
		// no overlaps.  
		for (int i=0; i<m_idx.size(); ++i) {
			if (m_idx[i].start < i) { return false; }
			if (m_idx[i].n <= 0) { return false; }
			if (i>0) {
				if (m_idx[i].start != (m_idx[i-1].start+m_idx[i-1].n)) {
					return false;
				}
			}
		}
		if (m_idx.size() >= 1 && m_idx[0].start != 0) {
			return false;
		}
		if ((m_idx.back().start + m_idx.back().n) != m_d.size()) {
			return false;
		}

		return true;
	}
		
private:
	struct idx {
		size_t start {0};
		size_t n {0};
	};
	std::vector<T> m_d {};
	std::vector<idx> m_idx {};
};

template<typename T>
bool operator!=(const chunkyseq<T>& lhs, const chunkyseq<T>& rhs) {
	return !(lhs==rhs);
}
