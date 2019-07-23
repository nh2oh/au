#include "mtrk_event_t.h"
#include "mtrk_event_methods.h"
#include "midi_raw.h"
#include "midi_vlq.h"
#include <string>
#include <cstdint>
#include <algorithm>
#include <limits>  // std::numeric_limits<uint32t>::max()



mtrk_event_t::mtrk_event_t() {  
	this->default_init(0);
}
mtrk_event_t::mtrk_event_t(uint32_t dt) {
	this->default_init(dt);
}

mtrk_event_t::mtrk_event_t(const unsigned char *p, 
							mtrk_event_t::size_type sz_max, unsigned char rs) {
	this->d_ = mtrk_event_t_internal::small_bytevec_t();
	auto dt = midi_interpret_vl_field(p,sz_max);
	if (!dt.is_valid) {
		this->default_init();
		return;
	}
	if (dt.N==sz_max) {
		// If the caller is passing in a buffer containing only a delta-time,
		// *(p+dt.N) is an invalid deref
		this->default_init(dt.val);
		return;
	}
	// Note that dt.N is the number of bytes the delta-time actually
	// occupies in the input buffer; this is not necessarily the same 
	// as the number of bytes it occupies if canonically encoded, which
	// is given by delta_time_field_size(dt.val).  
	auto s = get_status_byte(*(p+dt.N),rs);
	bool has_local_status = (s==*(p+dt.N));
	auto data_size = mtrk_event_get_data_size(p+dt.N,rs,(sz_max-dt.N));
	if (data_size==0) {
		// mtrk_event_get_data_size() returns 0 if the event is invalid 
		// in any way.  
		this->default_init();
		return;
	}
	mtrk_event_t::size_type sz = delta_time_field_size(dt.val)
		+ data_size + !has_local_status;
	sz = std::clamp(sz,0,mtrk_event_t::size_max);

	this->d_.resize(sz);
	auto dest = write_delta_time(dt.val,this->d_.begin());
	if (!has_local_status) {
		*dest++ = s;
	}
	std::copy(p+dt.N,p+dt.N+data_size,dest);
}
mtrk_event_t::mtrk_event_t(const uint32_t& dt, const unsigned char *p, 
							mtrk_event_t::size_type sz_max, unsigned char rs) {
	this->d_ = mtrk_event_t_internal::small_bytevec_t();
	auto data_size = mtrk_event_get_data_size(p,rs,sz_max);
	if (data_size==0) {
		// mtrk_event_get_data_size() returns 0 if the event is invalid 
		// in any way.  
		this->default_init(dt);
		return;
	}
	auto s = get_status_byte(*p,rs);
	bool has_local_status = (s==*p);
	
	mtrk_event_t::size_type sz = delta_time_field_size(dt)
		+ data_size + !has_local_status;
	sz = std::clamp(sz,0,mtrk_event_t::size_max);

	this->d_.resize(sz);
	auto dest = write_delta_time(dt,this->d_.begin());
	if (!has_local_status) {
		*dest++ = s;
	}
	std::copy(p,p+data_size,dest);
}

mtrk_event_t::mtrk_event_t(uint32_t dt, midi_ch_event_t md) {
	this->d_ = mtrk_event_t_internal::small_bytevec_t();
	unsigned char s = (md.status_nybble)|(md.ch);
	auto n = channel_status_byte_n_data_bytes(s);
	if (n==0) {
		this->default_init(dt);
	}
	// NB:  n==0 if s is invalid (ex, s==0xFFu)
	this->d_.resize(delta_time_field_size(dt)+1+n);  // +1=>s
	auto dest = write_delta_time(dt,this->d_.begin());
	*dest++ = s;
	*dest++ = (md.p1)&0x7Fu;
	if (n==2) {
		*dest++ = (md.p2)&0x7Fu;
	}
}
mtrk_event_t::mtrk_event_t(const mtrk_event_t& rhs) {
	this->d_=rhs.d_;
}
mtrk_event_t& mtrk_event_t::operator=(const mtrk_event_t& rhs) {
	this->d_ = rhs.d_;
	return *this;
}
mtrk_event_t::mtrk_event_t(mtrk_event_t&& rhs) noexcept {
	this->d_ = std::move(rhs.d_);
	rhs.default_init();
}
mtrk_event_t& mtrk_event_t::operator=(mtrk_event_t&& rhs) noexcept {
	this->d_ = std::move(rhs.d_);
	rhs.default_init();
	return *this;
}
mtrk_event_t::~mtrk_event_t() noexcept {  // dtor
	//...
}

