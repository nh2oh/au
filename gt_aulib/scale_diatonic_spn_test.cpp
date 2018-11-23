#include "gtest/gtest.h"
#include "..\aulib\scale\diatonic_spn.h"
#include "..\aulib\scale\spn.h"
#include "..\aulib\types\types_all.h"
#include <vector>
#include <cmath>  // std::floor()

// Default constructor should generate C-major
TEST(scaleDiatonicSpnTests, DefaultCtorCmaj) {
	diatonic_spn sc {};

	std::vector<ntl_t> expect_ntl {ntl_t{"C"}, ntl_t{"D"},ntl_t{"E"},ntl_t{"F"},
		ntl_t{"G"},ntl_t{"A"},ntl_t{"B"}};
	
	int N = expect_ntl.size();
	for (int i=-23; i<23; ++i) {
		auto ntl_expect = expect_ntl[((i%N)+N)%N];
		double r = std::floor(static_cast<double>(i)/static_cast<double>(7));
		octn_t oct_expect {static_cast<int>(r)};
		
		auto from_int = *sc.to_scd(i);
		EXPECT_EQ(from_int.ntl, ntl_expect);
		EXPECT_EQ(from_int.oct, oct_expect);
	}
}

// Check ntl sequence for C-minor
TEST(scaleDiatonicSpnTests, NtlSequenceCminor) {
	diatonic_spn sc {ntl_t{"C"},diatonic_spn::mode::minor};

	std::vector<ntl_t> expect_ntl {ntl_t{"C"}, ntl_t{"D"},ntl_t{"D#"},ntl_t{"F"},
		ntl_t{"G"},ntl_t{"G#"},ntl_t{"A#"}};
	
	int N = expect_ntl.size();
	for (int i=-23; i<23; ++i) {
		auto ntl_expect = expect_ntl[((i%N)+N)%N];
		double r = std::floor(static_cast<double>(i)/static_cast<double>(7));
		octn_t oct_expect {static_cast<int>(r)};
		
		auto from_int = *sc.to_scd(i);
		EXPECT_EQ(from_int.ntl, ntl_expect);
		EXPECT_EQ(from_int.oct, oct_expect);
	}
}


// Check ntl sequence for F#-major
TEST(scaleDiatonicSpnTests, NtlSequenceFSharpMajor) {
	diatonic_spn sc {ntl_t{"F#"},diatonic_spn::mode::major};

	std::vector<ntl_t> expect_ntl {ntl_t{"F#"}, ntl_t{"G#"},ntl_t{"A#"},ntl_t{"B"},
		ntl_t{"C#"},ntl_t{"D#"},ntl_t{"F"}};
		// Note that, at present, i have to write the final note as F instead of E#

	int N = expect_ntl.size();
	for (int i=-23; i<23; ++i) {
		auto ntl_expect = expect_ntl[((i%N)+N)%N];
		double r = std::floor(static_cast<double>(i)/static_cast<double>(7));
		octn_t oct_expect {static_cast<int>(r)};
		
		auto from_int = *sc.to_scd(i);
		EXPECT_EQ(from_int.ntl, ntl_expect);
		EXPECT_EQ(from_int.oct, oct_expect);
	}
}


