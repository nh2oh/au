#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>

void printx();

template<typename T, typename... Tail>
void printx(T head, Tail... tail) {
	std::cout << head << ' ';
	printx(tail...);
};

/*
template<typename T> struct uih_pred {
public:
	uih_pred(T predfunc, std::string failmsg) 
		: m_predfunc(predfunc), m_failmsg(failmsg) {};
	
	template<typename U>
	bool isvalid(U const& inpint) const {
		return m_predfunc(inpint);
	};

	std::string msg() const {
		return m_failmsg;
	};
private:
	const T m_predfunc;
	const std::string m_failmsg;
};

*/


/*
template<int N> struct kact; // { int val = N; };
template<> struct kact<5> { int val = 5; };

template<typename Tf, typename... T>
std::vector<Tf> tovectf(Tf vf, T... v) {
	//return std::vector<Tf>(std::initializer_list<Tf>(vf, v...));
	return std::vector<Tf>{vf, v...};
}
*/

/*

template<typename T2>
std::string tf(std::string fmt, T2 a2) {
	std::cout << __FUNCSIG__ << std::endl << std::endl << fmt << std::endl << std::endl;
	auto n = snprintf(nullptr,0,fmt.c_str(),a2);
	std::string s(n+1,'\0');
	std::snprintf(&s[0], n, fmt.c_str(), a2);
	return s;
}

template<typename T2, typename... Ttail>
std::string tf(std::string fmt, T2 a2, Ttail... atail) {
	std::cout << __FUNCSIG__ << std::endl << std::endl << fmt << std::endl;
	

	auto c = &(fmt.back());
	for (;; ++c) {
		if (*c == '\0') { break; }
		if (*c == '%' && *(c+1) != '%') { break; }
	}
	auto d = c - fmt.c_str();
	auto sfront = std::string(fmt.c_str(),0,d);
	auto dd = &(fmt.back()) - c;
	auto sback = std::string(c,0,dd);
	auto n = snprintf(nullptr,0,fmt.c_str(),a2);
	std::string s(n+1,'\0');
	std::snprintf(&s[0], n, fmt.c_str(), a2);

	std::cout << "-------------------------------" << std::endl;
	return tf(s, atail...);
}


*/


