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
class teejee {
public:
	struct nv_ph {  // Must be public b/c returned by levels()
		explicit nv_ph(const d_t, const d_t);

		d_t nv {d::q};
		d_t ph {d::z};  // Phase is always >= 0 && < nv.  
		bool operator==(const nv_ph&) const;
		bool operator<(const nv_ph&) const;
		bool operator>(const nv_ph&) const;
		// These operators sort by decreasing duration then 
		// increasing phase.  

		d_t phase(const d_t&, const d_t&) const; // phase(nv,offset)
		bool validate() const;
	};

	explicit teejee(const ts_t&, const std::vector<d_t>&, 
		const std::vector<beat_t>&, bool); // ts, dp, phases, allow barspan
	explicit teejee(const ts_t&, const rp_t&);

	ts_t ts() const;
	std::vector<nv_ph> levels() const;
	bar_t period() const; // Reads m_ts, m_nvsph, m_btres
		// Will always convert to an integer number of all m_nvsph elements (???)
	bool f_barspan();

	std::vector<beat_t> factors_at() const;
	int count() const;  // Count number of possible rp's
	bool onset_allowed_at(const beat_t) const;  // is there @ least 1 member allowed at beat?
	bool onset_allowed_at(const nv_ph, const beat_t) const; // nv_ph need not be a member
	bool span_possible(const bar_t) const;
	bool span_possible(const beat_t) const;

	std::string print() const;
	std::string print_g() const;

	bool operator==(const teejee&) const;
private:
	//----------------------------------------------------------------------------
	// Data
	ts_t m_ts {4_bt,d_t{d::q}};
	std::vector<nv_ph> m_levels {};
		// All elements unique & sorted by decreasing duration then 
		// increasing phase.  Phase is always >= 0 && < nv.  

	beat_t m_btres {0.0};
	beat_t m_period {0.0};
	bool m_f_barspan {false};

	//----------------------------------------------------------------------------
	// Methods
	bool insert_level(const d_t&, const d_t&);  // nv, ph
		// Inserts into m_levels, but does _not_ recalc the resolution or period.


	bool tg(d_t, d_t, beat_t) const;

	beat_t gres() const;
		// Computes the grid resolution in beats for the current m_ts, m_nvsph.

	int bt2step(const beat_t) const;
	int nv2step(const d_t&) const;
	beat_t step2bt(const int) const;
	d_t step2nv(const int) const;

};
