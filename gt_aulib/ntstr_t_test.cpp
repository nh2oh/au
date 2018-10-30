#include "gtest/gtest.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"  // Defines octn_t
#include <string>
#include <vector>

TEST(ntstr_t_tests, ConstructFromConstCharStarImplicitZeroOctave) {
	ntstr_t a {"C"};
	ntstr_t b {"C#"};
	ntstr_t c {"C#####"};
	ntstr_t d {"D&"};
	ntstr_t e {"D&&&&&"};

	EXPECT_EQ(a.print(),std::string{"C(0)"});
	EXPECT_EQ(b.print(),std::string{"C#(0)"});
	EXPECT_EQ(c.print(),std::string{"C#####(0)"});
	EXPECT_EQ(d.print(),std::string{"D&(0)"});
	EXPECT_EQ(e.print(),std::string{"D&&&&&(0)"});

	ntstr_t az {"C(0)"};
	ntstr_t bz {"C#(0)"};
	ntstr_t cz {"C#####(0)"};
	ntstr_t dz {"D&(00)"};
	ntstr_t ez {"D&&&&&(-0)"};
	EXPECT_EQ(az.print(),std::string{"C(0)"});
	EXPECT_EQ(bz.print(),std::string{"C#(0)"});
	EXPECT_EQ(cz.print(),std::string{"C#####(0)"});
	EXPECT_EQ(dz.print(),std::string{"D&(0)"});
	EXPECT_EQ(ez.print(),std::string{"D&&&&&(0)"});

	EXPECT_EQ(az,a);
	EXPECT_EQ(bz,b);
	EXPECT_EQ(cz,c);
	EXPECT_EQ(dz,d);
	EXPECT_EQ(ez,e);
}

TEST(ntstr_t_tests, CChromaticAllNotesManuallySetOctave) {
	std::vector<std::string> vs_sharps {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
	std::vector<std::string> vs_flats {"C","D&","D","E&","E","F","G&","G","A&","A","B&","B"};
	
	// Constructing from a string specifying the ntl and an int specifying the octave
	// gives the same result a construction from a string specifying both ntl and 
	// octave, ien "ntl(oct)"

	std::vector<ntstr_t> v_sharps {};
	std::vector<ntstr_t> v_flats {};
	std::vector<int> oct {};
	int curr_oct {-5};
	for (int i=0; i<vs_sharps.size(); ++i) {
		oct.push_back(curr_oct);
		v_sharps.push_back(ntstr_t{vs_sharps[i],curr_oct});
		v_flats.push_back(ntstr_t{vs_flats[i],curr_oct});
		++curr_oct;
	}

	for (int i=0; i<vs_sharps.size(); ++i) {
		std::string ans_sharp = vs_sharps[i] + "(" + std::to_string(oct[i]) + ")";
		std::string ans_flat = vs_flats[i] + "(" + std::to_string(oct[i]) + ")";
		EXPECT_EQ(v_sharps[i],ntstr_t{ans_sharp});
		EXPECT_EQ(v_flats[i],ntstr_t{ans_flat});

		// Test of print()
		EXPECT_EQ(ans_sharp,v_sharps[i].print());
		EXPECT_EQ(ans_flat,v_flats[i].print());

		// Test of oct() and ntl()
		EXPECT_EQ(octn_t{oct[i]},v_sharps[i].oct());
		EXPECT_EQ(octn_t{oct[i]},v_flats[i].oct());
		EXPECT_EQ(ntl_t{vs_sharps[i]},v_sharps[i].ntl());
		EXPECT_EQ(ntl_t{vs_flats[i]},v_flats[i].ntl());
	}

}



