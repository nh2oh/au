#include "rp_builder.h"
#include "g_data_pool.h"
#include "aulib\rpgen\rand_rp.h"
#include "aulib\types\ts_t.h"
#include "aulib\types\nv_t.h"
#include "aulib\util\au_util.h" // bsprintf() for printing m_pd elements
#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\nv_t_uih.h"
#include "aulib\numeric_simple_uih.h"
#include <vector>
#include <string>
#include <QStringListModel>

rp_builder::rp_builder(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	m_pd = m_defaults.pd;  // no setter or ui component yet
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
	set_nvpool();
		// moves the m_nvpool elements into m_nvpool_qsl_items and therefore into
		// the gui widget.  

	m_common_nvs = m_defaults.common_nvs;
	ui.nvpool_addnl->setModel(&m_comm_nvs_model);
	set_common_nvs();  // Same deal as set_nvpool()

	m_pd = m_defaults.pd;
	ui.nvprobs->setModel(&m_nvprobs_model);
	set_pd();  // Same deal as set_nvpool()

	set_rand_rp_inputs();
}

void rp_builder::on_generate_clicked() {
	set_rand_rp_inputs();
	if (!m_rand_rp_input_status.is_valid) { return; }

	//m_rand_rp_result = rand_rp(m_rand_rp_input);
	// rand_rp() returns a std::optional<std::vector<nv_t>>

}

void rp_builder::set_rand_rp_inputs() {
	if (m_ts_uih.is_valid() && m_nnts_uih.is_valid() && m_nbars_uih.is_valid()) {
		m_rand_rp_input = {m_ts_uih.get(),m_nvpool,m_pd,m_nnts_uih.get(),
			bar_t{m_nbars_uih.get()}};
		m_rand_rp_input_status = rand_rp_check_input(m_rand_rp_input);
	}

	if (!m_rand_rp_input_status.is_valid) {
		ui.rand_rp_status_msg->setText(QString::fromStdString(m_rand_rp_input_status.msg));
	}
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
	// Selected elements are added to m_nv_pool
	auto idxs = ui.nvpool_addnl->selectionModel()->selectedIndexes();
	for (auto e : idxs) {
		auto curr_nvstr = m_comm_nvs_qsl_items.at(e.row()).toStdString();
		m_nv_uih2.update(curr_nvstr);
		if (m_nv_uih2.is_valid()) { // Should always be valid...
			m_nvpool.insert(m_nv_uih2.get());
		}
	}

	m_nv_uih.update(ui.nv->text().toStdString());
	if (m_nv_uih.is_valid()) { 
		m_nvpool.insert(m_nv_uih.get());
	}

	// Should remove elements from common_nvs??

	set_nvpool();
	set_rand_rp_inputs();
}

void rp_builder::on_remove_nv_clicked() {
	// Items removed from the nvpool widget get added to the "common_nvs"
	// widget (added to m_common_nvs, removed from m_nvpool, followed by 
	// calls to set_nvpool() and set_common_nvs() to update the widgets
	// and other backend data).  
	auto idxs = ui.nvpool->selectionModel()->selectedIndexes();
	for (auto e : idxs) {
		auto curr_nvstr = m_nvpool_qsl_items.at(e.row()).toStdString();
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
	// Updates the widget w/ the elements of m_nvpool (a std::set<nv_t>).  
	m_nvpool_qsl_items.clear();
	for (auto e : m_nvpool) {
		m_nvpool_qsl_items.push_back(QString::fromStdString(e.print()));
	}
	//m_nvpool_model.setStringList(m_nvpool_qsl_items);  // Needed more than once??
}
void rp_builder::set_common_nvs() {
	// Updates the widget w/ the elements of m_common_nvs (a std::set<nv_t>).  
	m_comm_nvs_qsl_items.clear();
	for (auto e : m_common_nvs) {
		m_comm_nvs_qsl_items.push_back(QString::fromStdString(e.print()));
	}
	//m_comm_nvs_model.setStringList(m_comm_nvs_qsl_items);  // Needed more than once??
}
void rp_builder::set_pd() {
	// Updates the widget w/ the elements of m_nvprobs (a std::set<nv_t>).  
	m_nvprobs_qsl_items.clear();
	for (auto e : m_pd) {
		m_nvprobs_qsl_items.push_back(QString::fromStdString(bsprintf("%.3f",e)));
	}
	//m_nvprobs_model.setStringList(m_nvprobs_qsl_items);  // Needed more than once??
}
void rp_builder::on_import_btn_clicked() {
	if (!m_rand_rp_result) {
		// Error message box saying can't import "nothing"
		return;
	}

	gdp.create(*m_rand_rp_result,std::string{"yay"});
	this->close();
}

void rp_builder::on_cancel_clicked() {
	this->close();
}

