#include "midiread.h"
#include "dbklib\binfile.h"
#include <filesystem>
#include <algorithm>
#include <iterator>  // std::back_inserter()
#include <iostream>
#include <exception>


midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char* p) {
	midi_vl_field_interpreted result {};
	std::array<unsigned char,4> padded_field {0,0,0,0};
	for (uint8_t i=0; (i<4); ++i) {
		padded_field[i] = (*p & 0x7F);
		if (reinterpret_cast<uint8_t>(*p) <= 127) {
			result.N = i+1;
			result.val = midi_raw_interpret<uint32_t>(&(padded_field[0]));
			return result;
		}
		++p;
	}

	std::cout << "midi_length_vl_field(const unsigned char* p)\n";
	std::abort();
};

midi_chunk read_midi_chunk(const dbk::binfile& bf, size_t offset) {
	midi_chunk result {};

	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+4,result.id.begin());
	offset += 4;

	int sz = midi_raw_interpret<int>(&(bf.d[offset]));
	offset += 4;

	result.data.reserve(sz);
	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+sz,std::back_inserter(result.data));

	return result;
}

midi_file_header_data read_midi_fheaderchunk(const midi_chunk& chunk) {
	if (chunk.data.size() != 6) {
		std::cout << "The \"global\" header chunk of a midi file must have a 6-byte data section.";
		std::abort();
	}
	midi_file_header_data result {};
	size_t offset {0};

	result.fmt_type = midi_raw_interpret<int16_t>(&(chunk.data[offset]));
	offset += 2;

	result.num_trks = midi_raw_interpret<int16_t>(&(chunk.data[offset]));
	offset += 2;

	result.time_div = midi_raw_interpret<int16_t>(&(chunk.data[offset]));
	offset += 2;


	return result;
}


channel_event read_midi_event_stream(const midi_chunk& chunk) {
	// Check that the chunk.id == MTrk
	channel_event result {};

	size_t offset {0};

	midi_vl_field_interpreted sz = midi_interpret_vl_field(&(chunk.data[0]));
	result.delta_time = sz.val;
	offset += sz.N;
	


	return result;
}


int read_midi_file(const std::filesystem::path& fp) {
	//
	// Chunk header (8 bytes)
	//   4 byte ID string; identifies the type of chunk
	//   4 byte size; the chunk's length in bytes following the header. 
	// Chunk data
	//   ...
	//

	//
	// "Global" file header chunk (one per file)
	//   Sz   Name            Value(s)
	//   4    ID string       "MThd" (0x4D546864)
	//   4    size            6 (0x00000006)
	//   2    format type     0-2
	//   2    num-tracks      1-65,535
	//   2    time division   ...
	//

	//
	// Track chunk
	//   Sz   Name            Value(s)
	//   4    chunk ID        "MTrk" (0x4D54726B)     --------- HEADER
	//   4    chunk size      Num-bytes               --------- HEADER
	//   ...  Event data      stream of MIDI events
	//

	//
	// Event stream (track data)
	// 3 types of events:  Control Events, System Exclusive Events, Meta Events
	//
	//   Sz    Name            Value(s)
	//   ...   delta-time      Ontime relative to track's final event; 0 => play @ end
	//                         The final byte of the field is indicated by the MSB of the 
	//                         byte == 1.  
	//   4bit  Event-type 
	//   4bit  Channel
	//   1     P1
	//   1     P2
	//
	//
	//








	return 0;
}

