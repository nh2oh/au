#pragma once
#include "dbklib\binfile.h"
#include <filesystem>
#include <array>
#include <algorithm>
#include <vector>


// 
// Converts the field [p,p+N) where N is the number of unsigned chars occupied by a T to a T.  
// Swaps the byte order of the field so that the big endian fields of a midi file are converted
// to little endian for interpretation.  Obviously this byte order swapping is only needed for 
// LE architectures.  
// 
template<typename T>
T midi_raw_interpret(const unsigned char* p) {
	constexpr unsigned short N = sizeof(T)/sizeof(unsigned char);
	static_assert(N>=1);
	std::array<unsigned char,N> data {};
	std::rotate_copy(p,p+N-1,p+N,data.begin());

	return *reinterpret_cast<T*>(&(data[0]));
};

// 
// The max size of a vl field is 4 bytes; returns [0,4]
//
struct midi_vl_field_interpreted {
	uint8_t N {0};
	uint32_t val {0};
};
midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char*);


struct midi_chunk {
	std::array<unsigned char,4> id {};
	std::vector<unsigned char> data {};
};
struct midi_file_header_data {
	int16_t fmt_type {0};
	int16_t num_trks {0};
	int16_t time_div {0};
};

struct channel_event {
	uint32_t delta_time {0};
	uint8_t event_type {0};  // event-type (first 4 bits), channel (last 4 bits)
	uint8_t channel {0};  // event-type (first 4 bits), channel (last 4 bits)
	uint8_t p1 {0};
	uint8_t p2 {0};
};

struct event_container {
	midi_vl_field_interpreted delta_time {};

};

template<uint8_t N>
struct sysex_event {
	// F0 <length> <bytes to be transmitted after F0>
	// Sometimes F7... see std
	midi_vl_field_interpreted length {};
	std::array<unsigned char,N> data {};
};

template<uint8_t N>
struct midi_event {
	// ...
	midi_vl_field_interpreted length {};
	uint8_t type {0};  // 4 bits
	uint8_t channel {0};  // 4 bits
	uint8_t p1 {0};
	uint8_t p2 {0};
};

struct meta_event {
	//FF <type> <length> <bytes>
	uint8_t type {0};
	midi_vl_field_interpreted length {};
	std::vector<unsigned char> data {};
};

struct mtrk_event {
	uint32_t delta_time {0};
	event_container event {};  // midi-event || sysx-event || meta-event
};

midi_chunk read_midi_chunk(const dbk::binfile&, size_t);
midi_file_header_data read_midi_fheaderchunk(const midi_chunk&);

//midi_chunk read_midi_trackchunk(const midi_chunk&);
channel_event read_midi_event_stream(const midi_chunk&);

int read_midi_file(const std::filesystem::path&);





