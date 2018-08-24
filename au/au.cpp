#include <vector>
#include <string>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <cstdio>
#include <any>
#include <tuple>
#include <type_traits>
#include <functional>
#include <optional>
#include "aulib\aulib_all.h"

#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\nv_t_uih.h"
#include "aulib\numeric_simple_uih.h"

int main(int argc) {

	au::uih_parser<parse_userinput_ts> ts_parser{parse_userinput_ts{},
		"The ts (format: n/d[c] where c means \"compound\")"};
	au::uih<decltype(ts_parser)> ts_uih {ts_parser};

	au::uih_parser<parse_userinput_nv> nv_parser{parse_userinput_nv{},
		"The nv format: 1/d[.] where . means whatever..."};
	au::uih<decltype(nv_parser)> nv_uih {nv_parser};

	au::uih_parser<parse_userinput_double> m_bpm_parser{parse_userinput_double{},
		"The tempo in beats-per-minute"};
	au::uih_pred<ftr_gt> p_gtzero{ftr_gt{0.0},"A tempo is always > 0"};
	au::uih<decltype(m_bpm_parser),decltype(p_gtzero)> m_bpm_uih{m_bpm_parser,p_gtzero};

	au::uih_parser<parse_userinput_double> m_err_parser{parse_userinput_double{},
		"A number >= 0 (decimals allowed)."};
	au::uih_pred<ftr_geq> p_geqzero{ftr_geq{0.0},"Value must be >= 0."};
	au::uih<decltype(m_err_parser),decltype(p_geqzero)> m_err_uih{m_err_parser,p_geqzero};

	std::string s1 {"yay"};

	m_err_uih.update(s1);

    return 0;
}

