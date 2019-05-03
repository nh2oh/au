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
//#include "dbklib\contigmap.h"
//#include "dbklib\type_inspect.h"
//#include "dbklib\stats.h"
//#include "wavwr.h"
#include "dbklib\binfile.h"
//#include "..\aulib\input\midi\midi_examples.h"

// 
// TODO
// - chord_t, musel_t, line_t functionality should proably be folded into voice_t and removed.  
//
//
/*
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
}*/


int main(int argc) {
	
	std::vector<unsigned char> tc_a_rs {
		0x4D, 0x54, 0x68, 0x64,  // MThd
		0x00, 0x00, 0x00, 0x06,
		0x00, 0x01,  // Format 1 => simultanious tracks
		0x00, 0x02,  // 2 tracks (0,1)
		0x00, 0xF0,
	
		// Track 0 ---------------------------------------------------
		0x4D, 0x54, 0x72, 0x6B,  // MTrk
		0x00, 0x00, 0x00, 0x3D,  // 0x3D==61 bytes
	
		0x00,0xFF,0x01,0x0E,  // "some inst name"
			0x53,0x6F,0x6D,0x65,0x20,0x69,0x6E,0x73,0x74,0x20,0x6E,0x61,0x6D,0x65,
	
		0x00, 0xC0, 0x06,  // Program change to program 6 => Harpsichord
		0x00, 0xB0, 0x07, 0x64,  // Ctrl change 0x07 => set channel volume
	
		// note-on events; duration of each note is == 20
		0x00, 0x90, 0x48, 0x59,  // onset dt==0; off @ 20
		0x05, 0x49, 0x59,  // onset dt==5
		0x05, 0x4A, 0x59,  // onset dt==0x0A==10
		0x05, 0x4B, 0x59,  // onset dt==15; off @ 35
		0x05, 0x4C, 0x59,  // cum dt onset == 20; off @ 35
		
		0x00, 0x80, 0x48, 0x59,  // cum dt onset == 20
		0x05, 0x49, 0x59,  // cum dt onset == 25
		0x05, 0x4A, 0x59,  // cum dt onset == 30
		0x05, 0x4B, 0x59,  // cum dt onset == 35
		0x05, 0x4C, 0x59,   // cum dt onset == 40
	
		0x00, 0xFF, 0x2F, 0x00,   // End of track
	
		// Track 1 ---------------------------------------------------
		0x4D, 0x54, 0x72, 0x6B,  // MTrk
		0x00, 0x00, 0x00, 0x41,  // 0x41 == 65 bytes
		
		0x00,0xFF,0x01,0x14,  // text event 0x14==20 chars long ("some other inst name")
			0x53,0x6F,0x6D,0x65,0x20,0x6F,0x74,0x68,0x65,0x72,0x20,0x69,0x6E,0x73,0x74,0x20,0x6E,0x61,0x6D,0x65,
	
		0x00, 0xC0, 0x00,  // Program change to program 0 => Acoustic Grand Piano
		0x00, 0xB0, 0x07, 0x6F,  // Ctrl change 0x07 => set channel volume
		
		// Initial note-on is 1-tick after the first note of track 1
		// note-on events; duration of each note is == 10
		0x01, 0x90, 0x38, 0x59,  // cum dt onset == 1 (note on)
		0x0A, 0x3A, 0x59,  // cum dt onset == 0x01+0x0A==0x0B==11 (note on)
		0x00, 0x80, 0x38, 0x59,  // cum dt event onset => 0x01+0x0A==0x0B== 11 (note off)
		0x0A, 0x3A, 0x59,  // cum dt event onset => 0x01+0x0A+0x0A==0x15==21 (note off)
		
		0x00, 0x90, 0x3B, 0x59,  // cum dt event onset => 21 (note on)
		0x0A, 0x80, 0x3B, 0x59,  // cum dt event onset => ==31 (note off)
		
		0x00, 0x90, 0x3C, 0x59,  // cum dt event onset => 31 (note on)
		0x0A, 0x80, 0x3C, 0x59,  // cum dt event onset => ==41 (note off)

		0x00, 0xFF, 0x2F, 0x00   // End of track
	};

	std::string fn = "D:\\cpp\\nh2oh\\au\\gt_aulib\\test_data\\tc_a_rs.mid";
	std::filesystem::path fp(fn);
	std::ofstream fptr(fp,std::ios_base::out|std::ios_base::binary);
	bool is_good = fptr.good();
	std::copy(tc_a_rs.begin(),tc_a_rs.end(),std::ostreambuf_iterator(fptr));
	fptr.close();

	//midi_example();

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

