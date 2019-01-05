#pragma once


//
// The scd_t class
//
// An scd_t represents a scale degree of some unspecified scale.  It represents an absolute
// position within a scale.  scd_t is nothing more than a thin wrapper around 
// int and can be implictly converted to and from an int.  
//
//

class scd_t {
public:
	scd_t()=default;
	scd_t(int i);
	operator int() const;

	scd_t& operator++();  // prefix
	scd_t operator++(int);  // postfix
	scd_t& operator--();  // prefix
	scd_t operator--(int);  // postfix
	scd_t& operator+=(const int&);
	scd_t& operator-=(const int&);
	scd_t& operator+=(const scd_t&);
	scd_t& operator-=(const scd_t&);

	bool operator==(const scd_t&) const;
	bool operator!=(const scd_t&) const;
	bool operator<(const scd_t&) const;
	bool operator>(const scd_t&) const;
	bool operator>=(const scd_t&) const;
	bool operator<=(const scd_t&) const;
private:
	int m_value {0};
};

