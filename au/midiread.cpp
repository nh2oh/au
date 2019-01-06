#include "midiread.h"
#include "dbklib\binfile.h"
#include <filesystem>
#include <algorithm>
#include <iterator>  // std::back_inserter()
#include <iostream>
#include <exception>


midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char* p) {
	midi_vl_field_interpreted result {};
	result.val = 0;
	do {
		result.val = result.val << 8;
		result.val += (*p & 0x7F);
		++(result.N);
	} while (*p & 0x80);

	return result;
	
	/*std::array<unsigned char,4> padded_field {0,0,0,0};
	for (int8_t i=3; (i>=0); --i) {
		padded_field[i] = (*p & 0x7F);
		if (*p & 0x80) {
			result.N = i+1;
			result.val = midi_raw_interpret<int32_t>(&(padded_field[0]));
			return result;
		}
		++p;
	}

	std::cout << "midi_length_vl_field(const unsigned char* p)\n";
	std::abort();*/
};

midi_chunk read_midi_chunk(const dbk::binfile& bf, size_t offset) {
	midi_chunk result {};

	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+4,result.id.begin());
	offset += 4;

	result.length = midi_raw_interpret<int>(&(bf.d[offset]));
	offset += 4;

	result.data.reserve(result.length);
	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+result.length,std::back_inserter(result.data));

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


int read_mtrk_event_stream(const midi_chunk& chunk) {
	// Check that the chunk.id == MTrk
	// 1) Get the delta-time
	// 2) Determine sysex_event, midi_event or meta_event
	// 3) Call the right parsing fn based on (2)
	// ...
	
	size_t offset {0};
	while (offset < chunk.data.size()) {
		midi_vl_field_interpreted delta_t = midi_interpret_vl_field(&(chunk.data[offset]));
		offset += delta_t.N;

		auto x = 0xF0;
		
		if (chunk.data[offset] == 0xF0 || chunk.data[offset] == 0xF7) {  // sysex event; 0xF0==240, 0xF7==247
			sysex_event sx = parse_sysex_event(&(chunk.data[offset]));
			std::cout << sx.id << std::endl;
		} else if (chunk.data[offset] == 0xFF) {  // meta event
			meta_event mt = parse_meta_event(&(chunk.data[offset]));
			offset += (2 + mt.length.N + mt.length.val);
			std::cout << mt.id << std::endl;
		} else {  // midi event
			std::cout << "midi";
		}

		//...
	}  // To next MTrk event

		/*result.event_type = (chunk.data[offset] >> 4);
		result.event_type = (chunk.data[offset] << 4);
		offset += 1;

		result.p1 = chunk.data[offset];
		offset += 1;
		result.p2 = chunk.data[offset];
		offset += 1;

		events.push_back(result);
	}
	*/
	return 0;
}


sysex_event parse_sysex_event(const unsigned char* p) {
	sysex_event result {};
	result.id = *p;  // uchar -> uint8_t
	++p;

	result.length = midi_interpret_vl_field(p);
	p += result.length.N;

	result.data.reserve(result.length.val);
	std::copy(p,p+result.length.val,std::back_inserter(result.data));

	return result;
}

meta_event parse_meta_event(const unsigned char* p) {
	meta_event result {};
	result.id = *p;  // char -> uint8_t;  must == 0xFF
	++p;

	result.type = *p;  // char -> uint8_t;  ex: 58 => ts
	++p;

	result.length = midi_interpret_vl_field(p);
	p += result.length.N;

	result.data.reserve(result.length.val);
	std::copy(p,p+result.length.val,std::back_inserter(result.data));

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

