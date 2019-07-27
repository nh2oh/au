#include "delta_time_test_data.h"
#include <vector>
#include <cstdint>
#include <limits>

// Set A:  Positive values [0, limits<int32_t>::max()]
const std::vector<dt_test_data_t> dt_test_set_a {
	{0x00, 1, 0x00},
	{0x40, 1, 0x40},
	{0x7F, 1, 0x7F},
	{0x80, 2, 0x80},
	{0x2000, 2, 0x2000},
	{0x3FFF, 2, 0x3FFF},
	{0x4000, 3, 0x4000},
	{0x100000, 3, 0x100000},
	{0x1FFFFF, 3, 0x1FFFFF},
	{0x00200000, 4, 0x00200000},
	{0x08000000, 4, 0x08000000},
	{0x0FFFFFFF, 4, 0x0FFFFFFF},  // 268,435,455;  Max allowed dt value
	{0x10000000, 4, 0x0FFFFFFF},  // Exceeds the max allowed dt value by 1
	{2147483647, 4, 0x0FFFFFFF}  // max value for an int32_t
};
// Set B:  Negative values [limits<int32_t>::min(), 0]
const std::vector<dt_test_data_t> dt_test_set_b {
	{0x00, 1, 0},
	{-0x40, 1, 0},
	{-0x7F, 1, 0},
	{-0x80, 1, 0},
	{-0x2000, 1, 0},
	{-0x3FFF, 1, 0},
	{-0x4000, 1, 0},
	{-0x100000, 1, 0},
	{-0x1FFFFF, 1, 0},
	{-0x00200000, 1, 0},
	{-0x08000000, 1, 0},
	{-0x0FFFFFFF, 1, 0},  // -268,435,455;  -1*Max value
	{-268435456, 1, 0},  // -268,435,455;  -1*Max value  
	{std::numeric_limits<int32_t>::min(), 1, 0}  // -2,147,483,648; min value for an int32_t
};
// Set C:  Positive values on [0, limits<int32_t>::max()], a few negative 
// values on [limits<int32_t>::min(), 0]
const std::vector<dt_test_data_t> dt_test_set_c {
	{0x08000000, 4, 0x08000000},
	{0x00, 1, 0x00},
	{0x3FFF, 2, 0x3FFF},
	{0x10000000, 4, 0x0FFFFFFF},
	{0x7F, 1, 0x7F},
	{0x1FFFFF, 3, 0x1FFFFF},
	{0x80, 2, 0x80},
	{0x2000, 2, 0x2000},
	{0x4000, 3, 0x4000},
	{-0x1FFFFF, 1, 0},
	{0x40, 1, 0x40},
	{0x00200000, 4, 0x00200000},
	{0x100000, 3, 0x100000},
	{-0x4000, 1, 0},
	{0x0FFFFFFF, 4, 0x0FFFFFFF},  // 268,435,455;  Max value
	{2147483647, 4, 0x0FFFFFFF}  // max value for an int32_t
};

/*
Set D:  Values that overflow an int32_t
TODO:  Need a new dt_test_data_t w/ types able to hold values that
// overflow an int32_t.  
std::vector<dt_test_data_t> dt_test_set_d {
	{0x0FFFFFFFu, 4, 0x0FFFFFFFu},  // 268,435,455;  Max value
	{0x1FFFFFFFu, 4, 0x0FFFFFFFu},
	{0x2FFFFFFFu, 4, 0x0FFFFFFFu},
	{0x7FFFFFFFu, 4, 0x0FFFFFFFu},
	{0x8F'FF'FF'FFu, 4, 0x0FFFFFFFu},
	{0x9FFFFFFFu, 4, 0x0FFFFFFFu},
	{0xBFFFFFFFu, 4, 0x0FFFFFFFu},
	{0xEFFFFFFFu, 4, 0x0FFFFFFFu},
	{0xFFFFFFFFu, 4, 0x0FFFFFFFu}
};
*/
