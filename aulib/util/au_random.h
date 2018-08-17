#pragma once
#include <vector>
#include <random>

//
// Functions that make interacting with the c++ STL <random> library slightly
// less of a pain in the ass.  
//

std::mt19937 new_randeng(bool randseed=true);

std::vector<int> urandi(int,int,int);
std::vector<double> urandd(int,double,double);

std::vector<size_t> randset(int const&, std::vector<double> const&, std::mt19937&);

std::vector<double> normalize_probvec(std::vector<double>);

