#include "gtest/gtest.h"
#include "..\aulib\scale\spn12tet3.h"
#include "..\aulib\types\types_all.h"
#include <vector>



// Default constructor should generate A440 w/ the expected ntl set
TEST(scaleSPN12tet3Tests, DefaultCtorNtlMembers) {
	spn12tet3 sc {};
	pitch_std3 ps {};
	// 1)  All these ntl's are scale-members.
	// 2)  For a range of octn's, conversion to an scd then from an scd to a note
	//     returns the same ntl.  
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	for (int i=0; i<ntls_in_sc.size(); ++i) {
		EXPECT_TRUE(sc.isinsc(ntls_in_sc[i]));
		for (int o=-4; o<5; ++o) {
			auto scd = sc.to_scd(ntls_in_sc[i],octn_t{o});
			auto nt = *scd;
			EXPECT_TRUE(sc.isinsc(nt));
			EXPECT_TRUE(nt.ntl == ntls_in_sc[i]);
			EXPECT_TRUE(nt.oct == octn_t{o});
		}
	}
}



// Check ntl ordering and location of octave breaks w/ different pith stds.
// C is always the first ntl of the octave; the pitch std is not a factor
TEST(scaleSPN12tet3Tests, LocationOctaveBreaks) {
	spn12tet3 sca4 {};

	pitch_std3 ps {};
	ps.ref_note = note_t{"C#"_ntl, octn_t{5}, frq_t{330}};
	ps.gen_int = 2;
	ps.ntet = 12;
	spn12tet3 sc_cs_5_330 {ps};

	// C is always the first ntl of the octave, even for different pitch stds
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	for (int o=-6; o<7; ++o) {
		auto scd_a4 = sca4.to_scd(ntls_in_sc[0],octn_t{o});
		auto scd_cs5 = sc_cs_5_330.to_scd(ntls_in_sc[0],octn_t{o});

		octn_t oct_expected {o};
		for (int i=0; i<25; ++i) {
			if (i>0 && i%ntls_in_sc.size() == 0) {
				oct_expected = octn_t {oct_expected.to_int()+1};
			}

			// A4-440Hz---------------------------------------------------
			auto curr_note = *scd_a4;
			EXPECT_TRUE(curr_note.ntl == ntls_in_sc[i%ntls_in_sc.size()]);
			EXPECT_TRUE(curr_note.oct == oct_expected);
			++scd_a4;

			// C#5-330Hz---------------------------------------------------
			auto curr_note_cs = *scd_cs5;
			EXPECT_TRUE(curr_note_cs.ntl == ntls_in_sc[i%ntls_in_sc.size()]);
			EXPECT_TRUE(curr_note_cs.oct == oct_expected);
			++scd_cs5;
		}
	}
}


// The scale is not dynamic.  Dereferencing an scd multiple times 
// always returns the same note
TEST(scaleSPN12tet3Tests, MultipleScdDeref) {
	spn12tet3 sc {};
	auto scd_a4 = sc.to_scd(ntl_t{"A"},octn_t{4});
	auto n_a4 = *scd_a4;
	EXPECT_TRUE(n_a4.frq == frq_t{440});
	EXPECT_TRUE(n_a4.ntl == ntl_t{"A"});
	EXPECT_TRUE(n_a4.oct == octn_t{4});

	n_a4 = *scd_a4;
	EXPECT_TRUE(n_a4.frq == frq_t{440});
	EXPECT_TRUE(n_a4.ntl == ntl_t{"A"});
	EXPECT_TRUE(n_a4.oct == octn_t{4});

	n_a4 = *scd_a4;
	EXPECT_TRUE(n_a4.frq == frq_t{440});
	EXPECT_TRUE(n_a4.ntl == ntl_t{"A"});
	EXPECT_TRUE(n_a4.oct == octn_t{4});
}


// Note-letter members and non-members are not affected by the pitch std
TEST(scaleSPN12tet3Tests, ConstructDiffPStdsNtlMembers) {
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::vector<octn_t> octs { octn_t {-4}, octn_t {-3}, octn_t {-2}, octn_t {-1},
		octn_t {0}, octn_t {1}, octn_t {2}, octn_t {3}, octn_t {4}, octn_t {5}};
	std::vector<pitch_std3> random_pitch_stds {};
	for (int i=0; i<20; ++i) {
		pitch_std3 curr_ps {};
		curr_ps.ref_note.ntl = ntls_in_sc[i%ntls_in_sc.size()];
		curr_ps.ref_note.oct = octs[i%octs.size()];
		curr_ps.ref_note.frq = frq_t {1234.01+2*i+3*i*i};
		curr_ps.gen_int = i%4+1;
		curr_ps.ntet = i%15 + 5;

		random_pitch_stds.push_back(curr_ps);
	}

	//  None of these ntl's are scale-members, no matter the pitch std/ref note
	std::vector<ntl_t> ntls_not_in_sc {"S"_ntl,"S#"_ntl,"H"_ntl,"H#"_ntl,"J"_ntl,
		"CC"_ntl,"CC#"_ntl,"a"_ntl,"what#"_ntl,"y"_ntl,"a#"_ntl,"b"_ntl,"a"_ntl,
		"#A"_ntl, "&A"_ntl};

	for (const auto& e : random_pitch_stds) {
		spn12tet3 curr_sc {e};

		for (const auto& ntl_yes : ntls_in_sc) {
			EXPECT_TRUE(curr_sc.isinsc(ntl_yes));

			auto curr_scd = curr_sc.to_scd(e.ref_note.ntl, e.ref_note.oct);
			auto curr_note = *curr_scd;
			EXPECT_TRUE(curr_note.ntl == e.ref_note.ntl);
			EXPECT_TRUE(curr_note.frq == e.ref_note.frq);
			EXPECT_TRUE(curr_note.oct == e.ref_note.oct);
		}

		for (const auto& ntl_no : ntls_not_in_sc) {
			EXPECT_FALSE(curr_sc.isinsc(ntl_no));
		}
	}

}


