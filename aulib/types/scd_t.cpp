#include "scd_t.h"
#include "..\util\au_util.h"
#include <string>
#include <cmath>  // floor()


scd_t::scd_t(int i) {
	m_value=i;
}
scd_t::operator int() const {
	return m_value;
}

scd_t& scd_t::operator++() { // prefix
	++m_value;
	return *this;
}
scd_t scd_t::operator++(int dummy_int) { // postfix
	scd_t copy_preinc = *this;
	++m_value;
	return copy_preinc;
}
scd_t& scd_t::operator--() { // prefix
	--m_value;
	return *this;
}
scd_t scd_t::operator--(int dummy_int) { // postfix
	scd_t copy_preinc = *this;
	--m_value;
	return copy_preinc;
}

scd_t& scd_t::operator+=(const scd_t& rhs) {
	m_value += rhs.m_value;
	return *this;
}
scd_t& scd_t::operator-=(const scd_t& rhs) {
	m_value -= rhs.m_value;
	return *this;
}
scd_t& scd_t::operator+=(const int& rhs) {
	m_value += rhs;
	return *this;
}
scd_t& scd_t::operator-=(const int& rhs) {
	m_value -= rhs;
	return *this;
}


bool scd_t::operator==(const scd_t& rhs) const {
	return m_value==rhs.m_value;
}
bool scd_t::operator!=(const scd_t& rhs) const {
	return !(m_value==rhs.m_value);
}
bool scd_t::operator<(const scd_t& rhs) const {
	return m_value<rhs.m_value;
}
bool scd_t::operator>(const scd_t& rhs) const {
	return m_value>rhs.m_value;
}
bool scd_t::operator>=(const scd_t& rhs) const {
	return !(m_value<rhs.m_value);
}
bool scd_t::operator<=(const scd_t& rhs) const {
	return !(m_value>rhs.m_value);
}

