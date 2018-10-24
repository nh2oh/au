#include "au_meta.h"
#include "../types/types_all.h"
#include "dbklib/type_inspect.h"
#include "dbklib/tinyformat_dbk.h"
#include <vector>
#include <string>


std::string print_tt_table() {
	dbk::contigmap<std::string,dbk::contigmap<std::string,bool>> all {};
	frq_t frq_def {}; all["frq_t"] = dbk::inspect_type(frq_def);
	cent_t cent_def {}; all["cent_t"] = dbk::inspect_type(cent_def);
	oct_t oct_def {}; all["oct_t"] = dbk::inspect_type(oct_def);
	scd_t scd_def {}; all["scd_t"] = dbk::inspect_type(scd_def);
	ntl_t ntl_def {}; all["ntl_t"] = dbk::inspect_type(ntl_def);
	ntstr_t ntstr_def {}; all["ntstr_t"] = dbk::inspect_type(ntstr_def);
	beat_t beat_def {}; all["beat_t"] = dbk::inspect_type(beat_def);
	bar_t bar_def {}; all["bar_t"] = dbk::inspect_type(bar_def);
	rp_t rp_def {}; all["rp_t"] = dbk::inspect_type(rp_def);
	ts_t ts_def {}; all["ts_t"] = dbk::inspect_type(ts_def);
	d_t d_def {}; all["d_t"] = dbk::inspect_type(d_def);
	tmetg_t tmetg_def {}; all["tmetg_t"] = dbk::inspect_type(tmetg_def);
	musel_t<scd_t> muselscd_def {scd_t{},{}};  all["musel_t<scd_t>"] = dbk::inspect_type(muselscd_def);
	line_t<scd_t> linescd_def {}; all["line_t<scd_t>"] = dbk::inspect_type(linescd_def);
	teejee teejee_def {}; all["teejee_t"] = dbk::inspect_type(teejee_def);
	chord_t<scd_t> chordscd_def {}; all["chord_t<scd_t>_t"] = dbk::inspect_type(chordscd_def);

	size_t num_type_traits {43};

	std::vector<std::string> lines(num_type_traits,"");
	std::string s_header {};
	s_header += dbk::bsprintf("%-45s","Type trait");
	
	bool first_iter {true};
	for (auto type : all) {
		s_header += dbk::bsprintf("%-18s",type.k);
		size_t i {0};
		for (auto tt : type.v) {
			if (first_iter) {
				lines[i] += dbk::bsprintf("%-45s",tt.k);
			}
			lines[i] += dbk::bsprintf("%-18d",tt.v);
			++i;
		}
		first_iter = false;
	}

	std::string res {};
	res = dbk::bsprintf("%s\n",s_header);
	for (const auto& tt : lines) {
		res += dbk::bsprintf("%s\n",tt);
	}

	return res;
}





