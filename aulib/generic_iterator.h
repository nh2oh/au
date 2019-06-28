#pragma once
#include <iterator>  // std::random_access_iterator_tag;
#include <type_traits>  // std::conditional<...>

//
// TODO
// -> a==cb where a=>generic_ra_iterator<C,false> and 
//    ca=>generic_ra_iterator<C,true> won't compile.  
//
template<typename C, bool Is_const=false>
class generic_ra_iterator {
public:
	using iterator_category = std::random_access_iterator_tag;
	using value_type = typename C::value_type;
	using difference_type = typename C::difference_type;
	using pointer = typename std::conditional<Is_const,
		typename C::const_pointer, typename C::pointer>::type;
	using reference = typename std::conditional<Is_const,
		typename C::const_reference, typename C::reference>::type;

	// Converting ctor from a non-const (Is_const==false) iterator to
	// a non-const iterator.  
	// TODO:  This seems to have the same signature as a copy ctor for the
	// case where generic_ra_iterator == generic_ra_iterator<C,false>
	generic_ra_iterator(const generic_ra_iterator<C,false>& it) : p_(it.p_) {};

	generic_ra_iterator(pointer p) : p_(p) {};

	reference operator*() const {
		return *(this->p_);
	};
	pointer operator->() const {
		return this->p_;
	};
	generic_ra_iterator& operator++() {  // pre
		++(this->p_);
		return *this;
	};
	generic_ra_iterator operator++(int) {  // post
		generic_ra_iterator temp = *this;
		++(this->p_);
		return temp;
	};
	generic_ra_iterator& operator--() {  // pre
		--(this->p_);
		return *this;
	};
	generic_ra_iterator operator--(int) {  // post
		generic_ra_iterator temp = *this;
		--(this->p_);
		return temp;
	};
	generic_ra_iterator& operator+=(const difference_type n) {
		this->p_ += n;
		return *this;
	};
	generic_ra_iterator operator+(const difference_type n) const {
		generic_ra_iterator temp = *this;
		return temp += n;
	};
	generic_ra_iterator& operator-=(const difference_type n) {
		this->p_ -= n;
		return *this;
	};
	difference_type operator-(const difference_type n) const {
		generic_ra_iterator temp = *this;
		return temp -= n;
	};
	difference_type operator-(const generic_ra_iterator rhs) const {
		return (this->p_ - rhs.p_);
	};
	bool operator==(const generic_ra_iterator& rhs) const {
		return this->p_ == rhs.p_;
	};
	bool operator!=(const generic_ra_iterator& rhs) const {
		return !(*this == rhs);
	};
	bool operator<(const generic_ra_iterator& rhs) const {
		return this->p_ < rhs.p_;
	};
	bool operator>(const generic_ra_iterator& rhs) const {
		return rhs < *this;  // NB:  Impl in terms of <
	};
	bool operator<=(const generic_ra_iterator& rhs) const {
		return !(rhs < *this);  // NB:  Impl in terms of <
	};
	bool operator>=(const generic_ra_iterator& rhs) const {
		return !(*this < rhs);  // NB:  Impl in terms of <
	};
private:
	pointer p_;
};

void generic_iterator_tests();


