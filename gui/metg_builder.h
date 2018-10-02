#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_metg_builder.h"
#include "aulib\types\teejee.h"
#include "aulib\types\metg_t.h"
#include "aulib\types\nv_t.h"
#include "aulib\types\beat_bar_t.h"
#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"  // NB:  ts_t.h not needed
#include <vector>
#include <set>
#include <utility>
#include <qstandarditemmodel.h>

class metg_builder : public QMainWindow {
	Q_OBJECT
public:
	metg_builder(QWidget *parent = Q_NULLPTR);
private:

	au::uih_parser<parse_userinput_ts> m_ts_parser {parse_userinput_ts {},
		"The time signature (format: n/d[c] where c means \"compound\")"};
	au::uih<decltype(m_ts_parser)> m_ts_uih {m_ts_parser};

	struct defaults {
		std::string ts {"4/4"};
		std::string nv {""};

		std::set<d_t> common_nvph {d::dw, d::w, d::h, d::hd, d::hdd, 
			d::q, d::qd, d::qdd, d::e, d::ed, d::edd, 
			d::sx, d::sxd, d::sxdd};
		std::vector<teejee::nv_ph> nvph {{d::w,d::z}, {d::h,d::z}, {d::q,d::z}, {d::e,d::z}};
	};
	defaults m_defaults {};

	QStandardItemModel m_nvph_display_model {};
	QStandardItemModel m_pg_display_model {};

	tmetg_t m_metg {};

	Ui::metg_builder ui;

	void set_pg_display();
	void set_nvph_display();
	void set_ts();
private slots:

	void on_normalize_btn_clicked();
	void on_set_levels_btn_clicked();
	void on_set_ts_btn_clicked();
	void on_import_btn_clicked();
	void on_cancel_btn_clicked();

};

