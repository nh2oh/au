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
	element_t<scd_t,note_value> e1 {scd_t{5},note_value{0.25}};
	element_t<scd_t,note_value> e2 {scd_t{7},note_value{1.0/8.0}};
	element_t<frq_t,note_value> e3 (frq_t{2553},note_value{1.0/2.0,1});
	element_t e4 {scd_t{57},note_value{0.25}};
	std::vector<element_t<scd_t,note_value>> ve1 {e1,e2};

	line_t ell1 {e1,e2,e1,e1,e2};

	std::vector<apt_t> vapt {scd_t{5},scd_t{7},frq_t{2535},scd_t{33},frq_t{4000}};

	std::variant<int,double, std::string> v1 {};
	if (argc == 1) {
		vapt.push_back(scd_t{22});
	} else {
		vapt.push_back(frq_t{2200});
	}

	v1=7;
	v1=std::string("yay");

	int y = 0;
	auto z = v1.index();

	std::cout << std::endl << std::endl;
	std::cout << std::endl << std::endl;
	/*
	ts_t ts1 {beat_t{3},note_value{1.0/4.0}};
	ts_t ts2 {beat_t{9},note_value{1.0/8.0},true};
	ts_t ts3 {beat_t{4},note_value{1.0/4.0}};
	ts_t ts4 = "9/8"_ts;
	std::cout << "ts4 bar-unit:  " + ts4.bar_unit().print() << std::endl;
	ts_t ts5 = "12/8c"_ts;

	std::cout << "ts1:  " << ts1.print() << std::endl;
	std::cout << "ts2:  " << ts2.print() << std::endl;
	std::cout << "ts3:  " << ts3.print() << std::endl;
	std::cout << "ts4:  " << ts4.print() << std::endl;
	std::cout << "ts5:  " << ts5.print() << std::endl;

	std::cout << rp_t_info() << std::endl;
	
	auto x = rapprox(8.0/9.0,256);
	auto xi = rapprox(9.0/8.0,256);
	
	std::vector<note_value> dp1 {note_value{1.0/1.0},note_value{1.0/2.0},
		note_value{1.0/4.0},note_value{1.0/8.0},note_value{1.0/16.0}};
	std::vector<note_value> dp3 {note_value{1.0/2.0},note_value{1.0/4.0,1},note_value{1.0/4.0}}; 
	std::vector<note_value> dp2 {note_value{1/4.0,1},note_value{1/8.0}}; 
	std::vector<note_value> dp4 {note_value{1.0/1.0,0},note_value{1.0/2.0,1},note_value{1/4.0,1},note_value{1/8.0}};

	//auto tmgreturn = tmetg(ts1,dp4,std::vector<beat_t>(dp4.size(),beat_t{0.0}));
	//std::cout << print_tg(tmgreturn) << std::endl;
	std::cout << std::endl << std::endl;

	tmetg_t mytg {"6/8c"_ts,dp4,std::vector<beat_t>{beat_t{0.0},beat_t{0.0},beat_t{-2.0/12.0},beat_t{0.0}}};
	auto tg_print = mytg.print();
	std::cout << tg_print << std::endl;

	tmetg_t mytgnc {"6/8"_ts,dp4,std::vector<beat_t>{beat_t{0.0},beat_t{0.0},beat_t{-2.0/12.0},beat_t{0.0}}};
	auto tg_printnc = mytgnc.print();
	std::cout << tg_printnc << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << std::endl << std::endl;
	*/

	/*auto rrp1 = rand_rp(myts3,dp2,pd,0,bar_t{3});
	if (rrp1) { 
		std::cout << "rrp1 ==\t" << printrp(myts3,*rrp1) << std::endl;
	} else {
		std::cout << "rrp1 timeout!" << std::endl;
	}*/
	/*
	auto rrp2 = rand_rp(ts2,dp2,std::vector<double>(dp2.size(),1),0,bar_t{3});
	if (rrp2) { 
		std::cout << "rrp2 ==\t" << printrp(ts2,*rrp2) << std::endl << std::endl;
	} else {
		std::cout << "rrp2 timeout!" << std::endl << std::endl;
	}
	
	auto rrp3 = rand_rp(ts3,dp1,std::vector<double>(dp1.size(),1),0,bar_t{15});
	if (rrp3) { 
		std::cout << "rrp3 ==\t" << printrp(ts3,*rrp3) << std::endl;
	} else {
		std::cout << "rrp3 timeout!" << std::endl;
	}
	*/
	/*auto rrp3_30sec = rand_rp(ts3,dp,pd,15,15,{std::chrono::seconds {30}});
	if (rrp3_30sec) { 
		std::cout << "rrp3_30sec ==\t" << printrp(ts3,*rrp3_30sec) << std::endl;
	} else {
		std::cout << "rrp3_30sec timeout!" << std::endl;
	}*/
	
	std::cout << std::endl << std::endl;
	/*
	rp_t myntd1 {rp_t::s};
	rp_t myntd2 { rp_t::sixteenth };
	rp_t myntd3 {rp_t::s};
	bool testbool1 = ((int)myntd1 == 1);
	bool testbool2 = (static_cast<int>(myntd1) == 16);
	bool testbool3 = (myntd1 == myntd2);
	//bool testbool4 = (myntd3 == 8);  // want this to fail
	auto testadd1 = myntd3 + 1;//myntd2;
	//auto testadd2 = myntd3 + 1.1651865;

	std::vector<rp_t> myrp {rp_t::q,rp_t::q,rp_t::e,rp_t::e,rp_t::h};
	auto mynbard = nbeat<frac>(myts3,myrp);
	auto mynbari = nbeat<double>(myts3, myrp);
	auto mycumnbt_d = nbeat_cum<double>(myts3,myrp);
	auto mycumnbt_f = nbeat_cum<frac>(myts3,myrp);
	//std::cout << "An empty scale:" << std::endl;
	//auto empty_scale = scale();
	//std::cout << empty_scale.print() << std::endl;

	auto nbr_d = nbar_cum<double>(myts3,myrp);
	auto nbr_f = nbar_cum<frac>(myts3,myrp);

	//std::vector<std::vector<frq_t>> vvfrq{ { 2, 3 },{ 4 } };
	
	ntl_t myntl1 {"yay"};
	ntl_t myntl2 {"what"};
	std::vector<ntl_t> vnl1 { myntl1, myntl2 };
	std::vector<ntl_t> vnl2 {"yay"_ntl, "whatever"_ntl};
	std::vector<std::vector<ntl_t>> vvnl {vnl1, {myntl1}, vnl2 };
	oct_t myoct {23};
	
	ntstr_t new_ntstr_from_string_a("C(5)");
	ntstr_t new_ntstr_from_string_b("F#(-2)");
	ntstr_t new_ntstr_from_string_c("D&(-0)");
	ntstr_t new_ntstr_from_string_d("yay#no&(0)");
	//ntstr_t new_ntstr_from_string_e {std::string("Z#")};
	*/
	/*
	std::cout << "Construction of the \"C Chromatic\" scale" << std::endl;
	auto cchrom_name = std::string("C Chromatic");
	auto cchrom_dsc = std::string("The \"chromatic scale\" beginning on C");
	auto cchrom_ntls = std::vector<std::vector<ntl_t>>\
		{{ntl_t("B#"),ntl_t("C")}, {ntl_t("C#"),ntl_t("D&")},\
		{ntl_t("D")},{ntl_t("D#"),ntl_t("E&")},{ntl_t("E"),ntl_t("F&")},\
		{ntl_t("E#"),ntl_t("F")},{ntl_t("F#"),ntl_t("G&")},\
		{ntl_t("G")},{ntl_t("G#"),ntl_t("A&")},{ntl_t("A")},\
		{ntl_t("A#"),ntl_t("B&")},{ntl_t("B"),ntl_t("C&")}};
	std::vector<int> dn_vec {-57,-56,-55,-54,-53,-52,-51,-50,-49,-48,-47,-46};
	auto cchrom_frqs = frq_eqt(dn_vec,frq_t{440.0},12,2);
	auto cchrom = scale(cchrom_name, cchrom_dsc, cchrom_ntls, cchrom_frqs);
	std::cout << cchrom.print() << std::endl << std::endl;
	
	std::cout << "Setting a new name and description: " << std::endl;
	cchrom.set_name("C Chromatic yay");
	cchrom.set_description("whatever:\tThe \"chromatic scale\" beginning on C");
	std::cout << cchrom.print() << std::endl << std::endl;

	std::cout << "Removing a note, adding a note: " << std::endl;
	cchrom.delete_npc(10); // 10 => {"A#","B&"} (second last)
	cchrom.insert_npc(0,frq_t{50.0},{ntl_t("H"),ntl_t("H#&")}); // @ the very start
	std::cout << cchrom.print() << std::endl << std::endl;

	std::cout << "Alter scd == 4: " << std::endl;
	auto tf = cchrom.set_npc(4,frq_t{252.0},{ntl_t("D#"),ntl_t("E&"),ntl_t("theYAYnote")}); // 3=> "D#","E&"
	std::cout << cchrom.print() << std::endl << std::endl;
	std::cout << "Done with scale tests" << std::endl;
	std::cout << std::endl;

	std::vector<scd_t> myscds {1,2,3,4,5,6,7,8,1,0,-1,-2,-3,-5,-6-7,-8,234,-354,42,23};
	for (auto cscd : myscds) {
		std::cout << "scd == " << cscd << "\t=>\tntstr == " \
			<< to_ntstr(cchrom,cscd) << std::endl;
	}
	
	auto tfrqa = 4*cchrom.get_frq(3); auto roa = to_rscd(cchrom,tfrqa);
	auto tfrqb = 8 * cchrom.get_frq(0); auto rob = to_rscd(cchrom, tfrqb);
	std::cout << "tfrqa == " << tfrqa << "\t=>\tntstr == " << to_ntstr(cchrom,tfrqa) << std::endl;
	std::cout << "tfrqb == " << tfrqb << "\t=>\tntstr == " << to_ntstr(cchrom, tfrqb) << std::endl;

	auto x_tf = new_ntstr_from_string_a == new_ntstr_from_string_b;
	//std::getchar();
	*/
    return 0;
	
}

