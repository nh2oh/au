#include "gtest/gtest.h"
#include "..\aulib\types\ntl_t.h"
#include <string>
#include <vector>

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



