#pragma once
#include <filesystem>
#include <vector>

// Associates a filename with its contents held as a std::vector<unsigned char>
// in field binfile.d.  
struct binfile {
	std::filesystem::path file {};
	std::vector<unsigned char> d {};

	// Why unsigned char and not char?
	// Modified from:
	// https://stackoverflow.com/questions/13642381/c-c-why-to-use-unsigned-char-for-binary-data
	// In C, the unsigned char data type is the only data type:
	// 1) that has no padding bits; all storage bits contribute to the value.  
	// 2) for which no bitwise operation starting from a value of that type, when converted
	//    back into that type, can produce overflow, trap representations or 
	//    undefined behavior.
	//    There is no permutation of the bit sequence that produces overflow, trap 
	//    representations or undefined behavior.  
	// 3) that may alias other data types without violating the "aliasing rules;" 
	//    access to the same data through a pointer that is typed differently will be 
	//    guaranteed to see all modifications.  
	//
};


// Binary-mode file reader
binfile readfile(std::filesystem::path);

int readriff(const binfile&);




