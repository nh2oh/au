#include "gtest/gtest.h"
#include "..\aulib\types\cent_oct_t.h"
#include <vector>
#include <string>
#include <cmath>


/*
// 
TEST(centTests, ACnameCompoundIntervals) {
	
	struct st_acname {
		int n {0};
		std::string acname {};
		std::string fcname {};
	};
	// 18 => d12 && A11; 24 => P15, double-octave
	std::vector<st_acname> ans {{1200,"d9","Diminished 9'th"},{1300,"m9","Minor 9'th"},
		{1400,"M9","Major 9'th"},{1500,"m10","Minor 10'th"},{1600,"M10","Major 10'th"},
		{1700,"P11","Perfect 11'th"},{1800,"d12","Diminished 12'th"},
		{1900,"P12","Perfect 12'th"},{2000,"m13","Minor 13'th"},{2100,"M13","Major 13'th"},
		{2200,"m14","Minor 14'th"},{2300,"M14","Major 14'th"},{2400,"P15","Perfect 15'th"},
		{2500,"A15","Augmented 15'th"}
	};

	for (const auto& e : ans) {
		cent_t curr_cent {e.n};
		std::string curr_acname = curr_cent.whatever();

		EXPECT_EQ(curr_acname, e.acname)

	// Default-constructed value is 1 Hz
	frq_t one_hz {1,frq_t::unit::Hz};
	frq_t zeroargs {};
	EXPECT_TRUE(zeroargs == one_hz);
}
*/

