#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\smf_t.h"
#include <cstdint>
#include <array>
#include <filesystem>
#include <cmath>
#include <vector>

// 
// Tests of:
// ...
// 
TEST(midi_tempo_and_timediv_tests, interconversionSecondsTicks) {
	std::vector<time_division_t> tdivs {
		// Small values
		time_division_t(1),time_division_t(2),time_division_t(3),
		// "Normal" values
		time_division_t(24),time_division_t(25),time_division_t(48),
		time_division_t(96),time_division_t(152),
		// Max allowed value for ticks-per-qnt
		time_division_t(0x7FFF)
	};
	std::vector<int32_t> tempos {1,2,3,250000,500000,750000,0xFFFFFF};//,0};
	std::vector<int32_t> tks {1,2,3,154,4287,24861,251111,52467,754714,
		0x7FFFFFFF,0};
	
	double max_permissible_err_s = 1.0/1000000000.0;  // 1 ns
	double cum_err = 0.0;
	for (const auto& tdiv: tdivs) {
		for (const auto& tempo: tempos) {
			for (const auto& tk : tks) {
				double curr_tpq = static_cast<double>(tdiv.get_tpq());  // tks/q-nt
				double curr_uspq = static_cast<double>(tempo);  // us/q-nt
				double curr_spq = curr_uspq/1000000.0;  // us/q-nt
				double curr_sptk = curr_spq/curr_tpq;  // "seconds-per-tick"

				auto expect_s = (tk*curr_spq)/curr_tpq;
				
				auto s = ticks2sec(tk,tdiv,tempo);
				auto err = std::abs(s-expect_s);
				
				EXPECT_LT(err,max_permissible_err_s);
			}
		}
	}
	//auto cum_sec = ticks2sec(cum_num_tks,tc.tdiv,tc.tempo);
	//auto cum_tk = sec2ticks(cum_sec,tc.tdiv,tc.tempo);
}



