#include "line_t.h"
#include "types\scd_t.h"
#include "types\frq_t.h"
#include "types\ntl_t.h"
#include "types\rp_t.h"
#include<variant>

apt_t::apt_t(scd_t scd_in) {
	m_pdata = scd_in;
}

apt_t::apt_t(frq_t frq_in) {
	m_pdata = frq_in;
}

apt_t::apt_t(ntstr_t ntstr_in) {
	m_pdata = ntstr_in;
}

