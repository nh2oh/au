#include "midi_container.h"
#include "midi_raw.h"
#include "dbklib\binfile.h"
#include <iostream>
#include <string>
#include <vector>


// channel_voice, channel_mode, sysex_common, ...
midi_msg_t midi_event_container_t::type() const {
	return parse_midi_event(this->p_, this->midi_status_).type;
	/*auto p = this->p_;
	auto event_parsed = parse_midi_event(this->p_, this->midi_status_);
	p += event_parsed.delta_t.N;
	if (event_parsed.has_status_byte && event_parsed.n_data_bytes>0) {
		p+=1;
	}
	// p now points at the first data byte.  Note that for allidi messages, n_data_bytes
	// == 1 or 2.  

	if ((this->midi_status_ & 0xF0) == 0xB0) {
		if ((*p & 0xF8) == 0x78) {
			return midi_msg_t::channel_mode;
		}
	} else if ((this->midi_status_ & 0xF0) == 0xF0) {
		return midi_msg_t::system_something;
	} else {
		return midi_msg_t::channel_voice;
	}*/
}

channel_msg_t midi_event_container_t::channel_msg_type() const {
	if (type() == midi_msg_t::channel_voice) {
		switch (this->midi_status_ & 0xF0) {
			case 0x80:  return channel_msg_t::note_off; break;
			case 0x90:  return channel_msg_t::note_on; break;
			case 0xA0:  return channel_msg_t::key_pressure; break;
			case 0xB0:  return channel_msg_t::control_change; break;
			case 0xC0:  return channel_msg_t::program_change; break;
			case 0xD0:  return channel_msg_t::channel_pressure; break;
			case 0xE0:  return channel_msg_t::pitch_bend; break;
		}
	} else if (type() == midi_msg_t::channel_mode) {
		return channel_msg_t::channel_mode;
	}

	return channel_msg_t::invalid;
}

// for midi_msg_t::channel_voice || channel_mode
int8_t midi_event_container_t::channel_number() const {
	return this->midi_status_ & 0x0F;
}

// for channel_msg_t::note_on || note_off || key_pressure
int8_t midi_event_container_t::note_number() const {
	auto p = this->p_;
	auto event_parsed = parse_midi_event(this->p_, this->midi_status_);
	p += event_parsed.delta_t.N;
	if (event_parsed.has_status_byte && event_parsed.n_data_bytes>0) {
		p+=1;
	}
	// p now points at the first data byte, unless event_parsed.n_data_bytes==0, in which
	// case p points at the status byte.  

	return *p;
}



