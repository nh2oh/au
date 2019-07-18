#include "mthd_t.h"
#include "generic_chunk_low_level.h"
#include "midi_raw.h"
#include "midi_vlq.h"
#include <cstdint>
#include <string>
#include <algorithm>  // std::copy() in mthd_t ctor(s), std::clamp()
#include <limits>  // in format, ntrks, ... setters




//
// mthd_t class & related methods
//
mthd_t::mthd_t(int32_t fmt, int32_t ntrks, time_division_t tdf) {
	this->set_format(fmt);
	this->set_ntrks(ntrks);
	this->set_division(tdf);
}
mthd_t::mthd_t(int32_t fmt, int32_t ntrks, int32_t tpq) {
	this->set_format(fmt);
	this->set_ntrks(ntrks);
	this->set_division_tpq(tpq);
}
mthd_t::mthd_t(int32_t fmt, int32_t ntrks, int32_t tcf, int32_t subdivs) {
	this->set_format(fmt);
	this->set_ntrks(ntrks);
	this->set_division_smpte(tcf,subdivs);
}


mthd_t::size_type mthd_t::size() const {
	return static_cast<mthd_t::size_type>(this->d_.size());
}
mthd_t::size_type mthd_t::nbytes() const {
	return static_cast<mthd_t::size_type>(this->d_.size());
}
mthd_t::pointer mthd_t::data() {  // private
	return this->d_.data();
}
mthd_t::const_pointer mthd_t::data() const {
	return this->d_.data();
}
mthd_t::reference mthd_t::operator[](mthd_t::size_type idx) {  // private
	return this->d_[idx];
}
mthd_t::const_reference mthd_t::operator[](mthd_t::size_type idx) const {
	return this->d_[idx];
}
mthd_t::iterator mthd_t::begin() {  // private
	return mthd_t::iterator(this->d_.data());
}
mthd_t::iterator mthd_t::end() {  // private
	return mthd_t::iterator(this->d_.data()) + this->d_.size();
}
mthd_t::const_iterator mthd_t::begin() const {
	return mthd_t::const_iterator(this->d_.data());
}
mthd_t::const_iterator mthd_t::end() const {
	return mthd_t::const_iterator(this->d_.data()) + this->d_.size();
}
mthd_t::const_iterator mthd_t::cbegin() const {
	return mthd_t::const_iterator(this->d_.data());
}
mthd_t::const_iterator mthd_t::cend() const {
	return mthd_t::const_iterator(this->d_.data()) + this->d_.size();
}

