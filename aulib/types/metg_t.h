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
class tmetg_t {
public:
	tmetg_t() = delete;
	tmetg_t(ts_t, rp_t);
	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>);

	//  Random rp generation
	void set_rand_pg();
	std::vector<double> nt_prob(beat_t) const;
	std::vector<d_t> draw() const;  // Generate a random rp
	void enumerate() const;  // Generate all possible rp's

	bool allowed_at(beat_t) const;
	bool allowed_next(beat_t,d_t) const;
	std::string print() const;
	std::string print_pg() const;

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
	ts_t m_ts {4_bt,d_t{d::q}};

	// Grid resolution:  The largest number of beats such that all beat-numbers
	// corresponding to a bar or a note-value can be reached as an integer number
	// increments.  
	beat_t m_btres {0.0};  // grid resolution
	beat_t m_period {0.0}; // The shortest repeating unit

	// Probability grid
	struct pgcell {
		int ix_nvsph {0};  // Idx of m_nvsph
		int stepsz {0};
		double lgp {0.0};
	};
	std::vector<std::vector<pgcell>> m_pg {};
	bool m_f_pg_extends {true};

	struct rpp {  // "rp with probability"
		std::vector<int> rp {};
		double p {1.0};
	};

	bool allowed_at(d_t, beat_t) const;
	std::vector<int> levels_allowed(beat_t) const;
		// idx to allowed nv's @ the given beat; these numerical
		// indices mean nothing to an external caller, hence this
		// method is private.  

	//----------------------------------------------------------------------------
	// Methods
	beat_t gres() const;
	beat_t period() const;

	void m_enumerator(std::vector<rpp>&, 
		std::vector<std::vector<pgcell>> const&, int&, int) const;
	
	d_t gcd(const std::vector<d_t>&) const;  // greatest common divisor
	d_t gcd(const d_t&, const d_t&) const;
	beat_t round(beat_t) const;
};


namespace autest::metg {
	std::string tests1();
}


