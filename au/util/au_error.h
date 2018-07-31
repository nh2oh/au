#pragma once
#include <string>

void au_error(std::string);

// If the first arg is false, calls au_error(arg 2)
void au_assert(bool,std::string);
void au_assert(bool);
