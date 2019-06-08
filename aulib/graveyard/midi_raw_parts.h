channel_msg_type mtrk_event_get_ch_msg_type_dtstart_unsafe(const unsigned char*,
	unsigned char=0x00u);
	

	
	
	
	

struct parse_channel_event_result_t {
	bool is_valid {false};
	channel_msg_type type {channel_msg_type::invalid};
	midi_vl_field_interpreted delta_t {};
	bool has_status_byte {false};
	unsigned char status_byte {0};
	uint8_t n_data_bytes {0};  // 0, 1, 2
	int32_t size {0};
	int32_t data_length {0};  // Everything not delta_time
};
parse_channel_event_result_t parse_channel_event(const unsigned char*, unsigned char=0, int32_t=0);





// arg 1 => status byte, arg 2 => first data byte, needed iff arg1 & 0xF0 == 0xB0.  
// Not really "_unsafe"-worthy since will return channel_msg_type::invalid if the
// status byte is not legit.  
channel_msg_type channel_msg_type_from_status_byte(unsigned char, unsigned char=0);





enum class channel_msg_type : uint8_t {
	note_on,
	note_off,
	key_pressure,
	control_change,
	program_change,
	channel_pressure,
	pitch_bend,
	channel_mode,
	invalid
};




// TODO:  Deprecate
unsigned char midi_event_get_status_byte(const unsigned char*);  // dtstart




// For the midi _channel_ event implied by the status byte arg 1 (or, if arg 1
// is not a status byte, the status byte arg 2), returns the number of expected
// data bytes + 1 if the first arg is a valid status byte; + 0 if arg 1 is not
// a valid status byte but arg 2 is a valid status byte.  Returns 0 if neither
// arg 1, arg 2 are valid status bytes.
int midi_channel_event_n_bytes(unsigned char, unsigned char); 




bool midi_event_has_status_byte(const unsigned char*);




// Result is only valid for channel_voice or channel_mode status bytes:  Does not 
// verify that the input is a legit channel_voice or _mode status byte.  
int8_t channel_number_from_status_byte_unsafe(unsigned char);









//
// Validation & processing of MTrk chunks
//
// validate_mtrk_chunk(const unsigned char*, uint32_t=0) checks that p
// points to the start of a valid MTrk chunk (ie, the 'M' of MTrk...),
// then iterates through the all events in the track and verifies that
// each begins with a valid delta-time vlq and can be classified as one
// of channel_voice, channel_mode, meta, or sysex f0/f7 (any event 
// classifying as smf_event_type::invalid returns w/ 
// error==mtrk_validation_error::event_error.  A return value with
// error==mtrk_validation_error::no_error ensures that an mtrk_event_t 
// can be constructed from all the events in the track.  
//
// TODO:  validate_mtrk_chunk() is completely obsoleted by make_mtrk().  
// It is still useful to have a validation only fn, however.  To
// validate views, for example...
//
// If the input is a valid MTrk chunk, the validate_mtrk_chunk_result_t
// returned can be passed to the ctor of mtrk_container_t to instantiate
// a valid object.  
//
// All the rules of the midi standard as regards sequences of MTrk events
// are validated here for the case of the single track indicated by the 
// input.  Ex, that each midi event has a status byte (either as part of 
// the event or implictly through running status), etc.  
//
enum class mtrk_validation_error : uint8_t {
	invalid_chunk,  // No 4-char ASCII header, reported size exceeds max-size, ...
	non_track_chunk,  // *p != "MTrk"
	data_length_not_match,  // Reported length does not match length of track
	event_error,  // some sort of error w/an internal event
	delta_time_error,
	no_end_of_track,
	unknown_error,
	no_error
};
struct validate_mtrk_chunk_result_t {
	// points at the 'M' of "MTrk"...
	const unsigned char *p {nullptr};
	// TODO:  The size,data_size relations claimed below are not always true,
	// for example, where the header-reported size is > than the number of
	// bytes before the end-of-track msg.  
	// Always == reported size (data_length) + 8
	uint32_t size {0};  
	// The reported length; !including the "MTrk" and length fields
	// Thus, always == size-8.
	uint32_t data_size {0};  
	mtrk_validation_error error {mtrk_validation_error::unknown_error};
};
// ptr, max_size
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*, uint32_t);
std::string print_error(const validate_mtrk_chunk_result_t&);





