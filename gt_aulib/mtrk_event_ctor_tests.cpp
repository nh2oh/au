#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\midi_vlq.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include "..\aulib\input\midi\mtrk_event_methods.h"
#include <vector>
#include <cstdint>
#include <array>

std::array<unsigned char,4> default_ctord_data {0x00u,0x90u,0x3Cu,0x3Fu};

// 
// Test of the default-constructed value, a middle C (note-num==60)
// Note-on event on channel "1" w/ velocity 60 and delta-time == 0.  
//
TEST(mtrk_event_ctor_tests, defaultCtor) {
	const auto d = mtrk_event_t();

	EXPECT_EQ(d.size(),4);
	EXPECT_TRUE(d.size()<=d.capacity());
	EXPECT_EQ(d.dt_end()-d.dt_begin(),1);
	EXPECT_EQ(d.end()-d.begin(),d.size());

	EXPECT_EQ(d.type(),smf_event_type::channel);
	EXPECT_EQ(d.delta_time(),0);
	EXPECT_EQ(d.status_byte(),0x90u);
	EXPECT_EQ(d.running_status(),0x90u);
	EXPECT_EQ(d.data_size(),3);
	
	for (int i=0; i<d.size(); ++i) {
		EXPECT_EQ(d[i],default_ctord_data[i]);
	}
}

// 
// Test of the mtrk_event_t(uint32_t dt) ctor, which constructs a middle C
// (note-num==60) note-on event on channel "1" w/ velocity 60 and as
// specified.  For values of delta_time>max allowed, the value written is the
// max allowed.  
//
TEST(mtrk_event_ctor_tests, dtOnlyCtor) {
	std::vector<uint32_t> tests {
		0x00u,0x40u,0x7Fu,  // field size == 1
		0x80u,0x2000u,0x3FFFu,  // field size == 2
		0x4000u,0x100000u,0x1FFFFFu,  // field size == 3
		0x00200000u,0x08000000u,0x0FFFFFFFu,  // field size == 4
		// Attempt to write values exceeding the allowed max; field size
		// should be 4, and all values written should be == 0x0FFFFFFFu
		0x1FFFFFFFu,0x2FFFFFFFu,0x7FFFFFFFu,0x8FFFFFFFu,
		0x9FFFFFFFu,0xBFFFFFFFu,0xEFFFFFFFu,0xFFFFFFFFu
	};

	std::array<unsigned char,6> ans_dt_encoded;

	for (const auto& tc : tests) {
		auto ans_dt = (0x0FFFFFFFu&tc);
		ans_dt_encoded.fill(0x00u);
		write_delta_time(ans_dt,ans_dt_encoded.begin());
		auto ans_dt_sz = 1;
		if ((tc>=0x00u) && (tc<0x80u)) {
			ans_dt_sz = 1;
		} else if ((tc>= 0x80u) && (tc<0x4000u)) {
			ans_dt_sz = 2;
		} else if ((tc>= 0x4000u) && (tc<0x00200000u)) {
			ans_dt_sz = 3;
		} else {
			ans_dt_sz = 4;
		}
		auto ans_data_size = 3;
		auto ans_size = ans_data_size + ans_dt_sz;
		
		const mtrk_event_t ev(tc);

		EXPECT_EQ(ev.size(),ans_size);
		EXPECT_TRUE(ev.size()<=ev.capacity());
		EXPECT_EQ(ev.dt_end()-ev.dt_begin(),ans_dt_sz);
		EXPECT_EQ(ev.end()-ev.begin(),ev.size());

		EXPECT_EQ(ev.type(),smf_event_type::channel);
		EXPECT_EQ(ev.delta_time(),ans_dt);
		EXPECT_EQ(ev.status_byte(),0x90u);
		EXPECT_EQ(ev.running_status(),0x90u);
		auto ds=ev.data_size();
		EXPECT_EQ(ev.data_size(),ans_data_size);
	
		for (int i=0; i<ans_dt_sz; ++i) {
			EXPECT_EQ(ev[i],ans_dt_encoded[i]);
		}
		for (int i=ans_dt_sz; i<ev.size(); ++i) {
			EXPECT_EQ(ev[i],default_ctord_data[i-(ans_dt_sz-1)]);
		}
	}
}


