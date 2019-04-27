#include "midi_raw_test_data.h"
#include <vector>
#include <cstdint>
#include <array>


//
// Tests for:
// mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u);
// mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 1:  running-status is a valid midi status byte.
//
// Sets of meta events, sysex_f0/f7 events, and midi events, paired with 
// "random" but valid running-status bytes.  Events also have dt fields of
// varying size.  For sysex and meta events, 0x00 should always be returned
// since these event types reset the running-status.  There are two sets of 
// midi events: set 1 are all valid w/ status byte => each has 2 
// data bytes, set 2 are all valid w/ status byte => each has 1 data
// byte.  
//
//
std::vector<test_setab_t> set_a_valid_rs {
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
	// ... _with_ event-local status byte; expect the event-
	// local status to override the running status, even though all
	// the rs bytes are valid.  
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xA2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xB2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0xC2u,0x90u},
	{{0x83u,0x00u,0x90u,0x41u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xA2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xB2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0xC2u,0x90u},
	{{0x00u,0x90u,0x41u,0x00u},1,0x90u,0x90u},
	// ... the same events, but _without_ the event-local status; expect
	// the running-status to apply.  
	{{0x83u,0x00u,0x41u,0x00u},2,0xA2u,0xA2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0xB2u,0xB2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0xE2u,0xE2u},
	{{0x83u,0x00u,0x41u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x41u,0x00u},1,0xA2u,0xA2u},
	{{0x00u,0x41u,0x00u},1,0xB2u,0xB2u},
	{{0x00u,0x41u,0x00u},1,0xE2u,0xE2u},
	{{0x00u,0x41u,0x00u},1,0x90u,0x90u},

	// __set 2__ midi_channel/voice events
	// ... _with_ event-local status byte
	{{0x83u,0x00u,0xC0u,0x00u},2,0xA2u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0xB2u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0xC0u,0xC0u},
	{{0x83u,0x00u,0xC0u,0x00u},2,0x90u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xA2u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xB2u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0xC0u,0xC0u},
	{{0x00u,0xC0u,0x00u},1,0x90u,0xC0u},
	// ... the same events, but _without_ the event-local status byte
	{{0x83u,0x00u,0x00u,0x00u},2,0xA2u,0xA2u},
	{{0x83u,0x00u,0x00u,0x00u},2,0xB2u,0xB2u},
	{{0x83u,0x00u,0x00u},2,0xC0u,0xC0u},
	{{0x83u,0x00u,0x00u,0x00u},2,0x90u,0x90u},
	{{0x00u,0x00u,0x00u},1,0xA2u,0xA2u},
	{{0x00u,0x00u,0x00u},1,0xB2u,0xB2u},
	{{0x00u,0x00u},1,0xC0u,0xC0u},
	{{0x00u,0x00u,0x00u},1,0x90u,0x90u}
};


