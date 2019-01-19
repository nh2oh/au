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

	return result;
};



detect_chunk_result detect_chunk_type(const unsigned char* p) {
	detect_chunk_result result {midi_chunk_t::unknown,0,false};
	
	// All chunks begin w/ A 4-char identifier
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

//
// MIDI messages consist of either:
// 1)  A sequence of data bytes, or
// 2)  A single status byte followed by a sequence of data bytes.  
// Messges not containing a status byte have an implied status set by the most recent status
// byte encountered in the stream.  Status bytes have the MSBit set
//
midi_event parse_midi_event(const unsigned char *p, const unsigned char prev_stat) {
	midi_event result {};

	result.delta_t = midi_interpret_vl_field(p);
	p += result.delta_t.N;

	unsigned char high_bit = 0x80;
	unsigned char high_4bits = 0xF0;

	if ((*p & high_bit) == 0x80) {
		result.running_status = false;
		result.status = *p;
		++p;
	} else {
		result.running_status = true;
		result.status = prev_stat;
		// Not incrementing p; *p is the first data byte in running-status mode
	}
	// result.status is always the valid status byte applicable to the present message, even if
	// the present message is part of a "running status" stream and does not itself contain the
	// status byte.  
	if ((result.status & high_bit) != 0x80) { std::abort(); }

	//
	// Logic is duplicated in:
	// uint8_t midi_event_num_data_bytes(const midi_event& event_in);
	uint8_t n_data_bytes {0};
	if ((result.status & high_4bits) == 0xC0 || (result.status & 0xF0) == 0xD0) {  // MSBit == 1 => byte is a status byte
		n_data_bytes = 1;
	} else {
		n_data_bytes = 2;
	}

	// At this point, p points at the first data byte of the message
	if ((*p & high_bit) == 0x80) {
		std::abort();  // The MSB of all data bytes is 0
	}
	result.p1 = *p;
	
	if (n_data_bytes == 2) {
		++p; 
		if ((*p & high_bit) == 0x80) {
			std::abort();  // The MSB of all data bytes is 0
		}
		result.p2 = *p;
	}

	return result;
}








status_byte_type classify_status_byte(const unsigned char *p) {
	unsigned char high = 0xF0;

	status_byte_type result {status_byte_type::unknown};
	switch (*p & high) {
		case 0x80:  result = status_byte_type::channel; break;
		case 0x90:  result = status_byte_type::channel; break;
		case 0xA0:  result = status_byte_type::channel; break;
		case 0xB0:  result = status_byte_type::channel; break;
		case 0xC0:  result = status_byte_type::channel; break;
		case 0xD0:  result = status_byte_type::channel; break;
		case 0xE0:  result = status_byte_type::channel; break;
		case 0xF0:  result = status_byte_type::system; break;
		default:    result = status_byte_type::unknown; break;
	}

	return result;
}

midi_byte classify_byte(const unsigned char* p) {
	if ((*p & 0x80) == 0x80) {
		return midi_byte::status;
	}
	return midi_byte::data;
}

ch_msg_type classify_channel_status_byte(const unsigned char* p) {
	if ((*p & 0xF0) == 0xB0) {
		if (*(p+1) == 0x78) {  // 0b01111000 == 120
			return ch_msg_type::mode;
		}
	}
	return ch_msg_type::voice;
}

int channel_number(const unsigned char* p) {
	return (*p & 0x0F) + 1;
}

sys_msg_type classify_system_status_byte(const unsigned char* p) {
	sys_msg_type result {sys_msg_type::unknown};
	if (*p == 0xF0) {
		result = sys_msg_type::exclusive;
	} else if ((*p & 0xF8) == 0xF0) {
		// 0b11110sss where sss : [1,7]
		if ((*p & 0x07) == 0) {
			result = sys_msg_type::unknown;
		} else {
			result = sys_msg_type::common;
		}
	} else if ((*p & 0xF8) == 0xF8) {
		// 0b11110ttt where ttt : [0,7]
		result = sys_msg_type::realtime;
	}

	return result;
}

midi_message_info classify_midi_msg(const unsigned char* p) {
	if (classify_byte(p) == midi_byte::data) {
		return {midi_msg_type::invalid,0};
	}

	status_byte_type curr_status_byte_type = classify_status_byte(p);  // Channel or System or Unknown

	midi_message_info result {};
	if (curr_status_byte_type == status_byte_type::channel) {
		result.type = to_midi_msg_type(classify_channel_status_byte(p));
	} else if (curr_status_byte_type == status_byte_type::system) {
		result.type = to_midi_msg_type(classify_channel_status_byte(p));
	} else if (curr_status_byte_type == status_byte_type::unknown) {
		result.type = midi_msg_type::invalid;
	}

	++p;
	while (classify_byte(p)!=midi_byte::status) {
		++(result.n_data_bytes);
		++p;
	}

	return result;
}
midi_msg_type to_midi_msg_type(ch_msg_type m) {
	switch (m) {
		case ch_msg_type::voice:  return midi_msg_type::ch_voice; break;
		case ch_msg_type::mode:  return midi_msg_type::ch_mode; break;
		default: return midi_msg_type::ch_unknown; break;
	}
};
midi_msg_type to_midi_msg_type(sys_msg_type m) {
	switch (m) {
		case sys_msg_type::exclusive:  return midi_msg_type::sys_exclusive; break;
		case sys_msg_type::common:  return midi_msg_type::sys_common; break;
		case sys_msg_type::realtime:  return midi_msg_type::sys_realtime; break;
		case sys_msg_type::unknown:  return midi_msg_type::sys_unknown; break;
		default: return midi_msg_type::sys_unknown; break;
	}
};










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
			<< "for this->fdata_[i] with i == " << i << ".  A valid midi file must contain a "
			<< "sequence of track chunks following the header chunk.  \nAborting...\n";
			std::abort();
		}
		this->chunk_idx_.push_back({i,static_cast<uint64_t>(8+curr_chunk_type.data_length),midi_chunk_t::track});
		
		
		// Now that we know that the present chunk (location this->fdata_[i]) is a Track chunk 
		// ("MTrk"), move to the data section (begins 8 bytes following the "MTrk") and iterate 
		// over the MTrk events contained within.   
		std::vector<midi_file::mtrk_event_idx> curr_mtrk_event_idx {};
		uint64_t j = i+8;
		unsigned char prev_midi_status_byte {0};
		while (j < (this->chunk_idx_.back().offset+this->chunk_idx_.back().length)) {
			// Classify the present event as midi, sysex, or meta
			detect_mtrk_event_result curr_event = detect_mtrk_event_type(&(this->fdata_[j]));
			if (!curr_event.is_valid) {
				std::abort();
			}

			uint64_t curr_event_length {0};
			if (curr_event.type == mtrk_event_t::sysex) {
				// From the std (p.136):
				// Sysex events and meta-events cancel any running status which was in effect.  
				// Running status does not apply to and may not be used for these messages.
				//
				sysex_event sx = parse_sysex_event(&(this->fdata_[j]));
				curr_event_length = curr_event.delta_t.N + 1 + sx.length.N + sx.length.val;
			} else if (curr_event.type == mtrk_event_t::meta) {
				// From the std (p.136):
				// Sysex events and meta-events cancel any running status which was in effect.  
				// Running status does not apply to and may not be used for these messages.
				//
				meta_event mt = parse_meta_event(&(this->fdata_[j]));
				curr_event_length = curr_event.delta_t.N + 2 + mt.length.N + mt.length.val;
			} else {  // midi event
				midi_event md = parse_midi_event(&(this->fdata_[j]),prev_midi_status_byte);
				prev_midi_status_byte = md.status;
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




