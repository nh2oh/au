#include "gtest/gtest.h"
#include "..\aulib\input\midi\midi_raw.h"
#include "..\aulib\input\midi\mtrk_t.h"
#include "..\aulib\input\midi\mthd_t.h"
#include "..\aulib\input\midi\smf_t.h"
#include <vector>
#include <cstdint>

// 
// Manually constructed/verified  first few events from chementi.mid.  
// Running-status is not in use
//
/*
TEST(smf_chrono_iterator_tests, clementi_no_rs) {
	std::vector<unsigned char> raw_bytes {
		0x4D, 0x54, 0x68, 0x64,  // MThd
		0x00, 0x00, 0x00, 0x06,
		0x00, 0x01,  // Format 1 => simultaneous tracks
		0x00, 0x03,  // 3 tracks (0,1,2)
		0x00, 0xF0,
	
		// Track 0 ---------------------------------------------------
		0x4D, 0x54, 0x72, 0x6B, // MTrk
		0x00, 0x00, 0x00, 0x1C,  // 0x1C==28 bytes
		0x00, 0xFF,	0x58, 0x04, 0x04, 0x02, 0x18, 0x08,
		0x00, 0xFF, 0x51, 0x03, 0x07, 0xA1,	0x20,
		0x00, 0xFF, 0x01, 0x05, 0x53, 0x65, 0x71, 0x2D, 0x31,
		0x00, 0xFF,	0x2F, 0x00,  // End of track

		// Track 1 ---------------------------------------------------
		0x4D, 0x54, 0x72, 0x6B,  // MTrk
		0x00, 0x00, 0x00, 0x9C,  // 0x9C==156 bytes
	
		0x00, 0xFF, 0x01, 0x10, 0x48, 0x61, 0x72, 0x70, 0x73, 0x69, 0x63, 0x68, 0x6F, 0x72, 0x64, 0x20,
		0x48, 0x69, 0x67, 0x68, 
	
		0x00, 0xC0, 0x06,  // Program change to program 6
		0x00, 0xB0, 0x07, 0x64,  // Ctrl change 0x07 => set channel volume
		0x00, 0xB0, 0x0A, 0x40,  // Ctrl change 0x07 => set pan
		0x00, 0xB0, 0x5B, 0x50,  // Ctrl change 0x5B => Effects 1 Depth (formerly External Effects Depth) 
		0x00, 0xB0, 0x5D, 0x00,  // Ctrl change 0x5B => Effects 3 Depth (formerly Chorus Depth) 
	
		0x05, 0x90, 0x48, 0x59,
		0x81, 0x05, 0x80, 0x48, 0x17,
	
		0x07, 0x90, 0x4C, 0x5F,
		0x43, 0x90, 0x48, 0x58,
		0x10, 0x80, 0x4C, 0x0C,
		0x32, 0x80, 0x48, 0x12,
	
		0x00, 0x90, 0x43, 0x58,
		0x1B, 0x80, 0x43, 0x1C,
	
		0x69, 0x90, 0x43, 0x5F,
		0x21, 0x80, 0x43, 0x23,
	
		0x6E, 0x90, 0x48, 0x5E,
		0x71, 0x80, 0x48, 0x1B,
	
		0x07, 0x90, 0x4C, 0x65,
		0x43, 0x90, 0x48, 0x64,
		0x06, 0x80, 0x4C, 0x16,
		0x32, 0x80, 0x48, 0x20,
	
		0x0C, 0x90, 0x43, 0x65,
		0x23, 0x80, 0x43, 0x25,
	
		0x67, 0x90, 0x4F, 0x66,
		0x22, 0x80, 0x4F, 0x1A,
	
		0x63, 0x90, 0x4D, 0x63,
		0x3D, 0x90, 0x4C, 0x60,
		0x15, 0x80, 0x4D, 0x18,
		0x26, 0x90, 0x4A, 0x63,
		0x18, 0x80, 0x4C, 0x0F,
		0x1C, 0x80, 0x4A, 0x23,
	
		0x0B, 0x90, 0x48, 0x5F,
		0x16, 0xB0, 0x7D, 0x5C,  // 0x7D==123 => All notes off; velocity==0x5C (whatever)
	
		0x00, 0xFF, 0x2F, 0x00,   // End of track
	
		// Track 2 ---------------------------------------------------
		0x4D, 0x54, 0x72, 0x6B,  // MTrk
		0x00, 0x00, 0x00, 0x8C,  // 0x8C == 140 bytes
		0x00, 0xFF, 0x01, 0x0F,	0x48, 0x61, 0x72, 0x70, 0x73, 0x69, 0x63, 0x68, 0x6F, 0x72, 0x64, 0x20,
		0x4C, 0x6F, 0x77,
	
		0x05, 0x90, 0x30, 0x43,
		0x81, 0x06, 0x80, 0x30, 0x1C,
	
		0x83, 0x1E, 0x90, 0x30, 0x3B,
		0x81, 0x32, 0x80, 0x30, 0x11,
	
		0x82, 0x5C,	0x90, 0x30, 0x3D,
		0x70, 0x80, 0x30, 0x11,
	
		0x81, 0x0A, 0x90, 0x30, 0x49,
		0x63, 0x80, 0x30, 0x13,
	
		0x81, 0x20, 0x90, 0x2B, 0x48,
		0x4E, 0x80, 0x2B, 0x1E,
	
		0x81, 0x21, 0x90, 0x37, 0x4E,
		0x32, 0x80, 0x37, 0x0D,
	
		0x0D, 0x90, 0x35, 0x4B,
		0x3A, 0x80, 0x35, 0x13,
	
		0x01, 0x90, 0x34, 0x51,
		0x37, 0x80,	0x34, 0x11,
	
		0x09, 0x90, 0x32, 0x51,
		0x4A, 0x90, 0x30, 0x49,
		0x09, 0x80,	0x32, 0x0D,
		0x81, 0x38, 0x80, 0x30, 0x16,
	
		0x85, 0x4D, 0x90, 0x36, 0x38,
		0x71, 0x80, 0x36, 0x14,
	
		0x08, 0x90, 0x37, 0x4C,
		0x5F, 0x80, 0x37, 0x15,
	
		0x20, 0x90, 0x30, 0x47,
		0x66, 0x80, 0x30, 0x1D,
	
		0x14, 0x90, 0x32, 0x54,
		0x16, 0xB0, 0x7D, 0x5C, // 0x7D==123 => All notes off; velocity==0x5C (whatever)
	};

	//auto rawfile_check_result = validate_smf(&raw_bytes[0],raw_bytes.size(),"clementi_no_rs.mid");
	//smf_t mf(rawfile_check_result);
	
	//auto it_end = mf.event_iterator_end();
	//for (auto it=mf.event_iterator_begin(); it!=it_end; ++it) {
	//
	//}
}
*/

