#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_metg_builder.h"
#include "aulib\types\nv_t.h"
#include "aulib\types\beat_bar_t.h"
#include <vector>
#include <set>


class metg_builder : public QMainWindow {
	Q_OBJECT
public:
	metg_builder(QWidget *parent = Q_NULLPTR);
private:
	struct defaults {
		std::string ts {"4/4"};
		std::string nv {""};

		std::set<d_t> common_nvs {d::dw, d::w, d::h, d::hd, d::hdd, 
			d::q, d::qd, d::qdd, d::e, d::ed, d::edd, 
			d::sx, d::sxd, d::sxdd};
	};
	defaults m_defaults {};

	Ui::metg_builder ui;
private slots:

};

