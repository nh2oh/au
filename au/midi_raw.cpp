#include "midi_raw.h"
#include <string>
#include <array>
#include <vector>
#include <exception>


// TODO:  Max 4 bytes; stop after 4 bytes
midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char* p) {
	midi_vl_field_interpreted result {};
	result.val = 0;

	while (true) {
		result.val += (*p & 0x7F);
		++(result.N);
		if (!(*p & 0x80) || result.N==4) { 
			break;
		} else {
			result.val = result.val << 7;
			++p;
		}
	}
	return result;
};


// Default value of sep == ' '; see header
std::string print_hexascii(const unsigned char *p, int n, const char sep) {
	std::string s {};  s.reserve(3*n);

	std::array<unsigned char,16> nybble2ascii {'0','1','2','3','4','5',
		'6','7','8','9','A','B','C','D','E','F'};

	for (int i=0; i<n; ++i) {
		s += nybble2ascii[((*p) & 0xF0)>>4];
		s += nybble2ascii[((*p) & 0x0F)];
		s += sep;
		++p;
	}

	return s;
}



std::string print(const event_type& et) {
	if (et == event_type::meta) {
		return "meta";
	} else if (et == event_type::midi) {
		return "midi";
	} else if (et == event_type::sysex) {
		return "sysex";
	}
}
std::string print(const smf_event_type& et) {
	if (et == smf_event_type::meta) {
		return "meta";
	} else if (et == smf_event_type::channel_voice) {
		return "channel_voice";
	} else if (et == smf_event_type::channel_mode) {
		return "channel_mode";
	} else if (et == smf_event_type::sys_exclusive) {
		return "sys_exclusive";
	} else if (et == smf_event_type::sys_common) {
		return "sys_common";
	} else if (et == smf_event_type::invalid) {
		return "invalid";
	}
}



parse_meta_event_result_t parse_meta_event(const unsigned char *p) {
	parse_meta_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p != 0xFF) {
		result.is_valid = false;
		return result;
	}
	++p;

	result.type = *p;
	++p;

	auto length_field = midi_interpret_vl_field(p);
	result.data_size = 2 + length_field.N + length_field.val;

	result.is_valid = true;
	return result;
}

parse_sysex_event_result_t parse_sysex_event(const unsigned char *p) {
	parse_sysex_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	if (*p != 0xF0 || *p != 0xFF) {
		result.is_valid = false;
		return result;
	}
	result.type = *p;
	++p;

	auto length_field = midi_interpret_vl_field(p);
	result.data_size = 1 + length_field.N + length_field.val;

	result.is_valid = true;
	return result;
}

parse_midi_event_result_t parse_midi_event(const unsigned char *p, unsigned char prev_status_byte) {
	parse_midi_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	result.data_size = 0;
	if ((*p & 0x80) == 0x80) {  // Present message has a status byte
		result.has_status_byte = true;
		result.status_byte = *p;
		result.data_size += 1;
		++p;
	} else {
		result.has_status_byte = false;
		if ((prev_status_byte& 0x80) != 0x80) {
			// prev_status_byte is invalid
			result.is_valid = false;
			return result;
		}
		result.status_byte = prev_status_byte;
		// Not incrementing p; *p is the first data byte in running-status mode
	}
	// At this point, p points at the first data byte

	if ((result.status_byte & 0xF0) < 0xF0) {  // => not a system msg
		if ((result.status_byte & 0xF0) == 0xB0) {
			if ((*p & 0x78) == 0x78) {  // channel mode msg w/ 2 data bytes
				result.type = midi_msg_t::channel_mode;
				result.n_data_bytes = 2;
			} else {
				result.type = midi_msg_t::channel_voice;
				result.n_data_bytes = 2;
			}
		} else if ((result.status_byte & 0xF0) == 0xC0  || (result.status_byte & 0xF0) == 0xD0) {
			result.type = midi_msg_t::channel_voice;
			result.n_data_bytes = 1;
		} else {
			result.type = midi_msg_t::channel_voice;
			result.n_data_bytes = 2;
		}
	} else { // some sort of system message
		// I do not consider anything here to be of event_type::midi; it's all event_type::sysex.
		// None of this should ever trigger.  
		if (*p == 0xF0) {  
			result.type = midi_msg_t::system_exclusive;
			// unknown n_data_bytes
		} else if ((*p & 0xF8) == 0xF8) {
			result.type = midi_msg_t::system_realtime;
			result.n_data_bytes = 0;
		} else if ((*p & 0xF8) == 0xF0) {
			result.type = midi_msg_t::system_common;
			// unknown n_data_bytes:  0 or 2
		}
	}

	/*if ((result.status_byte & 0xF0) == 0xC0 || (result.status_byte & 0xF0) == 0xD0) {
		result.n_data_bytes = 1;
	} else {
		result.n_data_bytes = 2;
	}*/
	result.data_size += result.n_data_bytes;

	// Check first data byte
	if ((*p & 0x80) != 0) {
		result.is_valid = false;
		return result;
	}

	// Optionally check second data byte
	if (result.n_data_bytes == 2) {
		++p;
		if ((*p & 0x80) != 0) {
			result.is_valid = false;
			return result;
		}
	}

	result.is_valid = true;
	return result;
}



