#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\util\au_util.h" // bsprintf() for printing m_pd elements
#include "aulib\types\rp_t.h"  // printrp() for printing the output of rand_rp()
#include <string>
#include <QStringListModel>
#include <qstandarditemmodel.h>
#include <utility>
#include <map>
#include <set>
#include <vector>
#include <qwidget.h>
#include <qscrollbar.h>

rp_builder::rp_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	ui.ts->setText(QString().fromStdString(m_defaults.ts));
	set_ts();
	ui.nv->setText(QString().fromStdString(m_defaults.nv));
	set_nv();
	ui.nnts->setText(QString().fromStdString(m_defaults.nnts));
	set_nnts();
	ui.nbars->setText(QString().fromStdString(m_defaults.nbars));
	set_nbars();

	m_nvpool = m_defaults.nv_pool;
	ui.nvpool->setModel(&m_nvpool_model);
	set_nvpool();  // Syncs m_nvpool and the model m_nvpool_model

	m_common_nvs = m_defaults.common_nvs;
	ui.nvpool_addnl->setModel(&m_comm_nvs_model);
	set_common_nvs();  // Same deal as set_nvpool()

	set_rand_rp_inputs();
}

void rp_builder::on_generate_clicked() {
	set_rand_rp_inputs();
	if (!m_rand_rp_uih.is_valid()) {return;}
	m_rand_rp_result = rand_rp(m_rand_rp_uih.get());

	if (!m_rand_rp_result) {
		return;
	}

	//ui.rp_result->setPlainText(QString::fromStdString(printrp(m_ts_uih.get(),*m_rand_rp_result)));
	//auto x = *m_rand_rp_result;
	ui.rp_result->setPlainText(QString::fromStdString((*m_rand_rp_result).print()));
}

void rp_builder::set_rand_rp_inputs() {
	if (!m_ts_uih.is_valid() || !m_nnts_uih.is_valid() || 
		!m_nbars_uih.is_valid()) {
		return;
	}

	// TODO:  It's silly to pull these into temp vars... fix me
	std::set<d_t> nvs {};
	std::vector<double> probs {};
	for (auto& e : m_nvpool) {
		nvs.insert(e.first);
		probs.push_back(e.second);
	}

	m_rand_rp_uih.update(randrp_input{m_ts_uih.get(),nvs,probs,m_nnts_uih.get(),
		bar_t{m_nbars_uih.get()}});
	ui.rand_rp_status_msg->setText(QString::fromStdString(m_rand_rp_uih.msg()));
}

void rp_builder::set_ts() {
	m_ts_uih.update(ui.ts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_ts_uih.msg()));
	if (!m_ts_uih.is_valid()) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_ts_returnPressed() {
	set_ts();
	set_rand_rp_inputs();
}
void rp_builder::on_ts_textEdited() {
	set_ts();
	set_rand_rp_inputs();
}

