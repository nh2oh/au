#include <vector>
#include <string>
#include <iostream>
#include "scale.h"
#include "scale_12tet.h"
#include "scale_diatonic12tet.h"
#include "types\frq_t.h"
#include "types\scd_t.h"
#include "types\ntl_t.h"
#include "util\au_util_all.h"
#include "types\rp_t.h"
#include "rand_rp.h"
#include "line_t.h"
#include <variant>

using namespace std::literals::string_literals;

int main(int argc) {
	auto qnv = tonset2rp_demo();
	auto s = printrp("4/4"_ts,qnv);
	std::cout << s << std::endl;
    return 0;
}