mtrk_event_t::size_type mtrk_event_t::size() const {
	return this->d_.size();
	//auto sz = mtrk_event_get_size_dtstart_unsafe(this->d_.begin(),0x00u);
	//auto cap = this->capacity();
	//return sz > cap ? cap : sz;
}
mtrk_event_t::size_type mtrk_event_t::capacity() const {
	return this->d_.capacity();
}
mtrk_event_t::size_type mtrk_event_t::resize(mtrk_event_t::size_type new_cap) {
	new_cap = std::clamp(new_cap,0,mtrk_event_t::size_max);
	return this->d_.resize(new_cap);
}
mtrk_event_t::size_type mtrk_event_t::reserve(mtrk_event_t::size_type new_cap) {
	new_cap = std::clamp(new_cap,0,mtrk_event_t::size_max);
	return this->d_.reserve(new_cap);
}
unsigned char *mtrk_event_t::data() {
	return this->d_.begin();
}
const unsigned char *mtrk_event_t::data() const {
	return this->d_.begin();
}
mtrk_event_t::iterator mtrk_event_t::begin() {
	return mtrk_event_t::iterator(this->data());
}
mtrk_event_t::const_iterator mtrk_event_t::begin() const {
	return mtrk_event_t::const_iterator(this->data());
}
mtrk_event_t::iterator mtrk_event_t::end() {
	return this->begin()+this->size();
}
mtrk_event_t::const_iterator mtrk_event_t::end() const {
	return this->begin()+this->size();
}
mtrk_event_t::const_iterator mtrk_event_t::dt_begin() const {
	return this->begin();
}
mtrk_event_t::iterator mtrk_event_t::dt_begin() {
	return this->begin();
}
mtrk_event_t::const_iterator mtrk_event_t::dt_end() const {
	return advance_to_dt_end(this->begin(),this->end());
}
mtrk_event_t::iterator mtrk_event_t::dt_end() {
	return advance_to_dt_end(this->begin(),this->end());
}
mtrk_event_t::const_iterator mtrk_event_t::event_begin() const {
	return advance_to_dt_end(this->begin(),this->end());
}
mtrk_event_t::iterator mtrk_event_t::event_begin() {
	return advance_to_dt_end(this->begin(),this->end());
}
mtrk_event_t::const_iterator mtrk_event_t::payload_begin() const {
	auto it = this->event_begin();
	if (this->type()==smf_event_type::meta) {
		it += 2;  // 0xFFu, type-byte
		it = advance_to_dt_end(it);
	} else if (this->type()==smf_event_type::sysex_f0
					|| this->type()==smf_event_type::sysex_f7) {
		it += 1;  // 0xF0u or 0xF7u
		it = advance_to_dt_end(it);
	} // else { smf_event_type::channel_voice, _mode, unknown, invalid...
	return it;
}
mtrk_event_t::iterator mtrk_event_t::payload_begin() {
	auto it = this->event_begin();
	if (this->type()==smf_event_type::meta) {
		it += 2;  // 0xFFu, type-byte
		it = advance_to_dt_end(it);
	} else if (this->type()==smf_event_type::sysex_f0
					|| this->type()==smf_event_type::sysex_f7) {
		it += 1;  // 0xF0u or 0xF7u
		it = advance_to_dt_end(it);
	} // else { smf_event_type::channel_voice, _mode, unknown, invalid...
	return it;
}
iterator_range_t<mtrk_event_t::const_iterator> mtrk_event_t::payload_range() const {
	auto sz = this->size();
	auto it = this->event_begin();
	if (this->type()==smf_event_type::meta) {
		it += 2;  // 0xFFu, type-byte
		it = advance_to_dt_end(it);
	} else if (this->type()==smf_event_type::sysex_f0
					|| this->type()==smf_event_type::sysex_f7) {
		it += 1;  // 0xF0u or 0xF7u
		it = advance_to_dt_end(it);
	} // else { smf_event_type::channel_voice, _mode, unknown, invalid...
	return {it,this->end()};
}
iterator_range_t<mtrk_event_t::iterator> mtrk_event_t::payload_range() {
	auto sz = this->size();
	auto it = this->event_begin();
	if (this->type()==smf_event_type::meta) {
		it += 2;  // 0xFFu, type-byte
		it = advance_to_dt_end(it);
	} else if (this->type()==smf_event_type::sysex_f0
					|| this->type()==smf_event_type::sysex_f7) {
		it += 1;  // 0xF0u or 0xF7u
		it = advance_to_dt_end(it);
	} // else { smf_event_type::channel_voice, _mode, unknown, invalid...
	return {it,this->end()};
}
const unsigned char mtrk_event_t::operator[](mtrk_event_t::size_type i) const {
	return *(this->data()+i);
};
unsigned char mtrk_event_t::operator[](mtrk_event_t::size_type i) {
	return *(this->data()+i);
};
unsigned char *mtrk_event_t::push_back(unsigned char c) {
	return this->d_.push_back(c);
}