void rp_builder::set_nnts() {
	m_nnts_uih.update(ui.nnts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_nnts_uih.msg()));
	if (!m_nnts_uih.is_valid()) {
		ui.nnts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nnts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nnts_returnPressed() {
	set_nnts();
	set_rand_rp_inputs();
}
void rp_builder::on_nnts_textEdited() {
	set_nnts();
	set_rand_rp_inputs();
}

void rp_builder::set_nbars() {
	m_nbars_uih.update(ui.nbars->text().toStdString());
	ui.nbars->setToolTip(QString::fromStdString(m_nbars_uih.msg()));
	if (!m_nbars_uih.is_valid()) {
		ui.nbars->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nbars->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nbars_returnPressed() {
	set_nbars();
	set_rand_rp_inputs();
}
void rp_builder::on_nbars_textEdited() {
	set_nbars();
	set_rand_rp_inputs();
}

void rp_builder::set_nv() {
	m_nv_uih.update(ui.nv->text().toStdString());
	ui.rp_result->setPlainText(ui.nv->text());

	ui.rp_result->appendPlainText(ui.nv->text());
	if (!m_nv_uih.is_valid() && !ui.nv->text().isEmpty()) {
		ui.nv->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.nv->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}
void rp_builder::on_nv_returnPressed() {
	set_nv();
	set_rand_rp_inputs();
}
void rp_builder::on_nv_textEdited() {
	set_nv();
	set_rand_rp_inputs();
}
void rp_builder::on_add_nv_clicked() {
	// Selected elements are added to m_nv_pool and removed from m_common_nvs
	// if applicable
	
	auto idxs_nvpa_smodel = ui.nvpool_addnl->selectionModel()->selectedIndexes();
		// "indices of the nvpool_addnl selection model"
		// ...refer to the model assigned to the QListView widget by the call to 
		// the widget's setModel() (i do this in the window constructor),
		// here, m_comm_nvs_model.  The set_common_nvs() function assigns 
		// data to the m_comm_nvs_model.  
		// A possibly safer way to work this is to do:
		// auto widget_model = ui.nvpool_addnl->model();
		// Then use widget_model instead of manually specifying m_comm_nvs_model.  
	for (auto e : idxs_nvpa_smodel) {
		auto curr_nvstr = m_comm_nvs_model.data(e,Qt::DisplayRole).toString().toStdString();
		m_nv_uih2.update(curr_nvstr);
		if (m_nv_uih2.is_valid()) { // Should always be valid...
			m_nvpool.insert(std::make_pair(m_nv_uih2.get(),1.0));
			m_common_nvs.erase(m_nv_uih2.get());
		}
	}

	m_nv_uih.update(ui.nv->text().toStdString());
	if (m_nv_uih.is_valid()) { 
		m_nvpool.insert(std::make_pair(m_nv_uih.get(),1.0));
	}

	set_nvpool();
	set_common_nvs();
	set_rand_rp_inputs();
}

void rp_builder::on_remove_nv_clicked() {
	// Items removed from the nvpool widget get added to the "common_nvs"
	// widget (added to m_common_nvs, removed from m_nvpool, followed by 
	// calls to set_nvpool() and set_common_nvs() to update the widgets
	// and other back-end data).  
	auto idxs = ui.nvpool->selectionModel()->selectedIndexes();
	for (auto e : idxs) {
		auto curr_nvstr = m_nvpool_model.data(e,Qt::DisplayRole).toString().toStdString();
		m_nv_uih2.update(curr_nvstr);
		if (m_nv_uih2.is_valid()) {  // Should always be valid...
			m_nvpool.erase(m_nv_uih2.get());
			m_common_nvs.insert(m_nv_uih2.get());
		}
	}

	set_nvpool();
	set_common_nvs();
	set_rand_rp_inputs();
}

void rp_builder::set_nvpool() {
	// Updates the widget w/ the elements of m_nvpool (a std::map<nv_t,double>).  
	m_nvpool_model.clear();
	int r=0;
	for (auto e : m_nvpool) {
		QStandardItem *curr_nv = new QStandardItem(QString::fromStdString(e.first.print()));
		QStandardItem *curr_prob = new QStandardItem(QString::fromStdString(dbk::bsprintf("%.3f",e.second)));
		m_nvpool_model.setItem(r,0,curr_nv);
		m_nvpool_model.setItem(r,1,curr_prob);
		++r;
	}

	// Column resizing.  Note the emperical fudge-factor of 4
	auto vsb = ui.nvpool->verticalScrollBar();
	auto vh = ui.nvpool->verticalHeader();
	double w = (ui.nvpool->width()-(*vsb).width()-(*vh).width()-4);
	ui.nvpool->setColumnWidth(0,w/2.0);
	ui.nvpool->setColumnWidth(1,w/2.0);
}
void rp_builder::set_common_nvs() {
	// Updates the widget w/ the elements of m_common_nvs (a std::set<nv_t>).  
	QStringList nvs {};
	for (auto e : m_common_nvs) {
		nvs.push_back(QString::fromStdString(e.print()));
	}
	m_comm_nvs_model.setStringList(nvs);
}

void rp_builder::on_import_btn_clicked() {
	if (!m_rand_rp_result) {
		// TODO:  Error message box saying can't import "nothing"
		return;
	}

	//gdp.create(*m_rand_rp_result,std::string{"what"});
	this->close();
}

void rp_builder::on_cancel_clicked() {
	this->close();
}

