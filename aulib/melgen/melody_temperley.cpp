#pragma once
#include "randmel_gens.h"
#include "..\scale\spn.h"
#include "..\types\ntl_t.h"
#include "..\util\au_random.h"
#include "dbklib\math.h"
#include <vector>

//
// Melody generator function using the method implied by Temperley Chapter 
// 4 p. 56-61.  Notes in a growing sequence are selected one at a time by 
// drawing from a normal distribution where the stdev and mean depend on the 
// previous note.  
//
// Temperley's algorithm only makes sense on cchrom, hence the sc is not a user
// param, though in ptinciple it could be.  Allowing any scale whatsoever will
// cause problems internally with array indexing.  See the comments within.  
//
//  TODO:  Test against ks_key()
//
std::vector<note_t> melody_temperley(const melody_temperley_params& p) {
	// All notes are drawn from the chromatic scale.  Probability distributions 
	// controlling note selection are what make notes "diatonic" to the key 
	// more probable.  
	spn sc {};
	
	// The scdpool is the "domain" of chromatic scale degrees from which the melody is drawn
	std::vector<int> scdpool {};  scdpool.reserve(p.sz_scdpool);
	for (int i=0; i<p.sz_scdpool; ++i) {
		scdpool.push_back(i);
	}

	auto re = new_randeng(true);

	// The Central Pitch Profile (CP)
	// The "Central pitch profile:" A Normal distribution about C(4) (Middle C)
	// w/a variance of 25.0; C(4) is represented by the integer 60 (p.57).  
	// From the CPP, a value central_scd is drawn to represent center of the range of 
	// pitches from which the melody will be drawn.  central_scd is _not_ the tonal 
	// center (the key) of the piece, it is merely the center of the range.  
	//
	// For the default-constructed spn12tet, "Middle C" => 261.63 Hz => scd 48.
	// scd 0 is C(0)
	//
	std::vector<double> CP = normpdf(scdpool,p.CP_mean,p.CP_stdev);
	int central_scd {scdpool[randset(1,CP,re)[0]]};

	// The Key Profile (KP)
	// 
	std::vector<double> KP_base;  
	if (urandd(1,0,1)[0] <= p.p_major) {
		KP_base =
			{0.184,0.001,0.155,0.003,0.191,0.109,0.005,0.214,0.001,0.078,0.004,0.055};
	} else {
		KP_base = 
			{0.192,0.005,0.149,0.179,0.002,0.144,0.002,0.201,0.038,0.012,0.053,0.022}; // p.60
	}  // TODO:  Why are these different from the ks_key() params?

	// The elements of KP must "align" correctly with the elements of scdpool.  For 
	// example,  if key_scd == 4 (corresponding to scdpool[4]), KP[4] needs to 
	// be == KP_base[0].  If the caller's key_rscd is 1, KP[1] == KP_ base[0], 
	// KP[2] == KP_ base[1], ...  KP[key_rscd+12n] == KP_base[0] for all n.  
	//
	// Note that i am naming the scd obtained from the "0" octave the "r"scd
	int key_rscd = sc.to_scd(p.key_ntl,octn_t{0});
	if (key_rscd >= 12 || key_rscd < 0) {
		// Obviously spn12tet should never cause this, but in the future this function
		// might be refactored to allow the caller to pass in any random scale.  An out-
		// of-bounds key_rscd will break the vector indexing in some of the loops below.  
		std::abort();
	}
	std::vector<double> KP;  KP.reserve(scdpool.size());
	for (int i=0; i<scdpool.size(); ++i) {
		KP.push_back(KP_base[(i-key_rscd+12)%12]);  // Same idxing as in ks_key()
	}

	// The Range Profile (RP)
	//
	// Creates the RP centered around the central pitch central_scd.  See p.61
	//
    std::vector<double> RP = normpdf(scdpool,central_scd,p.RP_stdev);
	std::vector<double> RPKP = vprod(RP,KP);
	RPKP = normalize_probvec(RPKP);
	
	std::vector<note_t> melody {};  melody.reserve(p.nnts);
	int newest_scd = scdpool[randset(1,RPKP,re)[0]];
	melody.push_back(sc[newest_scd]);
	for (int i=1; i<p.nnts; ++i) {
		auto curr_PP = normpdf(scdpool,newest_scd,p.PP_stdev); // Mean is the previous pitch

		std::vector<double> curr_RPKPPP = vprod(RPKP,curr_PP);
		curr_RPKPPP = normalize_probvec(curr_RPKPPP);
		newest_scd = scdpool[randset(1,curr_RPKPPP,re)[0]];
		melody.push_back(sc[newest_scd]);
	}


	return melody;
}


std::vector<note_t> melody_temperley(ntl_t key_ntl, int nnts) {
	melody_temperley_params p {};
	p.key_ntl = key_ntl;
	p.nnts = nnts;

	return melody_temperley(p);
}

