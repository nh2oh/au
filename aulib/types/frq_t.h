#pragma once
#include <string>

class cent_t;
class oct_t;

//
// The frq_t class
//
// Represents a frequency in units of Hz.  As frequencies < 0 Hz make no physical sense, a 
// frq_t only represents values >= 0.  std::abort() is called upon any operation causing the
// value to fall below 0.  
//
// frq_t/frq_t, returns a double, not a frq_t.  Thus, the numerical value of a frq_t can be 
// extracted with operator/(frq_t).  To convert some frq_t object myfrq to, for example, a 
// "number of Hz:"  
// auto number_of_Hz = myfrq/frq_t{1};
//
// Explictly deleted operations: 
// frq_t*frq_t => a (frq_t)^2
// T/frq_t for all T other than frq_t
// frq_t +,- double,int, etc
//
//

//
// TODO:  Just put in the same file as cent_oct_t... othwerwise don't get the +/- cent operators
//

class frq_t {
public:
	/*enum class unit : int {
		Hz=0,
		kHz=3,
		MHz=6,
		GHz=9
	};*/

	frq_t() = default;  // 0 Hz
	explicit frq_t(double);
	//explicit frq_t(double, unit const& =frq_t::unit::Hz);

	std::string print() const;

	double operator/(const frq_t&) const;
	frq_t& operator+=(const frq_t&);
	frq_t& operator-=(const frq_t&);
	frq_t& operator/=(double);
	frq_t& operator*=(double);

	bool operator==(const frq_t&) const;
	bool operator!=(const frq_t&) const;
	bool operator<(const frq_t&) const;
	bool operator>(const frq_t&) const;
	bool operator<=(const frq_t&) const;
	bool operator>=(const frq_t&) const;
private:
	double m_frq {0.0};  // Units are always Hz
	//int m_unit {0};  // 1=>Hz, 1000=>kHz, 10^6=>MHz, ...
	
	//static const int m_prec;  // precision for operator ==
};

frq_t operator+(frq_t, const frq_t&);
frq_t operator-(frq_t, const frq_t&);
frq_t operator*(double, frq_t);
frq_t operator*(frq_t, double);

// Forbid anything involving 1/frq_t other than frq_t/frq_t, ex, double/frq_t.  
template <typename Tnum, typename Tresult, typename std::enable_if<!std::is_same<Tnum,frq_t>::value>::type>
Tresult operator/(Tnum, frq_t)=delete;  

// Forbid anything involving frq_t +/- anything other than another frq_t.  
template <typename Tlhs, typename Tresult, typename std::enable_if<!std::is_same<Tlhs,frq_t>::value>::type>
Tresult operator+(frq_t, Tlhs)=delete;  
template <typename Trhs, typename Tresult, typename std::enable_if<!std::is_same<Trhs,frq_t>::value>::type>
Tresult operator+(Trhs,frq_t)=delete;  
template <typename Tlhs, typename Tresult, typename std::enable_if<!std::is_same<Tlhs,frq_t>::value>::type>
Tresult operator-(frq_t, Tlhs)=delete;  
template <typename Trhs, typename Tresult, typename std::enable_if<!std::is_same<Trhs,frq_t>::value>::type>
Tresult operator-(Trhs,frq_t)=delete;  



