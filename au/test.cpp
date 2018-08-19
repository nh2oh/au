#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\numeric_simple_uih.h"

int bfunc() {
	au::uih_parser m_ts_parser {parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0."};
	au::uih m_ts_uih {m_ts_parser};

	au::uih_parser m_bpm_parser {parse_userinput_double {},
		"A number > 0 (decimals allowed)."};
	au::uih_pred bpm_pred_positive {[](double const& inp){return (inp > 0.0);},
		"Value must be > 0"};
	au::uih m_bpm_uih {m_bpm_parser,bpm_pred_positive};

	return 0;
}

