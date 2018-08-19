#pragma once
#include "..\uih.h"
#include "ts_t.h"
#include <string>


struct ts_t_parts {
	double bt_per_bar {0.0};
	double nv_per_bt {0.0};
	bool is_compound {false};
	ts_t ts {};
};  // Not used

// My primary ts parser
struct parse_userinput_ts {
	au::uih_parser_result<ts_t> operator()(std::string const&) const;
};

