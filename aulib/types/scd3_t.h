#pragma once
#include <string>  // for declaring the print() members

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
