#pragma once
#include <variant>
#include "nv_t.h"
#include "chord_t.h"


//
// musel_t
//
// A "musical element."  Class musel_t represents any single "musical 
// element," that is, any element that occupies horizontal space on a staff:
// a single note, single chord, or single rest.  A de_t containing a
// chord_t<T> has type de_t<T>, the same as a de_t containing a single note 
// of type T (scd_t, ntl_t, ...).  
// 
// TODO:  rest_t containment, type conversion, etc
// 

template<typename T>
class musel_t {
public:
	musel_t(chord_t<T> e, /*d_t d, */bool isrest) {
		m_e = e;
		m_isrest = isrest;
	};
	musel_t(T e, /*d_t d, */bool isrest) {
		m_e = e;
		m_isrest = isrest;
	};

	std::string print() const {
		std::string s {};
		if (std::holds_alternative<T>(m_e)) {
			s += std::get<T>(m_e).print();
		} else {
			s += std::get<chord_t<T>>(m_e).print();
		}

		return s;
	};

	bool isrest() const {
		return m_isrest;
	};

	size_t n() const {
		if (std::holds_alternative<T>(m_e)) {
			return 1;
		} else {
			return std::get<chord_t<T>>(m_e).n();
		}
	};

	T operator[](size_t idx) const {
		if (std::holds_alternative<T>(m_e)) {
			//au_assert(idx==0,"Not a chord: idx must == 0");
			return std::get<T>(m_e);
		} else {
			return std::get<chord_t<T>>(m_e).notes()[idx];
		}
	};

private:
	std::variant<T,chord_t<T>> m_e {};
	bool m_isrest {false};
};



//
// rest_t
//
// A rest_t is analagous to a note-type.  It is little more than a "tag"-type
// which can be inserted into a de_t.m_e to create a "rest" as a de_t<rest_t>
// with an associated duration.  
//

class rest_t {
public:
private:
};