smf_event_type mtrk_event_t::type() const {
	auto p = advance_to_dt_end(this->begin(),this->end());
	return classify_status_byte(*p);
	// Faster than classify_mtrk_event_dtstart_unsafe(s,rs), b/c the latter
	// calls classify_status_byte(get_status_byte(s,rs)); here, i do not 
	// need to worry about the possibility of the rs byte.  
}
uint32_t mtrk_event_t::delta_time() const {
	return midi_interpret_vl_field(this->data()).val;
}
unsigned char mtrk_event_t::status_byte() const {
	return *advance_to_dt_end(this->d_.begin(),this->d_.end());
}
unsigned char mtrk_event_t::running_status() const {
	auto p = advance_to_dt_end(this->d_.begin(),this->d_.end());
	return get_running_status_byte(*p,0x00u);
}
mtrk_event_t::size_type mtrk_event_t::data_size() const {  // Not including delta-t
	auto end = this->end();
	auto p = advance_to_dt_end(this->begin(),end);
	return end-p;
}

uint32_t mtrk_event_t::set_delta_time(uint32_t dt) {
	dt &= 0x0FFFFFFFu;
	auto new_dt_size = delta_time_field_size(dt);
	auto beg = this->begin();  auto end = this->end();
	auto curr_dt_size = advance_to_dt_end(beg,end)-beg;
	if (curr_dt_size == new_dt_size) {
		write_delta_time(dt,beg);
	} else if (new_dt_size < curr_dt_size) {  // shrink present event
		auto new_event_beg = beg+new_dt_size;
		std::copy(this->event_begin(),this->end(),new_event_beg);
		write_delta_time(dt,this->begin());
		// TODO:  resize() this->d_ ?
		//midi_rewrite_dt_field_unsafe(dt,this->data(),0x00u);
		auto new_size = this->size()-(curr_dt_size-new_dt_size);
		this->d_.resize(new_size);
	} else if (new_dt_size > curr_dt_size) {  // grow present event
		auto old_size = this->size();
		auto new_size = old_size + (new_dt_size-curr_dt_size);
		this->d_.resize(new_size);  // NB:  Invalidates iterators!
		std::copy_backward(this->begin(),this->begin()+old_size,this->end());
		write_delta_time(dt,this->begin());
	}
	return this->delta_time();
}

bool mtrk_event_t::operator==(const mtrk_event_t& rhs) const {
	auto it_lhs = this->begin();  auto lhs_end = this->end();
	auto it_rhs = rhs.begin();  auto rhs_end = rhs.end();
	if ((lhs_end-it_lhs) != (rhs_end-it_rhs)) {
		return false;
	}
	while (it_lhs!=lhs_end && it_rhs!=rhs_end) {
		if (*it_lhs != *it_rhs) {
			return false;
		}
		++it_lhs;
		++it_rhs;
	}
	return true;
}
bool mtrk_event_t::operator!=(const mtrk_event_t& rhs) const {
	return !(*this==rhs);
}

void mtrk_event_t::default_init(uint32_t dt) {
	this->d_ = mtrk_event_t_internal::small_bytevec_t();
	this->d_.resize(delta_time_field_size(dt)+3);
	auto it = write_delta_time(dt,this->d_.begin());
	*it++ = 0x90u;  // Note-on, channel "1"
	*it++ = 0x3Cu;  // 0x3C==60=="Middle C" (C4, 261.63Hz)
	*it++ = 0x3Fu;  // 0x3F==63, ~= 127/2
}


