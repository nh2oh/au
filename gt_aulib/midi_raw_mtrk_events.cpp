#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include <vector>
#include <array>
#include <cstdint>

namespace midi_raw_mtrk_events_tdata {


std::vector<std::vector<unsigned char>> delta_time {
	{0x00u},{0x64u},
	{0x83u,0x00u}
	//...
}
};  // namespace midi_raw_mtrk_events_tdata




//
// Tests for:
// unsigned char mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 1:  running-status is a valid midi status byte.
//
TEST(midi_raw_tests, GetMIDIStatusByteUnsafeValidRSB) {
	struct test_t {
		std::vector<unsigned char> data {};
		uint8_t dtsize {0};
		unsigned char rs {0x00u};
		unsigned char ans {0x00u};
	};

	std::vector<test_t> tests {
		// Meta events; ans == 0x00u
		// text events
		{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0xA2u,0x00u},
		{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0xB2u,0x00u},
		{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0xC2u,0x00u},
		{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0xC2u,0x00u},
		// time signature
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xA2u,0x00u},
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xB2u,0x00u},
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xC2u,0x00u},
		// end of track
		{{0x00u,0xFFu,0x2Fu,0x00u},1,0xA2u,0x00u},
		{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0xB2u,0x00u},
		{{0x00u,0xFFu,0x2Fu,0x00u},1,0xC2u,0x00u},
		{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0xC2u,0x00u},

		// Sysex_f0/f7 events; ans == 0x00u
		// sysex_f0 from p. 136 of the midi std:
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xA2u,0x00u},
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xB2u,0x00u},
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xC2u,0x00u},
		// sysex_f7 from p. 136 of the midi std:
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xA2u,0x00u},
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xB2u,0x00u},
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xC2u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xA2u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xB2u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xC2u,0x00u},

		// __set 1__ midi_channel/voice events
		// ... _with_ event-local status byte == 0x90u
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xA2u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xB2u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xC2u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0x90u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0xA2u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0xB2u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0xC2u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0x90u,0x90u},
		// ... _without_ event-local status byte == 0x90u
		{{0x83u,0x00u,0x41u,0x00u},2,0xA2u,0xA2u},
		{{0x83u,0x00u,0x41u,0x00u},2,0xB2u,0xB2u},
		{{0x83u,0x00u,0x41u,0x00u},2,0xC2u,0xC2u},
		{{0x83u,0x00u,0x41u,0x00u},2,0x90u,0x90u},
		{{0x00u,0x41u,0x00u},1,0xA2u,0xA2u},
		{{0x00u,0x41u,0x00u},1,0xB2u,0xB2u},
		{{0x00u,0x41u,0x00u},1,0xC2u,0xC2u},
		{{0x00u,0x41u,0x00u},1,0x90u,0x90u},

		// __set 2__ midi_channel/voice events
		// ... _with_ event-local status byte == 0xC0u
		{{0x83u,0x00u,0xC0u,0x00u},2,0xA2u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0xB2u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0xC0u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0x90u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0xA2u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0xB2u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0xC0u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0x90u,0xC0u},
		// ... _without_ event-local status byte == 0xC0u
		{{0x83u,0x00u,0x00u},2,0xA2u,0xA2u},
		{{0x83u,0x00u,0x00u},2,0xB2u,0xB2u},
		{{0x83u,0x00u,0x00u},2,0xC0u,0xC0u},
		{{0x83u,0x00u,0x00u},2,0x90u,0x90u},
		{{0x00u,0x00u},1,0xA2u,0xA2u},
		{{0x00u,0x00u},1,0xB2u,0xB2u},
		{{0x00u,0x00u},1,0xC0u,0xC0u},
		{{0x00u,0x00u},1,0x90u,0x90u}
	};
	for (const auto& e : tests) {
		const unsigned char *p = &(e.data[0])+e.dtsize;
		auto s = mtrk_event_get_midi_status_byte_unsafe(p, e.rs);
		EXPECT_EQ(s,e.ans);
	}
}

