#pragma once
#include "dbklib\binfile.h"
#include <filesystem>
#include <array>
#include <algorithm>


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


struct midi_chunk {
	std::array<unsigned char,4> id {};
	std::vector<unsigned char> data {};
};
struct midi_header_data {
	std::array<unsigned char,2> fmt_type {};
	std::array<unsigned char,2> num_trks {};
	std::array<unsigned char,2> time_div {};
};

midi_chunk read_midi_chunk(const dbk::binfile&, size_t);

int read_midi_file(const std::filesystem::path&);