const unsigned char *mtrk_event_t::raw_begin() const {
	return this->d_.raw_begin();
}
const unsigned char *mtrk_event_t::raw_end() const {
	return this->d_.raw_end();
}
unsigned char mtrk_event_t::flags() const {
	return *(this->d_.raw_begin());
}
bool mtrk_event_t::is_big() const {
	return !(this->flags()&0x80u);
}
bool mtrk_event_t::is_small() const {
	return !(this->is_big());
}


maybe_mtrk_event_t::operator bool() const {
	auto tf = (this->error==mtrk_event_error_t::errc::no_error);
	return tf;
}
validate_channel_event_result_t::operator bool() const {
	return this->error==mtrk_event_error_t::errc::no_error;
}
validate_meta_event_result_t::operator bool() const {
	return this->error==mtrk_event_error_t::errc::no_error;
}
validate_sysex_event_result_t::operator bool() const {
	return this->error==mtrk_event_error_t::errc::no_error;
}
void mtrk_event_error_t::set(mtrk_event_error_t::errc code, int32_t dt,
			unsigned char rs, unsigned char s, const unsigned char *beg, 
			const unsigned char *end) {
	this->code = code;
	this->dt_input = dt;
	this->header.fill(0x00u);
	this->rs = rs;
	this->s = s;
	if (!beg || !end || (beg>=end)) { return; }
	auto n = end-beg;
	if (n > static_cast<int>(this->header.size())) {
		n = static_cast<int>(this->header.size());
	}
	std::copy(beg,beg+n,this->header.begin());
}

maybe_mtrk_event_t make_mtrk_event(const unsigned char *beg,
					const unsigned char *end, unsigned char rs,
					mtrk_event_error_t *err) {
	maybe_mtrk_event_t result;
	result.error = mtrk_event_error_t::errc::other;

	auto set_err = [&err](mtrk_event_error_t::errc code,int32_t dt, 
				unsigned char rs, unsigned char s, const unsigned char *beg, 
				const unsigned char *end) -> void {
		if (!err) { return; }
		err->set(code,dt,rs,s,beg,end);
	};

	if (!beg || !end || (beg>=end)) {
		result.error = mtrk_event_error_t::errc::zero_sized_input;
		set_err(result.error,0,rs,0x00u,nullptr,nullptr);
		return result;
	}

	auto dt = midi_interpret_vl_field(beg,end);
	if (!dt.is_valid) {
		result.error = mtrk_event_error_t::errc::invalid_delta_time;
		set_err(result.error,dt.val,rs,0x00u,beg,end);
		return result;
	}
	result = make_mtrk_event(dt.val,beg+dt.N,end,rs,err);
	return result;
}

maybe_mtrk_event_t make_mtrk_event(int32_t dt, const unsigned char *beg,
					const unsigned char *end, unsigned char rs,
					mtrk_event_error_t *err) {
	maybe_mtrk_event_t result;
	result.error = mtrk_event_error_t::errc::other;

	auto set_err = [&err](mtrk_event_error_t::errc code,int32_t dt, 
				unsigned char rs, unsigned char s, const unsigned char *beg, 
				const unsigned char *end) -> void {
		if (!err) { return; }
		err->set(code,dt,rs,s,beg,end);
	};

	if (!is_valid_delta_time(dt)) {
		result.error = mtrk_event_error_t::errc::invalid_delta_time;
		set_err(result.error,dt,rs,0x00u,nullptr,nullptr);
		return result;
	}
	auto dt_n = delta_time_field_size(dt);

	if (!beg || !end || (beg>=end)) {
		result.error = mtrk_event_error_t::errc::zero_sized_input;
		set_err(result.error,dt,rs,0x00u,nullptr,nullptr);
		return result;
	}

	auto s = get_status_byte(*beg,rs);
	if (is_channel_status_byte(s)) {
		auto ch = validate_channel_event(beg,end,rs);
		if (!ch) {
			result.error = ch.error;
			set_err(result.error,dt,rs,s,beg,end);
			return result;
		}
		result.event.resize(dt_n);
		write_delta_time(dt,result.event.begin());
		result.event.push_back(ch.data.status_nybble|ch.data.ch);
		result.event.push_back(ch.data.p1);
		if (channel_status_byte_n_data_bytes(s)==2) {
			result.event.push_back(ch.data.p2);
		}
	} else if (is_meta_status_byte(s)) {
		auto mt = validate_meta_event(beg,end);
		if (!mt) {
			result.error = mt.error;
			set_err(result.error,dt,rs,s,mt.begin,mt.end);
			return result;
		}
		result.event.resize(dt_n+(mt.end-mt.begin));
		auto it = write_delta_time(dt,result.event.begin());
		std::copy(mt.begin,mt.end,it);
	} else if (is_sysex_status_byte(s)) {
		auto sx = validate_sysex_event(beg,end);
		if (!sx) {
			result.error = sx.error;
			set_err(result.error,dt,rs,s,sx.begin,sx.end);
			return result;
		}
		result.event.resize(dt_n+(sx.end-sx.begin));
		auto it = write_delta_time(dt,result.event.begin());
		std::copy(sx.begin,sx.end,it);
	} else {
		result.error = mtrk_event_error_t::errc::invalid_status_byte;
		set_err(result.error,dt,rs,0x00u,beg,end);
		return result;
	}
	result.error = mtrk_event_error_t::errc::no_error;
	return result;
}

