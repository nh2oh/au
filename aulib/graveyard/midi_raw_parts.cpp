
channel_msg_type mtrk_event_get_ch_msg_type_dtstart_unsafe(const unsigned char *p, unsigned char s) {
	auto dt = midi_interpret_vl_field(p);
	s = get_status_byte(*(p+dt.N),s);

	channel_msg_type result = channel_msg_type::invalid;
	if ((s&0xF0u) != 0xB0u) {
		switch (s&0xF0u) {
			case 0x80u:  result = channel_msg_type::note_off; break;
			case 0x90u: result = channel_msg_type::note_on; break;
			case 0xA0u:  result = channel_msg_type::key_pressure; break;
			//case 0xB0:  ....
			case 0xC0u:  result = channel_msg_type::program_change; break;
			case 0xD0u:  result = channel_msg_type::channel_pressure; break;
			case 0xE0u:  result = channel_msg_type::pitch_bend; break;
			default: result = channel_msg_type::invalid; break;
		}
	} else {  // (s&0xF0u) == 0xB0u
		// To distinguish a channel_mode from a control_change msg, have to look at the
		// first data byte.  
		unsigned char p1 = mtrk_event_get_midi_p1_dtstart_unsafe(p,s);
		if (p1 >= 121 && p1 <= 127) {
			result = channel_msg_type::channel_mode;
		} else {
			result = channel_msg_type::control_change;
		}
	}

	return result;
}






//
// TODO:  There are some other conditions to verify:  I think only certain types of 
// channel_msg_type are allowed to be in running_status mode.  
//
parse_channel_event_result_t parse_channel_event(const unsigned char *p,
				unsigned char prev_status_byte,	int32_t max_size) {
	parse_channel_event_result_t result {};
	result.delta_t = midi_interpret_vl_field(p);
	if (result.delta_t.N > 4) {
		result.is_valid = false;
		return result;
	}
	p += result.delta_t.N;
	max_size -= result.delta_t.N;
	if (max_size <= 0) {
		result.is_valid = false;
		return result;
	}

	result.data_length = 0;
	if ((*p & 0x80) == 0x80) {  // Present message has a status byte
		result.has_status_byte = true;
		result.status_byte = *p;
		result.data_length += 1;
		++p; --max_size;
		if (max_size <= 0) {
			result.is_valid = false;
			return result;
		}
	} else {
		result.has_status_byte = false;
		if ((prev_status_byte & 0x80) != 0x80) {
			// prev_status_byte is invalid
			result.is_valid = false;
			return result;
		}
		result.status_byte = prev_status_byte;
		// Not incrementing p; *p is the first data byte in running-status mode
	}
	// At this point, p points at the first data byte

	// To distinguish a channel_mode from a control_change msg, have to look at the
	// first data byte.  
	// TODO:  Should probably check max_size before doing this ???
	// TODO:  parse_mtrk_event_type has already made this classification as 
	// smf_event_type::channel_mode || smf_event_type::channel_voice
	result.type = channel_msg_type_from_status_byte(result.status_byte, *p);

	if (result.type == channel_msg_type::invalid) {
		result.is_valid = false;
		return result;
	}

	if ((result.status_byte & 0xF0) == 0xC0 || (result.status_byte & 0xF0) == 0xD0) {
		result.n_data_bytes = 1;
	} else {
		result.n_data_bytes = 2;
	}
	result.data_length += result.n_data_bytes;
	max_size -= result.n_data_bytes;
	if (max_size < 0) {
		result.is_valid = false;
		return result;
	}

	// Check that the first and second (if appropriate) data bytes do not have the high bit set, 
	// a state only valid for status bytes.  
	if ((*p & 0x80) != 0) {
		result.is_valid = false;
		return result;
	}
	if (result.n_data_bytes == 2) {
		++p;
		if ((*p & 0x80) != 0) {
			result.is_valid = false;
			return result;
		}
	}
	result.size = result.data_length + result.delta_t.N;
	result.is_valid = true;
	return result;
}





channel_msg_type channel_msg_type_from_status_byte(unsigned char s, unsigned char p1) {
	channel_msg_type result = channel_msg_type::invalid;
	if ((s & 0xF0) != 0xB0) {
		switch (s & 0xF0) {
			case 0x80:  result = channel_msg_type::note_off; break;
			case 0x90: result = channel_msg_type::note_on; break;
			case 0xA0:  result = channel_msg_type::key_pressure; break;
			//case 0xB0:  ....
			case 0xC0:  result = channel_msg_type::program_change; break;
			case 0xD0:  result = channel_msg_type::channel_pressure; break;
			case 0xE0:  result = channel_msg_type::pitch_bend; break;
			default: result = channel_msg_type::invalid; break;
		}
	} else if ((s & 0xF0) == 0xB0) {
		// To distinguish a channel_mode from a control_change msg, have to look at the
		// first data byte.  
		if (p1 >= 121 && p1 <= 127) {
			result = channel_msg_type::channel_mode;
		} else {
			result = channel_msg_type::control_change;
		}
	}

	return result;
}





