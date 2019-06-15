#include "gtest/gtest.h"
#include "mtrk_test_data.h"
#include "..\aulib\input\midi\mtrk_t.h"
#include "..\aulib\input\midi\mtrk_event_t.h"
#include <vector>
#include <cstdint>
#include <iterator>
#include <iostream>

using namespace mtrk_tests;

mtrk_t make_tsb() {
	auto mtrk_tsb = mtrk_t();  // auto to avoid MVP
	for (const auto& e : tsb) {
		auto curr_ev = mtrk_event_t(e.d.data(),e.d.size());
		mtrk_tsb.push_back(curr_ev);
	}
	return mtrk_tsb;
}


// 
// OIt split_copy_if(InIt beg, InIt end, OIt dest, UPred pred)
// Copy out all channel_voice events for note number 67
//
TEST(mtrk_t_tests, SplitCopyIfForNoteNum67WithTSB) {
	auto mtrk_b = make_tsb();

	auto isntnum43 = [](const mtrk_event_t& ev)->bool {
		auto md = get_channel_event(ev);
		return (is_channel_voice(ev) && (md.p1==67));  // 67 == 0x43u
	};

	auto new_mtrk = mtrk_t();
	auto it = split_copy_if(mtrk_b.begin(),mtrk_b.end(),
		std::back_inserter(new_mtrk),isntnum43);

	uint64_t tk_onset = 0;
	EXPECT_EQ(new_mtrk.size(),tsb_note_67_events.size());
	for (int i=0; i<new_mtrk.size(); ++i) {
		tk_onset += new_mtrk[i].delta_time();
		EXPECT_TRUE(is_channel_voice(new_mtrk[i]));
		auto md = get_channel_event(new_mtrk[i]);
		EXPECT_EQ(md.p1,67);
		EXPECT_EQ(tk_onset,tsb_note_67_events[i].tkonset);

		// The raw data in tsb_note_67_events have the delta time fields from
		// mtrk_b
		auto ev = mtrk_event_t(tsb_note_67_events[i].d.data(),
			tsb_note_67_events[i].d.size(),0x00u);
		ev.set_delta_time(new_mtrk[i].delta_time());
		EXPECT_EQ(new_mtrk[i],ev);
	}
}


// 
// OIt split_copy_if(InIt beg, InIt end, OIt dest, UPred pred)
// Copy out all meta events
//
TEST(mtrk_t_tests, SplitCopyIfForMetaEventsWithTSB) {
	auto mtrk_b = make_tsb();

	auto ismeta = [](const mtrk_event_t& ev)->bool {
		return is_meta(ev);
	};

	auto new_mtrk = mtrk_t();
	auto it = split_copy_if(mtrk_b.begin(),mtrk_b.end(),
		std::back_inserter(new_mtrk),ismeta);

	uint64_t tk_onset = 0;
	EXPECT_EQ(new_mtrk.size(),tsb_meta_events.size());
	for (int i=0; i<new_mtrk.size(); ++i) { 
		tk_onset += new_mtrk[i].delta_time();
		EXPECT_TRUE(is_meta(new_mtrk[i]));
		EXPECT_EQ(tk_onset,tsb_meta_events[i].tkonset);

		// The raw data in tsb_meta_events have the delta time fields from
		// mtrk_b
		auto ev = mtrk_event_t(tsb_meta_events[i].d.data(),
			tsb_meta_events[i].d.size(),0x00u);
		ev.set_delta_time(new_mtrk[i].delta_time());
		EXPECT_EQ(new_mtrk[i],ev);
	}
}


// 
// 
// Split out all channel_voice events for note number 67
//
TEST(mtrk_t_tests, SplitIfForNoteNum67WithTSB) {
	auto mtrk_b = make_tsb();

	auto isntnum43 = [](const mtrk_event_t& ev)->bool {
		auto md = get_channel_event(ev);
		return (is_channel_voice(ev) && (md.p1==67));  // 67 == 0x43u
	};

	auto new_mtrk = mtrk_t();
	auto it = split_if(mtrk_b.begin(),mtrk_b.end(),isntnum43);
	std::cout << print_event_arrays(mtrk_b) << std::endl;

	uint64_t tk_onset = 0;
	/*EXPECT_EQ(new_mtrk.size(),tsb_note_67_events.size());
	for (int i=0; i<new_mtrk.size(); ++i) {
		tk_onset += new_mtrk[i].delta_time();
		EXPECT_TRUE(is_channel_voice(new_mtrk[i]));
		auto md = get_channel_event(new_mtrk[i]);
		EXPECT_EQ(md.p1,67);
		EXPECT_EQ(tk_onset,tsb_note_67_events[i].tkonset);

		// The raw data in tsb_note_67_events have the delta time fields from
		// mtrk_b
		auto ev = mtrk_event_t(tsb_note_67_events[i].d.data(),
			tsb_note_67_events[i].d.size(),0x00u);
		ev.set_delta_time(new_mtrk[i].delta_time());
		EXPECT_EQ(new_mtrk[i],ev);
	}*/
}
