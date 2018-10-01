#pragma once
#include "metg_t.h"
#include "beat_bar_t.h"
#include "ts_t.h"
#include "nv_t.h"
#include "rp_t.h"
#include "teejee.h"
#include <vector>
#include <string>

//
// Class metg_uih
//

	struct metg_input_pg {
		ts_t ts {};
		std::vector<teejee::nv_ph> nvph {};
		std::vector<std::vector<double>> pg {};

		bool operator==(const metg_input_pg& rhs) const {
			return (ts== rhs.ts && nvph == rhs.nvph && pg == rhs.pg);
		};
	};

	struct metg_input_rp {
		ts_t ts {};
		rp_t rp {};

		bool operator==(const metg_input_rp& rhs) const {
			return (ts== rhs.ts && rp.to_duration_seq() == rhs.rp.to_duration_seq());
		};
	};

	struct metg_input_nvph {
		ts_t ts {};
		std::vector<d_t> nv {};
		std::vector<beat_t> ph {};

		bool operator==(const metg_input_nvph& rhs) const {
			return (ts==rhs.ts && nv==rhs.nv && ph==rhs.ph);
		};
	};

template <typename T>
class metg_uih {
public:
	// Constructors
	metg_uih();
	void update(T);

	// -----------
	bool is_valid() const { return m_is_valid; };
	std::string msg() const { return m_msg; };
	tmetg_t get() const { return m_metg; };
private:
	T m_uinput_last {};
	bool m_is_default {true};
	bool m_is_valid {false};
	std::string m_msg {"Default-constructed"};
	tmetg_t m_metg {ts_t{4_bt,d::q},{d::w,d::h,d::q},{0_bt,0_bt,0_bt}};
};


