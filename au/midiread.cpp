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

	while (*p & 0x80) {
		result.val += (*p & 0x7F);
		result.val = result.val << 8;
		++(result.N);
		++p;
	}
	result.val += (*p & 0x7F);
	++(result.N);

	/*
	do {
		result.val = result.val << 8;
		result.val += (*p & 0x7F);
		++(result.N);
		++p;
	} while ((*p & 0x80) && (result.N <= 4));*/

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

	result.data_length = midi_raw_interpret<int32_t>(p);
	p+=4;
	if (result.data_length < 0) {
		result.is_valid = false;
		return result;
	}
	if (result.type == midi_chunk_t::header && result.data_length != 6) {
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
		result.type = mtrk_event_t::midi;
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

	result.delta_t = midi_interpret_vl_field(p);
	p += result.delta_t.N;

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

	result.delta_t = midi_interpret_vl_field(p);
	p += result.delta_t.N;

	result.id = *p;  // char -> uint8_t;  must == 0xFF
	if (result.id != 0xFF) {
		std::abort();
	}
	++p;

	result.type = *p;  // char -> uint8_t;  ex: 58 => ts
	if (result.type > 128) {
		std::abort();
	}
	++p;

	result.length = midi_interpret_vl_field(p);
	p += result.length.N;

	result.data.reserve(result.length.val);
	std::copy(p,p+result.length.val,std::back_inserter(result.data));

	return result;
}

midi_event parse_midi_event(const unsigned char* p) {
	midi_event result {};

	result.delta_t = midi_interpret_vl_field(p);
	p += result.delta_t.N;

	short n_data_bytes {0};
	if (*p & 1<<7) {  // MSBit == 1 => byte is a status byte (Equiv: *p & 0x80).  
		n_data_bytes = 2;
		result.type = (*p & 0xF0);
		result.channel = (*p & 0x0F);
		switch ((*p)>>4) {  // Testing the 4 MSBits of *p
			case 0x08:  // Note off
				break;
			case 0x09:  // Note on
				break;
			case 0x0A:  // Polyphonic key pressure/Aftertouch
				break;
			case 0x0B:  // Control change
				break;
			case 0x0C:  // Program change Channel
				n_data_bytes = 1;
				break;
			case 0x0D:  // Pressure/After touch
				n_data_bytes = 1;
				break;
			case 0x0E:  // Pitch bend change
				break;
			default:
				std::abort();
				break;
		}
	} else {  // MSB == 0 => byte is a data byte
		std::abort();
	}

	++p; result.p1 = *p;
	if ((result.p1)>>7 != 0) {
		// The MSB of all data bytes is 0
		std::abort();
	}
	if (n_data_bytes == 2) {
		++p; result.p2 = *p;
		if ((result.p2)>>7 != 0) {
			// The MSB of all data bytes is 0
			std::abort();
		}
	}

	return result;
}

uint8_t midi_event_num_data_bytes(const midi_event& event_in) {

	if (event_in.type == 0xC0 || event_in.type == 0xD0) {
		return 1;
	} else {
		return 2;
	}
}
	





midi_file::midi_file(const std::filesystem::path& fp) {
	this->fdata_ = dbk::readfile(fp).d;
	
	uint64_t i {0};
	detect_chunk_result curr_chunk_type = detect_chunk_type(&(this->fdata_[i]));
	if (!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::header) {
		std::cout << "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::header\n"
			<< "for this->fdata_[i] with i == " << i << ".  A valid midi file must begin with a "
			<< "midi header chunk.  \nAborting...\n";
		std::abort();
	}
	this->chunk_idx_.push_back({i,static_cast<uint64_t>(8+curr_chunk_type.data_length),midi_chunk_t::header});
	i += this->chunk_idx_.back().length;

	while (i<this->fdata_.size()) {  // For each Track trunk...
		curr_chunk_type = detect_chunk_type(&(this->fdata_[i]));
		if (!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::track) {
			std::cout << "!curr_chunk_type.is_valid || curr_chunk_type.type != midi_chunk_t::track\n"
			<< "for this->fdata_[i] with i == " << i << ".  A valid midi must contain a sequence of track "
			<< "chunks following the header chunk.  \nAborting...\n";
			std::abort();
		}
		this->chunk_idx_.push_back({i,static_cast<uint64_t>(8+curr_chunk_type.data_length),midi_chunk_t::track});
		
		
		// Now that we know that the present chunk is a Track chunk, move to the data section and 
		// iterate over the MTrk events contained within.  
		std::vector<midi_file::mtrk_event_idx> curr_mtrk_event_idx {};
		uint64_t j = i+8;
		while (j < (this->chunk_idx_.back().offset+this->chunk_idx_.back().length)) {
			detect_mtrk_event_result curr_event = detect_mtrk_event_type(&(this->fdata_[j]));
			if (!curr_event.is_valid) {
				std::abort();
			}

			uint64_t curr_event_length {0};
			if (curr_event.type == mtrk_event_t::sysex) {
				sysex_event sx = parse_sysex_event(&(this->fdata_[j]));
				curr_event_length = curr_event.delta_t.N + 1 + sx.length.N + sx.length.val;
			} else if (curr_event.type == mtrk_event_t::meta) {
				meta_event mt = parse_meta_event(&(this->fdata_[j]));
				curr_event_length = curr_event.delta_t.N + 2 + mt.length.N + mt.length.val;
			} else {  // midi event
				midi_event md = parse_midi_event(&(this->fdata_[j]));
				curr_event_length = curr_event.delta_t.N + 1;
				auto yay = midi_event_num_data_bytes(md);
				curr_event_length += yay;
			}

			curr_mtrk_event_idx.push_back({j,curr_event_length,curr_event.type});
			j += curr_event_length;
		}
		this->mtrk_event_idx_.push_back(curr_mtrk_event_idx);

		i+=this->chunk_idx_.back().length;
	}  // to next midi_chunk (Track chunk)

}