validate_channel_event_result_t
validate_channel_event(const unsigned char *beg, const unsigned char *end,
						unsigned char rs) {
	validate_channel_event_result_t result;
	result.error = mtrk_event_error_t::errc::other;
	if (!end || !beg || ((end-beg)<1)) {
		result.error = mtrk_event_error_t::errc::channel_overflow;
		return result;
	}
	auto s = get_status_byte(*beg,rs);
	if (!is_channel_status_byte(s)) {
		result.error = mtrk_event_error_t::errc::channel_invalid_status_byte;
		return result;
	}
	result.data.status_nybble = s&0xF0u;
	result.data.ch = s&0x0Fu;
	int expect_n_data_bytes = channel_status_byte_n_data_bytes(s);
	if (*beg==s) {
		++beg;  // The event has a local status-byte
	}
	if ((end-beg)<expect_n_data_bytes) {
		result.error = mtrk_event_error_t::errc::channel_calcd_length_exceeds_input;
		return result;
	}

	result.data.p1 = *beg++;
	if (!is_data_byte(result.data.p1)) {
		result.error = mtrk_event_error_t::errc::channel_invalid_data_byte;
		return result;
	}
	if (expect_n_data_bytes==2) {
		result.data.p2 = *beg;
		if (!is_data_byte(result.data.p2)) {
			result.error = mtrk_event_error_t::errc::channel_invalid_data_byte;
			return result;
		}
	}

	result.error=mtrk_event_error_t::errc::no_error;
	return result;
}

validate_meta_event_result_t
validate_meta_event(const unsigned char *beg, const unsigned char *end) {
	validate_meta_event_result_t result;
	result.error = mtrk_event_error_t::errc::other;
	if (!end || !beg || ((end-beg)<3)) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_overflow_in_header;
		return result;
	}
	if (!is_meta_status_byte(*beg)) {
		result.error = mtrk_event_error_t::errc::invalid_status_byte;
		return result;
	}
	auto p = beg+2;
	auto len = midi_interpret_vl_field(p,end);
	if (!len.is_valid) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_invalid_vlq_length;
		return result;
	}
	if ((end-p)<(len.N+len.val)) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_calcd_length_exceeds_input;
		return result;
	}
	// TODO:  Should return len.val and re-compute its normalized vlq rep
	result.error = mtrk_event_error_t::errc::no_error;
	result.begin = beg;
	result.end = beg + 2 + len.N + len.val;
	return result;
}


validate_sysex_event_result_t
validate_sysex_event(const unsigned char *beg, const unsigned char *end) {
	validate_sysex_event_result_t result;
	result.error = mtrk_event_error_t::errc::other;
	if (!end || !beg || ((end-beg)<2)) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_overflow_in_header;
		return result;
	}
	if (!is_sysex_status_byte(*beg)) {
		result.error = mtrk_event_error_t::errc::invalid_status_byte;
		return result;
	}
	auto p = beg+1;
	auto len = midi_interpret_vl_field(p,end);
	if (!len.is_valid) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_invalid_vlq_length;
		return result;
	}
	if ((end-p)<(len.N+len.val)) {
		result.error = mtrk_event_error_t::errc::sysex_or_meta_calcd_length_exceeds_input;
		return result;
	}
	// TODO:  Should return len.val and re-compute its normalized vlq rep
	result.error = mtrk_event_error_t::errc::no_error;
	result.begin = beg;
	result.end = beg + 1 + len.N + len.val;
	return result;
}

