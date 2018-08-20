#include "aulib\uih.h"
#include "aulib\types\ts_t.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\numeric_simple_uih.h"
#include <vector>


//template<typename T>
class aclass {
public:

	double e = 34.3432;
	//auto ee = static_cast<double>(34.3432);
	std::vector<decltype(e)> y = std::vector(12,e);


	
	au::uih_parser<parse_userinput_ts> m_ts_parser1 = 
		au::make_uih_parser(parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0.");
	au::uih_parser<parse_userinput_ts> m_ts_parser2 {parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0."};
	au::uih<decltype(m_ts_parser2)> m_ts_uih {m_ts_parser2};

	au::uih_parser<parse_userinput_double> m_bpm_parser {parse_userinput_double {},
		"A number > 0 (decimals allowed)."};
	au::uih<decltype(m_bpm_parser),ftr_gtzero> m_bpm_uih {m_bpm_parser,ftr_gtzero{}};
	//au::uih m_bpm_uih {m_bpm_parser,bpm_pred_positive};

	/*
	auto x = std::vector {1.0,12.2421,-1234.432,123.4234};
	au::uih_parser m_ts_parser {parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0."};
	au::uih m_ts_uih {m_ts_parser};

	au::uih_parser m_bpm_parser {parse_userinput_double {},
		"A number > 0 (decimals allowed)."};
	au::uih_pred bpm_pred_positive {[](double const& inp){return (inp > 0.0);},
		"Value must be > 0"};
	au::uih m_bpm_uih {m_bpm_parser,bpm_pred_positive};
	*/
private:
};


int bfunc() {
	auto y = std::vector(12,34.3432);

	auto ts_parser = au::make_uih_parser(parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0.");

	au::uih_parser m_ts_parser {parse_userinput_ts {},
		"The format of a ts is n/d[c] where n,d are both integers > 0."};
	au::uih m_ts_uih {m_ts_parser};

	au::uih_parser m_bpm_parser {parse_userinput_double {},
		"A number > 0 (decimals allowed)."};
	au::uih_pred bpm_pred_positive {[](double const& inp){return (inp > 0.0);},
		"Value must be > 0"};
	au::uih m_bpm_uih {m_bpm_parser,bpm_pred_positive};

	return 0;
};

