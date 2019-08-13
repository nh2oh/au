#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include <cstdint>
#include <cmath>
#include <vector>
#include <limits>


// 
// Tests of ticks2sec() and sec2ticks()
// 
TEST(midi_tempo_and_timediv_tests, maxErrorTickSecondInterconversion) {
	std::vector<time_division_t> tdivs {
		// Small values
		time_division_t(1),time_division_t(2),time_division_t(3),
		// "Normal" values
		time_division_t(24),time_division_t(25),time_division_t(48),
		time_division_t(96),time_division_t(152),
		// Max allowed value for ticks-per-qnt
		time_division_t(0x7FFF)
	};
	std::vector<int32_t> tempos {0,1,2,3,250000,500000,750000,0xFFFFFF};
	std::vector<int32_t> tks {0,1,2,3,154,4287,24861,251111,52467,754714,
		0x7FFFFFFF};

	for (const auto& tdiv: tdivs) {
		for (const auto& tempo: tempos) {
			for (const auto& tk : tks) {
				auto s_from_tk = ticks2sec(tk,tdiv,tempo);
				if (tempo==0) {
					// If the tempo is 0, sec obviously == 0 no matter the
					// value of tk, however, you can't go back to tk...
					continue;
				}
				// ticks2sec() returns a value within 1 ulp of the exact value
				auto max_err_computed_sec = 
					std::numeric_limits<double>::epsilon()*s_from_tk;
				auto s_from_tk_min = s_from_tk - max_err_computed_sec;
				auto s_from_tk_max = s_from_tk + max_err_computed_sec;

				auto tk_from_s = sec2ticks(s_from_tk,tdiv,tempo);
				auto tk_from_s_min = sec2ticks(s_from_tk_min,tdiv,tempo);
				auto tk_from_s_max = sec2ticks(s_from_tk_max,tdiv,tempo);

				double curr_tk = static_cast<double>(tk);
				EXPECT_GE(curr_tk,tk_from_s_min);
				EXPECT_LE(curr_tk,tk_from_s_max);
			}
		}
	}
}



