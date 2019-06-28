#include "generic_iterator.h"
#include <vector>

struct gi_test_types_a_t {
	using value_type = int;
	using size_type = uint32_t;
	using difference_type = std::ptrdiff_t;  // TODO:  Inconsistent
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
};

void generic_iterator_tests() {
	//std::vector<int> vi {0,1,2,3,4,5,6,7,8,9};
	//int *pbeg = &vi[0];  int *pend = &vi[9]+1;
	//const int *cpbeg = &vi[0];  const int *cpend = &vi[9]+1;

	//auto a = generic_ra_iterator<std::vector<int>,false>(pbeg);
	//auto b = generic_ra_iterator<std::vector<int>,false>(pend);
	//auto ca = generic_ra_iterator<std::vector<int>,true>(cpbeg);
	//auto cb = generic_ra_iterator<std::vector<int>,true>(cpend);

	//bool a_b = a==b;
	//bool ca_cb = ca==cb;
	//bool a_cb = a==cb;
	//bool ca_b = ca==b;
}