detect_chunk_type_result_t detect_chunk_type(const unsigned char *p, int32_t max_size) {
	detect_chunk_type_result_t result {};
	result.p = p;
	if (max_size < 8) {
		result.is_valid = false;
		return result;
	}

	// All chunks begin w/ A 4-char identifier
	std::string idstr {};
	std::copy(p,p+4,std::back_inserter(idstr));
	if (idstr == "MThd") {
		result.type = chunk_type::header;
	} else if (idstr == "MTrk") {
		result.type = chunk_type::track;
	} else {
		result.type = chunk_type::unknown;
	}
	p+=4;

	auto data_length = midi_raw_interpret<int32_t>(p);
	p+=4;

	if (data_length < 0) {
		result.msg = "MIDI chunks must have data length >= 0";
		result.is_valid = false;
		return result;
	}
	if (data_length+8 > max_size) {
		result.msg = "data_length+8 > max_size";
		result.is_valid = false;
		return result;
	}
	/*
	if (result.type == chunk_type::header && data_length != 6) {
		result.msg = "MThd chunks must have a data length of 6";
		result.is_valid = false;
		return result;
	} else if (result.type == chunk_type::track) {
		auto first_event = parse_mtrk_event_type(p);
		if (!(first_event.is_valid) || first_event.type != event_type::midi
			|| !midi_event_has_status_byte(p)) {
			result.msg = "MTrk chunks must begin w/a MIDI event containing a status byte";
			result.is_valid = false;
			return result;
		}
	}*/
	result.size = 4 + 4 + data_length;  // id + length_field + ...

	result.is_valid = true;
	return result;
}


event_type detect_mtrk_event_type_dtstart_unsafe(const unsigned char *p) {
	auto dt = midi_interpret_vl_field(p);
	p += dt.N;
	return detect_mtrk_event_type_unsafe(p);
}

//
// Pointer to the first data byte following the delta-time, _not_ to the start of
// the delta-time.  
//
smf_event_type detect_mtrk_event_type_unsafe(const unsigned char *p) {
	if ((*p & 0xF0) >= 0x80 && (*p & 0xF0) <= 0xE0) {
		return smf_event_type::channel_voice;
	} else if (*p == 0xFF) { 
		// meta event in an smf; system_realtime in a stream.  All system_realtime messages
		// are single byte events (status byte only); in a meta-event, the status byte is followed
		// by an "event type" byte then a vl-field giving the number of subsequent data bytes.
		// It should be possible to peek forward and distinguish meta from system_realtime, but
		// at present i am not doing that.  
		return event_type::meta;
	} else if ((*p & 0xF8) == 0xF7) {
		// system_exclusive or system_common
		return event_type::sysex;
	}
	return event_type::invalid;
	/*if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		return event_type::sysex;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		return event_type::meta;
	} else {
		// midi event
		return event_type::midi;
	}*/
}

//
// Takes a pointer to the first byte of the vl delta-time field.
//
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char *p, int32_t max_inc) {
	parse_mtrk_event_result_t result {};
	if (max_inc == 0) {
		result.type = smf_event_type::invalid;
		return result;
	}
	// All mtrk events begin with a delta-time occupying a maximum of 4 bytes
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;

	max_inc -= result.delta_t.N;
	if (max_inc <= 0) {
		result.type = smf_event_type::invalid;
		return result;
	}
	result.type = detect_mtrk_event_type_unsafe(p);
	if (result.type == event_type::invalid) {
		result.is_valid = false;
	}

	result.is_valid = true;
	return result;

	/*if (*p == static_cast<unsigned char>(0xF0) || *p == static_cast<unsigned char>(0xF7)) {
		// sysex event; 0xF0==240, 0xF7==247
		result.is_valid = true;
		result.type=event_type::sysex;
	} else if (*p == static_cast<unsigned char>(0xFF)) {  
		// meta event; 0xFF == 255
		result.is_valid = true;
		result.type = event_type::meta;
	} else {
		// midi event
		result.is_valid = true;
		result.type = event_type::midi;
	}
	return result;*/
}

bool midi_event_has_status_byte(const unsigned char *p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return (*p) & 0x80;
}
unsigned char midi_event_get_status_byte(const unsigned char* p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return *p;
}


