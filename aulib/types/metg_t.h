#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
#include "teejee.h"
#include <vector>
#include <string>

//
// Class metg_t
//
// Probabilistic representation of an rp _and_ a corresponding tg to which
// it must align.  The tg is a function only of the ts and the set of nv_t's
// and phases.  
//
// The rp is represented by m_pg, an m_tg.levels().size()xn array of doubles 
// which represent the probability of finding element m_tg.levels()[r] at 
// beat-number c over the other elements in m_tg.levels() elligible to occur 
// at c.  Nonzero elements r* in m_pg[c-bt2step(m_btstart)] must belong to the 
// set m_tg.which_allowed_at(step2bt(c)).  
// That is, for all c, r* for which:
//     m_pg[c-bt2step(m_btstart)][r*] > 0,
//     m_tg.onset_allowed_at(m_tg.levels()[r*],step2bt(c)) == true
//
// Although the tg is extendable, m_pg may not be.  A default constructed 
// metg_t has a random pg spanning exactly 1 m_tg.period() and is therefore
// extendable (and factorable wherever m_tg is factorable), however, a metg_t
// can represent segments of otherwise extendable metg_t's, so may not be.  
// For example, any indivdual metg_t's returned by factor() can not be
// repeatedly concatenated to itself to generate a valid (m_tg-aligning) 
// metg_t.  
// Note that an m_pg need not span an integer number of m_tg.period()s to be
// extendable.  An m_pg may be "simpler" than its m_tg, since it can specify
// a probabiltiy of 0 for "troublesome" elements of m_tg.  
//
// m_pg properties
// - For all c, m_pg[c].size() == m_tg.levels().size()
// - For any c, all entries r*_i > 0 point into a c'_i > c, with no two c'_i
//   the same (a consequence of the constraint that all members of m_tg must 
//   be > d::z and unique).  
// - For all c, the sum of m_pg[c][r] for 0 >= r > n == 0 or == 1.  
// - Contains no _internal_ zero-pointers, but may contain external 
//   zero-pointers.  May contain orphans.  
//


// TODO:  
// TODO:  m_tg.period() might be > < the period applicable to m_pg.  Need to
// calc and store this period.  
//
// TODO:  enumerate() needs to take limits... niter, max rps, max-mem... something
//
// TODO:  Where ph's !=0 bar spanning elements mean nbars() is not correct for all
// rps.  Also often the case where slice()'d not on a period boundry.  
//
// TODO:  Does validate() check for zero-pointers??  orphans??
//
// TODO:  Make a custom 1d vector to accumulate the results of m_enumerator()
// 
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

	tmetg_t() = delete;
	tmetg_t(ts_t, rp_t);
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>); // ts, dp, ph
	explicit tmetg_t(ts_t,std::vector<teejee::nv_ph>,
		std::vector<std::vector<double>>);
		// Manually specify the whole pg
	explicit tmetg_t(teejee);

	// Getters
	std::vector<bar_t> nbars() const;
	ts_t ts() const;
	std::vector<teejee::nv_ph> levels() const;  // passthrough to m_tg.levels()
	bool onset_allowed_at(beat_t) const;  // beat-number, not number-of-beats
	bool onset_allowed_at(d_t, beat_t) const;  // d_t must be a member @ beat-number
	bool span_possible(beat_t) const;  // number-of-beats, not beat-number
	bool span_possible(bar_t) const;  // number-of-bars, not bar-number
	std::string print() const;
	std::string print_pg() const;
	std::string print_tg() const;
	bool validate() const;

	// Setters
	tmetg_t slice(beat_t, beat_t) const;  // Extends and/or truncates
	std::vector<tmetg_t> factor() const;
	bool set_length_exact(beat_t);  // Also extends/truncates as necessary

	// Samplers
	std::vector<d_t> draw() const;  // Generate a random rp
	std::vector<rpp> enumerate() const;  // Generate all possible rp's

	// Setters
	void set_pg_random(int = 0);  // argument => mode
	void set_pg_zero();
	bool set_pg(teejee::nv_ph,beat_t,double);
	bool set_pg(beat_t,std::vector<double>);  // Set a whole col
	bool set_pg(teejee::nv_ph,std::vector<double>);  // Set a whole row
	

	// Operators
	bool operator==(const tmetg_t&) const;
	bool operator!=(const tmetg_t&) const;
private:
	// Data
	teejee m_tg {};
	std::vector<std::vector<double>> m_pg {};  // m_pg[col][row]
	beat_t m_btstart {0.0};
	bool m_f_pg_extends {true};

	// Methods
	int bt2step(beat_t) const;  // bt number -> m_pg col idx
	beat_t step2bt(int) const;  // m_pg col idx -> bt number
	int bt2stride(beat_t) const;  // number-of-bts -> number-of-steps
	int nv2stride(d_t) const;    // nv_t -> number-of-steps
	int level2stride(int) const;  // number-of-steps per unit of given level idx
	int nvph2level(const teejee::nv_ph&) const;

	void init_pg(beat_t);

	bool pg_extends(const std::vector<std::vector<double>>&) const;
	int pg_min_period() const;  // Not implemented
	bool internal_zero_pointers(const std::vector<std::vector<double>>&) const;
	bool internal_orphans(const std::vector<std::vector<double>>&) const;
	std::vector<std::vector<int>> find_internal_zero_pointers(const std::vector<std::vector<double>>&) const;
	bool is_zero_col(const int&, const std::vector<std::vector<double>>&) const;

	std::vector<std::vector<double>> extend_pg(std::vector<std::vector<double>>,beat_t, beat_t) const;
	std::vector<std::vector<double>> set_pg_length_exact(std::vector<std::vector<double>>, beat_t) const;
	std::vector<std::vector<double>> normalize_pg(std::vector<std::vector<double>>) const;

	struct enumerator_pgcell {  // Used internally by m_enumerator()
		int level {0};  // Idx of m_tg.levels()
		int stride {0};
		double lgp {0.0};
	};
	struct nvp_p { // "note-value pointer with probability"
		std::vector<int> rp {};  // Idxs of m_tg.levels()
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


