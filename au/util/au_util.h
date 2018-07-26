#pragma once
#include <string>
#include <optional>
#include <vector>

int str2int(std::string);

std::string int_suffix(int const&);

std::optional<std::vector<std::optional<std::string>>> 
	rx_match_captures(std::string const&, std::string const&);

int wait();







