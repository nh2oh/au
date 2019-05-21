#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "midi_raw_test_data.h"


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
//
// Sysex_{f0,f7}, meta, and channel_{voice,mode} events paired w/ a legal 
// running-status byte e.rs_pre.  
//
TEST(get_running_status_byte, RandomMtrkEventsAllRSBytesValid) {
	for (const auto& e : set_a_valid_rs) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		EXPECT_EQ(dt.N,e.dtsize);
		auto p = p_dtstart+dt.N;
		auto rs = get_running_status_byte(*p,e.rs_pre);
		EXPECT_EQ(rs,e.rs_post);
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
//
// Sysex_{f0,f7}, meta, and channel_voice events paired w/ an illegal 
// running-status byte illegal e.rs_pre.  
//
TEST(get_running_status_byte, RandomMtrkEventsAllRSBytesInvalid) {
	for (const auto& e : set_b_invalid_rs) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		EXPECT_EQ(dt.N,e.dtsize);
		auto p = p_dtstart+dt.N;
		
		auto rs = get_running_status_byte(*p,e.rs_pre);
		EXPECT_EQ(rs,e.rs_post);
	}
}

//
// Tests for:
// get_status_byte(unsigned char, unsigned char);
//
// Part 3:  Channel_{voice,mode} events only; the value supplied for the
// running-status may or may not be a legal rs byte, but all composite events 
// (data+rs byte) are valid and interpretible.  
//
TEST(get_status_byte, RandomMIDIEventsRSandNonRS) {
	for (const auto& e : set_c_midi_events_valid) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		EXPECT_EQ(dt.val,e.dt_value);
		EXPECT_EQ(dt.N,e.dt_field_size);
		auto p = p_dtstart+dt.N;
		
		auto s = get_status_byte(*p,e.midisb_prev_event);
		EXPECT_EQ(s,e.applic_midi_status);
	}
}

//
// Tests for:
// get_status_byte(unsigned char, unsigned char)
//
// Part 4:  All events lack an event-local status byte (for all events, the
// the status-byte in the data array d[n] is such that !(d[n]&0x80u), ie, 
// even 0xFFu,0xF0u,0xF7u are absent).  Further, for all test cases, the 
// "running-status" byte supplied as e.midisb_prev_event is invalid as a
// running-status byte (for most cases !(e.midisb_prev_event&0x80u), but 
// there are some e.midisb_prev_event==0xFFu||0xF0u||0xF7u.  Delta-time 
// value are random.  Since there is no way to get a status byte for any of
// the cases, they are essentially completely uninterpretible other than the
// dt field.  
// Where p points to the first byte following the delta-time,
// -> get_status_byte(*p,e.midisb_prev_event) should return 0x00u for each 
//    example.  
// -> get_running_status_byte(*p,,e.midisb_prev_event) should return 0x00u
//    for each example.  
// For all test cases, e.in_running_status,.n_data_bytes,.data_length are
// == 0.  These fields are not use in these tests.  
//
TEST(get_status_byte, RandomMIDIEventsRSInvalidAndNoLocalStatusByte) {
	for (const auto& e : set_d_midi_events_nostatus_invalid) {
		const unsigned char *p_dtstart = &(e.data[0]);
		auto dt = midi_interpret_vl_field(p_dtstart,e.data.size());
		EXPECT_EQ(dt.val,e.dt_value);
		EXPECT_EQ(dt.N,e.dt_field_size);
		auto p = p_dtstart+dt.N;
		
		auto s = get_status_byte(*p,e.midisb_prev_event);
		EXPECT_EQ(s,e.applic_midi_status);

	}
}


