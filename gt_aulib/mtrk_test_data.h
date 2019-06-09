#pragma once
#include <vector>
#include <cstdint>


namespace mtrk_tests {

struct mtrk_properties_t {
	uint32_t size {0};  // nevents
	uint64_t duration_tks {0};

};

struct event_sequence_with_cumtk_t {
	std::vector<unsigned char> d {};
	uint64_t cumtk {};
};

// Test set a
// Created June 09 2019
extern std::vector<event_sequence_with_cumtk_t> tsa;


};  // namespace mtrk_tests

