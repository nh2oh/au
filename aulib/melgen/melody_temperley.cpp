#pragma once
#include "randmel_gens.h"
#include "..\scale\scale_12tet.h"
#include "..\types\line_t.h"
#include "..\types\ntl_t.h"
#include "..\types\ts_t.h"
#include "..\types\beat_bar_t.h"
#include "..\types\metg_t.h"
#include "..\randrp_metg.h"
#include "..\util\au_random.h"
#include <string>
#include <numeric>

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
// key randomly then use kk_key to shift it if a particular key is desired.
// The current implementation does not gaurantee keyntl will be the resultant 
// key.  
//
//
//

line_t<ntstr_t> melody_temperley(ntl_t keyntl, bool ismajor, ts_t ts, bar_t nbars) {
	// All notes are drawn from the chromatic scale.  Probability distributions 
	// controlling note selection are what make notes "diatonic" to the key 
	// more probable.  
	scale_12tet sc {};

	std::vector<d_t> nvpool {d::q,d::e,d::sx};
	std::vector<beat_t> ph {0_bt,0_bt,0_bt};
	tmetg_t base_metg {ts,nvpool,ph};
	rp_t rp = randrp_metg(base_metg,0,nbars);

	// The scdpool is the "domain" of notes from which the melody is drawn
	std::vector<scd_t> scdpool(120,scd_t{0});
	std::iota(scdpool.begin(),scdpool.end(),scd_t{0});

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
	scd_t CP_mean {60}; scd_t CP_stdev {13};
	std::vector<double> CP = normpdf(scdpool,CP_mean,CP_stdev);
	scd_t c {scdpool[randset(1,CP,re)[0]]};  // The central pitch for the melody (_not_ the key)

	// The Key Profile (KP)
	// 
	// The elements of KP must "align" correctly with the elements of scdpool.  For 
	// example,  if key_scd == 4 (corresponding to scdpool[4]), KP[4] needs to 
	// be == KP_base[0].  
	scd_t key_scd = sc.to_scd(ntstr_t{keyntl,4});
	rscdoctn_t key_rscd {key_scd,12};
	
	std::vector<double> KP_base;
	if (ismajor) {
		KP_base =
			{0.184,0.001,0.155,0.003,0.191,0.109,0.005,0.214,0.001,0.078,0.004,0.055};
	} else {
		KP_base = 
			{0.192,0.005,0.149,0.179,0.002,0.144,0.002,0.201,0.038,0.012,0.053,0.022}; // p.60
	}

	std::vector<double> KP(scdpool.size(),0.0);
	for (int i=0; i<scdpool.size(); ++i) {
		KP[i] = KP_base[(i+key_rscd.to_int())%KP_base.size()];
	}

	// The Range Profile (RP)
	//
	// Creates the RP centered around the central pitch c, selected above from
	// the CP.  Note that c is not nec. they key scd.  
	// Use the RP to select the initial note of the melody.  
	// p.61
	//
	scd_t RP_stdev {5}; 
    std::vector<double> RP = normpdf(scdpool,c,RP_stdev);
	std::vector<double> RPKP(RP.size(),0.0);
	for (int i=0; i<RP.size(); ++i) {
		RPKP[i] = RP[i]*KP[i];
	}
	RPKP = normalize_probvec(RPKP);
	
	std::vector<scd_t> scds(rp.nelems(),scd_t{});
	scds[0] = scdpool[randset(1,RPKP,re)[0]];
	scd_t PP_s {7}; //double PP_stdev = 7.2; 
	for (int i=1; i<rp.nelems(); ++i) {
		auto curr_PP = normpdf(scdpool,scds[i-1],PP_s); // Mean is the previous pitch
		std::vector<double> curr_RPKPPP(RP.size(),0.0);
		for (int j=0; j<curr_RPKPPP.size(); ++j) {
			curr_RPKPPP[j] = RP[j]*KP[j]*curr_PP[j];
		}
		curr_RPKPPP = normalize_probvec(curr_RPKPPP);
		scds[i] = scdpool[randset(1,curr_RPKPPP,re)[0]];
	}

	//std::vector<ntstr_t> ntstrs(scds.size(),ntstr_t{});
	//for (int i=0; i<scds.size(); ++i) {
	//	ntstrs[i] = sc.to_ntstr(scds[i]);
	//}
	std::vector<ntstr_t> ntstrs = sc.to_ntstr(scds);
	return line_t {ntstrs,rp};
}


