#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include <vector>
#include <string>

//-----------------------------------------------------------------------------
// Class tmetg
//
class tmetg_t {
public:
	tmetg_t() = delete;
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
	
private:
	struct nvs_ph {
		d_t nv {};
		beat_t nbts {};
		beat_t ph {};
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
		int ix_nvsph {0};
		int stepsz {0};
		double lgp {0.0};
	};
	//std::vector<std::vector<double>> m_pg {};
	std::vector<std::vector<pgcell>> m_pg2 {};

	struct rpp {  // "rp with probability"
		std::vector<int> rp {};
		double p {1.0};
	};

	std::vector<int> levels_allowed(beat_t) const;
		// idx to allowed nv's @ the given beat; these numerical
		// indices mean nothing to an external caller, hence this
		// method is private.  

	//void m_enumerator(std::vector<std::vector<int>>&, 
	//	std::vector<std::vector<int>> const&, int&,int&) const;
	void m_enumerator2(std::vector<rpp>&, 
		std::vector<std::vector<pgcell>> const&, int&,int&) const;
	
	d_t gcd(const std::vector<d_t>&) const;  // greatest common divisor
	d_t gcd(const d_t&, const d_t&) const;
};


namespace autest::metg {
	std::string tests1();
}


