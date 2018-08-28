#pragma once
#include <string>
#include <optional>
#include <vector>
#include <regex>
#include <set>
#include <cstdio>

template<typename... Ts>
std::string bsprintf(const char *fmt, Ts... args) {
	size_t size = std::snprintf(nullptr,0,fmt,args...);
	std::string s_out(size+1,'\0');
	std::sprintf(&s_out[0], fmt, args...);
	return s_out.substr(0,size);
}

template<typename... Ts>
std::string bsprintf(std::string const& fmt, Ts... args) {
	return bsprintf(fmt.c_str(),args...);
}

std::string int_suffix(int const&);

// Specify regex as string (arg 1); function calls the std::regex
// constructor
std::optional<std::vector<std::optional<std::string>>> 
	rx_match_captures(std::string const&, std::string const&);

// Call the std::regex constructor yourself and pass the object in
std::optional<std::vector<std::optional<std::string>>> 
	rx_match_captures(std::regex const&, std::string const&);


// Waits a v. short amount of time... sometimes useful for debugging
int wait();

/*
template<typename T>
class uih_pred {
public:
	explicit uih_pred(T const& upred, const char *fail_msg) : m_upred(upred), m_fail_msg(fail_msg) { };
	
	bool operator()(std::string const& str_in) { return m_upred(str_in); };
	const char* msg() const { return m_fail_msg; };

private:
	T m_upred;
	const char *m_fail_msg;
};

// To make one of these, define a function of only one parameter (a
// std::string) that returns a uih_parser::uih_parse_res struct and pass this
// function to the constructor.  
template<typename T, typename U>
class uih_parser {
public:
	struct uih_parse_res<U> {
		std::optional<U> v {};
		std::string msg {};
	};

	explicit uih_parser(T const& uparser) : m_parser(uparser) { };

	uih_parse_res<U> operator()(std::string const& str_in) const { return m_parser(str_in); };
private:
	T m_parser;
};

template<typename T>
class uih {
public:
	explicit uih(uih_parser<T> const& , const char *fail_msg) : m_upred(upred),
		m_fail_msg(fail_msg) { };
private:
	
};

*/







