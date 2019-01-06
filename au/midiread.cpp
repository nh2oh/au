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
	} while ((*p & 0x80) && (result.N <= 4));

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



detect_chunk_result detect_chunk_type(const unsigned char* p) {
	detect_chunk_result result {midi_chunk_t::unknown,0,false};
	
	// All chunks with a 4-char identifier
	std::string idstr {};
	std::copy(p,p+4,std::back_inserter(idstr));
	if (idstr == "MThd") {
		result.type = midi_chunk_t::header;
	} else if (idstr == "MTrk") {
		result.type = midi_chunk_t::track;
	} else {
		result.is_valid = false;
		return result;
	}
	p+=4;

	result.length = midi_raw_interpret<int32_t>(p);
	p+=4;
	if (result.length < 0) {
		result.is_valid = false;
		return result;
	}
	if (result.type == midi_chunk_t::header && result.length != 6) {
		result.is_valid = false;
		return result;
	}

	result.is_valid = true;
	return result;
}


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


detect_mtrk_event_result detect_mtrk_event_type(const unsigned char* p) {
	detect_mtrk_event_result result {mtrk_event_t::unknown,midi_vl_field_interpreted {},false};
	
	// All mtrk events begin with a delta-time occupying a maximum of 4 bytes
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		result.type = mtrk_event_t::sysex;
		result.is_valid = true;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		result.type = mtrk_event_t::meta;
		result.is_valid = true;
	} else {
		// midi event
		result.type = mtrk_event_t::meta;
		result.is_valid = true;
	}

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

		detect_mtrk_event_result mtrk_type = detect_mtrk_event_type(&(chunk.data[offset]));
		if (!mtrk_type.is_valid) {
			std::cout << "(!mtrk_type.is_valid)" << std::endl;
			break;
		}

		offset += mtrk_type.delta_t.N;
		if (mtrk_type.type == mtrk_event_t::sysex) {
			sysex_event sx = parse_sysex_event(&(chunk.data[offset]));
			offset += (1 + sx.length.N + sx.length.val);
			std::cout << sx.id << std::endl;
		} else if (mtrk_type.type == mtrk_event_t::meta) {
			meta_event mt = parse_meta_event(&(chunk.data[offset]));
			offset += (2 + mt.length.N + mt.length.val);
			std::cout << mt.id << std::endl;
		} else {  // midi event
			std::cout << "midi";
		}

		//...
	}  // To next MTrk event

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

midi_event parse_midi_event(const unsigned char* p) {
	midi_event result {};
	//...

	return result;
}








midi_file::midi_file(const std::filesystem::path& fp) {
	this->fdata_ = dbk::readfile(fp).d;
	
	uint64_t i {0};
	detect_chunk_result curr_chunk_type = detect_chunk_type(&(this->fdata_[i]));
	if (!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::header) {
		std::cout << "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::header\n";
		std::abort();
	}
	this->chunk_idx_.push_back({i,static_cast<uint64_t>(8+curr_chunk_type.length),midi_chunk_t::header});
	i += this->chunk_idx_.back().length;

	detect_mtrk_event_result curr_event {};
	
	while (i<this->fdata_.size()) {
		curr_chunk_type = detect_chunk_type(&(this->fdata_[i]));
		if (!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::track) {
			std::cout << "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::track\n";
			std::abort();
		}
		this->chunk_idx_.push_back({i,static_cast<uint64_t>(8+curr_chunk_type.length),midi_chunk_t::track});
		auto j = i+8;

		detect_mtrk_event_result curr_event = detect_mtrk_event_type(&(this->fdata_[i]));
		std::vector<midi_file::mtrk_event_idx> curr_mtrk_event_idx {};
		while (curr_event.is_valid) {
			//auto j = i + curr_event.delta_t.N;
			j += curr_event.delta_t.N;

			if (curr_event.type == mtrk_event_t::sysex) {
				sysex_event sx = parse_sysex_event(&(this->fdata_[j]));
				j += (1 + sx.length.N + sx.length.val);
			} else if (curr_event.type == mtrk_event_t::meta) {
				meta_event mt = parse_meta_event(&(this->fdata_[j]));
				j += (2 + mt.length.N + mt.length.val);
			} else {  // midi event
				std::abort();
			}

			curr_mtrk_event_idx.push_back({i,(j-i),curr_event.type});
			i += curr_mtrk_event_idx.back().length;
		}
		this->mtrk_event_idx_.push_back(curr_mtrk_event_idx);

		i+=this->chunk_idx_.back().length;
	}  // to next midi_chunk

}




