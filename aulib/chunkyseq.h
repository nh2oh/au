#pragma once 
#include <vector>

template<typename T>
class chunkyseq {
public:
	size_t size() const {
		return m_idx.size();
	};

	void insert (T d, size_t i) {
		std::vector<T> post(m_idx.back().end-m_idx[i].start, T{});
		for (size_t j=m_idx[i].start; j<m_idx.back().end; ++j) {
			post[j-m_idx[i].start] = m_d[j];
		}
	};

	void push_back(T d) {
		m_idx.push_back({m_d.size(), m_d.size()+1);
		m_d.push_back(d[i]);
	};

	void push_back(std::vector<T> d) {
		m_idx.push_back({m_d.size(),m_d.size()+d.size());
		for (size_t i=0; i<d.size(); ++i) {
			m_d.push_back(d[i]);
		}
	};

	std::vector<T> get(size_t i) const {
		std::vector<T> res(m_idx[i].end-m_idx[i].start,T {});
		for (size_t j=m_idx[i].start; j<m_idx[i].end; ++j) {
			res[j-m_idx[i].start] = m_d[j];
		}
		return res;
	};
private:
	struct idx {
		size_t start {0};
		size_t end {0};
	};
	std::vector<T> m_d {};
	std::vector<idx> m_idx {};
};







template<typename T>
class chunk {
public:
	int size() const {
		return m_d.size();
	};

	int push_back(T);

	T *operator[](int i) {
		return m_d[i];
	};
private:
	std::vector<T> m_d {};
}







