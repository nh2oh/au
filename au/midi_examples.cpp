#include "midi_examples.h"
#include "midi_raw.h"
#include "mthd_container_t.h"
#include "mtrk_container_t.h"
#include "smf_container_t.h"
#include "midi_container.h"
#include "midi_utils.h"
#include "dbklib\binfile.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <type_traits>

int midi_example() {
	/*struct big_t {
		unsigned char *p;
		uint64_t size;
		uint64_t capacity;
	};  // sizeof() == 24
	struct small_t {
		std::array<unsigned char,23> a;
		unsigned char f;
	};

	constexpr auto uorb = std::has_unique_object_representations<big_t>::value;
	constexpr auto xb = sizeof(big_t);
	constexpr auto ab = alignof(big_t);
	constexpr auto ao = alignof(int64_t);
	small_t yay {};
	constexpr auto xy = sizeof(yay);
	constexpr auto xst = sizeof(small_t);
	constexpr auto ast = alignof(small_t);
	constexpr auto xs = sizeof(std::string);
	constexpr auto xv = sizeof(std::vector<int>);
	constexpr auto xa = sizeof(std::array<char,2>);*/
	test_midi_vl_field_equiv_value();

	auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID").d;
	//auto rawfiledata = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\test.mid").d;
	auto rawfile_check_result = validate_smf(&rawfiledata[0],rawfiledata.size(),
		"C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID");

	smf_container_t mf {rawfile_check_result};
	
	std::cout << print(mf) << std::endl << std::endl;
	//std::cout << print_notelist(mf.get_track(1)) << std::endl << std::endl;

	//auto h = mf.get_header();
	//std::cout << print(h) << std::endl << std::endl;
	//auto t1 = mf.get_track(0);
	//std::cout << "TRACK 1\n" << print(t1) << std::endl << std::endl;
	//auto t2 = mf.get_track(1);
	//std::cout << "TRACK 2\n" << print(t2) << std::endl << std::endl;
	//auto t3 = mf.get_track(2);
	//std::cout << "TRACK 3\n" << print(t3) << std::endl << std::endl;

	return 0;
}


