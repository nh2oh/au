#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_rp_builder.h"
#include "aulib\types\rp_t.h"
#include "aulib\types\nv_t.h"
#include "aulib\nv_t_uih.h"
#include "aulib\types\beat_bar_t.h"
#include "aulib\numeric_simple_uih.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"  // NB:  ts_t.h not needed
#include <vector>
#include <optional> // rand_rp() returns a std::optional<std::vector<nv_t>>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <QStringListModel>
#include <qstandarditemmodel.h>

class rp_builder : public QMainWindow {
	Q_OBJECT
public:
	rp_builder(QWidget *parent = Q_NULLPTR);
private:
	struct defaults {
		std::string ts {"4/4"};
		std::string nv {""};
		std::string nnts {"0"};
		std::string nbars {"2"};

		std::set<d_t> common_nvs {d::dw, d::w, d::h, d::hd, d::hdd, 
			d::q, d::qd, d::qdd, d::e, d::ed, d::edd, 
			d::sx, d::sxd, d::sxdd};

		std::map<d_t,double> nv_pool {{d::sx, 0.25}, {d::e,0.25}, 
			{d::q,0.25}, {d::h,0.25}};
	};
	defaults m_defaults {};

	au::uih_parser<parse_userinput_ts> m_ts_parser {parse_userinput_ts {},
		"The time signature (format: n/d[c] where c means \"compound\")"};
	au::uih<decltype(m_ts_parser)> m_ts_uih {m_ts_parser};

	au::uih_parser<parse_userinput_nv> m_nv_parser {parse_userinput_nv {},
		"The note-value format:  1/n[.] where n is a positive integer and [.] is 1 or more dots."};
	au::uih<decltype(m_nv_parser)> m_nv_uih {m_nv_parser};
	au::uih<decltype(m_nv_parser)> m_nv_uih2 {m_nv_parser};

	au::uih_parser<parse_randrp_input> m_rand_rp_input_parser {parse_randrp_input {},
		"rand_rp(ts_t, std::vector<nv_t>, std::vector<double>, int, bar_t, rand_rp_opts)"};
	au::uih<decltype(m_rand_rp_input_parser)> m_rand_rp_uih {m_rand_rp_input_parser};

	au::uih_parser<parse_userinput_int> m_nnts_parser {parse_userinput_int {},
		"Constrain the generated sequence to contain this many notes"};
	au::uih_pred<ftr_geq> p_geqzero {ftr_geq{0.0},"Must be >= 0"};
	au::uih<decltype(m_nnts_parser),decltype(p_geqzero)> m_nnts_uih {m_nnts_parser,p_geqzero};

	au::uih_parser<parse_userinput_double> m_nbars_parser {parse_userinput_double {},
		"Number of bars to generate"};
	au::uih<decltype(m_nbars_parser),decltype(p_geqzero)> m_nbars_uih {m_nbars_parser,p_geqzero};

	QStandardItemModel m_nvpool_model {};
	std::map<d_t,double> m_nvpool {};

	QStringListModel m_comm_nvs_model {};
	std::set<d_t> m_common_nvs;

	//std::optional<std::vector<nv_t>> m_rand_rp_result;  // output of rand_rp()
	std::optional<rp_t> m_rand_rp_result;

	void set_ts();
	void set_nv();
	void set_nvpool();
	void set_common_nvs();
	void set_rand_rp_inputs();
	void set_nnts();
	void set_nbars();

	Ui::rp_builder ui;
private slots:

	void on_ts_returnPressed();
	void on_ts_textEdited();
	void on_nnts_returnPressed();
	void on_nnts_textEdited();
	void on_nbars_returnPressed();
	void on_nbars_textEdited();
	void on_nv_returnPressed();
	void on_nv_textEdited();
	void on_add_nv_clicked();
	void on_remove_nv_clicked();
	void on_generate_clicked();
	void on_import_btn_clicked();
	void on_cancel_clicked();
};

