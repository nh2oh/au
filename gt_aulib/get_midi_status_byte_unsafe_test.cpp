#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "midi_raw_test_data.h"
#include <vector>
#include <array>
#include <cstdint>



//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
//
// Part 1:  running-status is a valid midi status byte.
//
// Sets of meta events, sysex_f0/f7 events, and midi events, paired with 
// "random" but valid running-status bytes.  Events also have dt fields of
// varying size.  For sysex and meta events, 0x00 should always be returned
// since these event types reset the running-status.  There are two sets of 
// midi events: set 1 are all valid w/ status byte 0x90u or 0xE0u (=> each 
// has 2 data bytes), set 2 are all valid w/ status byte 0xC0 (=> each has
// 1 data byte).  
//
//
TEST(get_running_status_byte, RandomMtrkEventsAllRSBytesValid) {
	for (const auto& e : set_a_valid_rs) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		auto p = p_dtstart+dt.N;
		auto rs = get_running_status_byte(*p,e.rs);
		EXPECT_EQ(rs,e.ans);
		//auto s_dtstart = mtrk_event_get_midi_status_byte_dtstart_unsafe(p_dtstart, e.rs);
		//EXPECT_EQ(s_dtstart,e.ans);
		//
		//const unsigned char *p = p_dtstart+e.dtsize;
		//auto s = mtrk_event_get_midi_status_byte_unsafe(p, e.rs);
		//EXPECT_EQ(s,e.ans);
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
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
// get_running_status_byte() should return 0x00.  
//
TEST(get_running_status_byte, RandomMtrkEventsAllRSBytesInvalid) {
	for (const auto& e : set_b_invalid_rs) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		auto p = p_dtstart+dt.N;
		
		auto rs = get_running_status_byte(*p,e.rs);
		EXPECT_EQ(rs,e.ans);
		
		//const unsigned char *p_dtstart = &(e.data[0]);
		//auto s_dtstart = mtrk_event_get_midi_status_byte_dtstart_unsafe(p_dtstart, e.rs);
		//EXPECT_EQ(s_dtstart,e.ans);

		//const unsigned char *p = p_dtstart+e.dtsize;
		//auto s = mtrk_event_get_midi_status_byte_unsafe(p, e.rs);
		//EXPECT_EQ(s,e.ans);
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
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
TEST(get_running_status_byte, RandomMIDIEventsRSandNonRS) {
	for (const auto& e : set_c_midi_events_valid) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		auto p = p_dtstart+dt.N;
		
		auto rs = get_running_status_byte(*p,e.midisb_prev_event);
		EXPECT_EQ(rs,e.applic_midi_status);



		//const unsigned char *p_dtstart = &(e.data[0]);
		//auto s_dtstart = mtrk_event_get_midi_status_byte_dtstart_unsafe(p_dtstart,e.midisb_prev_event);
		//EXPECT_EQ(s_dtstart,e.applic_midi_status);
		//
		//const unsigned char *p = p_dtstart+e.dt_field_size;
		//auto s = mtrk_event_get_midi_status_byte_unsafe(p,e.midisb_prev_event);
		//EXPECT_EQ(s,e.applic_midi_status);
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char)
//
// Part 4:  midi events only; running-status byte is in all cases invalid, yet 
// all events lack a local status byte.  These are essentially uninterpretible;
// expect to return 0x00u in all cases.  
// I have zeroed the fields is_rs, ndata, data_length; they are not used in the
// tests.  
//
TEST(get_running_status_byte, RandomMIDIEventsRSInvalidAndNoLocalStatusByte) {
	for (const auto& e : set_d_midi_events_nostatus_invalid) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		auto p = p_dtstart+dt.N;
		
		auto rs = get_running_status_byte(*p,e.midisb_prev_event);
		EXPECT_EQ(rs,e.applic_midi_status);




		//const unsigned char *p_dtstart = &(e.data[0]);
		//auto s_dtstart = mtrk_event_get_midi_status_byte_dtstart_unsafe(p_dtstart,e.midisb_prev_event);
		//EXPECT_EQ(s_dtstart,e.applic_midi_status);

		//const unsigned char *p = p_dtstart+e.dt_field_size;
		//auto s = mtrk_event_get_midi_status_byte_unsafe(p,e.midisb_prev_event);
		//EXPECT_EQ(s,e.applic_midi_status);
	}
}


