#pragma once
#include <vector>
#include <cstdint>



//
// Status bytes,data bytes; valid and invalid
// "sbs" == "status bytes"
// "dbs" == "data bytes"
//
extern std::vector<unsigned char> sbs_invalid;
extern std::vector<unsigned char> sbs_meta_sysex;
extern std::vector<unsigned char> sbs_unrecognized;
extern std::vector<unsigned char> sbs_ch_mode_voice;
extern std::vector<unsigned char> dbs_valid;
extern std::vector<unsigned char> dbs_invalid;




struct test_setab_t {
	std::vector<unsigned char> data {};
	uint8_t dtsize {0};
	unsigned char rs_pre {0x00u};
	unsigned char rs_post {0x00u};
};
extern std::vector<test_setab_t> set_a_valid_rs;
extern std::vector<test_setab_t> set_b_invalid_rs;

struct test_setc_t {
	std::vector<unsigned char> data {};  // data.size()==dt_field_size+data_length
	unsigned char midisb_prev_event {};  // "midi status byte previous event"
	unsigned char applic_midi_status {};  // "applicable midi status byte"
	bool in_running_status {};
	uint8_t n_data_bytes {};  // Based on value of applic_midi_status
	uint32_t data_length {};  // n_data_bytes (+ 1 if not in running status)
	uint32_t dt_value {};
	uint8_t dt_field_size {};
};
extern std::vector<test_setc_t> set_c_midi_events_valid;
extern std::vector<test_setc_t> set_d_midi_events_nostatus_invalid;
