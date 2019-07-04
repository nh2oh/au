#include "sysex_factory_test_data.h"
#include <vector>
#include <cstdint>

// ans_pyld_len == payload_in.size()+1; the factory should add a single
// 0xF7u to the end of the input payload.  
std::vector<sysex_test_set_a_t> f0_tests_no_terminating_f7_on_pyld {
	// Fits in the small-buffer
	{0, {0x01u,0x02u,0x03u,0x04u}, 0, 5},
	// Fits in small buffer; dt.size() => 2
	{0x000000FFu, {0x01u,0x02u,0x03u,0x04u}, 0x000000FFu, 5},
	// Fits in small buffer; dt.size() => 4
	{0x0FFFFFFFu, {0x01u,0x02u,0x03u,0x04u}, 0x0F'FF'FF'FFu, 5},
	// Fits in small buffer; dt.size() => 4; note that dt.in > max 
	// allowed dt
	{0xFFFFFFFFu, {0x01u,0x02u,0x03u,0x04u}, 0x0FFFFFFFu, 5},
	// Total size = 1(dt) + 1(f0) + 1(len) + 18 == 21; Fits in the small buffer
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u}, 0, 18},
	// Total size = 1(dt) + 1(f0) + 1(len) + 19 == 22; Fits in the small buffer
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u}, 0, 19},
	// Total size = 1(dt) + 1(f0) + 1(len) + 20 == 23; Fits in the small buffer
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u}, 0, 20},
	// Total size = 1(dt) + 1(f0) + 1(len) + 21 == 24; Exceeds the small 
	// buffer, but note that the payload_in.size() is < sbo.size()
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u,0x04u}, 0, 21},
	// Total size = 1(dt) + 1(f0) + 1(len) + 22 == 25; Exceeds the small 
	// buffer, but note that the payload_in.size() is < sbo.size()
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u,0x04u, 0x01u}, 0, 22},
	// Exceeds the small-buffer; payload_in.size() > sbo.size()
	{0, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u}, 0, 33},
	// Exceeds the small-buffer; payload_in.size() > sbo.size();
	// dt size == 4
	{0x0FFFFFFFu, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u}, 0x0FFFFFFFu, 33},
	// Exceeds the small-buffer; payload_in.size() > sbo.size();
	// dt size == 4, dt value > max allowed
	{0xAFFFFFFFu, {0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u, 
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u,
		0x01u,0x02u,0x03u,0x04u, 0x01u,0x02u,0x03u,0x04u}, 0x0FFFFFFFu, 33},
};


