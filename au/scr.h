#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <vector>
#include <tuple>
#include <functional>
#include <type_traits>
#include <optional>

// EX:
// stdoptional_internaltype<std::optional<std::string>>::type should_bea_string {};
template<typename T> struct stdoptional_internaltype {
	using type = typename std::remove_reference<decltype(std::declval<T>().value())>::type;
};

template<typename T> struct uih_parser_result {
	using PFType = typename T;
	uih_parser_result() = default;
	uih_parser_result(std::optional<T> o_retval, std::string const& parse_failmsg) :
		o_result(o_retval), failmsg(parse_failmsg) {};
	
	std::optional<T> o_result {};
	std::string failmsg {"This parser supplies no info message."};
};

//
// A uih_parser pairs a "parsefunc" (see below) with an "info message"
// explaining to the user the expected format of the input.  
//
// A parsefunc is a function object that returns a uih_parser_result struct:
// { std::optional<T> o_result; std::string failmsg; }
// where T is some sort of type (possibly itself a struct) containing the 
// elements parsed out of the input string.  If the input string could not 
// be parsed, parsefunc returns the uih_parser_result as:
// { std::optional<T> {}, "a helpful error message"}.  
// 
//
template<typename T> struct uih_parser {
	// "Return type" and "Return fundamental type"
	// PRType ~ uih_parser_result {std::optional<T> o_result; std::string failmsg}
	using PRType = typename std::invoke_result<T,std::string>::type;
	using RFType = typename PRType::PFType; //typename PRType::PFType;
	//using RFType = typename std::invoke_result<T,std::string>::PFType;
	//using RFType = typename std::remove_reference<decltype(std::declval<ROType>().value())>::type;
public:
	uih_parser(T parsefunc, std::string infomsg) : 
		m_parsefunc(parsefunc),
		m_info_msg(infomsg) {};

	// Runs the parser...
	std::optional<RFType> operator()(std::string const& str_in) {
		m_parser_result = m_parsefunc(str_in);
		//m_o_parse_result = m_parser_result.o_result;
		m_fail_msg = m_parser_result.failmsg;
		
		return m_parser_result.o_result;
	};

	/*RFType get() {
		return *(m_parser_result.o_result);
	};*/

	std::string infomsg() {
		return m_info_msg;
	};

	std::string failmsg() {
		return m_fail_msg;
	};
private:
	const T m_parsefunc;  // why const???
	PRType m_parser_result;  // struct {std::optional<RFType> o_result; std::string failmsg}
	//std::optional<RFType> m_o_parse_result;  //
	std::string m_fail_msg {};
	std::string m_info_msg {};
};

template<typename Tpf, typename Tpa> struct uih_pred2 {
public:
	uih_pred2(Tpf predfunc, std::string failmsg) 
		: m_predfunc(predfunc), m_failmsg(failmsg) {};

	bool isvalid(Tpa const& predarg) const {
		return m_predfunc(predarg);
	};

	std::string msg() const {
		return m_failmsg;
	};
private:
	const Tpf m_predfunc;
	const std::string m_failmsg;
};

template<typename Tprsr, typename... T>
class testyclass {
	//std::enable_if<Tprsr,T>;
	//using Tprsr_rftype = typename Tprsr::RFtype;
public:
	testyclass(Tprsr uih_parserfunc, T... uih_preds) :
		m_parser(uih_parserfunc), m_preds(uih_preds...) { };
	
	void update(std::string const& str_in) {
			if (str_in == m_str_last) {	return;	}
			m_str_last = str_in;

			m_parse_result = m_parser(str_in);
			if (!m_parse_result) {
				m_is_valid = false;
				m_msg = "Unable to parse :(";
				return;
			}

			bool all_preds_passed = 
				eval_preds<std::tuple_size<std::tuple<T...>>::value - 1>(*m_parse_result);
			if (!all_preds_passed) {
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

	// Data
	Tprsr m_parser;
	//std::optional<typename Tprsr::RFType> m_parse_result {};
	std::optional<typename Tprsr::RFType> m_parse_result {};
	std::tuple<T...> m_preds;
	bool m_is_valid {false};
	std::string m_msg {};
	std::string m_str_last {};
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
		//	return;
		//}
		//m_str_last = str_in;

		bool all_preds_passed = eval_preds<std::tuple_size<std::tuple<T...>>::value - 1>(str_in);
		if (!all_preds_passed) {
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


