#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include <vector>

//-----------------------------------------------------------------------------
// Class tmetg
//
class tmetg_t {
public:
	tmetg_t() = delete;
	// ts, dp, phases
	explicit tmetg_t(ts_t,std::vector<d_t>,std::vector<beat_t>);

	// Set, read probability grid
	void set_rand_pg();
	std::vector<double> nt_prob(beat_t);

	void enumerate() const;

	// list of allowed nv_ts @ given beat
	//std::vector<nv_t> which_allowed(beat_t, std::vector<nv_t>, int=1) const;
	std::vector<int> levels_allowed(beat_t) const;
	// Is the input nv_t allowed @ the input beat?
	bool allowed_at(beat_t) const;
	std::vector<d_t> draw() const;
	// If I put nv_t @ beat, is any note value allowed at the next beat?
	bool allowed_next(beat_t,d_t) const;
	std::string print() const;
	
private:
	ts_t m_ts {beat_t{4.0},d_t{d::q}};
	std::vector<d_t> m_nvs {};

	// "beat-pool" => number of beats spanned by each element of m_nv_ts
	std::vector<beat_t> m_beat_values {}; 

	// phase-shift of each note-value level (units == beats)
	std::vector<beat_t> m_ph {};  

	// Grid resolution:  The largest number of beats such that all beat-numbers
	// corresponding to a bar or a note-value can be reached as an integer number
	// increments.  
	beat_t m_btres {0.0};  // grid resolution
	beat_t m_period {0.0}; // The shortest repeating unit

	// Probability grid
	bool m_pg_set {false};
	std::vector<std::vector<double>> m_pg {};

	// The maximum number of subdivisions of the beat used in calculating btres
	static const int m_bt_quantization; 

	void m_enumerator(std::vector<std::vector<int>>&, 
		std::vector<std::vector<int>> const&, int&,int&) const;

	
	d_t gcd(const std::vector<d_t>&) const;  // greatest common divisor
};





namespace autest::metg {
	std::string tests1();
}





