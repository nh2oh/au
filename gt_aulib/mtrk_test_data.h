#pragma once
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>


namespace mtrk_tests {

struct mtrk_properties_t {
	uint32_t size {0};  // nevents
	uint64_t duration_tks {0};
};

struct eventdata_with_cumtk_t {
	std::vector<unsigned char> d {};
	uint64_t cumtk {};
};
struct eventdata_with_cumtk_and_tkonset_t {
	std::vector<unsigned char> d {};
	uint64_t cumtk {};
	uint64_t tkonset {};
};
// Test set a
// Created June 09 2019
extern std::vector<eventdata_with_cumtk_t> tsa;
extern mtrk_properties_t tsa_props;

// Test set b
// Created June 15, 2019
extern std::vector<eventdata_with_cumtk_and_tkonset_t> tsb;


};  // namespace mtrk_tests

