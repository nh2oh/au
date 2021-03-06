#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <chrono>
#include <filesystem>
#include <fstream>
#include "..\aulib\aulib_all.h"
#include "dbklib\binfile.h"
#include "..\aulib\input\midi\midi_vlq.h"
#include "..\aulib\input\midi\midi_examples.h"
//#include "..\aulib\input\midi\midi_raw.h"
//#include "..\aulib\input\midi\midi_delta_time.h"
//#include "..\aulib\input\midi\midi_vlq.h"
//#include "..\aulib\input\midi\mtrk_event_t.h"
//#include "..\aulib\input\midi\mtrk_event_methods.h"
//#include "..\aulib\input\midi\mtrk_t.h"

// 
// TODO
// - chord_t, musel_t, line_t functionality should proably be folded into voice_t and removed.  
//
//
size_t quantize_and_count_unique(
	std::vector<std::chrono::milliseconds> dt,
	std::chrono::milliseconds res, size_t error) {
	decltype(res/res) one = {1};
	for (auto& e : dt) {  // This loop assigns to dt!  e != const
		auto r = e/res;	
		// "round():" round up if e is exactly betweeen r*res and (r+1)*res;
		// round up if e < res => never round e to 0.  
		(e-r*res <= ((r+1)*res-e)) ? r=r : r=(r+1);  

		// "error = abs(e-e*res)"
		if (e > r*res) {
			error += (e-r*res).count();
		} else {
			error += (r*res-e).count();
		}
		e=(r*res);
	}

    std::sort(dt.begin(),dt.end());
    auto last = std::unique(dt.begin(),dt.end());
    return last-dt.begin();
}


int main(int argc) {
	midi_example();

	//auto mf_binfile = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID");
	//mc::midi_chunk_container_t<mc::midi_chunk_t::header> mctnr {&(mf_binfile.d[0])};
	//auto myid = mctnr.id();
	//auto mydl = mctnr.data_length();
	//auto mysz = mctnr.size();
	//std::cout << mc::print(mctnr) << std::endl;
	//std::cout << std::endl;

	//midi_file mf {"C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID"};
	//midi_file mf {"C:\\Users\\ben\\Desktop\\scr\\test.mid"};
	//std::cout << mf.print_mthd() << std::endl;
	//std::cout << mf.print_mtrk_seq() << std::endl;

	/*auto bf = dbk::readfile("C:\\Users\\ben\\Desktop\\scr\\CLEMENTI.MID");
	auto mc1 = read_midi_chunk(bf,0);
	auto fhc = read_midi_fheaderchunk(mc1);
	auto mc2 = read_midi_chunk(bf,mc1.data.size()+8);
	auto ev1 = read_mtrk_event_stream(mc2);*/
	return 0;
	/*
	ts_t ts {4_bt,d::q,false};
	std::vector<d_t> basevdt {d::e,d::e,d::e,d::qd,d::e,d::e,
		d::sx,d::sx,d::e,d::sx,d::sx,d::sx,d::sx,d::e,d::q,d::e,
		d::e,d::e,d::e,d::qd,d::sx,d::sx,d::e,
		d::h,d::h};

	rp_t baserp {ts,basevdt};
	std::cout << baserp.print() << std::endl;
	tmetg_t bmg {ts,baserp};
	rp_t newrp = randrp_metg(bmg,basevdt.size(),4_br);
	std::cout << newrp.print() << std::endl;


	melody_hiller_params p {};
	p.nnts = 16;
	p.nvoice = 2;
	p.max_rejects_tot = 100000;
	p.debug_lvl = 1;
	p.min = "C(3)";
	p.max = "C(5)";

	auto hrp = rhythm_hiller_ex32(p);
	auto dbkrp = hillerrp2dbkrp(hrp);

	for (int i=0; i<5; ++i) {
		//std::cout << i << "\n";
		auto vnt = melody_hiller_ex21(p);
		std::cout << "r4 r4 r4 r4" << std::endl;
	}
	
    return 0;*/

}

