#pragma once
#include "types/nv_t.h"
#include <set>

//
// Duration element
// A vertical note group 
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
	explicit de_t(nv_t);
	explicit de_t(T,nv_t);
	explicit de_t(std::vector<T>,nv_t);

	void insert(T);
	void remove(const T&);
	void remove(const T&);

	template<typename TNotRest>  // only enable if T=rest_dummy_t
	operator de_t<TNotRest>() { return de_t<TNotRest>{m_dv}; };
private:
	nv_t m_dv {};
	std::vector<T> m_nts {};
};

template<typename T>
using chord_t = typename de_t<T>;
	//  But note that a chord does not have duration
	//  Call it an "ntg_t" ?? ntvg_t
	//  Using the name "chord_t" uses up a name i probably want
	//  to use for a collection of notes w/o duration...
using rest_t = typename de_t<rest_dummy_t>;