//  The note returned by dereferencing an scd should be consistent w/ the
// ntl, oct, frq, etc that generated the scd. 
TEST(scaleSPN12tet3Tests, ScdNoteNtlOctInterconversionDefaultCtor) {
	spn12tet3 sc {};
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};

	// Conversions between scd_t, frq_t, note_t
	// The range -1000->1000 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-1000; i<1000; ++i) {
		// 1)  Generate a "random" octave ans_curr_octn and ntl "ans_curr_ntl."  
		// 2)  Get the scd for this note => ans_curr_scd
		// 3)  Deref to generate the corresponding note => ans_curr_note
		// 4)  Get the frq from the note => ans_curr_frq
		// 
		// Self-consistent set ans_curr_{ntl, note, frq, oct}
		//
		octn_t ans_curr_octn {static_cast<int>(std::floor(std::abs(i)/12))};
		ntl_t ans_curr_ntl {ntls_in_sc[((i%12)+12)%12]};
		auto ans_curr_scd = sc.to_scd(ans_curr_ntl,ans_curr_octn);
		auto ans_curr_note = *ans_curr_scd;
		frq_t ans_curr_frq = ans_curr_note.frq;

		EXPECT_TRUE(ans_curr_note.ntl == ans_curr_ntl);
		EXPECT_TRUE(ans_curr_note.oct == ans_curr_octn);

		// Get a new scd/note from the frq.  It should be the same as the initial note
		auto scd_from_frq = sc.to_scd(ans_curr_frq);
		auto nt_from_frq = *scd_from_frq;
		EXPECT_TRUE(nt_from_frq.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_frq.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_frq.frq == ans_curr_frq);
		if (nt_from_frq.ntl != ans_curr_ntl) {
			// This block is here to help w/ debugging.  If the tests are passing it
			// obviously should not be entered.   
			auto xsf = i+1;
			auto dbg_scd_from_frq = sc.to_scd(ans_curr_frq);
			auto dbg_nt_from_frq = *scd_from_frq;
		}

		// Get a new scd/note from the note.{ntl,octave}.  It should be the same as
		// the initial ntl and oct
		auto scd_from_ntlo = sc.to_scd(ans_curr_note.ntl,ans_curr_note.oct);
		auto nt_from_ntlo = *scd_from_ntlo;
		EXPECT_TRUE(nt_from_ntlo.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_ntlo.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_ntlo.frq == ans_curr_frq);

	}
}



// The note returned by dereferencing an scd should be consistent w/ the
// ntl, oct, frq, etc that generated the scd.  
// Exactly the same test as ScdNoteNtlOctInterconversionDefaultCtor (above)
// but the scale here uses a weird different pitch std.  
TEST(scaleSPN12tet3Tests, ScdNoteNtlOctInterconversionCSharpFive330Hz) {
	pitch_std3 ps {};
	ps.ref_note = note_t{"C#"_ntl, octn_t{5}, frq_t{330}};
	ps.gen_int = 2;
	ps.ntet = 12;
	spn12tet3 sc {ps};
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};

	// Conversions between scd_t, frq_t, note_t
	// The range -1000->1000 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-1000; i<1000; ++i) {
		// 1)  Generate a "random" octave ans_curr_octn and ntl "ans_curr_ntl."  
		// 2)  Get the scd for this note => ans_curr_scd
		// 3)  Deref to generate the corresponding note => ans_curr_note
		// 4)  Get the frq from the note => ans_curr_frq
		// 
		// Self-consistent set ans_curr_{ntl, note, frq, oct}
		//
		octn_t ans_curr_octn {static_cast<int>(std::floor(std::abs(i)/12))};
		ntl_t ans_curr_ntl {ntls_in_sc[((i%12)+12)%12]};
		auto ans_curr_scd = sc.to_scd(ans_curr_ntl,ans_curr_octn);
		auto ans_curr_note = *ans_curr_scd;
		frq_t ans_curr_frq = ans_curr_note.frq;

		EXPECT_TRUE(ans_curr_note.ntl == ans_curr_ntl);
		EXPECT_TRUE(ans_curr_note.oct == ans_curr_octn);

		// Get a new scd/note from the frq.  It should be the same as the initial note
		auto scd_from_frq = sc.to_scd(ans_curr_frq);
		auto nt_from_frq = *scd_from_frq;
		EXPECT_TRUE(nt_from_frq.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_frq.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_frq.frq == ans_curr_frq);
		if (nt_from_frq.ntl != ans_curr_ntl) {
			// This block is here to help w/ debugging.  If the tests are passing it
			// obviously should not be entered.   
			auto xsf = i+1;
			auto dbg_scd_from_frq = sc.to_scd(ans_curr_frq);
			auto dbg_nt_from_frq = *scd_from_frq;
		}

		// Get a new scd/note from the note.{ntl,octave}.  It should be the same as
		// the initial ntl and oct
		auto scd_from_ntlo = sc.to_scd(ans_curr_note.ntl,ans_curr_note.oct);
		auto nt_from_ntlo = *scd_from_ntlo;
		EXPECT_TRUE(nt_from_ntlo.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_ntlo.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_ntlo.frq == ans_curr_frq);

	}
}





