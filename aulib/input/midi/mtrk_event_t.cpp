#include "mtrk_event_t.h"
#include "midi_raw.h"
#include "midi_vlq.h"
#include "midi_delta_time.h"
#include <cstdint>
#include <algorithm>
#include <utility>  // std::move()


mtrk_event_t::mtrk_event_t() {
	this->default_init(0);
}
mtrk_event_t::mtrk_event_t(int32_t dt) {
	this->default_init(dt);
}
mtrk_event_t::mtrk_event_t(int32_t dt, midi_ch_event_t md) {
	this->d_ = mtrk_event_t_internal::small_bytevec_t();
	md = normalize(md);
	unsigned char s = (md.status_nybble)|(md.ch);
	auto n = channel_status_byte_n_data_bytes(s);
	// NB:  n==0 if s is invalid, but this is impossible after a call
	// to normalize().  
	this->d_.resize(delta_time_field_size(dt)+1+n);  // +1=>s
	auto dest = write_delta_time(dt,this->d_.begin());
	*dest++ = s;
	*dest++ = md.p1;
	if (n==2) {
		*dest++ = md.p2;
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
	rhs.default_init(0);
}
mtrk_event_t& mtrk_event_t::operator=(mtrk_event_t&& rhs) noexcept {
	this->d_ = std::move(rhs.d_);
	rhs.default_init(0);
	return *this;
}
mtrk_event_t::~mtrk_event_t() noexcept {  // dtor
	//...
}

mtrk_event_t::size_type mtrk_event_t::size() const {
	return this->d_.size();
}
mtrk_event_t::size_type mtrk_event_t::capacity() const {
	return this->d_.capacity();
}
mtrk_event_t::size_type mtrk_event_t::reserve(mtrk_event_t::size_type new_cap) {
	new_cap = std::clamp(new_cap,0,mtrk_event_t::size_max);
	return this->d_.reserve(new_cap);
}
const unsigned char *mtrk_event_t::data() {
	return this->d_.begin();
}
const unsigned char *mtrk_event_t::data() const {
	return this->d_.begin();
}
mtrk_event_t::const_iterator mtrk_event_t::begin() {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::begin() const {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::cbegin() {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::cbegin() const {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::end() {
	return mtrk_event_t::const_iterator(this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::end() const {
	return mtrk_event_t::const_iterator(this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::cend() {
	return mtrk_event_t::const_iterator(this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::cend() const {
	return mtrk_event_t::const_iterator(this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::dt_begin() const {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::dt_begin() {
	return mtrk_event_t::const_iterator(this->d_.begin());
}
mtrk_event_t::const_iterator mtrk_event_t::dt_end() const {
	return advance_to_dt_end(this->d_.begin(),this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::dt_end() {
	return advance_to_dt_end(this->d_.begin(),this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::event_begin() const {
	return advance_to_dt_end(this->d_.begin(),this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::event_begin() {
	return advance_to_dt_end(this->d_.begin(),this->d_.end());
}
mtrk_event_t::const_iterator mtrk_event_t::payload_begin() const {
	return this->payload_range_impl().begin;
	return this->payload_range_impl().begin;
}
mtrk_event_t::const_iterator mtrk_event_t::payload_begin() {
	return this->payload_range_impl().begin;
}
iterator_range_t<mtrk_event_t::const_iterator> mtrk_event_t::payload_range() const {
	return this->payload_range_impl();
}
iterator_range_t<mtrk_event_t::const_iterator> mtrk_event_t::payload_range() {
	return this->payload_range_impl();
}
const unsigned char mtrk_event_t::operator[](mtrk_event_t::size_type i) const {
	return *(this->d_.begin()+i);
};
unsigned char mtrk_event_t::operator[](mtrk_event_t::size_type i) {
	return *(this->d_.begin()+i);
};
unsigned char *mtrk_event_t::push_back(unsigned char c) {  // Private
	return this->d_.push_back(c);
}
iterator_range_t<mtrk_event_t::const_iterator> mtrk_event_t::payload_range_impl() const {
	auto it_end = this->d_.end();
	auto it = advance_to_dt_end(this->d_.begin(),it_end);
	auto t = classify_status_byte(*it);
	if (t==smf_event_type::meta) {
		it += 2;  // 0xFFu, type-byte
		it = advance_to_vlq_end(it,it_end);
	} else if (t==smf_event_type::sysex_f0
					|| t==smf_event_type::sysex_f7) {
		it += 1;  // 0xF0u or 0xF7u
		it = advance_to_vlq_end(it,it_end);
	} // else { smf_event_type::channel_voice, _mode, unknown, invalid...
	return {it,it_end};
}
smf_event_type mtrk_event_t::type() const {
	auto p = advance_to_dt_end(this->d_.begin(),this->d_.end());
	return classify_status_byte(*p);
}
int32_t mtrk_event_t::delta_time() const {
	return read_delta_time(this->d_.begin(),this->d_.end()).val;
}
unsigned char mtrk_event_t::status_byte() const {
	return *advance_to_dt_end(this->d_.begin(),this->d_.end());
}
unsigned char mtrk_event_t::running_status() const {
	auto p = advance_to_dt_end(this->d_.begin(),this->d_.end());
	return get_running_status_byte(*p,0x00u);
}
mtrk_event_t::size_type mtrk_event_t::data_size() const {  // Not including delta-t
	auto end = this->d_.end();
	auto p = advance_to_dt_end(this->d_.begin(),end);
	return end-p;
}

int32_t mtrk_event_t::set_delta_time(int32_t dt) {
	auto new_dt_size = delta_time_field_size(dt);
	auto beg = this->d_.begin();  auto end = this->d_.end();
	auto curr_dt_size = advance_to_dt_end(beg,end)-beg;
	if (curr_dt_size == new_dt_size) {
		write_delta_time(dt,beg);
	} else if (new_dt_size < curr_dt_size) {  // shrink present event
		auto curr_event_beg = beg+curr_dt_size;
		auto it = write_delta_time(dt,beg);
		it = std::copy(curr_event_beg,end,it);
		this->d_.resize(it-beg);
	} else if (new_dt_size > curr_dt_size) {  // grow present event
		auto old_size = this->d_.size();
		auto new_size = old_size + (new_dt_size-curr_dt_size);
		this->d_.resize(new_size);  // NB:  Invalidates iterators!
		auto new_beg = this->d_.begin();
		auto new_end = this->d_.end();
		std::copy_backward(new_beg,new_beg+old_size,new_end);
		write_delta_time(dt,this->d_.begin());
	}
	return this->delta_time();
}

bool mtrk_event_t::operator==(const mtrk_event_t& rhs) const {
	auto it_lhs = this->d_.begin();  auto lhs_end = this->d_.end();
	auto it_rhs = rhs.d_.begin();  auto rhs_end = rhs.d_.end();
	if ((lhs_end-it_lhs) != (rhs_end-it_rhs)) {
		return false;
	}
	while (it_lhs!=lhs_end) {
		if (*it_lhs++ != *it_rhs++) {
			return false;
		}
	}
	return true;
}
bool mtrk_event_t::operator!=(const mtrk_event_t& rhs) const {
	return !(*this==rhs);
}

void mtrk_event_t::default_init(int32_t dt) {
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
mtrk_event_debug_helper_t debug_info(const mtrk_event_t& ev) {
	mtrk_event_debug_helper_t r;
	r.raw_beg = ev.d_.raw_begin();
	r.raw_end = ev.d_.raw_end();
	r.flags = *(r.raw_beg);
	r.is_big = ev.d_.debug_is_big();
	return r;
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

	auto dt = read_delta_time(beg,end);
	if (!dt.is_valid) {
		result.error = mtrk_event_error_t::errc::invalid_delta_time;
		set_err(result.error,dt.val,rs,0x00u,beg,end);
		return result;
	}
	result = make_mtrk_event(dt.val,beg+dt.N,end,rs,err);
	result.size += dt.N;
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
		result.event.d_.resize(dt_n);
		write_delta_time(dt,result.event.d_.begin());
		result.event.push_back(ch.data.status_nybble|ch.data.ch);
		result.event.push_back(ch.data.p1);
		if (channel_status_byte_n_data_bytes(s)==2) {
			result.event.push_back(ch.data.p2);
		}
		result.size = ch.size;
	} else if (is_meta_status_byte(s)) {
		auto mt = validate_meta_event(beg,end);
		if (!mt) {
			result.error = mt.error;
			set_err(result.error,dt,rs,s,mt.begin,mt.end);
			return result;
		}
		result.event.d_.resize(dt_n+(mt.end-mt.begin));
		auto it = write_delta_time(dt,result.event.d_.begin());
		std::copy(mt.begin,mt.end,it);
		result.size = mt.end-mt.begin;
	} else if (is_sysex_status_byte(s)) {
		auto sx = validate_sysex_event(beg,end);
		if (!sx) {
			result.error = sx.error;
			set_err(result.error,dt,rs,s,sx.begin,sx.end);
			return result;
		}
		result.event.d_.resize(dt_n+(sx.end-sx.begin));
		auto it = write_delta_time(dt,result.event.d_.begin());
		std::copy(sx.begin,sx.end,it);
		result.size = sx.end-sx.begin;
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
	auto p = beg;
	auto s = get_status_byte(*p,rs);
	if (!is_channel_status_byte(s)) {
		result.error = mtrk_event_error_t::errc::channel_invalid_status_byte;
		return result;
	}
	result.data.status_nybble = s&0xF0u;
	result.data.ch = s&0x0Fu;
	int expect_n_data_bytes = channel_status_byte_n_data_bytes(s);
	if (*p==s) {
		++p;  // The event has a local status-byte
	}
	if ((end-p)<expect_n_data_bytes) {
		result.error = mtrk_event_error_t::errc::channel_calcd_length_exceeds_input;
		return result;
	}

	result.data.p1 = *p++;
	if (!is_data_byte(result.data.p1)) {
		result.error = mtrk_event_error_t::errc::channel_invalid_data_byte;
		return result;
	}
	if (expect_n_data_bytes==2) {
		result.data.p2 = *p++;
		if (!is_data_byte(result.data.p2)) {
			result.error = mtrk_event_error_t::errc::channel_invalid_data_byte;
			return result;
		}
	}
	result.size = p-beg;
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
	auto len = read_vlq(p,end);
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
	auto len = read_vlq(p,end);
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


maybe_mtrk_event_t yay(const unsigned char *it, const unsigned char *end,
						mtrk_event_error_t *err) {
	maybe_mtrk_event_t r;

	int i = 0;
	unsigned char uc = 0;

	auto set_error = [&r](const mtrk_event_error_t::errc& ec)->void {
		r.event = mtrk_event_t();
		r.error = ec;
	};

	auto inl_read_vlq = [&it, &end, &i, &uc]()->int32_t {
		uint32_t uval = 0;
		while (it!=end) {
			uc = *it++;  ++i;
			uval += uc&0x7Fu;
			if ((uc&0x80u) && (i<4)) {
				uval <<= 7;  // Note:  Not shifting on the final iteration
			} else {  // High bit not set => this is the final byte
				break;
			}
		}
		return static_cast<int32_t>(uval);
	};

	// 10 == 4-byte dt + 0xFF + type-byte + 4-byte len vlq
	r.event.d_.resize(10);
	auto r_it = r.event.d_.begin();

	auto dt = inl_read_vlq();
	if (uc & 0x80u) {
		set_error(mtrk_event_error_t::errc::invalid_delta_time);
		return r;
	}
	r_it = write_delta_time(dt,r_it);

	// The status byte
	if (it==end) {
		set_error(mtrk_event_error_t::errc::no_data_following_delta_time);
		return r;
	}
	uc = *it++;  ++i;
	
	if (is_channel_status_byte(uc)) {
		*r_it++ = uc;
		auto s = uc;
		for (int j=0; j<channel_status_byte_n_data_bytes(s); ++j) {
			if (it==end) {
				set_error(mtrk_event_error_t::errc::channel_calcd_length_exceeds_input);
				return r;
			}
			uc = *it++;  ++i;
			if (!is_data_byte(uc)) {
				set_error(mtrk_event_error_t::errc::channel_invalid_data_byte);
				return r;
			}
			*r_it++ = uc;  // Resize first...
		}
	} else if (is_sysex_or_meta_status_byte(uc)) {
		*r_it++ = uc;  // 0xFF || 0xF7 || 0xF0
		if (is_meta_status_byte(uc)) {
			if (it==end) {
				set_error(mtrk_event_error_t::errc::sysex_or_meta_overflow_in_header);
				return r;
			}
			uc = *it++;  ++i;  // Type byte
			*r_it++ = uc;
		}
		auto len = inl_read_vlq();
		r_it = write_vlq(static_cast<uint32_t>(len),r_it);  // Resize invalidates r_it...
		r.event.d_.resize(i+len);
		int j=0;
		for (true; (j<len && it!=end); ++j) {
			*r_it++ = *it++;  ++i;
			// uc is not the most recent byte anymore
		}
		if (j!=len) {
			set_error(mtrk_event_error_t::errc::sysex_or_meta_calcd_length_exceeds_input);
			return r;
		}
	} else if (is_unrecognized_status_byte(uc)) {
		set_error(mtrk_event_error_t::errc::invalid_status_byte);
		return r;
	}

	return r;
}





