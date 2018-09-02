#pragma once
#include "nv_t.h"
#include <set>
#include <algorithm>
#include <type_traits>

//
// Duration element
// A vertical note group.  If m_nts.size() == 0, it's a rest.  
//
// As purely syntactic sugar, it'd be nice if, when an object is constructed
// as a rest (w/ T=rest_dummy_t), said object has a converting constructor
// (or maybe a copy constructor) to any other valid de_t<T>.  This way, a
// user can instantiate a rest_t (ex, defined as alias of de_t<rest_dummy_t>)
// and stick it into a sequence of, for example, de_t<scd_t>, and have it 
// convert to a de_t<scd> w/ an empty m_nts.  
// Other ideas:
// A rest should not permit insert() or remove() (the methods should not
//    exist).  
// It may or may not be a good idea to allow a non-rest de_t<T> to call 
//    remove() until empty... such an object is functionally a rest.  
//
//
// ----------------------------------------------------------------------------
//
// m_nts is not sorted, since all nt types (ex: ntl_t) do not define >, <
//    operators.  
//
// I named the duration m_dv instead of m_nv in anticipation of renaming
//    nv_t to d_t (or something), and defining nt_t as corresponding to
//    some sort of composition of a d_t and a note(s) (ie, this) and 
//    rest_t as below.  
//
//
//
//
//
//
struct rest_dummy_t {
	//bool rest_dummy_t_data {true};
};

template<typename T=rest_dummy_t>
class de_t {
public:
	de_t()=default;
	explicit de_t(d_t dv) {
		m_dv = dv;
	};  // Creates a rest: T=rest_dummy_t

	explicit de_t(T nt, d_t dv) {
		m_nts.push_back(nt);
		m_dv = dv;
	};

	explicit de_t(std::vector<T> nts, d_t dv) {
		for (auto e : nts) {
			insert(e);
		}
		m_dv = dv;
	};

	void insert(T nt) {
		auto it = std::find(m_nts.begin(),m_nts.end(),nt);
		if (it == m_nts.end()) { 
			m_nts.push_back(nt);
		}
		return;
	};

	void remove(const T& nt) {
		m_nts.erase(std::remove(m_nts.begin(),m_nts.end(),nt));
		return;
	};

	void setd(d_t dv) {  // set duration
		m_dv = dv;
		return;
	};

	d_t dv() {
		return m_dv;
	};

	std::vector<T> nts() {
		return m_nts;
	};

private:
	d_t m_dv {};
	std::vector<T> m_nts {};
};

//template<typename T>
//using chord_t = typename de_t<T>;
	//  But note that a chord does not have duration
	//  Call it an "ntg_t" ?? ntvg_t
	//  Using the name "chord_t" uses up a name i probably want
	//  to use for a collection of notes w/o duration...
using rest_t = typename de_t<rest_dummy_t>;




