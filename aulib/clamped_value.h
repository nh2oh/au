#pragma once
#include <limits>
#include <iostream>

//
// A numeric T clamped on [VMin,VMax] w/ explicit conversion to
// wider numeric types.  
//
template<typename T, T VMin, T VMax>
class clamped_value {
public:
	static_assert(VMin<=VMax);
	static_assert((std::numeric_limits<T>::min()<=VMin
			&& std::numeric_limits<T>::max()>=VMax),
		"[VMin,VMax] exceeds the range of the underlying type");
	using value_type = T;
	static const T min_val = VMin;
	static const T max_val = VMax;

	explicit clamped_value()=default;

	template<typename From>
	explicit clamped_value(const From& val_in) {
		if (val_in < VMin) {
			this->val_=VMin;
		} else if (val_in > VMax) {
			this->val_= VMax;
		} else {
			this->val_= static_cast<T>(val_in);
		}
	};

	constexpr value_type value() const {
		return this->val_;
	};

	// Conversion to types capable of holding the full range of
	// allowed values.
	template<typename To, 
		typename = std::enable_if<
			(std::is_convertible<value_type,To>::value
			&& (std::numeric_limits<To>::min()<=VMin)
			&& (std::numeric_limits<To>::max()>=VMax))>::type
	>
	explicit operator To() const {
		return static_cast<To>(this->val_);
	};

	


	bool operator==(const clamped_value& rhs) const {
		return this->val_ == rhs.val_;
	};
	bool operator!=(const clamped_value& rhs) const {
		return !(*this == rhs);
	};
	bool operator<(const clamped_value& rhs) const {
		return this->val_ < rhs.val_;
	};
	bool operator>(const clamped_value& rhs) const {
		return this->val_ > rhs.val_;
	};
	bool operator<=(const clamped_value& rhs) const {
		return this->val_ <= rhs.val_;
	};
	bool operator>=(const clamped_value& rhs) const {
		return this->val_ >= rhs.val_;
	};

	clamped_value operator+(const clamped_value& rhs) const {
		return clamped_value(this->val_ + rhs.val_);
	};
	clamped_value operator-(const clamped_value& rhs) const {
		return clamped_value(this->val_ - rhs.val_);
	};
	clamped_value& operator+=(const clamped_value& rhs) {
		*this = clamped_value(this->val_ + rhs.val_);
		return *this;
	};
	clamped_value& operator-=(const clamped_value& rhs) {
		*this = clamped_value(this->val_ - rhs.val_);
		return *this;
	};
	clamped_value& operator++() {  // pre
		*this = clamped_value(++(this->val_));
		return *this;
	};
	clamped_value operator++(int) {  // post
		auto temp = this->val_;
		*this = clamped_value((this->val_)++);
		return clamped_value(temp);
	};
	clamped_value& operator--() {  // pre
		*this = clamped_value(--(this->val_));
		return *this;
	};
	clamped_value operator--(int) {  // post
		auto temp = this->val_;
		*this = clamped_value((this->val_)--);
		return clamped_value(temp);
	};
private:
	// Note that were this to be left unitialized, the invariant could 
	// be broken.  
	value_type val_ {VMin};  
};

template<typename T, T VMin, T VMax>
std::ostream& operator<<(std::ostream& os, const clamped_value<T,VMin,VMax>& cv) {
	os << static_cast<T>(cv);
	return os;
};