//
// Tests for:
// mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u);
// mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 2:  running-status is an _invalid_ midi status byte.
//
// Sets of meta events, sysex_f0/f7 events, and midi events, paired with 
// "random" but *invalid* running-status bytes.  Events also have dt fields of
// varying size.  For sysex and meta events, 0x00 should always be returned
// since these event types reset the running-status.  There are two sets of 
// midi events: set 1 are all valid w/ status byte 0x90u (=> each has 2 
// data bytes), set 2 are all valid w/ status byte 0xC0 (=> each has 1 data
// byte).  Half of both sets have an event-local status byte, but the other
// half do not.  In the latter case, since all these examples contain invalid
// running-status bytes, are essentially "malformed," uninterpretible input;
// mtrk_event_get_midi_status_byte_unsafe() should return 0x00.  
//
std::vector<test_setab_t> set_b_invalid_rs {
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



//
// Tests for:
// mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u);
// mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 3:  midi events only; running-status may or may not be valid, but all 
// composite events (data+rs byte) are valid and interpretible.  
//
// Set of midi events, paired with "random" but valid and invalid running-
// status bytes.  Those events paired w/ invalid rs bytes have a valid
// event-local status byte.  Events w/ valid rs bytes may or may not contain
// an event-local status byte; if not, the rs byte correctly describes the
// event.  Events also have dt fields of varying size.  
//
//
std::vector<test_setc_t> set_c_midi_events_valid {
	// data, rs, applic-sb, is_rs, ndata, data_length, dt_val, dt.N
	{{0x00,0x3F},0xD4,0xD4,1,1,1,0,1},
	{{0x00,0x9D,0x09,0x66},0x56,0x9D,0,2,3,0,1},
	{{0xD5,0x03,0x79,0x4E},0x95,0x95,1,2,2,10883,2},
	{{0x5E,0x7D,0x69},0x90,0x90,1,2,2,94,1},
	{{0x00,0x92,0x10,0x7F},0xA4,0x92,0,2,3,0,1},
	{{0x0F,0xD1,0x24},0xD6,0xD1,0,1,2,15,1},
	{{0x00,0x79},0xC9,0xC9,1,1,1,0,1},
	{{0x85,0xE2,0xC1,0x37,0xC4,0x71},0x6A,0xC4,0,1,2,12099767,4},
	{{0x93,0xB2,0x5E,0x35,0x26},0xAA,0xAA,1,2,2,317790,3},
	{{0xC1,0x68,0xC8,0x0F},0xCE,0xC8,0,1,2,8424,2},
	{{0x00,0x00},0xC1,0xC1,1,1,1,0,1},
	{{0x40,0x48},0xDA,0xDA,1,1,1,64,1},
	{{0x17,0xA6,0x7D,0x52},0x5D,0xA6,0,2,3,23,1},
	{{0x48,0x75},0xD5,0xD5,1,1,1,72,1},
	{{0x00,0x04,0x0F},0xE7,0xE7,1,2,2,0,1},
	{{0xF1,0x5E,0x7D,0x5B},0xB6,0xB6,1,2,2,14558,2},
	{{0x11,0xCA,0x03},0x15,0xCA,0,1,2,17,1},
	{{0x00,0xD3,0x30},0xA2,0xD3,0,1,2,0,1},
	{{0x46,0x7F,0x02},0xBD,0xBD,1,2,2,70,1},
	{{0x8F,0xC9,0x89,0x11,0x5F,0x70},0x8F,0x8F,1,2,2,32654481,4},
	{{0x00,0x91,0x65,0x29},0x4A,0x91,0,2,3,0,1},
	{{0x00,0xD1,0x46},0x5A,0xD1,0,1,2,0,1},
	{{0x12,0x1F},0xC4,0xC4,1,1,1,18,1},
	{{0x74,0x5F,0x49},0xE0,0xE0,1,2,2,116,1},
	{{0xE3,0xC5,0x59,0x67},0xCD,0xCD,1,1,1,1630937,3},
	{{0xD7,0x1A,0xA0,0x16,0x6E},0xBD,0xA0,0,2,3,11162,2},
	{{0xC3,0x5D,0xE0,0x21,0x38},0x59,0xE0,0,2,3,8669,2},
	{{0x33,0xD3,0x4B},0x61,0xD3,0,1,2,51,1},
	{{0x00,0x81,0x6E,0x2E},0x56,0x81,0,2,3,0,1},
	{{0x00,0x8A,0x0B,0x26},0x44,0x8A,0,2,3,0,1},
	{{0x71,0x62},0xD0,0xD0,1,1,1,113,1},
	{{0x61,0xC3,0x61},0xE8,0xC3,0,1,2,97,1},
	{{0x00,0x66},0xDF,0xDF,1,1,1,0,1},
	{{0x17,0x89,0x3E,0x58},0x67,0x89,0,2,3,23,1},
	{{0x11,0x5D,0x66},0x96,0x96,1,2,2,17,1},
	{{0x00,0x47,0x14},0xA8,0xA8,1,2,2,0,1},
	{{0xF7,0x16,0x01},0xDE,0xDE,1,1,1,15254,2},
	{{0x00,0xC1,0x0E},0xC8,0xC1,0,1,2,0,1},
	{{0x5B,0x5D},0xDA,0xDA,1,1,1,91,1},
	{{0x50,0x16},0xC6,0xC6,1,1,1,80,1},
	{{0x00,0x4D},0xC3,0xC3,1,1,1,0,1},
	{{0x00,0xB3,0x7C,0x6A},0xCF,0xB3,0,2,3,0,1},
	{{0x2F,0x7F},0xCD,0xCD,1,1,1,47,1},
	{{0x00,0x81,0x4B,0x68},0x94,0x81,0,2,3,0,1},
	{{0x00,0xCF,0x52},0xA7,0xCF,0,1,2,0,1},
	{{0x36,0x52},0xDA,0xDA,1,1,1,54,1},
	{{0x00,0x90,0x6A,0x20},0x48,0x90,0,2,3,0,1},
	{{0x95,0x1F,0xDB,0x2A},0xED,0xDB,0,1,2,2719,2},
	{{0x3F,0x6C,0x14},0xA8,0xA8,1,2,2,63,1},
	{{0x00,0xD0,0x50},0xBE,0xD0,0,1,2,0,1},
	{{0xFF,0x67,0xEB,0x3D,0x7D},0x10,0xEB,0,2,3,16359,2},
	{{0x6F,0xCC,0x34},0x2E,0xCC,0,1,2,111,1},
	{{0x7E,0x59},0xD2,0xD2,1,1,1,126,1},
	{{0x7F,0x69},0xCF,0xCF,1,1,1,127,1},
	{{0x00,0xB0,0x1A,0x32},0xEE,0xB0,0,2,3,0,1},
	{{0x05,0xDB,0x4C},0xE8,0xDB,0,1,2,5,1},
	{{0x63,0xCC,0x7E},0x51,0xCC,0,1,2,99,1},
	{{0xA7,0x90,0x97,0x07,0xE3,0x42,0x65},0x25,0xE3,0,2,3,82054023,4},
	{{0x00,0x6B,0x68},0x93,0x93,1,2,2,0,1},
	{{0x02,0x2B,0x10},0xEB,0xEB,1,2,2,2,1},
	{{0x71,0x2D,0x69},0xE0,0xE0,1,2,2,113,1},
	{{0x00,0x99,0x0B,0x02},0x93,0x99,0,2,3,0,1},
	{{0x00,0xD2,0x3A},0xE5,0xD2,0,1,2,0,1},
	{{0x0D,0x07},0xD1,0xD1,1,1,1,13,1},
	{{0x15,0xDE,0x71},0x1E,0xDE,0,1,2,21,1},
	{{0xAD,0xCA,0x7D,0x83,0x16,0x1D},0x39,0x83,0,2,3,746877,3},
	{{0x00,0xC3,0x72},0x7C,0xC3,0,1,2,0,1},
	{{0x00,0x23,0x67},0x87,0x87,1,2,2,0,1},
	{{0x00,0x4A},0xCD,0xCD,1,1,1,0,1},
	{{0x87,0xFF,0xDB,0x19,0x2A},0xD3,0xD3,1,1,1,16772505,4},
	{{0x8D,0x48,0xA7,0x2B,0x29},0x85,0xA7,0,2,3,1736,2},
	{{0x31,0x70},0xC1,0xC1,1,1,1,49,1},
	{{0x00,0x82,0x31,0x29},0xEA,0x82,0,2,3,0,1},
	{{0x00,0x48},0xC0,0xC0,1,1,1,0,1},
	{{0x43,0x5A,0x1F},0x85,0x85,1,2,2,67,1},
	{{0x43,0xA7,0x61,0x74},0x4C,0xA7,0,2,3,67,1},
	{{0x61,0x8D,0x3C,0x5E},0x8E,0x8D,0,2,3,97,1},
	{{0x00,0x73,0x63},0xA7,0xA7,1,2,2,0,1},
	{{0x00,0x54},0xCA,0xCA,1,1,1,0,1},
	{{0x00,0xD9,0x18},0xE1,0xD9,0,1,2,0,1},
	{{0x00,0xB0,0x7B,0x7F},0x40,0xB0,0,2,3,0,1},
	{{0x00,0xDF,0x0D},0xA4,0xDF,0,1,2,0,1},
	{{0x5F,0x9D,0x67,0x6C},0xE8,0x9D,0,2,3,95,1},
	{{0x00,0x39,0x28},0xA9,0xA9,1,2,2,0,1},
	{{0xBB,0xDB,0x9F,0x57,0x7E,0x78},0xB1,0xB1,1,2,2,125226967,4},
	{{0x00,0xDE,0x16},0x1F,0xDE,0,1,2,0,1},
	{{0x00,0x89,0x22,0x6B},0xEA,0x89,0,2,3,0,1},
	{{0x00,0x57},0xC1,0xC1,1,1,1,0,1},
	{{0xBF,0x98,0xDD,0x6C,0x1D},0xD9,0xD9,1,1,1,132525804,4},
	{{0x17,0x06},0xC0,0xC0,1,1,1,23,1},
	{{0xA1,0xDB,0x3F,0xC2,0x6D},0x16,0xC2,0,1,2,552383,3},
	{{0x83,0x35,0x63},0xD4,0xD4,1,1,1,437,2},
	{{0x49,0x10,0x74},0x8C,0x8C,1,2,2,73,1},
	{{0xBD,0xAC,0xF2,0x15,0xD7,0x4F},0xB2,0xD7,0,1,2,128661781,4},
	{{0xC1,0x57,0xDF,0x18},0x54,0xDF,0,1,2,8407,2},
	{{0x4D,0x61,0x5F},0xAC,0xAC,1,2,2,77,1},
	{{0x00,0x9D,0x44,0x6F},0xF9,0x9D,0,2,3,0,1},
	{{0x57,0x4F,0x6B},0xBA,0xBA,1,2,2,87,1},
	{{0x00,0x30},0xD0,0xD0,1,1,1,0,1},
	{{0xA3,0x3D,0xD9,0x38},0xA4,0xD9,0,1,2,4541,2}
};


//
// Tests for:
// mtrk_event_get_midi_status_byte_unsafe(const unsigned char*, unsigned char=0u)
//  and its _dtstart_ variant.  
//
// Part 4:  midi events only; running-status byte is in all cases invalid, yet 
// all events lack a local status byte.  These are essentially uninterpretible;
// expect to return 0x00u in all cases.  
// I have zeroed the fields is_rs, ndata, data_length; they are not used in the
// tests.  
//
std::vector<test_setc_t> set_d_midi_events_nostatus_invalid {
	// data, rs, applic-sb, is_rs, ndata, data_length, dt_val, dt.N
	{{0x00,0x3F},0x00,0x00,0,0,0,0,1},
	{{0x00,0xF0,0x09,0x66},0x56,0x00,0,0,0,0,1},
	{{0xD5,0x03,0x79,0x4E},0xFF,0x00,0,0,0,10883,2},
	{{0x5E,0x7D,0x69},0xF7,0x00,0,0,0,94,1},
	{{0x00,0xF0,0x10,0x7F},0xF0,0x000,0,0,0,1},
	{{0x0F,0x21,0x24},0xF0,0x00,0,0,0,15,1},
	{{0x00,0x79},0xF7,0x00,0,0,0,0,1},
	{{0x85,0xE2,0xC1,0x37,0x71},0x6A,0x00,0,0,0,12099767,4},
	{{0x93,0xB2,0x5E,0x35,0x26},0xFF,0x00,0,0,0,317790,3},
	{{0xC1,0x68,0x18,0x0F},0x00,0x00,0,0,0,8424,2}
};
