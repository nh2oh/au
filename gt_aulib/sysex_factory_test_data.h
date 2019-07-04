#pragma once
#include <vector>
#include <cstdint>


struct sysex_test_set_a_t {
	uint32_t dt_in {0};
	std::vector<unsigned char> payload_in {};

	uint32_t ans_dt {0};
	uint32_t ans_pyld_len {0};
};

extern std::vector<sysex_test_set_a_t> f0_tests_no_terminating_f7_on_pyld;


