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
	// PFType => "parser fundamental type" is whatever type contains the
	// results of a successfull parse.  The corresponding parser packages
	// an object of type PFType inside a std::optional and inserts it
	// into uih_parser_result.o_result.  
	using PFType = typename T;

	uih_parser_result() = default; // {nullopt, ""};

	uih_parser_result(std::optional<T> const& o_retval, std::string const& parse_failmsg) :
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
	// PRType ~ "parser return type" => the return type of the functor
	//   parsefunc, which is a:
	//   uih_parser_result {std::optional<T> o_result; std::string failmsg}
	// RFType ~ "return fundamental type" => if parsefunc is successfull,
	//   the type of:  *(parsefunc().o_result)
	using PRType = typename std::invoke_result<T,std::string>::type;
	using RFType = typename PRType::PFType;
	//using RFType = typename std::remove_reference<decltype(std::declval<PRType>().value())>::type;
public:
	uih_parser(T parsefunc, std::string infomsg) : 
		m_parsefunc(parsefunc),
		m_info_msg(infomsg) {};

	std::optional<RFType> operator()(std::string const& str_in) {
		m_parser_result = m_parsefunc(str_in);
		return m_parser_result.o_result;
	};

	std::string infomsg() {
		return m_info_msg;
	};

	std::string failmsg() {
		return m_parser_result.failmsg;
	};
private:
	const T m_parsefunc;  // why const???
	PRType m_parser_result;  // struct {std::optional<RFType> o_result; std::string failmsg}
	std::string m_info_msg {};
};


//
// A uih_pred functor associates some sort of unary predicate with a
// message explaining to the user the requirements of the predicate.  
//
//
template<typename T> struct uih_pred {
public:
	uih_pred(T predfunc, std::string failmsg) 
		: m_predfunc(predfunc), m_failmsg(failmsg) {};
	
	template<typename U>
	bool operator()(U const& predarg) const {
		return m_predfunc(predarg);
	};

	std::string msg() const { return m_failmsg; };
private:
	const T m_predfunc;
	const std::string m_failmsg;
};

//
// What you do is:
// 1)  Write a functor taking a std::string and returning a
//   uih_parser_result<whatever>.  For example, to parse a ts_t:
//   struct parse_userinput_ts {
//     uih_parser_result<ts_t> operator()(std::string) {
//       result = uih_parser_result<ts_t> {}
//       ...
//       if (successfull_parse) {
//	       result.o_result = parsed_ts_t_object;
//         result.failmsg = "";
//         // or just:  return {parsed_ts_t_object, ""};
//       } else {
//	       result.o_result = {};
//         result.failmsg = "nope :(";
//       }
//       return result;
//    };
//  };
//
//
// 2)  Construct a uih_parser object containing your functor
//   (parse_userinput_ts(std::string)) as a member:
//
//   uih_parser ts_parser {parse_userinput_ts, "The format of a ts
//     is n/d where n,d are both integers > 0."};
//
//
// 3)  Create any uih_pred functors as necessary.  These must 
//   take in the same type as the RFType of the uih_parser functor
//   returned by the parser.  
//   For example,
//   uih_pred pred_numerator {[](ts_t const& ts){return (ts.m_bpb() > beat_t{12});},  
//     "Numerators > 12 are not yet supported."};
//   uih_pred pred_compound {[](ts_t const& ts){return (!ts.is_compound());},  
//     "Compound time sigs are not yet supported."};
//
//
//
// 4)  Construct the uih object:
//   uih my_ts_helper {parse_userinput_ts, pred_numerator, pred_compound};
//
//
// Done !
//
//

template<typename Tprsr, typename... Tpredfuncs>
class testyclass {
public:
	using PAType = typename Tprsr::RFType;  // Predicate arg type

	testyclass(Tprsr uih_parserfunc, Tpredfuncs... uih_preds) :
		m_parser(uih_parserfunc), m_preds(uih_preds...) { };
	
	void update(std::string const& str_in) {
		if (str_in == m_str_last) {	return;	}
		m_str_last = str_in;

		m_parse_result = m_parser(str_in);
		if (!m_parse_result) {
			m_is_valid = false;
			m_msg = m_parser.failmsg();
			return;
		}

		m_is_valid = eval_preds<std::tuple_size<std::tuple<Tpredfuncs...>>::value - 1>(*m_parse_result);
	};

	bool is_valid() const { return m_is_valid; };
	bool msg() const { return m_msg; };
private:
	template<int N> bool eval_preds(PAType const& predarg) {
		std::cout << "eval_preds<" << N << ">(" << predarg << "):  \"" 
			<< std::get<N>(m_preds).msg() << "\"" << std::endl;
		bool pred_passed = std::get<N>(m_preds)(predarg);
		if (!pred_passed) {
			std::string s {};
			s += " => Predicate " + std::to_string(N) + " failed:  ";
			s += std::get<N>(m_preds).msg() + "\n";
			m_msg.append(s);
		}
		return (pred_passed && eval_preds<N-1>(predarg));
	};
	template<> bool eval_preds<-1>(PAType const& predarg) { // Recursion terminator
		return true;
	};

	// Data
	Tprsr m_parser;
	std::optional<typename Tprsr::RFType> m_parse_result {};
	std::tuple<Tpredfuncs...> m_preds;
	bool m_is_valid {false};
	std::string m_msg {};
	std::string m_str_last {};
};




