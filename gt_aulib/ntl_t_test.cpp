#include "gtest/gtest.h"
#include "..\aulib\types\ntl_t.h"
#include "..\aulib\types\scd_t.h"  // Defines octn_t
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
		EXPECT_EQ(vs_sharps[i],v_sharps[i].print());
		EXPECT_EQ(vs_flats[i],v_flats[i].print());
	}

}


TEST(ntl_t_tests, NumbersBracketsDashesUnderscores) {
	std::vector<std::string> weird {"C-","_C##","D_--12","D#1##2]","[E]","F[-7]","F[##_#-#6#]",
		"G","G##","A_____","-_-","--B-"};
	
	for (int i=0; i<weird.size(); ++i) {
		ntl_t curr_ntl {weird[i]};
		EXPECT_EQ(curr_ntl.print(),weird[i]);
	}

}


TEST(ntl_t_tests, IllegalChars) {
	std::vector<std::string> illegal  {"", " ", "	", 
		" C","C #","D ","D	#","E(","F(5)","(-2)F#","\\0G","G/#","A%","A!#","B*",
		"C$","D &",	"G.","G#.#","A__^___","-%_-","--~B-"};
	
	for (int i=0; i<illegal.size(); ++i) {
		EXPECT_FALSE(is_valid_ntl(illegal[i])) << "illegal[i] == " << illegal[i];
	}

}

TEST(ntl_t_tests, ParseSPNValidNtlsOctsNotSet) {
	struct test_set {
		std::string test_ntstr {};
		std::string ntl_base_str {};
		int nsharp {0};
		int nflat {0};
	};
	std::vector<test_set> valid {
		{"C","C",0,0},{"D","D",0,0},{"G","G",0,0},{"A","A",0,0},
		{"C#","C",1,0},{"D&","D",0,1},{"G#","G",1,0},
			{"A&","A",0,1},
		{"C##","C",2,0},{"D&&&","D",0,3},{"G#&#&#&","G",3,3},
			{"A##&&###&#&&#&","A",7,6}
	};

	for (int i=0; i<valid.size(); ++i) {
		auto res = parse_spn_ntstr(valid[i].test_ntstr);

		EXPECT_TRUE(res.is_valid);
		EXPECT_FALSE(res.is_oct_set);
		EXPECT_EQ(res.ntl_base_str, valid[i].ntl_base_str);
		EXPECT_EQ(res.nsharp, valid[i].nsharp);
		EXPECT_EQ(res.nflat, valid[i].nflat);
	}
}

TEST(ntl_t_tests, ParseSPNValidNtlsOctsSet) {
	struct test_set {
		std::string test_ntstr {};
		std::string ntl_base_str {};
		int nsharp {0};
		int nflat {0};
		int oct {0};
	};
	std::vector<test_set> valid {
		{"C(0)","C",0,0,0},{"D(-3)","D",0,0,-3},{"G(5)","G",0,0,5},
		{"A(0)","A",0,0,0},
		{"C#(0)","C",1,0,0},{"D&(-3)","D",0,1,-3},{"G#(5)","G",1,0,5},
			{"A&(0)","A",0,1,0},
		{"C##(0)","C",2,0,0},{"D&&&(-3)","D",0,3,-3},{"G#&#&#&(5)","G",3,3,5},
			{"A##&&###&#&&#&(0)","A",7,6,0}
	};

	for (int i=0; i<valid.size(); ++i) {
		auto res = parse_spn_ntstr(valid[i].test_ntstr);

		EXPECT_TRUE(res.is_valid);
		EXPECT_TRUE(res.is_oct_set);

		// Can't construct an ntl w/an octave specifier (can construct a note_t, 
		// but not a naked ntl_t).  
		//std::string ntstr_no_oct = parse_ntlstr(valid[i].test_ntstr).ntl_str;
		EXPECT_EQ(res.ntl_base_str, valid[i].ntl_base_str);
		EXPECT_EQ(res.nsharp, valid[i].nsharp);
		EXPECT_EQ(res.nflat, valid[i].nflat);
		EXPECT_EQ(res.oct, valid[i].oct);
	}
}


// Valid ntl's, but not SPN ntl's
// For non-SPN ntl's, terminal #,& chars are not considered to be sharp,flat modifiers,
// since they are otherwise valid ntl chars, hence nflat=nsharp=0 for all these 
// examples.  Note also that the base_ntl contains the #,& chars.  
TEST(ntl_t_tests, ParseSPNNonSPNNtlsOctsSet) {
	struct test_set {
		std::string test_ntstr {};
		std::string base_ntl {};
		int nsharp {0};
		int nflat {0};
		int oct {0};
	};
	std::vector<test_set> valid {
		{"c(0)","c",0,0,0},{"R(-3)","R",0,0,-3},{"GG(5)","GG",0,0,5},
		{"A1(0)","A1",0,0,0},
		{"c#(0)","c#",0,0,0},{"R&(-3)","R&",0,0,-3},{"GG#(5)","GG#",0,0,5},
			{"A1&(0)","A1&",0,0,0},
		{"c##(0)","c##",0,0,0},{"R&&&(-3)","R&&&",0,0,-3},
			{"GG#&#&#&(5)","GG#&#&#&",0,0,5},
			{"A1##&&###&#&&#&(0)","A1##&&###&#&&#&",0,0,0}
	};

	for (int i=0; i<valid.size(); ++i) {
		auto res = parse_spn_ntstr(valid[i].test_ntstr);
		EXPECT_FALSE(res.is_valid);
	}
}





/*





// Valid ntl's, but not SPN ntl's
// For non-SPN ntl's, terminal #,& chars are not considered to be sharp,flat modifiers,
// since they are otherwise valid ntl chars, hence nflat=nsharp=0 for all these 
// examples.  Note also that the base_ntl contains the #,& chars.  
TEST(ntl_t_tests, ParseSPNNonSPNNtlsOctsSet) {
	struct test_set {
		std::string test_ntstr {};
		std::string base_ntl {};
		int nsharp {0};
		int nflat {0};
		int oct {0};
	};
	std::vector<test_set> valid {
		{"c(0)","c",0,0,0},{"R(-3)","R",0,0,-3},{"GG(5)","GG",0,0,5},
		{"A1(0)","A1",0,0,0},
		{"c#(0)","c#",0,0,0},{"R&(-3)","R&",0,0,-3},{"GG#(5)","GG#",0,0,5},
			{"A1&(0)","A1&",0,0,0},
		{"c##(0)","c##",0,0,0},{"R&&&(-3)","R&&&",0,0,-3},
			{"GG#&#&#&(5)","GG#&#&#&",0,0,5},
			{"A1##&&###&#&&#&(0)","A1##&&###&#&&#&",0,0,0}
	};

	for (int i=0; i<valid.size(); ++i) {
		auto res = parse_spn_ntstr(valid[i].test_ntstr);

		EXPECT_TRUE(res.is_valid);
		EXPECT_TRUE(res.is_oct_set);

		// Can't construct an ntl w/an octave specifier (can construct a note_t, 
		// but not a naked ntl_t).  
		std::string ntstr_no_oct = parse_ntlstr(valid[i].test_ntstr).ntl_str;
		EXPECT_EQ(res.ntl_base_str, ntstr_no_oct);
		EXPECT_EQ(res.nsharp, valid[i].nsharp);
		EXPECT_EQ(res.nflat, valid[i].nflat);
		EXPECT_EQ(res.oct, valid[i].oct);
	}
}



*/


