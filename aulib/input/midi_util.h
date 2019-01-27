#include <array>
#include <algorithm>  // std::reverse_copy() in midi_raw_interpret()

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

template<typename T>  // T ~ integral
std::array<unsigned char,4> midi_encode_vl_field(T val) {
	static_assert(sizeof(T)<=4);  // The max size of a vl field is 4 bytes
	std::array<unsigned char,4> result {0x00,0x00,0x00,0x00};

	int i = 3;
	result[i] = val & 0x7F;
	while (i > 1 && (val >>= 7) > 0) {
		--i;
		result[i] |= 0x80;
		result[i] += (val & 0x7F);
	}

	for (int j=0; j<4; ++j) { 
		if (i<3) {
			result[j] = result[i];
			++i;
		} else {
			result[j] = 0x00;
		}
	}

	return result;
}



