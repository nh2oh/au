#include "scr2.h"

template<> void f<-1>() {
	std::cout << "f<" << -1 << ">():  Specialized <-1> => base-case " 
		<< "to terminate the recursion" << std::endl;
};



