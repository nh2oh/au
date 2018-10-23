#include "spn12tet.h"
#include "..\types\cent_oct_t.h"
#include "..\types\ntl_t.h"
#include "..\types\frq_t.h"
#include "..\types\scd_t.h"
#include "..\util\au_util.h"
#include "..\util\au_error.h"
#include "..\util\au_algs.h"  // ismember()
#include "..\util\au_algs_math.h"  //aprx_int()
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

spn12tet::spn12tet(spn12tet::pitch_std ps) {
	au_assert(ps.gen_int > 0,"ono");
	au_assert(ps.ntet>0,"ono");
	m_ps = ps;
	auto idx = std::find(m_ntls.begin(),m_ntls.end(),ntl_t{m_ps.ref_ntstr});
	au_assert(idx != m_ntls.end(),"ntl not found");
	m_shift_scd = (octn_t{ps.ref_ntstr}.to_int())*12+static_cast<int>(idx-m_ntls.begin());
	// Expect 57 for a ref pitch of A(4)
}

int spn12tet::n() const {
	return 12;
}
std::string spn12tet::name() const {
	return m_name;
}
std::string spn12tet::description() const {
	return m_description;
}
bool spn12tet::isinsc(ntl_t ntl_in) const {
	return ismember(ntl_in, m_ntls);
}
bool spn12tet::isinsc(frq_t frq_in) const {
	return aprx_int(n_eqt(frq_in, m_ps.ref_frq, m_ps.ntet, m_ps.gen_int));
}
std::string spn12tet::print(scd_t from, scd_t to) const {
	std::string s {};
	s = dbk::bsprintf("%s\n%s\n\n",m_name,m_description);

	for (scd_t curr_scd=from; curr_scd<to; ++curr_scd) {
		//std::string curr_line {};
		s += dbk::bsprintf("%d:\t%s\t%.3f\n",
			curr_scd.to_int(),to_ntstr(curr_scd).print(),to_frq(curr_scd)/frq_t{1});
	}
	s += "\n\n";
	return s;
}

ntstr_t spn12tet::to_ntstr(scd_t scd) const {  // Reads m_default_valid_ntls directly
	rscdoctn_t rscd {scd,12};
	return ntstr_t {m_ntls[rscd.to_int()], octn_t{scd,12}};
}
std::vector<ntstr_t> spn12tet::to_ntstr(const std::vector<scd_t>& scds) const {
	std::vector<ntstr_t> ntstrs(scds.size(),ntstr_t{});
	for (int i=0; i<scds.size(); ++i) {
		ntstrs[i] = to_ntstr(scds[i]);
	}
	return ntstrs;
}
ntstr_t spn12tet::to_ntstr(frq_t frq) const {  // wrapper
	scd_t scd = to_scd(frq);
	return to_ntstr(scd);
}

frq_t spn12tet::to_frq(scd_t scd) const {  // Calls frq_eqt() directly
	return frq_eqt(scd.to_int()-m_shift_scd,m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
}
std::vector<frq_t> spn12tet::to_frq(const std::vector<scd_t>& scds) const {
	std::vector<frq_t> res(scds.size(),frq_t{});
	for (int i=0; i<scds.size(); ++i) {
		res[i] = to_frq(scds[i]);
	}
	return res;
}
frq_t spn12tet::to_frq(ntstr_t ntstr) const {  // wrapper
	scd_t scd = to_scd(ntstr);
	return to_frq(scd);
}
std::vector<frq_t> spn12tet::to_frq(const std::vector<ntstr_t>& ntstr) const {
	std::vector<frq_t> res(ntstr.size(),frq_t{});
	for (int i=0; i<ntstr.size(); ++i) {
		res[i] = to_frq(ntstr[i]);
	}
	return res;
}
scd_t spn12tet::to_scd(frq_t frq) const {  //  Calls n_eqt() directly
	double dn_approx = n_eqt(frq,m_ps.ref_frq,m_ps.ntet,m_ps.gen_int);
	au_assert(aprx_int(dn_approx),"frq not in sc");
	return scd_t {static_cast<int>(std::round(dn_approx+m_shift_scd))};
}
scd_t spn12tet::to_scd(ntstr_t ntstr) const {  // Reads m_default_valid_ntls directly
	auto it = std::find(m_ntls.begin(),m_ntls.end(),ntl_t{ntstr});
	au_assert(it != m_ntls.end(), "ntl not in sc");
	int ntl_idx = it-m_ntls.begin();
	int octn = octn_t{ntstr}.to_int();
	return scd_t {ntl_idx+12*octn};
}
std::vector<scd_t> spn12tet::to_scd(const std::vector<ntstr_t>& ntstr) const {
	std::vector<scd_t> res(ntstr.size(),scd_t{});
	for (int i=0; i<ntstr.size(); ++i) {
		res[i] = to_scd(ntstr[i]);
	}
	return res;
}

octn_t spn12tet::to_octn(scd_t scd) const {
	return octn_t{scd,12};
}
octn_t spn12tet::to_octn(frq_t frq) const {
	scd_t scd = to_scd(frq);
	return to_octn(scd);
}


