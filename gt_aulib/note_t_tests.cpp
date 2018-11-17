#include "gtest/gtest.h"
#include "..\aulib\types\ntl_t.h"
#include <string>
#include <vector>



TEST(note_t_tests, DefaultCtor) {
	note_t def_nt {};
	EXPECT_EQ(def_nt.ntl,ntl_t{"C"});
	EXPECT_EQ(def_nt.oct,octn_t{0});
	EXPECT_EQ(def_nt.frq,frq_t{16.352});
}


TEST(note_t_tests, ConstructFromNtlOctFrq) {
	note_t a4440 {ntl_t{"A"},octn_t{4},frq_t{440}};
	EXPECT_EQ(a4440.ntl,ntl_t{"A"});
	EXPECT_EQ(a4440.oct,octn_t{4});
	EXPECT_EQ(a4440.frq,frq_t{440});

	// A few random "weird" notes
	note_t weird_a {ntl_t{"A_w"},octn_t{-4},frq_t{2458.0504}};
	EXPECT_EQ(weird_a.ntl,ntl_t{"A_w"});
	EXPECT_EQ(weird_a.oct,octn_t{-4});
	EXPECT_EQ(weird_a.frq,frq_t{2458.0504});

	note_t weird_b {ntl_t{"V[7]_w[-015]"},octn_t{8},frq_t{1}};
	EXPECT_EQ(weird_b.ntl,ntl_t{"V[7]_w[-015]"});
	EXPECT_EQ(weird_b.oct,octn_t{8});
	EXPECT_EQ(weird_b.frq,frq_t{1});
}


TEST(note_t_tests, OperatorEqAndNeq) {
	note_t def_nt {};
	note_t a4440 {ntl_t{"A"},octn_t{4},frq_t{440}};
	EXPECT_TRUE(def_nt != a4440);
	EXPECT_FALSE(def_nt == a4440);

	// A few random "weird" notes
	note_t weird_a {ntl_t{"A_w"},octn_t{-4},frq_t{2458.0504}};
	note_t weird_a_same {ntl_t{"A_w"},octn_t{-4},frq_t{2458.0504}};
	EXPECT_TRUE(weird_a != def_nt);
	EXPECT_FALSE(weird_a == def_nt);
	EXPECT_EQ(weird_a,weird_a_same);

	note_t weird_b {ntl_t{"V[7]_w[-015]"},octn_t{8},frq_t{1}};
	note_t weird_b_same {ntl_t{"V[7]_w[-015]"},octn_t{8},frq_t{1}};
	EXPECT_TRUE(weird_b != def_nt);
	EXPECT_TRUE(weird_b != weird_a);
	EXPECT_FALSE(weird_b == def_nt);
	EXPECT_FALSE(weird_b == weird_a);
	EXPECT_EQ(weird_b,weird_b_same);
}


TEST(note_t_tests, ParseNtstrNonmemberFunctionLegalNoOct) {
	std::vector<std::string> legal_nooct {  // oct not specified
		"C-","_C##","D_--12","D#1##2]","[E]","F[-7]","F[##_#-#6#]",
		"G","G##","A_____","-_-","--B-"
	};

	for (const auto& e : legal_nooct) {
		auto curr_ntparse = parse_ntstr(e);
		EXPECT_TRUE(curr_ntparse.is_valid);
		EXPECT_FALSE(curr_ntparse.is_oct_set);
		EXPECT_EQ(curr_ntparse.ntl,ntl_t{e});
		EXPECT_EQ(curr_ntparse.oct,octn_t{0});
	}

}


TEST(note_t_tests, ParseNtstrNonmemberFunctionLegalWithOct) {
	struct str_oct_pair {
		std::string str {};
		std::string str_nooct {};
		int oct {};
	};
	std::vector<str_oct_pair> legal_oct {  // oct specified
		{"_C##(0)","_C##",0},{"C-(0)","C-",0},{"D_--12(-1)","D_--12",-1},
		{"D#1##2](2)","D#1##2]",2},{"[E](10)","[E]",10},{"[E](00)","[E]",0},
		{"F[-7](-3)","F[-7]",-3},{"F[##_#-#6#](-11)","F[##_#-#6#]",-11},
		{"_C##(-0)","_C##",0},{"_C##(-00)","_C##",0}
	};
	for (const auto& e : legal_oct) {
		auto curr_ntparse = parse_ntstr(e.str);
		EXPECT_TRUE(curr_ntparse.is_valid);
		EXPECT_TRUE(curr_ntparse.is_oct_set);
		EXPECT_EQ(curr_ntparse.ntl,ntl_t{e.str_nooct});
		EXPECT_EQ(curr_ntparse.oct,octn_t{e.oct});
	}

}


TEST(note_t_tests, ParseNtstrNonmemberFunctionIllegal) {
	std::vector<std::string> illegal  {
		"", " ", "	",    // Illegal ntls
		" C","C #","D ","D	#","E(","F(--5)","(-2)F#",    // Illegal ntls
		"\\0G","G/#","A%","A!#","B*",  // Illegal ntls
		"C$","D &",	"G.","G#.#","A__^___","-%_-","--~B-",    // Illegal ntls
		"F()"  // Empty oct-spec parens
	};
	for (const auto& e : illegal) {
		auto curr_ntparse = parse_ntstr(e);
		EXPECT_FALSE(curr_ntparse.is_valid);
		EXPECT_FALSE(curr_ntparse.is_oct_set);
	}

}