//
// Tests for:
// unsigned char mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 2:  running-status is an _invalid_ midi status byte.
//
TEST(midi_raw_tests, GetMIDIStatusByteUnsafeInvalidRSB) {
	struct test_t {
		std::vector<unsigned char> data {};
		uint8_t dtsize {0};
		unsigned char rs {0x00u};
		unsigned char ans {0x00u};
	};

	std::vector<test_t> tests {
		// Meta events; ans == 0x00u
		// text events
		{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0x00u,0x00u},
		{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0x71u,0x00u},
		{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0xF0u,0x00u},
		{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0xFFu,0x00u},
		// time signature
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0x00u,0x00u},
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0x71u,0x00u},
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xF0u,0x00u},
		{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xFFu,0x00u},
		// end of track
		{{0x00u,0xFFu,0x2Fu,0x00u},1,0x00u,0x00u},
		{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0x71u,0x00u},
		{{0x00u,0xFFu,0x2Fu,0x00u},1,0xF0u,0x00u},
		{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0xFFu,0x00u},

		// Sysex_f0/f7 events; ans == 0x00u
		// sysex_f0 from p. 136 of the midi std:
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0x00u,0x00u},
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0x71u,0x00u},
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xF0u,0x00u},
		{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xFFu,0x00u},
		// sysex_f7 from p. 136 of the midi std:
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0x00u,0x00u},
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0x71u,0x00u},
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xF0u,0x00u},
		{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xFFu,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0x00u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0x71u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xF0u,0x00u},
		{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xFFu,0x00u},

		// __set 1__ midi_channel/voice events
		// ... _with_ event-local status byte == 0x90u
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0x00u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0x71u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xF0u,0x90u},
		{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xFFu,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0x00u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0x71u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0xF0u,0x90u},
		{{0x00u,0x90u,0x41u,0x00u},1,0xFFu,0x90u},
		// ... _without_ event-local status byte == 0x90u
		{{0x83u,0x00u,0x41u,0x00u},2,0x00u,0x00u},
		{{0x83u,0x00u,0x41u,0x00u},2,0x71u,0x00u},
		{{0x83u,0x00u,0x41u,0x00u},2,0xF0u,0x00u},
		{{0x83u,0x00u,0x41u,0x00u},2,0xFFu,0x00u},
		{{0x00u,0x41u,0x00u},1,0x00u,0x00u},
		{{0x00u,0x41u,0x00u},1,0x71u,0x00u},
		{{0x00u,0x41u,0x00u},1,0xF0u,0x00u},
		{{0x00u,0x41u,0x00u},1,0xFFu,0x00u},

		// __set 2__ midi_channel/voice events
		// ... _with_ event-local status byte == 0xC0u
		{{0x83u,0x00u,0xC0u,0x00u},2,0xF0u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0x71u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0xF0u,0xC0u},
		{{0x83u,0x00u,0xC0u,0x00u},2,0xFFu,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0x00u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0x71u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0xF0u,0xC0u},
		{{0x00u,0xC0u,0x00u},1,0xFFu,0xC0u},
		// ... _without_ event-local status byte == 0xC0u
		{{0x83u,0x00u,0x00u},2,0x00u,0x00u},
		{{0x83u,0x00u,0x00u},2,0x71u,0x00u},
		{{0x83u,0x00u,0x00u},2,0xF0u,0x00u},
		{{0x83u,0x00u,0x00u},2,0xFFu,0x00u},
		{{0x00u,0x00u},1,0x00u,0x00u},
		{{0x00u,0x00u},1,0x71u,0x00u},
		{{0x00u,0x00u},1,0xF0u,0x00u},
		{{0x00u,0x00u},1,0xFFu,0x00u}
	};
	for (const auto& e : tests) {
		const unsigned char *p = &(e.data[0])+e.dtsize;
		auto s = mtrk_event_get_midi_status_byte_unsafe(p, e.rs);
		EXPECT_EQ(s,e.ans);
	}
}



