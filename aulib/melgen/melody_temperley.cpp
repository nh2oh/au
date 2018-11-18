#pragma once
#include "randmel_gens.h"
#include "..\scale\spn12tet3.h"
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\types\metg_t.h"
#include "..\randrp_metg.h"
#include "..\util\au_random.h"
#include "..\util\au_algs_math.h"
//#include <string>
//#include <numeric>
#include <vector>

//
// Melody generator function using the method implied by Temperley Chapter 
// 4 p. 56-61.  Notes in a growing sequence are selected one at a time by 
// drawing from a normal distribution where the stdev and mean depend on the 
// previous note.  
//
//
// Notes:
// Since a line_t does not bundle a scale the result is sc-relative, however,
// the alg is absolutely not sc-relative: It's defined on cchrom.  Different
// return type that associates a scale?  
//
// The only reason the rp is needed is to determine the number of notes to 
// generate.  I could just pass nnts in as an arg.  The algorithm does not
// take the rp into account (does it not?  does the Temperley version use
// total ontimes of each note instead of number of ontimes?).  
//
// In Temperley's version, the key is random w/ pmaj = 0.88,  pmin = 0.12.
// It is probably easy enough to allow the present function to pick the
// key randomly then use ks_key() to shift it if a particular key is desired.
// The current implementation does not gaurantee keyntl will be the resultant 
// key.  
//
//
//

line_t<note_t> melody_temperley(ntl_t keyntl, bool ismajor, ts_t ts, bar_t nbars) {
	// All notes are drawn from the chromatic scale.  Probability distributions 
	// controlling note selection are what make notes "diatonic" to the key 
	// more probable.  

	spn12tet3 sc {};

	std::vector<d_t> nvpool {d::q,d::e,d::sx};
	std::vector<beat_t> ph {0_bt,0_bt,0_bt};
	tmetg_t base_metg {ts,nvpool,ph};
	rp_t rp = randrp_metg(base_metg,0,nbars);

	// The scdpool is the "domain" of chromatic scale degrees from which the melody is drawn
	int sz_scdpool {120};  // TODO:  Make user-settable
	std::vector<int> scdpool {};  scdpool.reserve(sz_scdpool);
	for (int i=0; i<sz_scdpool; ++i) {
		scdpool.push_back(i);
	}
	//std::iota(scdpool.begin(),scdpool.end(),0);

	auto re = new_randeng(true);

	// The Central Pitch Profile (CP)
	// The "Central pitch profile:" A Normal distribution about C(4) (Middle C)
	// w/a variance of 25.0; C(4) is represented by the integer 60 (p.57).  
	// From the CPP, a value c is drawn to represent center of the range of 
	// pitches from which the melody will be drawn.  c is _not_ the tonal center 
	// of the piece, it is merely the center of the range.  
	//
	// For the default-constructed scale_12tet, "Middle C" => 261.63 Hz => scd 60.
	// 
	int CP_mean {60}; int CP_stdev {13};
	std::vector<double> CP = normpdf(scdpool,CP_mean,CP_stdev);
	int central_scd {scdpool[randset(1,CP,re)[0]]};  // The central pitch for the melody (_not_ the key)
	// TODO:  Is c ever used below???

	// The Key Profile (KP)
	// 
	// The elements of KP must "align" correctly with the elements of scdpool.  For 
	// example,  if key_scd == 4 (corresponding to scdpool[4]), KP[4] needs to 
	// be == KP_base[0].  
	spn12tet3::scd3_t key_rscd = sc.to_scd(keyntl,octn_t{0});
	// Note that i am naming the scd obtained from the "0" octave the rscd
	//rscdoctn_t key_rscd {key_scd,12};
	
	// TODO:  Why are these different from the ks_key() params?
	std::vector<double> KP_base;
	if (ismajor) {
		KP_base =
			{0.184,0.001,0.155,0.003,0.191,0.109,0.005,0.214,0.001,0.078,0.004,0.055};
	} else {
		KP_base = 
			{0.192,0.005,0.149,0.179,0.002,0.144,0.002,0.201,0.038,0.012,0.053,0.022}; // p.60
	}

	std::vector<double> KP;  KP.reserve(scdpool.size());
	for (int i=0; i<scdpool.size(); ++i) {
		//KP[i] = KP_base[(i+key_rscd.to_int())%KP_base.size()];
		
		int val_rscd = key_rscd-sc.to_scd(0);
		KP.push_back(KP_base[(i+val_rscd)%KP_base.size()]);
		// TODO:  Check idx calc... compare to ks_key()
		// TODO:  Make a note somewhere... this type of idxing is dangerous if i 
		// make the scale user-specifyable. What if somehow, for some scale, val_rscd
		// is < 0 ?
	}

	// The Range Profile (RP)
	//
	// Creates the RP centered around the central pitch c, selected above from
	// the CP.  Note that c is not nec. they key scd.  
	// Use the RP to select the initial note of the melody.  
	// p.61
	//
	int RP_stdev {5};   // TODO:  Allow the user to specify?
    std::vector<double> RP = normpdf(scdpool,central_scd,RP_stdev);
	std::vector<double> RPKP{};  RPKP.reserve(RP.size());
	for (int i=0; i<RP.size(); ++i) {
		RPKP.push_back(RP[i]*KP[i]);
	}
	RPKP = normalize_probvec(RPKP);
	
	std::vector<int> melody {}; melody.reserve(rp.nelems());
	melody.push_back(scdpool[randset(1,RPKP,re)[0]]);
	int PP_s {7}; //double PP_stdev = 7.2;   // TODO:  Allow the user to specify?
	for (int i=1; i<rp.nelems(); ++i) {
		auto curr_PP = normpdf(scdpool,melody[i-1],PP_s); // Mean is the previous pitch
		
		//std::vector<double> curr_RPKPPP(RP.size(),0.0);
		//for (int j=0; j<curr_RPKPPP.size(); ++j) {
		//	curr_RPKPPP[j] = RP[j]*KP[j]*curr_PP[j];
		//}
		std::vector<double> curr_RPKPPP = vprod(RP,KP,curr_PP);
		curr_RPKPPP = normalize_probvec(curr_RPKPPP);
		melody.push_back(scdpool[randset(1,curr_RPKPPP,re)[0]]);
	}

	//std::vector<ntstr_t> ntstrs(scds.size(),ntstr_t{});
	//for (int i=0; i<scds.size(); ++i) {
	//	ntstrs[i] = sc.to_ntstr(scds[i]);
	//}

	std::vector<note_t> melody_notes {};  melody_notes.reserve(melody.size());
	for (const auto& e : melody) {
		melody_notes.push_back(*(sc.to_scd(e)));
	}
	//std::vector<ntstr_t> ntstrs = sc.to_ntstr(scds);
	//return line_t {ntstrs,rp};
	return line_t {melody_notes,rp};
}