// Check ntl sequence for B all modes
TEST(scaleDiatonicSpnTests, NtlSequenceBAllModes) {

	std::vector<std::vector<ntl_t>> expect_ntl {
		{ntl_t{"B"},ntl_t{"C#"},ntl_t{"D#"},ntl_t{"E"},ntl_t{"F#"},ntl_t{"G#"},ntl_t{"A#"}},
		{ntl_t{"B"},ntl_t{"C#"},ntl_t{"D"},ntl_t{"E"},ntl_t{"F#"},ntl_t{"G#"},ntl_t{"A"}},
		{ntl_t{"B"},ntl_t{"C"},ntl_t{"D"},ntl_t{"E"},ntl_t{"F#"},ntl_t{"G"},ntl_t{"A"}},
		{ntl_t{"B"},ntl_t{"C#"},ntl_t{"D#"},ntl_t{"F"},ntl_t{"F#"},ntl_t{"G#"},ntl_t{"A#"}},
		{ntl_t{"B"},ntl_t{"C#"},ntl_t{"D#"},ntl_t{"E"},ntl_t{"F#"},ntl_t{"G#"},ntl_t{"A"}},
		{ntl_t{"B"},ntl_t{"C#"},ntl_t{"D"},ntl_t{"E"},ntl_t{"F#"},ntl_t{"G"},ntl_t{"A"}},
		{ntl_t{"B"},ntl_t{"C"},ntl_t{"D"},ntl_t{"E"},ntl_t{"F"},ntl_t{"G"},ntl_t{"A"}}
	};
		// Note that, at present, i have to write the final note as F instead of E#
	int N = expect_ntl.size();

	for (int i=0; i<expect_ntl.size(); ++i) {
		auto curr_ntl_seq = expect_ntl[i];

		diatonic_spn::mode curr_mode = static_cast<diatonic_spn::mode>(i);
		diatonic_spn sc {ntl_t {"B"}, curr_mode};
	
		for (int j=-23; j<23; ++j) {
			auto ntl_expect = curr_ntl_seq[((j%N)+N)%N];
			double r = std::floor(static_cast<double>(j)/static_cast<double>(7));
			octn_t oct_expect {static_cast<int>(r)};
		
			auto from_int = *sc.to_scd(j);
			EXPECT_EQ(from_int.ntl, ntl_expect);
			EXPECT_EQ(from_int.oct, oct_expect);
		}
	}
}

// Middle C:  scd 48 => C(4)
// Midi-C_5:  scd 60 => C(5)
// The numeric scd's for the two scales are different, but the note w/ letter "C" and
// octave 4 has the same frq on both scales.  In no case does this depend on the choice
// of root note for the scale, nor the mode.  This is required y the dfn of "spn."  
TEST(scaleDiatonicSpnTests, ExpectedScdNtlRelationships) {
	spn sc_cchrom {};
	auto scd48_cchrom = sc_cchrom.to_scd(48);
	auto scd60_cchrom = sc_cchrom.to_scd(60);
	
	diatonic_spn sc_cmaj {};
	auto C4_cmaj = sc_cmaj.to_scd("C"_ntl, octn_t{4});
	EXPECT_EQ(*scd48_cchrom,*C4_cmaj);
	auto C5_cmaj = sc_cmaj.to_scd("C"_ntl, octn_t{5});
	EXPECT_EQ(*scd60_cchrom,*C5_cmaj);

	diatonic_spn sc_bphyg {"B"_ntl, diatonic_spn::mode::phygrian};
	auto C4_bphyg = sc_bphyg.to_scd("C"_ntl, octn_t{4});
	EXPECT_EQ(*scd48_cchrom,*C4_cmaj);
	auto C5_bphyg = sc_bphyg.to_scd("C"_ntl, octn_t{5});
	EXPECT_EQ(*scd60_cchrom,*C5_cmaj);
}

/*
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
TEST(scaleDiatonicspnTests, LocationOctaveBreaks) {
	spn sca4 {};

	pitch_std ps {};
	ps.ref_note = note_t{"C#"_ntl, octn_t{5}, frq_t{330}};
	ps.gen_int = 2;
	ps.ntet = 12;
	spn sc_cs_5_330 {ps};

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
TEST(scaleDiatonicspnTests, MultipleScdDeref) {
	spn sc {};
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
TEST(scaleDiatonicspnTests, ConstructDiffPStdsNtlMembers) {
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"C#"_ntl,"D"_ntl,"D#"_ntl,"E"_ntl,
		"F"_ntl,"F#"_ntl,"G"_ntl,"G#"_ntl,"A"_ntl,"A#"_ntl,"B"_ntl};
	std::vector<octn_t> octs { octn_t {-4}, octn_t {-3}, octn_t {-2}, octn_t {-1},
		octn_t {0}, octn_t {1}, octn_t {2}, octn_t {3}, octn_t {4}, octn_t {5}};
	std::vector<pitch_std> random_pitch_stds {};
	for (int i=0; i<20; ++i) {
		pitch_std curr_ps {};
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
		spn curr_sc {e};

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
TEST(scaleDiatonicspnTests, ScdNoteNtlOctInterconversionDefaultCtor) {
	spn sc {};
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
TEST(scaleDiatonicspnTests, ScdNoteNtlOctInterconversionCSharpFive330Hz) {
	pitch_std ps {};
	ps.ref_note = note_t{"C#"_ntl, octn_t{5}, frq_t{330}};
	ps.gen_int = 2;
	ps.ntet = 12;
	spn sc {ps};
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




*/

