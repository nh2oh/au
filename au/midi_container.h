#include <cstdint>
#include <array>
#include <algorithm>
#include <string>

namespace mc {

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







struct whatever {
	struct header {};
	struct track {};
	struct unknown {};
	struct invalid {};
};


template<typename T>
class midi_chunk_container_t {
public:
	using type = T;

	midi_chunk_container_t()=default;
	midi_chunk_container_t(const unsigned char *p) : p_(p) {};

	std::array<char,4> id() const {
		std::array<char,4> result {};
		std::copy(p_,p_+4,result.begin());
		return result;
	};

	int32_t data_length() const {
		return midi_raw_interpret<int32_t>(p_+4);
	};

	int32_t size() const {
		return this->data_length() + 8;
	};

	// iterator data_begin(), data_end() to return const unsigned char on deref?
	const unsigned char *p_ {};
private:
	
};


int16_t fmt_type(const midi_chunk_container_t<whatever::header>&);
int16_t num_trks(const midi_chunk_container_t<whatever::header>&);

enum class midi_time_division_field_type_t {
	ticks_per_quarter,
	SMPTE
};
midi_time_division_field_type_t detect_midi_time_division_type(const midi_chunk_container_t<whatever::header>&);
uint16_t interpret_tpq_field(const midi_chunk_container_t<whatever::header>&);  // assumes midi_time_division_field_type_t::ticks_per_quarter
struct midi_smpte_field {
	int8_t time_code_fmt {0};
	uint8_t units_per_frame {0};
};
midi_smpte_field interpret_smpte_field(const midi_chunk_container_t<whatever::header>&);  // assumes midi_time_division_field_type_t::SMPTE

std::string print(const midi_chunk_container_t<whatever::header>&);





};  // namespace mc

/*
int64_t somehow_calc_event_size(const unsigned char*);
int64_t somehow_detn_event_type(const unsigned char*);
struct mtrk_container_t;
struct mtrk_event_container_t;

// want to be obtainable only from a method of mtrk_container_t so that *begin_ always
// points at the start of a valid mtrk event.  
struct mtrk_container_iterator {
	const mtrk_container_t *container_ {};
	const unsigned char *begin_ {};

	mtrk_container_iterator& operator++() {
		auto sz = somehow_calc_event_size(this->begin_);
		this->begin_ += sz;
	};

	mtrk_event_container_t operator*() const {
		mtrk_event_container_t result {};
		//result.begin_ = this->begin_;
		//result.size = somehow_calc_event_size(this->begin_);
		return result;
	};


	bool end() const {
		auto this_end = this->begin_ + somehow_calc_event_size(this->begin_);
		return ((this_end - this->container_->begin_)==this->container_->size);
	};
};

struct mtrk_container_t {
	const unsigned char *begin_ {};

	// size of the whole chunk including the id and length fields
	const int64_t size {0};

	mtrk_container_iterator begin() const;
};


struct mtrk_event_container_t {
	enum event_type {
		midi,
		sysex,
		meta,
		unknown,
		invalid
	};
	mtrk_event_container_t::event_type type {mtrk_event_container_t::event_type::unknown};
	const unsigned char *begin_ {};

	// size of the whole event including the delta-t field
	const int64_t size {0};

	// ptr to the start of the delta-t field
	const unsigned char *delta_time() const;

	// ptr to event start (including the length field in the case of sysex & meta events)
	const unsigned char *event() const;
	
	// size of the event not including the delta-t field (but including the length field 
	// in the case of sysex & meta events)
	int32_t data_size() const;
};
*/

