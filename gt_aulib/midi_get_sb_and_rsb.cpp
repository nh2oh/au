#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\midi_vlq.h"
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
// get_running_status_byte(unsigned char, unsigned char);
// get_status_byte(unsigned char, unsigned char);
//
// The local sb is always a valid channel sb; the rs is always invalid
// as an sb (it's a db, or has 0xF as its most-sig 4 bits).  The local sb
// always wins as the rs and as the event-applicable sb; the rs that
// is supplied is irrelevant in all cases.  
//
TEST(get_running_and_local_status_byte, ValidChSBAsLocalValidAndInvalidSBsForRS) {
	for (const auto& loc : sbs_ch_mode_voice) {
		for (const auto& rs : sbs_meta_sysex) {
			EXPECT_EQ(get_running_status_byte(loc,rs),loc);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : sbs_unrecognized) {
			EXPECT_EQ(get_running_status_byte(loc,rs),loc);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : dbs_valid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),loc);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : sbs_invalid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),loc);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
// get_status_byte(unsigned char, unsigned char);
//
// The local sb is always invalid as a status byte, which implies that it's
// a data byte.  Note that things like 0xF1u are valid status bytes; they
// are "valid but unrecognized."  Where the rs is a channel status-byte, the
// rs is returned as the applicable status byte and as the rs.  Where the rs
// is invalid as a rs byte, 0x00u is returned from both funcs.  
//
TEST(get_running_and_local_status_byte, InvalidSBsAsLocalValidAndInvalidSBsForRS) {
	for (const auto& loc : sbs_invalid) {
		// Valid rs
		for (const auto& rs : sbs_ch_mode_voice) {
			EXPECT_EQ(get_running_status_byte(loc,rs),rs);
			EXPECT_EQ(get_status_byte(loc,rs),rs);
		}
		// meta and sysex sbs are invalid as running-status bytes and never
		// take precedence over the local byte, even when the local byte
		// is invalid as a status byte.  Same deal w/ sbs_unrecognized.  
		for (const auto& rs : sbs_meta_sysex) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),0x00u);
		}
		for (const auto& rs : sbs_unrecognized) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),0x00u);
		}
		for (const auto& rs : dbs_valid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),0x00u);
		}
		for (const auto& rs : sbs_invalid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),0x00u);
		}
	}
}


//
// Tests for:
// get_running_status_byte(unsigned char, unsigned char);
// get_status_byte(unsigned char, unsigned char);
//
// The local sb is always valid as a meta or sysex status byte.  Thus, the event-
// local sb is always returned by get_status_byte().  rs == 0x00u in all cases
// since sysex,meta events reset rs.  rs is irrelevant in all cases.
//
TEST(get_running_and_local_status_byte, SysexOrMetaSBsAsLocalValidAndInvalidSBsForRS) {
	for (const auto& loc : sbs_meta_sysex) {
		for (const auto& rs : sbs_ch_mode_voice) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u); 
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : sbs_meta_sysex) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : sbs_unrecognized) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : dbs_valid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
		for (const auto& rs : sbs_invalid) {
			EXPECT_EQ(get_running_status_byte(loc,rs),0x00u);
			EXPECT_EQ(get_status_byte(loc,rs),loc);
		}
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


