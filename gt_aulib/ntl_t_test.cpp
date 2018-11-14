#include "gtest/gtest.h"
#include "..\aulib\types\ntl_t.h"
#include <string>
#include <vector>



TEST(ntl_t_tests, DefaultCtor) {
	ntl_t def_ntl {};
	EXPECT_EQ(def_ntl,ntl_t{"C"});
	EXPECT_EQ(def_ntl.print(),std::string{"C"});
}


TEST(ntl_t_tests, ConstructFromConstCharStar) {
	ntl_t a {"C"};
	ntl_t b {"C#"};
	ntl_t c {"C#####"};
	ntl_t d {"D&"};
	ntl_t e {"D&&&&&"};

	EXPECT_EQ(a.print(),std::string{"C"});
	EXPECT_EQ(b.print(),std::string{"C#"});
	EXPECT_EQ(c.print(),std::string{"C#####"});
	EXPECT_EQ(d.print(),std::string{"D&"});
	EXPECT_EQ(e.print(),std::string{"D&&&&&"});
}

TEST(ntl_t_tests, CChromaticAllNotes) {
	std::vector<std::string> vs_sharps {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
	std::vector<std::string> vs_flats {"C","D&","D","E&","E","F","G&","G","A&","A","B&","B"};
	
	std::vector<ntl_t> v_sharps {};
	std::vector<ntl_t> v_flats {};
	for (int i=0; i<vs_sharps.size(); ++i) {
		v_sharps.push_back(ntl_t{vs_sharps[i]});
		v_flats.push_back(ntl_t{vs_flats[i]});
	}

	for (int i=0; i<vs_sharps.size(); ++i) {
		EXPECT_EQ(vs_sharps[i],v_sharps[i].print());
		EXPECT_EQ(vs_flats[i],v_flats[i].print());
	}

}

TEST(ntl_t_tests, MultiSharpMultiFlat) {
	std::vector<std::string> vs_sharps {"C","C##","D","D###","E","F","F#####","G","G##","A","A###","B"};
	std::vector<std::string> vs_flats {"C","D&&","D","E&&&","E","F","G&&&&&","G","A&&&&&&","A","B&","B"};
	
	std::vector<ntl_t> v_sharps {};
	std::vector<ntl_t> v_flats {};
	for (int i=0; i<vs_sharps.size(); ++i) {
		v_sharps.push_back(ntl_t{vs_sharps[i]});
		v_flats.push_back(ntl_t{vs_flats[i]});
	}

	for (int i=0; i<vs_sharps.size(); ++i) {
		EXPECT_TRUE(ntl_t::valid_string(vs_sharps[i]));
		EXPECT_EQ(vs_sharps[i],v_sharps[i].print());
		EXPECT_EQ(vs_flats[i],v_flats[i].print());
	}

}


TEST(ntl_t_tests, NumbersBracketsDashesUnderscores) {
	std::vector<std::string> weird {"C-","_C##","D_--12","D#1##2]","[E]","F[-7]","F[##_#-#6#]",
		"G","G##","A_____","-_-","--B-"};
	
	for (int i=0; i<weird.size(); ++i) {
		EXPECT_TRUE(ntl_t::valid_string(weird[i]));
		ntl_t curr_ntl {weird[i]};
		EXPECT_EQ(curr_ntl.print(),weird[i]);
	}

}


TEST(ntl_t_tests, IllegalChars) {
	std::vector<std::string> illegal  {"", " ", "	", 
		" C","C #","D ","D	#","E(","F(5)","(-2)F#","\\0G","G/#","A%","A!#","B*",
		"C$","D &",	"G.","G#.#","A__^___","-%_-","--~B-"};
	
	for (int i=0; i<illegal.size(); ++i) {
		EXPECT_FALSE(ntl_t::valid_string(illegal[i]));
	}

}

