#pragma once
#include "ntl_t.h"  // Defines note_t


template<typename SC>
class scale_const_iterator {
public:
	using difference_type = typename int;

	// Note these ctors are public... in general scale scd3_t's need not provide
	// public ctors...
	explicit scale_const_iterator<SC>(const SC *scale) {
		m_scd = 0;
		m_sc = scale;
	}

	explicit scale_const_iterator<SC>(difference_type scd, const SC *scale) {
		m_scd = scd;
		m_sc = scale;
	}

	note_t operator*() const {
		return m_sc->operator[](m_scd);
	}

	scale_const_iterator<SC>& operator++() {  // prefix
		++m_scd;
		return *this;
	}
	scale_const_iterator<SC> operator++(int) {  // postfix
		scale_const_iterator<SC> preinc_copy = *this;
		++m_scd;
		return preinc_copy;
	}
	scale_const_iterator<SC>& operator--() {  // prefix
		--m_scd;
		return *this;
	}
	scale_const_iterator<SC> operator--(int) {  // postfix
		scale_const_iterator<SC> preinc_copy = *this;
		--m_scd;
		return preinc_copy;
	}
	scale_const_iterator<SC>& operator+=(const difference_type& i) {
		m_scd += i;
		return *this;
	}
	scale_const_iterator<SC>& operator-=(const difference_type& i) {
		m_scd += i;
		return *this;
	}
	scale_const_iterator<SC>& operator/=(const difference_type& i) {
		m_scd /= i;
		return *this;
	}
	scale_const_iterator<SC>& operator*=(const difference_type& i) {
		m_scd *= i;
		return *this;
	}
	scale_const_iterator<SC> operator+(const difference_type& offset) {
		scale_const_iterator<SC> copy = *this;
		copy += offset;
		return copy;
	}
	scale_const_iterator<SC> operator-(const difference_type& offset) {
		scale_const_iterator<SC> copy = *this;
		copy -= offset;
		return copy;
	}
	// Note that scale_iterator<SC> + scale_iterator<SC> is not defined
	difference_type operator-(const scale_const_iterator<SC>& rhs) const {
		return m_scd-rhs;
	}

	// Note that only the m_scd field is checked; two iterators with different SC won't
	// satisfy these templates.  
	bool operator==(const scale_const_iterator<SC>& rhs) const {
		return m_scd==rhs.m_scd;
	}
	bool operator!=(const scale_const_iterator<SC>& rhs) const {
		return !(m_scd==rhs.m_scd);
	}
	bool operator<(const scale_const_iterator<SC>& rhs) const {
		return m_scd<rhs.m_scd;
	}
	bool operator>(const scale_const_iterator<SC>& rhs) const {
		return rhs.m_scd<m_scd;
	}
	bool operator<=(const scale_const_iterator<SC>& rhs) const {
		return !(rhs.m_scd<m_scd);
	}
	bool operator>=(const scale_const_iterator<SC>& rhs) const {
		return !(rhs.m_scd<m_scd);
	}
private:
	// There does not seem to be any reason for these to be private
	difference_type m_scd {0};
	const SC *m_sc;
};

template<class SC>
scale_const_iterator<SC> operator+(typename scale_const_iterator<SC>::difference_type offset, scale_const_iterator<SC> it) {
	return it += offset;
};
template<class SC>
scale_const_iterator<SC> operator-(typename scale_const_iterator<SC>::difference_type offset, scale_const_iterator<SC> it) {
	return it -= offset;
};








//-----------------------------------------------------------------------------
// The scd3_t class
//
// An scd3_t represents a scale degree of some unspecified scale.  It is a
// representation of an absolute position within a scale.  
//
/*

template<int N>
class scd3_t {
public:
	explicit scd3_t() = default;
	explicit scd3_t(int i) {
		m_scd = i;
	};
	explicit scd3_t(int i, int o) { // rscd, oct
		if (i>=N) { std::abort(); }
		m_scd = i + o*N;
	};

	scd3_t rscd() const {
		// ((i%N)+N)%N;
		// i-std::floor(static_cast<double>(i)/static_cast<double>(N))*N;
		return scd3_t {(m_scd+N)%N};
	};
	int octn() const {
		return m_scd/N;
	};
	int n() const {
		return N;
	};
	int to_int() const {
		return m_scd;
	};
	//std::string print() const;

	scd3_t& operator++() { // prefix
		m_scd++;
		return *this;
	};
	scd3_t operator++(int) { // postfix
		scd3_t scd = *this; ++*this;
		return scd;
	}
	scd3_t& operator--() { // prefix
		m_scd--;
		return *this;
	};
	scd3_t operator--(int) { // postfix
		scd3_t scd = *this; --*this;
		return scd;
	}
	scd3_t& operator+=(const scd3_t& rhs) {
		m_scd+rhs.m_scd;
		return *this;
	};
	scd3_t& operator-=(const scd3_t& rhs) {
		m_scd-rhs.m_scd;
		return *this;
	};
	scd3_t& operator*=(const scd3_t& rhs) {
		m_scd*rhs.m_scd;
		return *this;
	};
	scd3_t& operator/=(const scd3_t& rhs) {
		m_scd/rhs.m_scd;
		return *this;
	};

	bool operator==(const scd3_t& rhs) const {
		return m_scd==rhs.m_scd;
	};
	bool operator>(const scd3_t& rhs) const {
		return m_scd>rhs.m_scd;
	};
	bool operator<(const scd3_t& rhs) const {
		return m_scd<rhs.m_scd;
	};
private:
	int m_scd {0};
};


template<int N>
scd3_t<N> operator+(scd3_t<N> lhs, const scd3_t<N>& rhs) {
	return lhs+=rhs;
};
template<int N>
scd3_t<N> operator-(scd3_t<N> lhs, const scd3_t<N>& rhs) {
	return lhs-=rhs;
};
template<int N>
scd3_t<N> operator*(const scd3_t<N> lhs, const scd3_t<N>& rhs) {
	return lhs*=rhs;
};
template<int N>
scd3_t<N> operator/(scd3_t<N> lhs, const scd3_t<N>& rhs) {
	return lhs/=rhs;
};

template<int N>
bool operator!=(const scd3_t<N>& lhs, const scd3_t<N>& rhs) {
	return !(lhs==rhs);
};
template<int N>
bool operator>=(const scd3_t<N>& lhs, const scd3_t<N>& rhs) {
	return (lhs>rhs || lhs==rhs);
};
template<int N>
bool operator<=(const scd3_t<N>& lhs, const scd3_t<N>& rhs) {
	return (lhs<rhs || lhs==rhs);
};
*/

