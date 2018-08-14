#pragma once
#include <string>
#include <optional>
#include <vector>
#include <regex>

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


