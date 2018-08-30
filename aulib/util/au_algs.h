#pragma once
#include <vector>
#include <algorithm>
#include <map>

template<typename T>
bool ismember(T const& e, std::vector<T> const& s) {
    return (std::find(s.begin(),s.end(),e) != s.end());
};

// Returns a vector containing the elements of sa that are members of sb.
// sa and sb are passed by value because they must be sorted before
// std::set_intersection() can be called.
//
template<typename T>
std::vector<T> members(std::vector<T> sa, std::vector<T> sb) {
    std::sort(sa.begin(),sa.end());
    std::sort(sb.begin(),sb.end());
    std::vector<T> si {};  // "set intersection"
    std::set_intersection(sa.begin(),sa.end(),sb.begin(),sb.end(),si.begin());
	return si;
};

// Returns the unique elements in s.  
// s is passed by value because it must be sorted before std::unique()
// can be called.
template<typename T>
std::vector<T> unique(std::vector<T> s) {
    std::sort(s.begin(),s.end());  // std::unique() requires v be sorted
    auto last = std::unique(s.begin(),s.end());
    s.erase(last,s.end());
    return s;
};

// n_unique():  Counts the # of unique elements in s
template<typename T>
size_t n_unique(std::vector<T> s) {
    std::sort(s.begin(),s.end());
    auto last = std::unique(s.begin(),s.end());
    return last-s.begin();
};

// Returns a vector containing each unique element in s along with the 
// number of occurences.  
// TODO:  Fix the static_cast
template<typename T>
std::map<T,size_t> unique_n(std::vector<T> const& s) {
    auto s_uq = unique(s);  // Note:  passed by value

    std::map<T,size_t> result {};
    for (auto const& e : s_uq) {
        result[e] = static_cast<size_t>(std::count(s.begin(),s.end(),e));
    }

    return result;
};


// true if sa is a subset of sb (all members of sa exist in sb), false
// otherwise.
template<typename T>
bool issubset(std::vector<T> sa, std::vector<T> sb) {
    for (auto const& e : sa) {
        if (std::find(sb.begin(),sb.end(),e) == sb.end()) {
            return false;
        }
    }
    return true;
};

