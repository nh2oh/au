#include "midiread.h"
#include "dbklib\binfile.h"
#include <filesystem>
#include <algorithm>
#include <iterator>  // std::back_inserter()

midi_chunk read_midi_chunk(const dbk::binfile& bf, size_t offset) {
	midi_chunk result {};

	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+4,result.id.begin());
	offset += 4;

	int sz = midi_raw_interpret<int>(&(bf.d[offset]));

	result.data.reserve(sz);
	std::copy(bf.d.begin()+offset,bf.d.begin()+offset+sz,std::back_inserter(result.data));

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
	//   4    chunk ID        "MTrk" (0x4D54726B)
	//   4    chunk size      Num-bytes
	//   ...  Event data      stream of MIDI events

	return 0;
}

