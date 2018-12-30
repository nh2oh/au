#pragma once
#include "musel_t.h"
#include "ts_t.h"
#include "rp_t.h"
#include <vector>
#include <string>
#include <exception>


//
// line_t<T>
//
// Represents a sequence of musical elements with associated durations.  
// A "musical element" is any musel_t<T>:  either a single note of type T
// (scd_t, frq_t, ...), a single rest_t, or a single chord chord_t<T>.  
//
// Associates a d_t to each element in a sequence of musel_t.  
// Associates a ts_t.
//
//
// Notes:
// There are two possible designs for line_t:
// 1)  Hold a std::vector T where T is a note-type.  Also hold additional 
//     datastructures to dynamically indicate groups of T's corresponding to
//     chords, rests, etc.  (Alternatively, perhaps, hold several std::vector<T>'s:
//     T1=note-type, T2=chord_t<T1>, T3=rest_t).  IOW, hold note-types _directly_
//     and hold their durations separately.  Dynamically re-associate as needed.  
//     Insertion/deletion etc is very complex.  See, for example, rp_t.  
// 2)  Hold a std::vector<de_t<T>>.  
//
//
// 
// TODO:  Is associating a ts_t to a d_t seq what associating a scale_t is to a
//        note sequence?  Should i associate a scale?
//
//

template<typename T>
class line_t {
public:
	line_t()=default;

	explicit line_t(ts_t ts, std::vector<musel_t<T>> mes, std::vector<d_t> rp) {
		if (mes.size() != rp.size()) { std::abort(); }
		m_mes = mes;
		m_rp = rp_t {ts,rp};
	};

	explicit line_t(std::vector<T> nts, rp_t rp) {
		if (rp.nevents() != nts.size()) { std::abort(); }
		m_rp = rp;
		for (auto e : nts) {
			m_mes.push_back({e,false});
		}
	};

	void push_back(T nt, d_t dv) {
		m_mes.push_back({nt,false});
		m_rp.push_back(dv);
	};
	void push_back(musel_t<T> me, d_t dv) {
		m_rp.push_back(dv);
		m_mes.push_back(me);
	};
	void push_back(std::vector<musel_t<T>> mes, std::vector<d_t> rp) {
		if (mes.size() != rp.size()) { std::abort(); }
		for (int i=0; i<mes.size(); ++i) {
			m_mes.push_back(mes[i]);
			m_rp.push_back(rp[i]);
		}
	};

	// Getters
	bar_t nbars() const { return m_rp.nbars(); };
	beat_t nbeats() const { return m_rp.nbeats(); };
	size_t nelems() const { return m_rp.nevents(); };
	std::string print(const std::string& sep = " ") {
		d_t::opts rp_p_opts {};
		rp_p_opts.denom_only = true;

		std::string s {};
		for (int i=0; i<m_mes.size(); ++i) {
			s += m_mes[i].print();
			s += "/";
			s += m_rp[i].print(rp_p_opts);
			s += sep;
		}

		return s;
	};

	// Needs to take options:
	// - flatten_chords
	// - strip_rests
	template<typename U>
	struct notes_flat_element {
		U note {};
		d_t d {};
	};
	std::vector<notes_flat_element<T>> notes_flat() const {
	//std::vector<T> notes_flat() const {
		// Stripping all rests & flattening chords...
		// ... _very_ half-assed impl

		std::vector<notes_flat_element<T>> notes_flat_norests {};
		//std::vector<T> notes_flat_norests {};
		for (int i=0; i<m_mes.size(); ++i) {
			auto curr_musel = m_mes[i];
			if (curr_musel.isrest()) { continue; }
			for (int j=0; j<curr_musel.n(); ++j) {
				//notes_flat_norests.push_back(curr_musel[j]);
				notes_flat_norests.push_back({curr_musel[j],m_rp[i].e});
			}
		}
		return notes_flat_norests;
	};

private:
	//struct idxs {
	//	size_t begin {0};
	//	size_t end {0};
	//	bool ischord {false};
	//	bool isrest {false};
	//};

	rp_t m_rp;
	std::vector<musel_t<T>> m_mes {};  // "elements"
};




