#pragma once
#include <algorithm>  // std::reverse_copy() in midi_raw_interpret()
#include <string>
#include <vector>

//
// TODO:  validate_, parse_, detect_ naming inconsistency
//



// 
// Copies the bytes in the range [p,p+sizeof(T)) into the range occupied by a T such that the
// byte order in the source and destination ranges are the reverse of oneanother.  Hence
// big-endian encoded T in [p,p+sizeof(T)) is corrrectly interpreted on an LE architecture.  
// Obviously this byte order swapping is only needed for interpreting midi files on LE 
// architectures.  
// 
template<typename T>
T midi_raw_interpret(const unsigned char* p) {
	T result {};
	unsigned char *p_result = static_cast<unsigned char*>(static_cast<void*>(&result));
	std::reverse_copy(p,p+sizeof(T),p_result);
	return result;
};

// 
// The max size of a vl field is 4 bytes; returns [0,4]
//
struct midi_vl_field_interpreted {
	int8_t N {0};
	int32_t val {0};  // TODO:  uint32_t ???
};
midi_vl_field_interpreted midi_interpret_vl_field(const unsigned char*);


//
// Encodes T in the form of a VL quantity, the maximum size of which, according
// to the MIDI std is 4 bytes.  
// For values requiring less than 4 bytes in encoded form, the rightmost
// bytes of the array will be 0.  
//
template<typename T>  // T ~ integral
std::array<unsigned char,4> midi_encode_vl_field(T val) {
	static_assert(sizeof(T)<=4);  // The max size of a vl field is 4 bytes
	std::array<unsigned char,4> result {0x00,0x00,0x00,0x00};

	int i = 3;
	result[i] = val & 0x7F;
	while (i>0 && (val >>= 7) > 0) {
		--i;
		result[i] |= 0x80;
		result[i] += (val & 0x7F);
	}

	// Shift the elements of result to the left so that result[0] contains 
	// the first nonzero byte and any zero bytes are at the end of result (beyond
	// the first byte w/ bit 7 == 0).  
	for (int j=0; j<4; ++j) { 
		if (i<=3) {
			result[j] = result[i];
			++i;
		} else {
			result[j] = 0x00;
		}
	}

	return result;
};


namespace mc {

std::string print_hexascii(const unsigned char*, int, const char = ' ');


//
// There are two types of chunks: the Header chunk, containing data pertaining to the entire file 
// (only one per file), and the Track chunk (possibly >1 per file).  Both chunk types have the 
// layout implied by struct midi_chunk.  Note that the length field is always 4 bytes and is not a
// vl-type quantity.  From p. 132: "Your programs should expect alien chunks and treat them as if
// they weren't there."  
//
enum class chunk_type {
	header,  // MThd
	track,  // MTrk
	unknown  // The std requires that unrecognized chunk types be permitted
};
enum event_type {  // MTrk events
	midi,
	sysex,
	meta
};
std::string print(const event_type&);


struct parse_meta_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	uint8_t type {0};
	int32_t data_size {};  // Everything not delta_time
};
parse_meta_event_result_t parse_meta_event(const unsigned char*);

struct parse_sysex_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	uint8_t type {0};  // F0 or F7
	int32_t data_size {};  // Everything not delta_time
};
parse_sysex_event_result_t parse_sysex_event(const unsigned char*);

struct parse_midi_event_result_t {
	bool is_valid {false};
	midi_vl_field_interpreted delta_t {};
	bool has_status_byte {false};
	uint8_t status_byte {0};
	uint8_t n_data_bytes {0};  // 0, 1, 2
	int32_t data_size {};  // Everything not delta_time
};
parse_midi_event_result_t parse_midi_event(const unsigned char*, unsigned char=0);
bool midi_event_has_status_byte(const unsigned char*);
unsigned char midi_event_get_status_byte(const unsigned char*);


//
// Checks for the 4-char id and the 4-byte size.  Verifies that the id + size field 
// + the reported size does not exceed the max_size suppled as the second argument.  
// Does _not_ inspect anything past the end of the length field.  
//
struct detect_chunk_type_result_t {
	chunk_type type {chunk_type::unknown};
	int32_t size {0};
	std::string msg {};
	bool is_valid {false};
	const unsigned char* p {};
};
detect_chunk_type_result_t detect_chunk_type(const unsigned char*, int32_t);



struct parse_mtrk_event_result_t {
	bool is_valid {false};  // False if the delta-t vl field does not validate
	midi_vl_field_interpreted delta_t {};
	event_type type {event_type::midi};
};
parse_mtrk_event_result_t parse_mtrk_event_type(const unsigned char*);
event_type detect_mtrk_event_type_dtstart_unsafe(const unsigned char*);
event_type detect_mtrk_event_type_unsafe(const unsigned char*);


struct validate_mtrk_chunk_result_t {
	bool is_valid {false};
	std::string msg {};
	const unsigned char *p;
	int32_t size {0};
};
validate_mtrk_chunk_result_t validate_mtrk_chunk(const unsigned char*, int32_t);



struct chunk_idx_t {
	chunk_type type {};
	int32_t offset {0};
	int32_t size {0};
};
struct validate_smf_result_t {
	bool is_valid {};
	std::string msg {};

	// Needed by the smf_container_t ctor
	std::string fname {};
	const unsigned char *p {};
	int32_t size {0};
	int n_mtrk {0};  // Number of MTrk chunks
	int n_unknown {0};
	std::vector<chunk_idx_t> chunk_idxs {};
};
validate_smf_result_t validate_smf(const unsigned char*, int32_t, const std::string&);







};  // namespace mc

