#pragma once

#include <string>
#include <iostream>



template<int N> void f() {
	std::cout << "f<" << N << ">():  Unspecialized" << std::endl;
	f<N-1>();
};

template<> void f<-1>();







