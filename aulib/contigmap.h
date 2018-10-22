#pragma once
#include <vector>
#include <algorithm>
#include <exception> // std::terminate()
#include <string>

//
// contigmap<T_key,T_val>
// A map between a set of unique elements of type T_key and elements of type 
// T_val implemented in such a way that the {key,value} elements are stored 
// contiguously in memory.  
//
// contigmap[T_key k] returns a ref to the value associated w/ key k.  
//   If k is not a member of the set, it is added w/ a corresponding 
//   value of T_val {}.  
//
//
//



// Used as the return value of operator[] and as an argument to insert
template<typename T_key, typename T_val>
struct kvpair {
	T_key k {};
	T_val v {};
};


template<typename T_key, typename T_val>
class contigmap {
public:
	using kvpair_t = typename kvpair<T_key,T_val>;
	using iterator_t = typename std::vector<kvpair_t>::iterator;

	iterator_t insert(const kvpair_t& kv) {
		auto i = findkey(kv.k);
		if (i==m_kv.end()) {
			m_kv.push_back(kv);
			i = m_kv.back();
		}
		return i;
	};

	bool erase(const T_key& k) {
		auto i = findkey(k);
		if (i==m_kv.end()) { return false; }

		m_kv.erase(i);
		return true;
	};

	T_val& operator[](const T_key& k) {
		auto i = findkey(k);
		if (i==m_kv.end()) {
			m_kv.push_back(kvpair_t{k, T_val{}});
			i = m_kv.back();
		}
		return (*i).v;
	};

	iterator_t begin() {
		return m_kv.begin();
	};
	iterator_t end() {
		return m_kv.end();
	};

	bool ismember(const T_key& k) const {
		return (findkey(k)!=m_kv.end());
	};

	std::string print() {
		std::string s {};
		for (size_t i=0; i<m_k.size(); ++i) {
			s += "this["; s += std::to_string(m_k[i]); s += "] = ";
			s += std::to_string(m_v[i]);
			s += "\n";
		}
		return s;
	};

private:
	iterator_t findkey (const T_key& k) const {
		return std::find_if(m_kv.begin(), m_kv.end(),
			[&](const kvpair_t& e){return e.k==k;});
	};

	std::vector<kvpair_t> m_kv {};
};


