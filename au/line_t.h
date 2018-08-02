#pragma once
#include <vector>
#include <initializer_list>
#include<variant>
//#include "aulib\util\au_util_all.h"
#include "aulib\types\scd_t.h"
#include "aulib\types\frq_t.h"
#include "aulib\types\ntl_t.h"

//-----------------------------------------------------------------------------
// Template class element_t
// An element_t is the simplest form of music event; it is an association of 
// some object representing pitch (frq_t, ntl_t, ntstr_t, scd_t, 
// chord_t, ...) and some object representing duration (note_value, beat_t,
// ...).  It need not be possible to synthesize, hence it does not associate
// a ts_t or scale.  It is merely a simple union of a pair of abstract 
// representations or pitch and duration.  
//
//
template<typename Tpitch, typename Tduration>
class element_t {
public:
	explicit element_t(Tpitch p, Tduration d) : pitch{p}, duration{d} {};

	Tpitch pitch = Tpitch {};
	Tduration duration = Tduration {};
private:
};

class scd_t;
class frq_t;
class ntstr_t;

class apt_t {
public:
	apt_t() = delete;
	apt_t(scd_t);
	apt_t(frq_t);
	apt_t(ntstr_t);
private:
	std::variant<scd_t,frq_t,ntstr_t> m_pdata {};
};


//-----------------------------------------------------------------------------
// Class line_t
//
template<typename Tp, typename Td>
class line_t {
public:
	line_t() = delete;
	explicit line_t(std::initializer_list<element_t<Tp,Td>> il) {
		for (auto e : il) {
			m_elems.push_back(e);
		}
	};

private:
	std::vector<element_t<Tp,Td>> m_elems {};
};


