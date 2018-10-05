#pragma once
#include "tinyformat2.h"

namespace tinyformat {

//------------------------------------------------------------------------------
// Primary API functions

/// Make type-agnostic format list from list of template arguments.
///
/// The exact return type of this function is an implementation detail and
/// shouldn't be relied upon.  Instead it should be stored as a FormatListRef:
///
///   FormatListRef formatList = makeFormatList( /*...*/ );
template<typename... Args>
detail::FormatListN<sizeof...(Args)> makeFormatList(const Args&... args) {
	return detail::FormatListN<sizeof...(args)>(args...);
}

/// Format list of arguments to the stream according to the given format string.
///
/// The name vformat() is chosen for the semantic similarity to vprintf(): the
/// list of format arguments is held in a single function argument.
inline void vformat(std::ostream& out, const char* fmt, FormatListRef list) {
	detail::formatImpl(out, fmt, list.m_formatters, list.m_N);
}

/// Format list of arguments to the stream according to given format string.
template<typename... Args>
void format(std::ostream& out, const char* fmt, const Args&... args) {
	vformat(out, fmt, makeFormatList(args...));
}

/// Format list of arguments according to the given format string and return
/// the result as a string.
template<typename... Args>
std::string format(const char* fmt, const Args&... args) {
	std::ostringstream oss;
	format(oss, fmt, args...);
	return oss.str();
}

/// Format list of arguments to std::cout, according to the given format string
template<typename... Args>
void printf(const char* fmt, const Args&... args) {
	format(std::cout, fmt, args...);
}

template<typename... Args>
void printfln(const char* fmt, const Args&... args) {
	format(std::cout, fmt, args...);
	std::cout << '\n';
}


} // namespace tinyformat