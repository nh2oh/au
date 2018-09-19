#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
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
	struct nvs_ph {
		d_t nv {};
		d_t ph {0};  // ph = offset-std::floor(offset/nv)*nv
		bool operator==(const nvs_ph&) const;
		bool operator<(const nvs_ph&) const;
		bool operator>(const nvs_ph&) const;
		bool validate() const;
	};

	tmetg_t() = delete;
	tmetg_t(ts_t, rp_t);
	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>);

	tmetg_t slice(beat_t, beat_t) const;
	std::vector<tmetg_t> factor() const;

	//  Random rp generation
	void set_pg_random(int = 0);  // argument => mode
	void set_pg_zero(beat_t = 0_bt);
	std::vector<double> nt_prob(beat_t) const;  // => pg
	std::vector<d_t> draw() const;  // Generate a random rp
	std::vector<rpp> enumerate() const;  // Generate all possible rp's

	bool member_allowed_at(beat_t) const;  // => tg
	bool member_allowed_at(d_t, beat_t) const;  // => tg
	bool member_allowed_next(beat_t,d_t) const;  // => tg
	

	std::string print() const;
	std::string print_pg() const;
	std::string print_tg() const;

	bar_t nbars() const;
	ts_t ts() const;
	std::vector<nvs_ph> levels() const;

	bool validate() const;

	bool operator==(const tmetg_t&) const;
private:
	ts_t m_ts {4_bt,d_t{d::q}};

	// No two elements are the same.  Elements are sorted in order of decreasing 
	// duration, then by increasing phase.  phase is always >= 0 && < nv
	std::vector<nvs_ph> m_nvsph {};

	// m_btres:  The largest number of beats such that all beat-numbers
	// corresponding to a bar or a note-value can be reached as an integer 
	// number of m_btres-sized increments.  
	// m_period:  The smallest number of beats able to contain an integer
	// number of each element of m_nvsph and an integer number of bars 
	// (m_ts.bar_unit(), m_ts.beats_per_bar()).  
	beat_t m_btres {0.0};
	beat_t m_period {0.0};

	// If representing a sub-rp...
	beat_t m_btstart {0.0};
	beat_t m_btend {0.0};  // Constructor should default == m_period

	// Probability grid m_pg
	// m_pg.size() == whatever, m_pg[i].size() == m_nvsph.size() for all i.  
	// Note that m_pg does not store log-probability (despite the name of 
	// field pgcell.lgp); it stores "normal" probabilities.  The rp enumerator
	// creates its own special copy of m_pg which really does store 
	// log-probability.  
	struct pgcell {
		int ix_nvsph {0};  // Idx of m_nvsph
		int stepsz {0};
		double lgp {0.0};
	};
	std::vector<std::vector<pgcell>> m_pg {};  // m_pg[col][row]
	bool m_f_pg_extends {true};  

	//----------------------------------------------------------------------------
	// Methods
	bool tg(d_t, d_t, beat_t) const;

	beat_t gres() const;  // Reads m_ts, m_nvsph
	beat_t period() const;  // Reads m_ts, m_nvsph, m_btres

	int bt2step(beat_t) const;
	int nv2step(d_t) const;

	bool pg_extends() const;  // Should == m_f_pg_extends, but does not set.  
	std::vector<std::vector<pgcell>> extend_pg(beat_t, beat_t) const;

	std::vector<int> which_members_allowed(beat_t) const;  // => tg
		// idx to allowed nv's @ the given beat; these numerical
		// indices mean nothing to an external caller, hence this
		// method is private.  

	// "note-value pointer with probability"
	struct nvp_p {
		std::vector<int> rp {};
		double p {1.0};
	};
	void m_enumerator(std::vector<nvp_p>&, 
		std::vector<std::vector<pgcell>> const&, int&, int) const;
	void m_enumerator2(std::vector<nvp_p>&, 
		std::vector<std::vector<pgcell>> const&, int&, int) const;

};


namespace autests {
	std::string tests1();
};