int32_t mthd_t::length() const {
	auto l = read_be<uint32_t>(this->d_.cbegin()+4,this->d_.cend());
	return static_cast<int32_t>(l);
}
int32_t mthd_t::format() const {
	int32_t f = read_be<uint16_t>(this->d_.cbegin()+8,this->d_.cend());
	//return std::clamp(f,mthd_t::format_min,mthd_t::format_max);
	return f;
}
int32_t mthd_t::ntrks() const {
	return read_be<uint16_t>(this->d_.cbegin()+8+2,this->d_.cend());
}
time_division_t mthd_t::division() const {
	auto raw_val = read_be<uint16_t>(this->d_.cbegin()+12,this->d_.cend());
	//return time_division_t(raw_val);
	return make_time_division_from_raw(raw_val).value;
}
int32_t mthd_t::set_format(int32_t f) {
	if (this->ntrks() > 1) {
		f = std::clamp(f,1,mthd_t::format_max);
	} else {
		f = std::clamp(f,mthd_t::format_min,mthd_t::format_max);
	}
	write_16bit_be(static_cast<uint16_t>(f),this->d_.begin()+8);
	return this->format();
}
int32_t mthd_t::set_ntrks(int32_t ntrks) {
	auto f = this->format();
	if (f==0) {
		ntrks = std::clamp(ntrks,mthd_t::ntrks_min,
			mthd_t::ntrks_max_fmt_0);
	} else {
		ntrks = std::clamp(ntrks,mthd_t::ntrks_min,
			mthd_t::ntrks_max_fmt_gt0);
	}
	write_16bit_be(static_cast<uint16_t>(ntrks),this->d_.begin()+10);
	return this->ntrks();
}
time_division_t mthd_t::set_division(time_division_t tdf) {
	write_16bit_be(tdf.get_raw_value(),this->d_.begin()+12);
	return this->division();
}
int32_t mthd_t::set_division_tpq(int32_t tpq) {
	tpq = std::clamp(tpq,1,0x7FFF);
	write_16bit_be(static_cast<uint16_t>(tpq),this->d_.begin()+12);
	return tpq;
}
smpte_t mthd_t::set_division_smpte(int32_t tcf, int32_t subdivs) {
	if ((tcf!=-24) && (tcf!=-25) && (tcf!=-29) && (tcf!=-30)) {
		tcf = -24;
	}
	auto tcf_i8 = static_cast<int8_t>(tcf);
	subdivs = std::clamp(subdivs,1,0xFF);
	auto subdivs_ui8 = static_cast<uint8_t>(subdivs);
	auto psrc = static_cast<unsigned char*>(static_cast<void*>(&tcf_i8));
	this->d_[12] = *psrc;
	psrc = static_cast<unsigned char*>(static_cast<void*>(&subdivs_ui8));
	this->d_[13] = *psrc;
	return smpte_t {tcf_i8,subdivs_ui8};
}
int32_t mthd_t::set_length(int32_t len) {
	len = std::clamp(len,mthd_t::length_min,mthd_t::length_max);
	if (len != this->length()) {
		auto ulen = static_cast<std::vector<unsigned char>::size_type>(len);
		this->d_.resize(ulen+8,0x00u);
		write_32bit_be(static_cast<uint32_t>(len),this->d_.begin()+4);
	}
	return this->length();
}
bool mthd_t::verify() const {
	return true;
}

void set_from_bytes_unsafe(const unsigned char *beg, 
							const unsigned char *end, mthd_t *dest) {
	auto sz = end-beg;
	if (sz < 14) {
		sz = 14;
		// Certain setters (ex, set_division_smpte()) assume a min size
		// of 14.  
	}
	dest->d_.clear();
	dest->d_.resize(sz,0x00u);
	std::copy(beg,end,dest->d_.begin());
}
maybe_mthd_t make_mthd(const unsigned char *beg, const unsigned char *end) {
	return make_mthd(beg,end,nullptr);
}
maybe_mthd_t make_mthd(const unsigned char *beg, const unsigned char *end,
							mthd_error_t *err) {
	return make_mthd_impl(beg,end,err);
}

