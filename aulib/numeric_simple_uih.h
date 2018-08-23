#pragma once
#include "uih.h"
#include <string>


struct parse_userinput_double {
	au::uih_parser_result<double> operator()(std::string const&) const;
};

struct ftr_gt {  // "functor greater_than"
public:
	ftr_gt(double const value) : m_value{value} {};
	bool operator()(double const& arg) const {
		return arg > 0;
	};
private:
	const double m_value {0.0};
};

struct ftr_geq {
public:
	ftr_geq(double const value) : m_value{value} {};
	bool operator()(double const& arg) const {
		return arg >= m_value;
	};
private:
	const double m_value {0.0};
};
