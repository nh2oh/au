#include "metg_builder.h"
#include "g_data_pool.h"
#include "aulib\types\ts_t.h"
#include "aulib\util\au_util.h" // bsprintf() for printing m_pd elements
#include <vector>
#include <algorithm>  // max_element(m_metg.nbars())
#include <qwidget.h>
#include <qscrollbar.h>
#include <qstandarditemmodel.h>

metg_builder::metg_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	ui.ts->setText(QString().fromStdString(m_defaults.ts));
	set_ts();

	m_metg = tmetg_t {m_ts_uih.get(),m_defaults.nvph};

	ui.nvph_display->setModel(&m_nvph_display_model);
	set_nvph_display();
		// Updates m_nvph_display_model (=> also the nvph_display widget)
		// w/ m_metg.levels()

	ui.pg_display->setModel(&m_pg_display_model);
	set_pg_display();
}


// Updates the widget w/ the value of m_metg.  
void metg_builder::set_pg_display() {
	m_pg_display_model.clear();
	auto nbars_mg = m_metg.nbars();
	auto max_nbars = *std::max_element(nbars_mg.begin(),nbars_mg.end());
	beat_t max_nbeats = nbeat(m_metg.ts(),max_nbars);
	
	auto nvphs = m_metg.levels();
	for (int r=0; r<nvphs.size(); ++r) {
		auto curr_nvph_str = nvphs[r].nv.print() + " + " + nvphs[r].ph.print();
		QStandardItem *curr_nvph_modelitem = 
			new QStandardItem(QString::fromStdString(curr_nvph_str));

		m_pg_display_model.setItem(r,0,curr_nvph_modelitem);
	}

	int max_colnum = max_nbeats/m_metg.btres();
	for (int c=0; c<max_colnum; ++c) {
		beat_t curr_bt = c*m_metg.btres();
		for (int r=0; r<m_metg.levels().size(); ++r) {
			auto curr_prob = m_metg.onset_prob_at(m_metg.levels()[r],curr_bt);;
			QStandardItem *curr_prob_modelitem = 
				new QStandardItem(QString::fromStdString(bsprintf("%.3f",curr_prob)));
			
			m_pg_display_model.setItem(r,(c+1),curr_prob_modelitem);  // NOTE !! c+1 !!
		}
	}
}


// Updates the widget w/ the elements of m_metg.levels().  
void metg_builder::set_nvph_display() { 
	m_nvph_display_model.clear();
	auto nvphs = m_metg.levels();

	for (int r=0; r<nvphs.size(); ++r) {
		auto curr_nv_str = nvphs[r].nv.print();
		QStandardItem *curr_nv_modelitem = 
			new QStandardItem(QString::fromStdString(curr_nv_str));
		auto curr_ph_str = nvphs[r].ph.print();
		QStandardItem *curr_ph_modelitem = 
			new QStandardItem(QString::fromStdString(curr_ph_str));

		m_nvph_display_model.setItem(r,0,curr_nv_modelitem);
		m_nvph_display_model.setItem(r,1,curr_ph_modelitem);
	}

	// Column resizing.  Note the emperical fudge-factor of 4; see also rp_builder.cpp
	auto vsb = ui.nvph_display->verticalScrollBar();
	auto vh = ui.nvph_display->verticalHeader();
	double w = (ui.nvph_display->width()-(*vsb).width()-(*vh).width()-4);
	ui.nvph_display->setColumnWidth(0,w/2.0);
	ui.nvph_display->setColumnWidth(1,w/2.0);
}


void metg_builder::set_ts() {
	m_ts_uih.update(ui.ts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_ts_uih.msg()));
	if (!m_ts_uih.is_valid()) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}


void metg_builder::on_set_levels_btn_clicked() {
	//...
}
void metg_builder::on_set_ts_btn_clicked() {
	set_ts();
}
void metg_builder::on_normalize_btn_clicked() {
	//...
}
void metg_builder::on_import_btn_clicked() {
	this->close();
}
void metg_builder::on_cancel_btn_clicked() {
	this->close();
}