//
// Checks that p points at the start of a valid MTrk chunk (ie, the 'M' of MTrk...),
// then moves through the all events in the track and validates that each begins with a
// valid vl delta-time, is one of midi, meta, or sysex, and that for each such event a
// valid mtrk_event_container_t object can be created if necessary (it must be possible to
// determine the size in bytes of each event).  
//
// If the input is a valid MTrk chunk, the validate_mtrk_chunk_result_t returned can be
// passed to the ctor of mtrk_container_t to instantiate a valid object.  
//
// All the rules of the midi standard as regards sequences of MTrk events are validated
// here for the case of the single track indicated by the input.  Ex, that each midi 
// event has a status byte (either as part of the event or implictly through running 
// status), etc.  
// 
//
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char *p, int32_t max_size) {
	validate_mtrk_chunk_result_t result {};

	detect_chunk_type_result_t chunk_detect = detect_chunk_type(p,max_size);
	if (!(chunk_detect.is_valid) || chunk_detect.type != chunk_type::track) {
		result.is_valid = false;
		result.msg += "!(chunk_detect.is_valid) || chunk_detect.type != midi_chunk_t::track\n";
		result.msg += ("chunk_detect.msg = " + chunk_detect.msg + "\n");
		return result;
	}

	// Validate each mtrk event in the chunk
	auto mtrk_reported_size = chunk_detect.size;  // midi_raw_interpret<int32_t>(p+4);
	int32_t i {8};
	unsigned char most_recent_midi_status_byte {0};
	while (i<mtrk_reported_size) {
		int32_t curr_event_length {0};

		// Classify the present event as midi, sysex, or meta
		auto curr_event = parse_mtrk_event_type(p+i);
		if (!(curr_event.is_valid)) {
			result.is_valid = false;
			result.msg += "!(curr_event.is_valid)";
			return result;
		}

		if (curr_event.type == event_type::sysex) {
			// From the std (p.136):
			// Sysex events and meta-events cancel any running status which was in effect.  
			// Running status does not apply to and may not be used for these messages.
			//
			most_recent_midi_status_byte = unsigned char {0};
			auto sx = parse_sysex_event(p+i);
			if (!(sx.is_valid)) {
				result.is_valid = false;
				result.msg = "!(sx.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + sx.data_size;
		} else if (curr_event.type == event_type::meta) {
			// From the std (p.136):
			// Sysex events and meta-events cancel any running status which was in effect.  
			// Running status does not apply to and may not be used for these messages.
			//
			most_recent_midi_status_byte = unsigned char {0};
			auto mt = parse_meta_event(p+i);
			if (!(mt.is_valid)) {
				result.is_valid = false;
				result.msg = "!(mt.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + mt.data_size;
		} else if (curr_event.type == event_type::midi) {
			auto md = parse_midi_event(p+i,most_recent_midi_status_byte);
			if (!(md.is_valid)) {
				result.is_valid = false;
				result.msg = "!(md.is_valid)";
				return result;
			}
			curr_event_length = curr_event.delta_t.N + md.data_size;
			if (!(md.has_status_byte)) {
				most_recent_midi_status_byte = midi_event_get_status_byte(p+i);
			}
		} else {
			std::abort();
		}

		i += curr_event_length;
	}

	if (i != mtrk_reported_size) {
		result.is_valid = false;
		result.msg += "i != mtrk_reported_size";
		return result;
	}

	result.is_valid = true;
	result.size = i;
	result.p = p;
	return result;
}




//
//TODO:  The offset checks are repetitive...
//
validate_smf_result_t validate_smf(const unsigned char *p, int32_t offset_end, 
									const std::string& fname) {
	validate_smf_result_t result {};
	int n_tracks {0};
	int n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};

	int32_t offset {0};
	while (offset<offset_end) {  // For each chunk...
		detect_chunk_type_result_t curr_chunk = detect_chunk_type(p+offset,offset_end-offset);
		if (!curr_chunk.is_valid) {
			result.is_valid = false;
			result.msg += "!curr_chunk.is_valid\ncurr_chunk_type.msg== ";
			result.msg += curr_chunk.msg;
			return result;
		}

		if (curr_chunk.type == chunk_type::header) {
			if (offset > 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::header but offset > 0.  ";
				result.msg += "A valid midi file must contain only one MThd @ the very start.  \n";
				return result;
			}
		}

		if (curr_chunk.type == chunk_type::track) {
			if (offset == 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::track but offset == 0.  ";
				result.msg += "A valid midi file must begin w/an MThd chunk.  \n";
				return result;
			}
			validate_mtrk_chunk_result_t curr_track = validate_mtrk_chunk(p+offset,offset_end-offset);
			if (!curr_track.is_valid) {
				result.msg += "!curr_track.is_valid\ncurr_track.msg==";
				result.msg += curr_track.msg;
				return result;
			}
			++n_tracks;
		} else if (curr_chunk.type == chunk_type::unknown) {
			++n_unknown;
			if (offset == 0) {
				result.is_valid = false;
				result.msg += "curr_chunk.type == chunk_type::unknown but offset == 0.  ";
				result.msg += "A valid midi file must begin w/an MThd chunk.  \n";
				return result;
			}
		}
		chunk_idxs.push_back({curr_chunk.type,offset,curr_chunk.size});
		offset += curr_chunk.size;
	}

	result.is_valid = true;
	result.fname = fname;
	result.chunk_idxs = chunk_idxs;
	result.n_mtrk = n_tracks;
	result.n_unknown = n_unknown;
	result.size = offset;
	result.p = p;

	return result;
}
