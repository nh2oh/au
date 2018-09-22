#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
#include "teejee.h"
#include <vector>
#include <string>






//
// Class tmetg_t
//
// Probabilistic representation of an rp _and_ a corresponding tg to which
// it must align.  The tg is a function only of the ts and set of nv_t's and
// phases.  It is not represented explictly in the object.  The rp is
// represented by a pg which must align to the tg.  This is not actually that
// severe a constraint, since an nv_t of a given duration occuring at a
// beat that would otherwise be illegal can be made legal by including in in
// the nvsph set with an appropriate phase offset.  
// The tg is represented as m_ts, m_nvsph, m_btres and m_period.  
// Data members m_btstart, m_btend indicate tg beat-numbers for the cols of 
// the pg.  These data members exist only to align the pg to the tg.  
// TODO:  Then is m_btend redundant???
//
// pg properties
// - For any beat (col), all nonzero entries point into different beats (a
//   consequence of the uniqueness of the elements of m_nvsph).  
//   -- Probabilities are normalized, each col has the same number of rows 
//      as m_nvsph, ...
// - A pg may or may not be extendable:  There is no requirement that full
//   or partial concatenations to itself align to the tg.  For example, the
//   object may be representing only a segment of an rp.  
// - It is also possible that a pg spanning a noninteger number of m_peroid
//   beats may be extendable.  The pg may specify 0 for the probability of
//   certain nvsph elements such that the period is less than calculated by
//   the tg.  
//   m_f_pg_extends, pg_extends() reads this.  
//
//


// TODO:  
// To implement rdurmetg(), rp_b(), etc, need to draw a distinction between 
// the "tg" and the rp the object possibly represents.  
// At present, the tg is "virtual" ... its members are m_btres, m_period, m_nvsph,
// allowed_*(), levels_allowed().  
//
// TODO:
// The user should be able to set m_btstart, and, if set by the user, the
// tg calculation needs to take this into account
//
// TODO:  draw() ignores probabilities
//
// TODO:  enumerate() needs to take limits... niter, max rps, max-mem... something
//
// TODO:  The probabilities returned by enumerate() are *probably* wrong
//
// TODO:  Operator== is probably broken:  the m_pg comparison loop won't work


struct tmetg_t_opts {
	bool barspan {false};
	bool zero_pointers {false};
	// Does anyone want zero_pointers ever???
};


class tmetg_t {
public:
	struct rpp {  // "rp with probability" return type of enumerate()
		std::vector<d_t> rp {};
		double p {1.0};
		size_t n {0};
	};
	using nvs_ph = teejee::nv_ph;

	teejee m_tg {};

	tmetg_t() = delete;
	tmetg_t(ts_t, rp_t);
	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>);

	tmetg_t slice(beat_t, beat_t) const;
	std::vector<tmetg_t> factor() const;

	//  Random rp generation
	void set_pg_random(int = 0);  // argument => mode
	void set_pg_zero(beat_t = 0_bt);
	std::vector<d_t> draw() const;  // Generate a random rp
	std::vector<rpp> enumerate() const;  // Generate all possible rp's

	bool pg_member_allowed_at(beat_t) const;  // => pg
	bool member_allowed_at(d_t, beat_t) const;  // => tg
	bool span_possible(bar_t) const;
	bool span_possible(beat_t) const;

	std::string print() const;
	std::string print_pg() const;
	std::string print_tg() const;

	bar_t nbars() const;
	ts_t ts() const;
	std::vector<nvs_ph> levels() const;

	bool validate() const;

	bool operator==(const tmetg_t&) const;
private:
	// If representing a sub-rp...
	beat_t m_btstart {0.0};
	beat_t m_btend {0.0};  // Constructor should default == m_period

	// Probability grid m_pg
	// m_pg.size() == whatever, m_pg[i].size() == m_nvsph.size() for all i.  
	std::vector<std::vector<double>> m_pg {};  // m_pg[col][row]
	bool m_f_pg_extends {true};  

	//----------------------------------------------------------------------------
	// Methods

	// TODO:  Rename "step" -> "Stride" to disambiguate position from distance
	int bt2step(beat_t) const;
	int bt2stride(beat_t) const;
	int nv2stride(d_t) const;
	int level2stride(int) const;
	int nvph2level(const teejee::nv_ph&) const;

	bool pg_extends() const;  // Should == m_f_pg_extends, but does not set.  
	std::vector<std::vector<int>> zero_pointers() const;  // [c,r] of zp's in m_pg
	std::vector<std::vector<double>> extend_pg(beat_t, beat_t) const;

	// "note-value pointer with probability"
	struct enumerator_pgcell {
		int level {0};  // Idx of m_tg.levels()
		int stride {0};
		double lgp {0.0};
	};
	struct nvp_p {
		std::vector<int> rp {};
		double p {1.0};
	};
	void m_enumerator(std::vector<nvp_p>&, 
		std::vector<std::vector<enumerator_pgcell>> const&, int&, int) const;
	void m_enumerator2(std::vector<nvp_p>&, 
		std::vector<std::vector<enumerator_pgcell>> const&, int&, int) const;

};


namespace autests {
	std::string tests1();
};


