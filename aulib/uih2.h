#pragma once
#include <string>
#include <tuple>
#include <type_traits>  // For remove_ref<>, invoke<>, etc
#include <optional>


//
// A user needs to:
// 1)  Define a parser function object taking in some type PIType and 
//     returning a gprsr_res<PIType>.  gprsr_res<PIType> is always 
//     default-constructible: the res field is an empty 
//     std::optional<POType>, the info field is a generic default message,
//     and the msg field is an empty std::string.  
//

template<typename PIType, typename POType> struct prsr {
	//using PIType = typename PIType;
	//using POType = typename POType;

	void operator()(PIType const&);
	std::string info;
	std::string msg;
	std::optional<POType> res;
};
/*
namespace au {

template<typename T> struct uih_parser_result2 {
	using PFType = typename T;

	uih_parser_result2() = default; // {nullopt, ""};

	uih_parser_result2(std::optional<T> const& o_retval, std::string const& parse_failmsg) :
		o_result(o_retval), failmsg(parse_failmsg) {};
	
	std::optional<T> o_result {};
	std::string failmsg {"This is the default message indicating parser failure."};
};


template<typename T> struct uih_parser2 {
	using PIType = typename T::PIType;  // "parser input type"
	using PRType = typename std::invoke_result<T,PIType>::type;
	using RFType = typename PRType::PFType;
public:
	uih_parser2(T parsefunc, std::string infomsg) : 
		m_parsefunc(parsefunc),
		m_info_msg(infomsg) {};

	PRType operator()(PIType str_in) const {
		return m_parsefunc(str_in);
	};

	std::string infomsg() const { return m_info_msg; };
private:
	const T m_parsefunc;  // A parsing function has no state, hence const
	const std::string m_info_msg {};
};


template<typename T> struct uih_pred2 {
public:
	uih_pred2(T predfunc, std::string failmsg) 
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

template<typename Tprsr, typename... Tpredfuncs>
class uih2 {
public:
	using PAType = typename Tprsr::RFType;  // Predicate arg type
	using PIType = typename Tprsr::PIType;  // Parser input type
	//using PIType_noconst = typename std::remove_const<PIType>::type;
	using PIType_noref = typename std::remove_reference<PIType>::type;
	using PIType_norefconst = typename std::remove_const<PIType_noref>::type;
	
	uih2(Tprsr uih_parserfunc, Tpredfuncs... uih_preds) :
		m_parser(uih_parserfunc), m_preds(uih_preds...) { };
	
	void update(PIType raw_usr_input) {
		if (raw_usr_input == m_raw_usr_input_last) { return; }
		m_raw_usr_input_last = raw_usr_input;
		m_msg.clear();

		m_parse_result = m_parser(raw_usr_input);  // uih_parser_result struct
		if (!m_parse_result.o_result) {
			m_is_valid = false;
			m_msg = m_parse_result.failmsg;
			return;
		}
		constexpr int N = sizeof...(Tpredfuncs);
		m_is_valid = eval_preds<N-1>(*(m_parse_result.o_result));
	};

	bool is_valid() const { return m_is_valid; };
	std::string msg() const { return m_msg; };
	typename Tprsr::RFType get() const { return *(m_parse_result.o_result); };
private:
	template<int N> bool eval_preds(PAType const& predarg) {
		//std::cout << "eval_preds<" << N << ">(" << predarg << "):  \"" 
		//	<< std::get<N>(m_preds).msg() << "\"" << std::endl;
		bool pred_passed = std::get<N>(m_preds)(predarg);
		if (!pred_passed) {
			std::string s = " => Predicate " + std::to_string(N) + 
				" failed:  " + std::get<N>(m_preds).msg() + "\n";
			m_msg.append(s);
		}
		return (pred_passed && eval_preds<N-1>(predarg));
	};
	template<> bool eval_preds<-1>(PAType const& predarg) { // Recursion terminator
		return true;
	};

	//-------------------------------------------------------------------------
	// Data
	const Tprsr m_parser;
		// Returns a uih_parser_result, but itself has no state, hence can
		// be declared const.  

	typename Tprsr::PRType m_parse_result {};
		// Return-value of m_parser::operator()(std::string), a
		// uih_parse_result struct with fields o_result, failmsg
		// o_result is a std::optional<Tprsr::RFType>
	
	const std::tuple<Tpredfuncs...> m_preds;
		// Argument to operator() is Tprsr::RFType; same as the type
		// returned by get().  uih_pred's have no state hence this can
		// be declared const.  

	bool m_is_valid {false};
		// The user must check this before calling get().  False if
		// the parse fails _or_ if any of the m_preds fail.  

	std::string m_msg {};
		// Is set by a call to update(std::string) either w/ the output of
		// m_parser() (if parsing fails) or std::get<i>(m_preds).msg() for
		// those elements of m_preds that fail.  

	PIType_norefconst m_raw_usr_input_last {};
		// The most recent value passed to update().  Used to avoid 
		// unnecessary repeated processing of the same input string.  
};


}; // namespace au
*/
