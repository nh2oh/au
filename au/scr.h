#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <tuple>
#include <functional>
#include <type_traits>
#include <optional>

template<typename T> struct uih_parser {
	using RType = typename std::invoke_result<T,std::string>::type;
	using RFType = typename std::remove_reference<decltype(std::declval<RType>().value())>::type;
public:
	uih_parser(T parsefunc) : m_parsefunc(parsefunc), m_parseresult(RType {}) {};
private:
	const T m_parsefunc;
	RType m_parseresult;
};


template<typename T> struct uih_pred {
public:
	uih_pred(T predfunc, std::string failmsg) 
		: m_predfunc(predfunc), m_failmsg(failmsg) {};
	
	template<typename U>
	bool isvalid(U const& predarg) const {
		return m_predfunc(predarg);
	};

	std::string msg() const {
		return m_failmsg;
	};
private:
	const T m_predfunc;
	const std::string m_failmsg;
};


template<typename... T> class uih2 {
public:
	uih2(T... uih_preds) : m_preds(uih_preds...) { };

	void update(int const& str_in) {
		//if (str_in == m_str_last) {
			//return;
		//}
		m_str_last = str_in;

		bool x = eval_preds<std::tuple_size<std::tuple<T...>>::value - 1>(str_in);
		if (!x) {
			std::cout << "Invalid:  "  << m_msg << std::endl;
			m_is_valid = false;
		} else {
			m_is_valid = true;
		}
	};

	bool is_valid() { 
		return m_is_valid; 
	};

private:
	std::tuple<T...> m_preds;
	const int m_N =  std::tuple_size<std::tuple<T...>>::value;

	template<int N> bool eval_preds(int const& predarg) {
		std::cout << "eval_preds<" << N << ">(" << predarg << "):  \"" 
			<< std::get<N>(m_preds).msg() << "\"" << std::endl;
		bool pred_passed = std::get<N>(m_preds).isvalid(predarg);
		if (!pred_passed) {
			std::string s {};
			s += " => Predicate " + std::to_string(N) + " failed:  ";
			s += std::get<N>(m_preds).msg() + "\n";
			m_msg.append(s);
		}
		return (pred_passed && eval_preds<N-1>(predarg));
	};
	template<> bool eval_preds<-1>(int const& predarg) { // Recursion terminator
		return true;
	};

	bool m_is_valid {false};
	std::string m_msg {};
	std::string m_str_last {};
	
};




template<typename TP1, typename TP2>
class uih {
public:
	uih(uih_pred<TP1> p1, uih_pred<TP2> p2) : m_p1(p1),m_p2(p2) {};

private:
	uih_pred<TP1> m_p1;
	uih_pred<TP2> m_p2;
};



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


