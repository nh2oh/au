#pragma once
#include <vector>
#include <algorithm>
#include <exception> // std::terminate()
#include <string>

//
// contigmap<T_key,T_val>
// A "contiguous map" between a set of unique elements of type T_key
// and elements of type T_val.  
//
// contigmap[T_key k] returns a ref to the value associated w/ key k.  
//   If k is not a member of the set, it is added w/ a corresponding 
//   value of T_val {}.  
//
//
//




template<typename T_key, typename T_val>
struct kvpair {
	T_key k {};
	T_val v {};
};

template<typename T_key, typename T_val>
class contigmap_iterator {
public:
	using kvpair_t = typename kvpair<T_key,T_val>;

	typename std::vector<T_key>::iterator m_keyit {};
	typename std::vector<T_val>::iterator m_valit {};

	kvpair_t operator*() { return kvpair_t{*m_keyit, *m_valit}; };
	contigmap_iterator& operator++() {  // preincrement
		m_keyit++; m_valit++; 
		return *this;
	};
	contigmap_iterator operator++(int) {  // postincrement
		m_keyit++; m_valit++;
		return this;
	};

	bool operator!=(contigmap_iterator& rhs) {
		return (m_keyit != rhs.m_keyit || m_valit != rhs.m_valit);
	};
};

template<typename T_key, typename T_val>
class contigmap {
public:
	using kvpair_t = typename kvpair<T_key,T_val>;
	using iterator_t = typename contigmap_iterator<T_key,T_val>;

	void insert(kvpair_t kv) {
		auto i = val_it(kv.k);
		if (i==m_v.end()) {
			m_k.push_back(kv.k);
			m_v.push_back(kv.v);
		} else {
			*i = kv.k;
		}
	};

	bool erase(const T_key& k) {
		auto i = val_it(k);
		if (i==m_v.end()) {
			return false;
		} else {
			m_k.erase((i-m_v.begin())+m_k.begin());
			m_v.erase(i);
		}
		return true;
	};

	T_val& operator[](const T_key& k) {
		auto i = val_it(k);
		if (i==m_v.end()) {
			// std::terminate();
			insert(kvpair_t {k, T_val {}});
			return this->operator[](k);
		}
		return m_v[i-m_v.begin()];
	};

	iterator_t begin() {
		return iterator_t {m_k.begin(),m_v.begin()};
	}
	iterator_t end() {
		return iterator_t {m_k.end(),m_v.end()};
	}

	bool ismember(const T_key& k) const {
		return (val_it(k)!=m_v.end());
	};

	std::string print() {
		std::string s {};
		for (size_t i=0; i<m_k.size(); ++i) {
			s += "this[";
			s += std::to_string(m_k[i]);
			s += "] = ";
			s += std::to_string(m_v[i]);
			s += "\n";
		}
		return s;
	};

private:
	typename std::vector<T_val>::iterator val_it(T_key k) {
		auto i = std::find(m_k.begin(),m_k.end(),k);
		return (i-m_k.begin())+m_v.begin();
	};

	std::vector<T_key> m_k {};
	std::vector<T_val> m_v {};

};


