/*
#include "wavwr.h"
#include <filesystem>
#include <vector>
#include <exception>
#include <iostream>
#include <cstring>  // std::memcpy(), std::memcmp()
#include <fstream>

int readriff(const binfile& f) {
	// Adapted from MSDN:
	// https://docs.microsoft.com/en-us/windows/desktop/xaudio2/resource-interchange-file-format--riff-
	//
	// Four-character code (FOURCC) identifier:  a uint32_t created by concatenating four ASCII 
	// chars that identifies a chunk type; space chars are allowed.  
	// Ex:  "abcd" on a little-endian system == 0x64636261 ('a' == 0x61, 'b' == 0x62, ...). 
	//
	// The RIFF chunk:
	// "RIFF", fileSize, fileType, data
	// - "RIFF" is the literal FOURCC "RIFF"
	// - fileSize is a uint32_t giving the size (not including the "RIFF" FOURCC or itself).  
	// - fileType is a FOURCC identifying the file type.  
	// - data consists of chunks in any order.  
	//
	struct riffhead {
		std::array<char,4> fourcc;  // expect == "RIFF" (no \0)
		uint32_t fileSize;  // expect == f.d.size()-8
		std::array<char,4> fileType;  // expect == "WAVE" (no \0)
	};
	//
	// Other chunks: 
	// chunkID, chunkSize, data
	// - chunkID is a FOURCC identifying the data contained in the chunk
	// - chunkSize is a uint32_t giving the size of the data section (not including the padding).
	// - data is zero or more bytes of data padded to the nearest WORD boundary.
	//
	struct riffinternalchunkheader {  // 8 bytes
		std::array<char,4> chunkID;
		uint32_t chunkSize;
	};  

	//
	// Chunks for a wav file:
	// 1) RIFF header 
	//
	// 2) riffinternalchunkheader.{id="fmt ",.chunkSize=16||18||40}; data ~ wav_chunk_fmt_data
	//
	struct wav_chunk_fmt_data {  // 16 bytes
		uint16_t FormatTag;
		uint16_t Channels;
		uint32_t SamplesPerSec;
		uint32_t AvgBytesPerSec;
		uint16_t BlockAlign;
		uint16_t BitsPerSample;
	};
	struct wav_chunk_fmt_data_opt {
		uint16_t size;
		uint16_t ValidBitsPerSample;
		uint32_t ChannelMask;
		std::array<unsigned char,16> SubFormat;
	};
	enum class formatTag {  // TODO:  Underlying type capacity can store 0xFFFE?
		pcm=0x0001,
		ieee_float=0x0003,
		alaw=0x0006,
		mulaw=0x0007,
		extensible=0xFFFE
	};
	//
	// 3)  riffinternalchunkheader.{id="data",.chunkSize=varies}; data ~
	//     Expect size of data = SamplesPerSec * Channels * duration in seconds
	//     Note BitsPerSample => type to interpret each sample as uint8_t, ...
	// -----------------------------------------------------------------------------------------------
	//
	size_t pos {0};

	riffhead riff_header {};
	std::memcpy(&riff_header,&(f.d[pos]),pos+12);
	pos += 12;
	
	
	std::vector<riffinternalchunkheader> ichs {};
	std::vector<std::vector<unsigned char>> icds {};  // "internal chunk data sections"
	while (pos<f.d.size()) {
		riffinternalchunkheader curr_ich {};
		std::memcpy(&curr_ich,&(f.d[pos]),pos+8);
		pos += 8;
		ichs.push_back(curr_ich);
		
		size_t curr_padsz {0};
		std::vector<unsigned char> curr_icd(f.d.begin()+pos,f.d.begin()+pos+curr_ich.chunkSize+curr_padsz);
		icds.push_back(curr_icd);
		pos += curr_ich.chunkSize+curr_padsz;
	}

	std::array<char,4> expect_fmt {'f','m','t',' '};
	std::array<char,4> expect_data {'d','a','t','a'};
	wav_chunk_fmt_data wavfmt_data {};
	for (int i=0; i<ichs.size(); ++i) {
		if (std::memcmp(&(ichs[i].chunkID), &expect_fmt, 4)==0) {
			std::memcpy(&wavfmt_data,&(icds[i][0]),icds[i].size());
		} else if (std::memcmp(&(ichs[i].chunkID), &expect_data, 4)==0) {
			//...
		}
	}

	return 1;

}




// Binary-mode file reader
binfile readfile(std::filesystem::path fp) {
	std::ifstream f {fp, std::ios::in | std::ios::binary};
	if (!f.is_open() || f.fail()) {
		std::cout << "Unable to open file fp== " << fp.string() << "\n"
			<< "Aborting... " << std::endl;
		std::abort();
	}

	// Two methods to determine the size...
	f.seekg(0,std::ios::end);
	size_t fsize = f.tellg();
	f.seekg(0,std::ios::beg);
	auto x = std::filesystem::file_size(fp);
	if (fsize != std::filesystem::file_size(fp)) {
		std::cout << "fsize= " << fsize << std::endl;
		std::cout << "stdstd::filesystem::file_size(fp)= " << std::filesystem::file_size(fp) << std::endl;
		std::abort();
	}

	std::vector<char> fdata_c(fsize,{});
	f.read(&fdata_c[0],fsize);
	f.close();

	std::vector<unsigned char> fdata_uc {}; fdata_uc.reserve(fdata_c.size());
	for (const auto& e : fdata_c) {
		fdata_uc.push_back(static_cast<const char>(e));
	}
	return binfile {fp, fdata_uc};
}

*/