unsigned char midi_event_get_status_byte(const unsigned char* p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return *p;
}




// TODO:  This is wrong.  It treats 0xF0u,0xF7u,0xFFu as valid status bytes
int midi_channel_event_n_bytes(unsigned char p, unsigned char s) {
	int N = 0;
	if ((p & 0x80)==0x80) {
		++N;
		s=p;
	} else if ((s & 0x80) != 0x80) {  // Both p,s are non-status bytes
		return 0;
	}

	// At this point, s is the status byte of interest.  If p was a valid status byte,
	// the value of p has replaced the value passed in as s and N == 1; if p was not
	// a valid status byte but s was, N == 0.  If neither p, s were valid status bytes
	// this point will not be reached.  
	if ((s & 0xF0) == 0xC0 || (s & 0xF0) == 0xD0) {
		N += 1;
	} else {
		N += 2;
	}

	return N;
}




bool midi_event_has_status_byte(const unsigned char *p) {
	auto delta_t_vl = midi_interpret_vl_field(p);
	if (delta_t_vl.N > 4) {
		std::abort();
	}
	p += delta_t_vl.N;
	return (*p) & 0x80;
}






int8_t channel_number_from_status_byte_unsafe(unsigned char s) {
	return static_cast<uint8_t>(s&0x0Fu + 1);
}







// TODO:  Update to be consistent w/ make_mtrk()
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char *p, uint32_t max_size) {
	validate_mtrk_chunk_result_t result {};
	auto chunk_detect = validate_chunk_header(p,max_size);
	if (chunk_detect.type != chunk_type::track) {
		if (chunk_detect.type==chunk_type::invalid) {
			result.error = mtrk_validation_error::invalid_chunk;
		} else {
			result.error = mtrk_validation_error::non_track_chunk;
		}
		return result;
	}
	
	// Process the data section of the mtrk chunk until the number of bytes
	// processed == chunk_detect.data_size, or an end-of-track meta event is
	// encountered.  Note that an end-of-track could be hit before processing
	// chunk_detect.data_size bytes if the chunk is 0-padded past the end of 
	// the end-of-track msg.  
	bool found_eot = false;
	uint32_t mtrk_data_size = chunk_detect.data_size;
	uint32_t i = 0;  // offset into the chunk data section
	p += 8;  // skip "MTrk" & the 4-byte length
	unsigned char rs {0};  // value of the running-status
	validate_mtrk_event_result_t curr_event;
	while ((i<mtrk_data_size) && !found_eot) {
		curr_event = validate_mtrk_event_dtstart(p,rs,mtrk_data_size-i);
		if (curr_event.error!=mtrk_event_validation_error::no_error) {
			result.error = mtrk_validation_error::event_error;
			return result;
		}

		rs = curr_event.running_status;

		if (curr_event.type==smf_event_type::meta) {
			// Test for end-of-track msg
			if (curr_event.size>=4) {
				uint32_t last_four = dbk::be_2_native<uint32_t>(p+curr_event.size-4);
				if ((last_four&0x00FFFFFFu)==0x00FF2F00u) {
					found_eot = true;
				}
			}
		}
		i += curr_event.size;
		p += curr_event.size;
	}

	// The track data must not overrun the reported size in the MTrk header,
	// and the final event must be a meta-end-of-track msg.  
	// Note that it is possible that found_eot==true even though 
	// i<mtrk_data_size.  I am allowing for the possibility that 
	// i<mtrk_data_size (which could perhaps => that the track is 0-padded 
	// after the end-of-track msg), but forbidding i>mtrk_data_size.  In any 
	// case, the 'length' field in the MTrk header section must be >= than
	// the actual amount of track data preceeding the EOT message.  
	if (i>mtrk_data_size) {
		result.error = mtrk_validation_error::data_length_not_match;
		return result;
	}
	if (!found_eot) {
		result.error = mtrk_validation_error::no_end_of_track;
		return result;
	}

	result.size = chunk_detect.size;
	result.data_size = chunk_detect.data_size;
	result.p = p;  // pointer to the 'M' of "MTrk"...
	result.error = mtrk_validation_error::no_error;
	return result;
}

std::string print_error(const validate_mtrk_chunk_result_t& mtrk) {
	std::string result = "Invalid MTrk chunk:  ";
	switch (mtrk.error) {
		case mtrk_validation_error::invalid_chunk:
			result += "detect_chunk_type(p,max_size)==chunk_type::invalid; "
				"call detect_chunk_type() and examine the 'error' field of "
				"the result.  ";
			break;
		case mtrk_validation_error::non_track_chunk:
			result += "The first 4 bytes at p are not equal to the ASCII values "
				"for 'MTrk'";
			break;
		case mtrk_validation_error::data_length_not_match:
			result += "Data-length mismatch:  ???";
			break;
		case mtrk_validation_error::event_error:
			result += "Some kind of event-error";
			break;
		case mtrk_validation_error::unknown_error:
			result += "Unknown_error :(";
			break;
		case mtrk_validation_error::no_end_of_track:
			result += "Track does not terminate in an end-of-track meta-event.";
			break;
	}
	return result;
}