//
// Tests for:
// uint32_t mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 1:  running-status is a _valid_ midi status byte.
//
TEST(midi_raw_tests, GetMIDIStatusByteUnsafeInvalidRSB) {
	struct test_t {
		std::vector<unsigned char> data {};
		uint8_t dtsize {0};
		unsigned char rs {0x00u};
		unsigned char ans {0x00u};
	};

	std::vector<test_t> tests {
		// Meta events; ans == 0x00u
		// text events
		{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0xA2u,0x00u},
	{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0xB2u,0x00u},
	{{0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},1,0xC2u,0x00u},
	{{0x83u,0x00u,0xFFu,0x01u,0x09u,0x63u,0x72u,0x65u,0x61u,0x74u,0x6Fu,0x72u,0x3Au,0x20u},2,0xC2u,0x00u},
	// time signature
	{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xA2u,0x00u},
	{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xB2u,0x00u},
	{{0x00u,0xFFu,0x58u,0x04u,0x04u,0x02u,0x12u,0x08u},1,0xC2u,0x00u},
	// end of track
	{{0x00u,0xFFu,0x2Fu,0x00u},1,0xA2u,0x00u},
	{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0xB2u,0x00u},
	{{0x00u,0xFFu,0x2Fu,0x00u},1,0xC2u,0x00u},
	{{0x83u,0x00u,0xFFu,0x2Fu,0x00u},2,0xC2u,0x00u},

	// Sysex_f0/f7 events; ans == 0x00u
	// sysex_f0 from p. 136 of the midi std:
	{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xA2u,0x00u},
	{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xB2u,0x00u},
	{{0x00u,0xF0u,0x03u,0x43u,0x12u,0x00u},1,0xC2u,0x00u},
	// sysex_f7 from p. 136 of the midi std:
	{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xA2u,0x00u},
	{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xB2u,0x00u},
	{{0x81u,0x48u,0xF7u,0x06u,0x43u,0x12u,0x00u,0x43u,0x12u,0x00u},2,0xC2u,0x00u},
	{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xA2u,0x00u},
	{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xB2u,0x00u},
	{{0x64u,0xF7u,0x04u,0x43u,0x12u,0x00u,0xF7u},1,0xC2u,0x00u},

	// __set 1__ midi_channel/voice events
	// ... _with_ event-local status byte == 0x90u
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xA2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xB2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xC2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xA2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xB2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xC2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0x90u,0x90u},
	// ... _without_ event-local status byte == 0x90u
	{{0x83u,0x00u,0x41u,0x00u},2,0xA2u,0xA2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0xB2u,0xB2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0xC2u,0xC2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x41u,0x00u},1,0xA2u,0xA2u},
	{{0x00u,0x41u,0x00u},1,0xB2u,0xB2u},
	{{0x00u,0x41u,0x00u},1,0xC2u,0xC2u},
	{{0x00u,0x41u,0x00u},1,0x90u,0x90u},

	// __set 2__ midi_channel/voice events
	// ... _with_ event-local status byte == 0xC0u
	{{0x83u,0x00u,0xC0u,0x00u},2,0xA2u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0xB2u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0xC0u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0x90u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xA2u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xB2u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xC0u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0x90u,0xC0u},
	// ... _without_ event-local status byte == 0xC0u
	{{0x83u,0x00u,0x00u},2,0xA2u,0xA2u},
	{{0x83u,0x00u,0x00u},2,0xB2u,0xB2u},
	{{0x83u,0x00u,0x00u},2,0xC0u,0xC0u},
	{{0x83u,0x00u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x00u},1,0xA2u,0xA2u},
	{{0x00u,0x00u},1,0xB2u,0xB2u},
	{{0x00u,0x00u},1,0xC0u,0xC0u},
	{{0x00u,0x00u},1,0x90u,0x90u}
	};
	for (const auto& e : tests) {
		const unsigned char *p = &(e.data[0])+e.dtsize;
		auto s = mtrk_event_get_midi_status_byte_unsafe(p, e.rs);
		EXPECT_EQ(s,e.ans);
	}
}

