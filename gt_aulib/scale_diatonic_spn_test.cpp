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
		
		auto from_int = sc[i]; //*sc.to_scd(i);
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
		
		auto from_int = sc[i]; //*sc.to_scd(i);
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
		
		auto from_int = sc[i];  //*sc.to_scd(i);
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

	int N = expect_ntl.size();

	for (int i=0; i<expect_ntl.size(); ++i) {
		auto curr_ntl_seq = expect_ntl[i];

		diatonic_spn::mode curr_mode = static_cast<diatonic_spn::mode>(i);
		diatonic_spn sc {ntl_t {"B"}, curr_mode};
	
		for (int j=-23; j<23; ++j) {
			auto ntl_expect = curr_ntl_seq[((j%N)+N)%N];
			double r = std::floor(static_cast<double>(j)/static_cast<double>(7));
			octn_t oct_expect {static_cast<int>(r)};
		
			auto from_int = sc[j];  //*sc.to_scd(j);
			EXPECT_EQ(from_int.ntl, ntl_expect);
			EXPECT_EQ(from_int.oct, oct_expect);
		}
	}
}

// Middle C:  scd 48 => C(4)
// Midi-C_5:  scd 60 => C(5)
// The numeric scd's for the two scales are different, but the note w/ letter "C" and
// octave 4 has the same frq on both scales.  In no case does this depend on the choice
// of root note for the scale, nor the mode.  This is required by the dfn of "spn."  
TEST(scaleDiatonicSpnTests, ExpectedScdNtlRelationships) {
	spn sc_cchrom {};
	auto scd48_cchrom = sc_cchrom.to_scd(48);
	auto scd60_cchrom = sc_cchrom.to_scd(60);
	
	diatonic_spn sc_cmaj {};
	auto C4_cmaj = sc_cmaj.to_scd("C"_ntl, octn_t{4});
	EXPECT_EQ(*scd48_cchrom,sc_cmaj[C4_cmaj]);
	auto C5_cmaj = sc_cmaj.to_scd("C"_ntl, octn_t{5});
	EXPECT_EQ(*scd60_cchrom,sc_cmaj[C5_cmaj]);

	diatonic_spn sc_bphyg {"B"_ntl, diatonic_spn::mode::phygrian};
	auto C4_bphyg = sc_bphyg.to_scd("C"_ntl, octn_t{4});
	EXPECT_EQ(*scd48_cchrom,sc_bphyg[C4_bphyg]);
	auto C5_bphyg = sc_bphyg.to_scd("C"_ntl, octn_t{5});
	EXPECT_EQ(*scd60_cchrom,sc_bphyg[C5_bphyg]);
}


// For a range of octn's, conversion to an scd then from an scd to a note
// returns the same ntl.  
TEST(scaleDiatonicSpnTests, NtlOctnToScdAndBack) {
	diatonic_spn sc_cmaj {};

	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,
		"A"_ntl,"B"_ntl};
	for (int i=0; i<ntls_in_sc.size(); ++i) {
		EXPECT_TRUE(sc_cmaj.isinsc(ntls_in_sc[i]));
		for (int o=-4; o<5; ++o) {
			auto sc_it = sc_cmaj.zero() + sc_cmaj.to_scd(ntls_in_sc[i],octn_t{o});
			auto nt = *sc_it;
			EXPECT_TRUE(nt.ntl == ntls_in_sc[i]);
			EXPECT_TRUE(nt.oct == octn_t{o});

			// Excercise the various isinsc() overloads
			EXPECT_TRUE(sc_cmaj.isinsc(nt));
			EXPECT_TRUE(sc_cmaj.isinsc(nt.ntl));
			EXPECT_TRUE(sc_cmaj.isinsc(nt.frq));
		}
	}
}


//  The note returned by dereferencing an scd should be consistent w/ the
// ntl, oct, frq, etc that generated the scd. 
TEST(scaleDiatonicSpnTests, ScdNoteNtlOctInterconversion) {
	diatonic_spn sc {};
	std::vector<ntl_t> ntls_in_sc {"C"_ntl,"D"_ntl,"E"_ntl,"F"_ntl,"G"_ntl,
		"A"_ntl,"B"_ntl};

	// Conversions between scd_t, frq_t, note_t
	// The range -100->100 is absurd but stresses the floating-point ==
	// operations for frq_t equality
	for (int i=-100; i<100; ++i) {
		// 1)  Generate a "random" octave ans_curr_octn and ntl "ans_curr_ntl."  
		// 2)  Get the scd for this note => ans_curr_scd
		// 3)  Deref to generate the corresponding note => ans_curr_note
		// 4)  Get the frq from the note => ans_curr_frq
		// 
		// Self-consistent set ans_curr_{ntl, note, frq, oct}
		//
		octn_t ans_curr_octn {static_cast<int>(std::floor(std::abs(i)/7))};
		ntl_t ans_curr_ntl {ntls_in_sc[((i%7)+7)%7]};
		auto it_initial = sc.zero() + sc.to_scd(ans_curr_ntl,ans_curr_octn);
		auto ans_curr_note = *it_initial;
		frq_t ans_curr_frq = ans_curr_note.frq;

		EXPECT_TRUE(ans_curr_note.ntl == ans_curr_ntl);
		EXPECT_TRUE(ans_curr_note.oct == ans_curr_octn);

		// Get a new scd/note from the frq.  It should be the same as the initial note
		auto it_from_frq = sc.zero() + sc.to_scd(ans_curr_frq);
		auto nt_from_frq = *it_from_frq;
		EXPECT_TRUE(nt_from_frq.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_frq.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_frq.frq == ans_curr_frq);

		// Get a new scd/note from the note.{ntl,octave}.  It should be the same as
		// the initial ntl and oct
		auto it_from_ntlo = sc.zero() + sc.to_scd(ans_curr_note.ntl,ans_curr_note.oct);
		auto nt_from_ntlo = *it_from_ntlo;
		EXPECT_TRUE(nt_from_ntlo.ntl == ans_curr_ntl);
		EXPECT_TRUE(nt_from_ntlo.oct == ans_curr_octn);
		EXPECT_TRUE(nt_from_ntlo.frq == ans_curr_frq);
	}
}


