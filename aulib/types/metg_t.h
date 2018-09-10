#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h";
#include <vector>
#include <string>

//-----------------------------------------------------------------------------
// Class tmetg
//
struct tmetg_t_opts {
	bool barspan {false};
	bool zero_pointers {false};
	// Does anyone want zero_pointers ever???
};

// TODO:  _must_ change this so that each col of m_pg contains m_nvsph.size()
// rows.  
class tmetg_t {
public:
	struct rpp {  // "rp with probability" return type of enumerate()
		std::vector<d_t> rp {};
		double p {1.0};
	};

	tmetg_t() = delete;
	tmetg_t(ts_t, rp_t);
	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>);

	//  Random rp generation
	void set_pg_random(int = 0); // argument => mode
	void set_pg_zero(beat_t = 0_bt);
	std::vector<double> nt_prob(beat_t) const;
	std::vector<d_t> draw() const;  // Generate a random rp
	std::vector<rpp> enumerate() const;  // Generate all possible rp's

	bool allowed_at(beat_t) const;
	bool allowed_next(beat_t,d_t) const;
	bool allowed_at(d_t, beat_t) const;
	std::string print() const;
	std::string print_pg() const;
	std::string print_tg() const;
private:
	struct nvs_ph {
		d_t nv {};
		beat_t nbts {};
		beat_t ph {};
		bool operator==(const nvs_ph& rhs) const {
			return ((nv == rhs.nv) && (nbts == rhs.nbts) && (ph == rhs.ph));
		};
	};
	std::vector<nvs_ph> m_nvsph {};
	//int nv2lvlidx(d_t) const;

	ts_t m_ts {4_bt,d_t{d::q}};

	// m_btres:  The largest number of beats such that all beat-numbers
	// corresponding to a bar or a note-value can be reached as an integer 
	// number of m_btres-sized increments.  
	// m_period:  The smallest number of beats able to contain an integer
	// number of each element of m_nvsph and an integer number of bars 
	// (m_ts.bar_unit(), m_ts.beats_per_bar()).  
	beat_t m_btres {0.0};  // grid resolution
	beat_t m_period {0.0}; // The shortest repeating unit

	// Probability grid m_pg
	// m_pg.size() == m_period/m_btres; for all i,
	// m_nvsph.size() <= m_pg[i].size() >= 0
	// Cols for which there are no valid elements of m_nvsph may or may not be
	// empty.  
	// The purpose of m_pg is twofold
	// 1)  If the metg object was created from a user-supplied rp or set of 
	//     rp's, m_pg stores these input rp(s) in grid format.  
	// 2)  When enumerating large numbers of rp's, it is convienient to not 
	//     have to compute nbeat(m_ph,curr_nv)/m_btres (to get the number of
	//     grid steps spanned by curr_nv) and log(probability) for each nv
	//     under consideration.  m_pg associates this useful information 
	//     with each grid point.  
	// Note that m_ph does not store log-probability (despite the name of 
	// field pgcell.lgp); it stores "normal" probabilities.  The rp enumerator
	// creates its own special copy of m_pg which really does store 
	// log-probability.  
	struct pgcell {
		int ix_nvsph {0};  // Idx of m_nvsph
		int stepsz {0};
		double lgp {0.0};
	};
	std::vector<std::vector<pgcell>> m_pg {};
	bool m_f_pg_extends {true};

	// "note-value pointer with probability"
	// Data structure used by the rp enumerator.  
	struct nvp_p {
		std::vector<int> rp {};
		double p {1.0};
	};


	//----------------------------------------------------------------------------
	// Methods
	beat_t gres() const;  // Reads m_ts, m_nvsph
	beat_t period() const;  // Reads m_ts, m_nvsph, m_btres

	std::vector<int> levels_allowed(beat_t) const;
		// idx to allowed nv's @ the given beat; these numerical
		// indices mean nothing to an external caller, hence this
		// method is private.  

	void m_enumerator(std::vector<nvp_p>&, 
		std::vector<std::vector<pgcell>> const&, int&, int) const;
	
	d_t gcd(const std::vector<d_t>&) const;  // greatest common divisor
	d_t gcd(const d_t&, const d_t&) const;
	beat_t round(beat_t) const;
};


namespace autests {
	std::string tests1();
}


