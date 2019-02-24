#pragma once
#include "midi_raw.h"
#include "mthd_container_t.h"
#include "mtrk_container_t.h"
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>


//
// In an alternative design, i just have functions like is_midi_msg(), is_channel_voice(),
// is_note_on() functions that are either members of a midi_event_t class (as below), take
// arguments of some sort of midi_event_t class, or take an mtrk_event_container_t.  Since 
// they return bools, no assurances about the underlying data are required.  This might help
// reduce the enum class explosion.  It might be simpler than having member functions like type()
// and channel_msg_type().  
//
// function template mtrk_event_container_t.is_a(T)
// T is one of a family of structs holding static field sizes, byte patterns for status bytes,
// etc.  
//
//

/*  in midi_raw.h
enum class channel_msg_t {
	note_on,
	note_off,
	key_pressure,
	control_change,
	program_change,
	channel_pressure,
	pitch_bend,
	channel_mode,
	invalid
};*/
class midi_event_container_t {
public:
	midi_event_container_t(mtrk_event_container_t mtrkev, unsigned char status) 
		: midi_status_(status), p_(mtrkev.raw_begin()), size_(mtrkev.size()) {};

	unsigned char raw_status() const;
	bool status_is_running() const;

	midi_msg_t type() const;  // channel_voice, channel_mode, sysex_common, ...
	channel_msg_t channel_msg_type() const;  // channel_msg_t::...

	int8_t channel_number() const;  // for midi_msg_t::channel_voice || channel_mode

	int8_t note_number() const;  // for channel_msg_t::note_on || note_off || key_pressure
	int8_t velocity() const;  // for channel_msg_t::note_on || note_off
	int8_t key_pressure() const;   // for channel_msg_t::key_pressure
	int8_t control_number() const;  // for channel_msg_t::control_change
	int8_t control_value() const;  // for channel_msg_t::control_change
	int8_t program_number();  // for channel_msg_t::program_change
	int8_t channel_pressure();  // for channel_msg_t::channel_pressure
	int16_t pitch_bend_value() const;  // for channel_msg_t::pitch_bend

private:
	unsigned char midi_status_ {0};
	const unsigned char *p_ {};  // points at the delta_t
	int32_t size_ {0};  // delta_t + payload
};



