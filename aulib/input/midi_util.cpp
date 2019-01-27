#include "midi_util.h"

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
