#pragma once
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"  // Only to declare the constructor taking an rp_t
#include <vector>
#include <string>


//
// Class teejee
//
// Note that in general not all traversals of the grid will yield rp's of the
// same length.  For example, d_t = {d::w, d::h, d::q}; ph = {1_bt, 0_bt, 0_bt}
// yields a grid like:
//     0 1 0 0 |
//     1 0 1 0 |
//     1 1 1 1 |
// The single rp containing the w note, {q,w}, spans the bar, but all other rp's
// are exactly 1 bar long.  
// 
//
//
class teejee {
public:
	class nv_ph {  // Must be public b/c returned by levels()
	public:
		nv_ph(const d_t, const d_t);
		//d_t nv() const;  TODO ... 
		//d_t ph() const;

		d_t nv {d::q};
		d_t ph {d::z};  // Phase is always >= 0 && < nv.  
		bool operator==(const nv_ph&) const;
		bool operator!=(const nv_ph&) const;
		bool operator<(const nv_ph&) const;
		bool operator>(const nv_ph&) const;
		// These operators sort by decreasing duration then 
		// increasing phase.  

		d_t phase(const d_t&, const d_t&) const; // phase(nv,offset)
		bool validate() const;
	};

	explicit teejee() = default;
	explicit teejee(const ts_t&, const std::vector<d_t>&, 
		const std::vector<beat_t>&); // ts, dp, phases, allow barspan
	explicit teejee(const rp_t&);

	ts_t ts() const;
	std::vector<nv_ph> levels() const;
	bar_t period() const;
	beat_t gres() const;  // beats-per-col

	bool factors_at(const beat_t) const;
	int count() const;  // Count number of possible rp's
	std::vector<nv_ph> which_allowed_at(const beat_t) const;
	bool onset_allowed_at(const beat_t) const;  // is there @ least 1 member allowed at beat?
	bool onset_allowed_at(const nv_ph, const beat_t) const; // nv_ph need not be a member
	bool spans_bar(const beat_t, const d_t) const;  // d_t need not be a member
	bool ismember(const d_t) const;
	bool ismember(const nv_ph) const;

	std::string print() const;
	std::string print_g() const;

	bool operator==(const teejee&) const;

	bool validate() const;
private:
	//----------------------------------------------------------------------------
	// Data
	ts_t m_ts {4_bt,d_t{d::q}};
	std::vector<nv_ph> m_levels {};  // All elements unique & sorted
	beat_t m_btres {0.0};
	beat_t m_period {0.0};

	//----------------------------------------------------------------------------
	// Methods
	bool insert_level(const d_t&, const d_t&);  // nv, ph
		// Inserts into m_levels, but does _not_ recalc the resolution or period.

	beat_t calc_gres() const;
	beat_t calc_period() const; // Reads m_ts, m_levels, m_btres

	int bt2step(const beat_t) const;
	beat_t step2bt(const int) const;
	beat_t bt2reducedbt(const beat_t) const;
};