//
// Tests of the mtrk_event_t(uint32_t, const midi_ch_event_t&) ctor
// with valid data in the midi_ch_event_t struct.  
//
TEST(mtrk_event_ctor_tests, MidiChEventStructCtorValidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		// Events w/ 2 data bytes:
		{0, {note_on,0,57,32}, 3},
		{23, {note_off,1,57,32}, 3},
		{12354, {key_pressure,0,57,32}, 3},
		{0, {ctrl_change,15,72,100}, 3},
		{45541, {pitch_bend,0,127,127}, 3},
		// Events w/ 1 data byte:
		{785, {prog_change,14,127,0x00u}, 2},
		{2, {ch_pressure,2,0,0x00u}, 2}
	};
	
	for (const auto& tc : tests) {
		unsigned char curr_s = (tc.md_input.status_nybble + tc.md_input.ch);
		int curr_dt_size = midi_vl_field_size(tc.dt_input);
		int curr_size = curr_dt_size+tc.data_size;
		const mtrk_event_t ev(tc.dt_input,tc.md_input);

		EXPECT_EQ(ev.type(),smf_event_type::channel);
		EXPECT_EQ(ev.delta_time(),tc.dt_input);
		EXPECT_EQ(ev.size(),curr_size);
		EXPECT_EQ(ev.data_size(),tc.data_size);
		EXPECT_EQ(ev.status_byte(),curr_s);
		EXPECT_EQ(ev.running_status(),curr_s);

		auto it_beg = ev.event_begin();
		auto it_end = ev.end();
		EXPECT_EQ((it_end-ev.begin()),curr_size);
		EXPECT_EQ((it_end-it_beg),curr_size-curr_dt_size);
		EXPECT_EQ(it_beg,ev.payload_begin());
		EXPECT_EQ(*it_beg++,curr_s);
		EXPECT_EQ(*it_beg++,tc.md_input.p1);
		if (it_beg < it_end) {
			EXPECT_TRUE((it_end-it_beg)==1);
			EXPECT_EQ(*it_beg++,tc.md_input.p2);
		}
		EXPECT_TRUE(it_beg==it_end);
		EXPECT_TRUE(it_beg>=it_end);
		EXPECT_TRUE(it_beg<=it_end);
		EXPECT_FALSE(it_beg>it_end);
		EXPECT_FALSE(it_beg<it_end);
	}
}


//
// Tests of the mtrk_event_t(uint32_t, const midi_ch_event_t&) ctor
// with _invalid_ data in the midi_ch_event_t struct.  
//
// TODO:  The data in md_result is not used, since for this ctor the
// 'result' values are UB.  Keep to use this test data for tests of 
// normalize(midi_ch_event_t).  
TEST(mtrk_event_ctor_tests, MidiChEventStructCtorInvalidInputData) {
	struct test_t {
		uint32_t dt_input {0};
		midi_ch_event_t md_input {};
		midi_ch_event_t md_result {};
		uint32_t data_size {0};
	};
	// midi_ch_event_t {status, ch, p1, p2}
	std::vector<test_t> tests {
		{0, {note_on,16,57,32}, {note_on,0,57,32}, 3},  // Invalid channel (>15)
		{1, {note_on,127,57,32}, {note_on,15,57,32}, 3},  // Invalid channel (>15)
		{128, {note_on,14,128,32}, {note_on,14,0,32}, 3},  // Invalid p1
		{256, {note_on,14,129,32}, {note_on,14,1,32}, 3},  // Invalid p1
		{512, {note_on,14,7,130}, {note_on,14,7,2}, 3},  // Invalid p2
		{1024, {note_on,14,57,255}, {note_on,14,57,0x7Fu}, 3},  // Invalid p2

		// Exactly the same as the set above, but w/a 1-data-byte msg type
		{0, {prog_change,16,57,32}, {prog_change,0,57,0x80u}, 2},  // Invalid channel
		{1, {prog_change,127,57,32}, {prog_change,15,57,0x80u}, 2},  // Invalid channel
		{128, {prog_change,14,128,32}, {prog_change,14,0,0x80u}, 2},  // Invalid p1
		{256, {prog_change,14,129,32}, {prog_change,14,1,0x80u}, 2},  // Invalid p1
		{512, {prog_change,14,7,130}, {prog_change,14,7,0x80u}, 2},  // Invalid p2
		{1024, {prog_change,14,57,255}, {prog_change,14,57,0x80u}, 2},  // Invalid p2

		// Exactly the same as the set above, but w/an invalid status-nybble
		{0, {note_on&0x7Fu,16,57,32}, {note_on,0,57,32}, 3},  // Invalid channel
		{1, {note_on&0x7Fu,127,57,32}, {note_on,15,57,32}, 3},  // Invalid channel
		{128, {note_on&0x7Fu,14,128,32}, {note_on,14,0,32}, 3},  // Invalid p1
		{256, {note_on&0x7Fu,14,129,32}, {note_on,14,1,32}, 3},  // Invalid p1
		{512, {note_on&0x7Fu,14,7,130}, {note_on,14,7,2}, 3},  // Invalid p2
		{1024, {note_on&0x7Fu,14,57,255}, {note_on,14,57,0x7Fu}, 3}  // Invalid p2
	};
	
	for (const auto& tc : tests) {
		unsigned char curr_s = ((tc.md_input.status_nybble)|(tc.md_input.ch));
		int curr_dt_size = midi_vl_field_size(tc.dt_input);
		int curr_size = curr_dt_size+tc.data_size;
		const mtrk_event_t ev(tc.dt_input,tc.md_input);

		EXPECT_EQ(ev.type(),classify_status_byte(curr_s));
		EXPECT_EQ(ev.delta_time(),tc.dt_input);
		EXPECT_EQ(ev.status_byte(),curr_s);

		auto it_beg = ev.begin();
		auto it_evbeg = ev.event_begin();
		auto it_end = ev.end();
		EXPECT_TRUE(it_evbeg-it_beg,curr_dt_size);
		EXPECT_TRUE(it_end-it_beg<=ev.capacity());
		EXPECT_EQ(*it_evbeg,curr_s);
	}
}

