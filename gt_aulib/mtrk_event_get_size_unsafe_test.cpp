#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "midi_raw_test_data.h"
#include <vector>
#include <array>
#include <cstdint>



//
// Tests for:
// mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
//
// Part 1:  running-status is a valid midi status byte.
//
// Sets of meta events, sysex_f0/f7 events, and midi events, paired with 
// "random" but valid running-status bytes.  Events also have dt fields of
// varying size.  
// There are two sets of midi events: set 1 are all valid w/ status byte 
// 0x90u (=> each has 2 data bytes), set 2 are all valid w/ status byte
// 0xC0 (=> each has 1 data byte).  Half of both sets have an event-local 
// status byte, but the other half do not.  In the latter case, the number
// of data bytes can still be computed, since the running status byte is in
// all cases valid.  
//
//
TEST(mtrk_event_get_size_unsafe, RandomMtrkEventsAllRSBytesValid) {
	for (const auto& e : set_a_valid_rs) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto sz_dtstart = mtrk_event_get_size_dtstart_unsafe(p_dtstart, e.rs_pre);
		EXPECT_EQ(sz_dtstart,e.data.size());
	}
}


//
// Tests for:
// mtrk_event_get_size_dtstart_unsafe(const unsigned char*, unsigned char=0u);
// 
// Part 2:  running-status is an _invalid_ midi status byte.
//
// Sets of meta events, sysex_f0/f7 events, and midi events, paired with 
// "random" but *invalid* running-status bytes.  Events also have dt fields of
// varying size.  For sysex and meta events, the length should be calculated
// corretly, since the 0xFF,0xF0,0xF7 indicator byte following the delta-time
// is event local for all these event types; rs is never relied on.  
// 
// There are two sets of midi events: 
// set 1 are all valid w/ status byte 0x90u (=> each has 2 
// data bytes), set 2 are all valid w/ status byte 0xC0 (=> each has 1 data
// byte).  Half of both sets have an event-local status byte, but the other
// half do not.  In the latter case, since all these examples contain invalid
// running-status bytes, these events are essentially "malformed," 
// uninterpretible input, and i skip them.  
// 
//
TEST(mtrk_event_get_size_unsafe, RandomMtrkEventsAllRSBytesInvalid) {
	for (const auto& e : set_b_invalid_rs) {
		
		const unsigned char *p_dtstart = &(e.data[0]);

		auto maybe_loc_sb = *(p_dtstart+e.dtsize);
		if ((maybe_loc_sb&0x80u) != 0x80) { continue; }
		if (maybe_loc_sb!=0xFFu && maybe_loc_sb!=0xF0u && maybe_loc_sb!=0xF7u) {
			// not meta or sysex
			continue;
		}

		auto sz_dtstart = mtrk_event_get_size_dtstart_unsafe(p_dtstart, e.rs_pre);
		EXPECT_EQ(sz_dtstart,e.data.size());
	}
}


//
// Tests for:
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
TEST(mtrk_event_get_size_unsafe, RandomMIDIEventsRSandNonRS) {
	for (const auto& e : set_c_midi_events_valid) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto sz_dtstart = mtrk_event_get_size_dtstart_unsafe(p_dtstart,e.midisb_prev_event);
		EXPECT_EQ(sz_dtstart,e.data.size());
	}
}


