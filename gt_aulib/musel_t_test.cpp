#include "gtest/gtest.h"
#include "..\aulib\types\musel_t.h"
#include "..\aulib\types\chord_t.h"
#include "..\aulib\types\scd_t.h"
#include "..\aulib\types\ntl_t.h"
#include <vector>



// ...
TEST(musel_t_tests, ChordsAndSingleNotesOfTypeScd) {
	bool tf {false};
	std::vector<scd_t> vs1 {scd_t{2},scd_t{13},scd_t{8},scd_t{10}};
	std::vector<scd_t> vs2 {scd_t{1},scd_t{3},scd_t{5},scd_t{7}};
	std::vector<scd_t> vs3 {scd_t{0},scd_t{2},scd_t{4},scd_t{6},scd_t{8}};
	chord_t<scd_t> c1 {vs1};
	chord_t<scd_t> c2 {vs2};
	chord_t<scd_t> c3 {vs3};

	musel_t<scd_t> me_c1 {c1,false};
	tf=me_c1.isrest(); EXPECT_FALSE(tf);
	tf = me_c1.n()==c1.n(); EXPECT_TRUE(tf);
	for (int i=0; i<me_c1.n(); ++i) {
		tf=me_c1[i] == c1[i]; EXPECT_TRUE(tf);
	}

	musel_t<scd_t> me_c2 {c2,false};
	EXPECT_TRUE(me_c2.isrest()==false);
	EXPECT_TRUE(me_c2.n()==c2.n());
	for (int i=0; i<me_c2.n(); ++i) {
		EXPECT_TRUE(me_c2[i] == c2[i]);
	}

	musel_t<scd_t> me_c3 {c3,false};
	EXPECT_TRUE(me_c3.isrest()==false);
	EXPECT_TRUE(me_c3.n()==c3.n());
	for (int i=0; i<me_c3.n(); ++i) {
		EXPECT_TRUE(me_c3[i] == c3[i]);
	}

	musel_t<scd_t> me_s1 {scd_t{4},false};
	tf=me_s1.isrest(); EXPECT_FALSE(tf);
	tf = me_s1.n()==1; EXPECT_TRUE(tf);
	tf = me_s1[0] == scd_t{4}; EXPECT_TRUE(tf);
	
}


// ...
TEST(musel_t_tests, ChordsAndSingleNotesOfTypeNtl) {
	std::vector<ntl_t> vn1 {"C"_ntl,"E"_ntl,"G"_ntl,"F#"_ntl};
	std::vector<ntl_t> vn2 {"A"_ntl,"B"_ntl,"C#"_ntl,"D#"_ntl};
	chord_t<ntl_t> c1 {vn1};

	musel_t<ntl_t> me_c1 {c1,false};
	EXPECT_FALSE(me_c1.isrest());
	EXPECT_TRUE(me_c1.n()==c1.n());
	for (int i=0; i<me_c1.n(); ++i) {
		EXPECT_TRUE(me_c1[i] == c1[i]);
	}

	for (int i=0; i<vn2.size(); ++i) {
		musel_t<ntl_t> curr_me {vn2[i],false};
		EXPECT_FALSE(curr_me.isrest());
		EXPECT_TRUE(curr_me.n()==1);
		EXPECT_TRUE(curr_me[0] == vn2[i]);
	}

}










