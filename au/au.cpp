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
	//wrk edits
	std::vector<note_value> x {note_value{1.0/1.0,1}, note_value{1.0/2.0,1}};
	std::cout << printrp("4/4"_ts,x) << std::endl;

	auto s = tonset2rp_demo();
	std::cout << s << std::endl;
    return 0;
}

