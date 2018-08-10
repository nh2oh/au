#pragma once
#include <string>



class nv_t {
public:
	explicit nv_t();
	explicit nv_t(double const&,int const& =0);  // base-value (=1/2^m), n-dots

	std::string print() const;
	int ndot() const;
	nv_t base() const;  // The nv_t w/the same value of m_m, but with m_n = 0

	nv_t& add_dot(int const& =1); 
	nv_t& set_dots(int const&);
	nv_t& rm_dots(int const&);
	nv_t& rm_dots();  // Remove all the dots, if any

	friend double operator/(nv_t const&, nv_t const&);
	friend bool operator==(nv_t const&, nv_t const&);
	friend bool operator!=(nv_t const&, nv_t const&);
	friend bool operator<(nv_t const&, nv_t const&);
	friend bool operator>(nv_t const&, nv_t const&);
	friend bool operator<=(nv_t const&, nv_t const&);
	friend bool operator>=(nv_t const&, nv_t const&);
private:
	double nv() const;
	double bv() const;  // == 1/(2^m)

	int m_m {0};  // exponent of the base value
	int m_n {0};  // number of dots

	static double max_bv;
	static double min_bv;
	static int max_n;

};




// Adding and subtracting nv's does not make sense:  There is no nv
// that can represent the span of, say, (q + q.).  Although /some/ 
// "sums" may correspond to the spans of single nv's, not all do.  
// What we're really saying is "the amount of time occupied by 'this sum'
// is equal to "the amount of time occupied by 'that sum'."  We are
// implictly converting to time.  












