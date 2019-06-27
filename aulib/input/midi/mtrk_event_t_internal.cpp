#include "mtrk_event_t_internal.h"
#include "midi_raw.h"
#include <cstdint>
#include <algorithm>  

using namespace mtrk_event_t_internal;


unsigned char *small_t::begin() {
	return &(this->d_[0]);
}
const unsigned char *small_t::begin() const {
	return &(this->d_[0]);
}
unsigned char *small_t::end() {
	return &(this->d_[0]) + this->d_.size();  //) + mtrk_event_get_size_dtstart_unsafe(&(this->d_[0]),0x00u);
}
const unsigned char *small_t::end() const {
	return &(this->d_[0]) + this->d_.size();  //&(this->d_[0]) + mtrk_event_get_size_dtstart_unsafe(&(this->d_[0]),0x00u);
}
/*uint64_t small_t::size() const {
	return this->d_.size(); //mtrk_event_get_size_dtstart_unsafe(&(this->d_[0]),0x00u);
}*/
constexpr uint64_t small_t::capacity() const {
	//static_assert(sizeof(this->d_) > 1);
	//return sizeof(this->d_)-1;
	return this->d_.size();
}
/*uint64_t big_t::size() const {
	return this->cap_;
	//return this->sz_;
}*/
uint64_t big_t::capacity() const {
	return this->cap_;
}
unsigned char *big_t::begin() {
	return this->p_;
}
const unsigned char *big_t::begin() const {
	return this->p_;
}
unsigned char *big_t::end() {
	return this->p_ + this->cap_;
	//return this->p_ + this->sz_;
}
const unsigned char *big_t::end() const {
	return this->p_ + this->cap_;
	//return this->p_ + this->sz_;
}
bool sbo_t::is_small() const {
	return ((this->u_.s_.flags_)&0x80u)==0x80u;
}
bool sbo_t::is_big() const {
	return !(this->is_small());
}
void sbo_t::set_flag_small() {
	this->u_.s_.flags_ |= 0x80u;  // Note i always go through s_
}
void sbo_t::set_flag_big() {
	this->u_.s_.flags_ &= 0x7Fu;  // Note i always go through s_
}
void sbo_t::free_if_big() {
	if (this->is_big()) {
		delete this->u_.b_.p_;
		this->big_adopt(nullptr,0,0);  // sets flag big
	}
}
void sbo_t::big_adopt(unsigned char *p, uint32_t sz, uint32_t c) {
	this->set_flag_big();
	this->u_.b_.p_=p;
	//this->u_.b_.sz_=sz;
	this->u_.b_.cap_=c;
}
void sbo_t::zero_object() {
	this->set_flag_small();
	for (auto it=this->u_.s_.begin(); it!=this->u_.s_.end(); ++it) {
		*it=0x00u;
	}
}
constexpr uint64_t sbo_t::small_capacity() const {
	return this->u_.s_.capacity();
}
/*uint64_t sbo_t::size() const {
	if (this->is_small()) {
		return this->u_.s_.size();
	} else {
		return this->u_.b_.size();
	}
}*/
uint64_t sbo_t::capacity() const {
	if (this->is_small()) {
		return this->u_.s_.capacity();
	} else {
		return this->u_.b_.capacity();
	}
}
uint32_t sbo_t::resize(uint32_t new_cap) {
	new_cap = std::max(static_cast<uint64_t>(new_cap),this->small_capacity());
	if (new_cap == this->small_capacity()) {
		// Resize the object to fit in a small_t.  If it's presently small,
		// do nothing.  
		if (this->is_big()) {
			auto p = this->u_.b_.p_;
			auto sz = this->u_.b_.sz_;
			auto new_sz = std::min(sz,new_cap);
			this->zero_object();
			this->set_flag_small();
			std::copy(p,p+new_sz,this->u_.s_.begin());
			// No need to std::fill(...,0x00u); already called zero_object()
			delete p;
		}
	} else if (new_cap > this->small_capacity()) {
		// After resizing, the object will be held in a big_t
		if (this->is_big()) {  // presently big
			if (new_cap != this->u_.b_.cap_) {  // ... and are changing the cap
				auto p = this->u_.b_.p_;

				// Part of the attempt to remove the size() method:
				//auto sz = this->u_.b_.sz_;
				auto sz = this->u_.b_.cap_;
				auto new_sz = std::min(sz,new_cap);
				auto n_copy = std::min(this->u_.b_.cap_,new_cap);

				// Note that if new_cap is < sz, the data will be truncated
				// and the object will almost certainly be invalid.  
				unsigned char *new_p = new unsigned char[new_cap];
				auto new_end = std::copy(p,p+n_copy,new_p);
				std::fill(new_end,new_p+new_cap,0x00u);
				delete p;
				this->big_adopt(new_p,new_sz,new_cap);
				this->set_flag_big();
			}
			// if already big and new_cap==present cap;  do nothing
		} else {  // presently small 
			// Copy the present small object into a new big object.  The present
			// capacity of small_capacity() is < new_cap

			// Part of the attempt to remove the size() method:
			auto sz = new_cap;  //auto sz = this->u_.s_.size();

			unsigned char *new_p = new unsigned char[new_cap];
			auto new_end = std::copy(this->u_.s_.begin(),this->u_.s_.end(),new_p);
			std::fill(new_end,new_p+new_cap,0x00u);
			this->zero_object();
			this->big_adopt(new_p,sz,new_cap);
			this->set_flag_big();
		}
	}
	return new_cap;
}
unsigned char *sbo_t::begin() {
	if (this->is_small()) {
		return this->u_.s_.begin();
	} else {
		return this->u_.b_.begin();
	}
}
const unsigned char *sbo_t::begin() const {
	if (this->is_small()) {
		return this->u_.s_.begin();
	} else {
		return this->u_.b_.begin();
	}
}
unsigned char *sbo_t::end() {
	if (this->is_small()) {
		return this->u_.s_.end();
	} else {
		return this->u_.b_.end();
	}
}
const unsigned char *sbo_t::end() const {
	if (this->is_small()) {
		return this->u_.s_.end();
	} else {
		return this->u_.b_.end();
	}
}
unsigned char *sbo_t::raw_begin() {
	return &(this->u_.raw_[0]);
}
const unsigned char *sbo_t::raw_begin() const {
	return &(this->u_.raw_[0]);
}
unsigned char *sbo_t::raw_end() {
	return &(this->u_.raw_[0]) + this->u_.raw_.size();
}
const unsigned char *sbo_t::raw_end() const {
	return &(this->u_.raw_[0]) + this->u_.raw_.size();
}