maybe_mthd_t make_mthd_impl(const unsigned char *beg, const unsigned char *end,
							mthd_error_t *err) {
	// <Header Chunk> = <chunk type> <length> <format> <ntrks> <division> 
	//                   MThd uint32_t uint16_t uint16_t uint16_t
	maybe_mthd_t result;
	result.is_valid = false;

	auto header = read_chunk_header(beg,end);
	if (!header) {
		if (err) {
			chunk_header_error_t header_error {};
			read_chunk_header(beg,end,&header_error);
			err->code = mthd_error_t::errc::header_error;
			err->hdr_error = header_error;
		}
		return result;
	}
	if (header.id != chunk_id::mthd) {
		if (err) {
			err->code = mthd_error_t::errc::invalid_id;
		}
		return result;
	}

	if (header.length < 6) {
		if (err) {
			err->code = mthd_error_t::errc::length_lt_min;
			err->length = header.length;
		}
		return result;
	} else if (header.length > (end-(beg+8))) {
		if (err) {
			err->code = mthd_error_t::errc::overflow;
			err->length = header.length;
		}
		return result;
	} else if (header.length > mthd_t::length_max) {
		if (err) {
			err->code = mthd_error_t::errc::length_gt_mthd_max;
		}
		return result;
	}
	auto it = beg;
	it += 8;
	
	auto format = read_be<uint16_t>(it,end);
	it += 2;
	auto ntrks = read_be<uint16_t>(it,end);
	it += 2;

	if ((format==0) && (ntrks >1)) {
		if (err) {
			err->code = mthd_error_t::errc::inconsistent_format_ntrks;
			err->format = format;
			err->ntrks = ntrks;
		}
		return result;
	}

	auto division = read_be<uint16_t>(it,end);
	if (!is_valid_time_division_raw_value(division)) {
		if (err) {
			err->code = mthd_error_t::errc::invalid_time_division;
			err->division = division;
			err->format = format;
			err->ntrks = ntrks;
		}
		return result;
	}

	auto size = 8+header.length;
	set_from_bytes_unsafe(beg,beg+size,&(result.mthd));
	result.is_valid = true;
	return result;
}
std::string explain(const mthd_error_t& err) {
	std::string s;
	if (err.code==mthd_error_t::errc::no_error) {
		return s;
	}
	
	s += "Invalid MThd chunk:  ";
	if (err.code==mthd_error_t::errc::header_error) {
		s += explain(err.hdr_error);
	} else if (err.code==mthd_error_t::errc::overflow) {
		s += "The input range is not large enough to accommodate "
			"the number of bytes specified by the 'length' field.  "
			"length == ";
		s += std::to_string(err.length);
		s += ".  ";
	} else if (err.code==mthd_error_t::errc::invalid_id) {
		s += "Invalid MThd ID field; expected the first 4 bytes to be "
			"'MThd' (0x4D,54,68,64).  ";
	} else if (err.code==mthd_error_t::errc::length_lt_min) {
		s += "The 'length' field encodes a value < the minimum of 6 bytes.  length == ";
		s += std::to_string(err.length);
		s += ".  ";
	} else if (err.code==mthd_error_t::errc::length_gt_mthd_max) {
		s += "The length field encodes a value that is too large.  "
			"This library enforces a maximum MThd chunk length of ";
		s += std::to_string(mthd_t::length_max);
		s += ".  length == ";
		s += std::to_string(err.length);
		s += ".  ";
	} else if (err.code==mthd_error_t::errc::invalid_time_division) {
		int8_t time_code = 0;
		uint8_t subframes = ((err.division)&0x00FFu);
		uint8_t high = ((err.division)>>8);
		auto psrc = static_cast<const unsigned char*>(static_cast<const void*>(&high));
		auto pdest = static_cast<unsigned char*>(static_cast<void*>(&time_code));
		*pdest = *psrc;
		s += "The value of field 'division' is invalid.  It is probably an "
			"SMPTE-type field attempting to specify a time-code of something "
			"other than -24, -25, -29, or -30.  division == ";
		s += std::to_string(err.division);
		s += " => time-code == ";
		s += std::to_string(time_code);
		s += " => ticks-per-frame == ";
		s += std::to_string(subframes);
		s += ".  ";
	} else if (err.code==mthd_error_t::errc::inconsistent_format_ntrks) {
		s += "In a format==0 SMF, ntrks must be <= 1.  format == ";
		s += std::to_string(err.format);
		s += ", ntrks == ";
		s += std::to_string(err.ntrks);
		s += ".  ";
	} else if (err.code==mthd_error_t::errc::other) {
		s += "Error code mthd_error_t::errc::other.  ";
	} else {
		s += "Unknown error.  ";
	}
	return s;
}
std::string print(const mthd_t& mthd) {
	std::string s;  s.reserve(200);
	return print(mthd,s);
}
std::string& print(const mthd_t& mthd, std::string& s) {
	s += ("Header (MThd):  size() = " + std::to_string(mthd.size()) + ":\n");
	s += ("\tFormat type = " + std::to_string(mthd.format()) + ", ");
	s += ("Num Tracks = " + std::to_string(mthd.ntrks()) + ", ");
	s += "Time Division = ";
	auto timediv_type = mthd.division().get_type();
	if (timediv_type == time_division_t::type::smpte) {
		s += "SMPTE";
	} else if (timediv_type == time_division_t::type::ticks_per_quarter) {
		s += std::to_string(get_tpq(mthd.division()));
		s += " ticks-per-quarter-note ";
	}
	s += "\n\t";
	print_hexascii(mthd.cbegin(), mthd.cend(), std::back_inserter(s), '\0',' ');

	return s;
}


maybe_mthd_t::operator bool() const {
	return this->is_valid;
}

