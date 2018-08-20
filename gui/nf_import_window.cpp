#include "nf_import_window.h"
#include "aulib\input\notefile.h"
#include "aulib\util\au_util_all.h"
#include "aulib\types\rp_t.h"
#include "aulib\types\beat_bar_t.h" // for validate_bpm_str()
#include "data_pool.h"
#include "g_data_pool.h"
#include <string>
#include <vector>
#include <algorithm>
#include <QtWidgets/QFileDialog>

nf_import_window::nf_import_window(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QFileDialog fd(this);
	fd.setDirectory(QString().fromStdString(m_defaults.init_dir));
	auto fname = fd.getOpenFileName().toStdString();
	m_fname = fname;
	set_nf();  // Opens and reads in the note-file into m_nf
	set_nftable(); // Reads m_nf

	ui.curr_fname->setText(QString().fromStdString(m_fname));
	ui.ts->setText(QString().fromStdString(m_defaults.ts));
	set_ts();
	ui.bpm->setText(QString().fromStdString(m_defaults.bpm));
	set_bpm();
	ui.err->setText(QString().fromStdString(m_defaults.err));
	set_err();

	update_nv_t_count();
}

// Reads the notefile and populates m_nf.  From m_nf, populates
// m_nf_table, a vector of structs of QTableWidgetItems.  The table
// widget obtains its data by reference to this vector.
// This function should only be called with the notefile is being read
// or re-read, _not_ to update/sync the table widget and m_nf_table.  
void nf_import_window::set_nf() {
	m_nf = read_notefile(m_fname, notefileopts::seconds);
	m_status.nf = !m_nf.file_error;
	if (!m_status.nf) {return;}

	m_nf_table.reserve(m_nf.lines.size());
	for (int i=0; i<m_nf.lines.size(); ++i) {
		qt_nf_table_data curr_data;
		curr_data.ontime.setData(Qt::EditRole,m_nf.lines[i].ontime);
		curr_data.offtime.setData(Qt::EditRole,m_nf.lines[i].offtime);
		curr_data.pitch.setData(Qt::EditRole,m_nf.lines[i].pitch);
		curr_data.dt.setData(Qt::DisplayRole,m_nf.lines[i].dt);
		m_nf_table.push_back(curr_data);
	}
}

// Populates ui->nf_data with pointers to m_nf_table.  
// m_nf_table is not a widget, it's a std::vector<qt_nf_table_data>.  
void nf_import_window::set_nftable() {
	// This should really be a check on m_nf_table
	/*if (!m_status.nf) {
		m_status.nf_table = false;
		return;
	}*/

	for (int i=0; i<m_nf_table.size(); ++i) {
		ui.nf_data->setItem(i,0,&(m_nf_table[i].ontime));
		ui.nf_data->setItem(i,1,&(m_nf_table[i].offtime));
		ui.nf_data->setItem(i,2,&(m_nf_table[i].pitch));
		ui.nf_data->setItem(i,3,&(m_nf_table[i].dt));

		auto curr_dt = m_nf_table[i].dt.data(1).toDouble();
		if (curr_dt <= 0.0) {
			m_status.nf_table = false;
			ui.nf_data->item(i,3)->setBackground(QBrush(QColor(255,153,153)));
		}
	}
}

void nf_import_window::update_nv_t_count() {
	bool window_is_valid = m_ts_uih.is_valid() && m_bpm_uih.is_valid() &&
		m_err_uih.is_valid() && m_status.nf_table && m_status.nf;
	if (!window_is_valid) {
		return;
	}

	auto bpm = m_bpm_uih.get();
	auto err = m_err_uih.get();
	auto ts = m_ts_uih.get();

	auto nvs = deltat2rp(notefile2dt(m_nf),ts,bpm,err);
	auto uq_nvs = unique_n(nvs);

	ui.nv_t_counts->clear();
	/*for (int i=0; i<uq_nvs.values.size(); ++i) {
		std::string cline {};
		cline += uq_nvs.values[i].print();
		cline += " : ";
		cline += std::to_string(uq_nvs.counts[i]);
		
		ui.nv_t_counts->appendPlainText(QString().fromStdString(cline));
	}*/
}

void nf_import_window::on_ts_textEdited() {
	set_ts();
	update_nv_t_count();
}

void nf_import_window::on_ts_returnPressed() {
	set_ts();
	update_nv_t_count();
}

void nf_import_window::set_ts() {
	m_ts_uih.update(ui.ts->text().toStdString());
	ui.ts->setToolTip(QString::fromStdString(m_ts_uih.msg()));
	if (!m_ts_uih.is_valid()) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}

void nf_import_window::on_bpm_returnPressed() {
	set_bpm();
	update_nv_t_count();
}

void nf_import_window::on_bpm_textEdited() {
	set_bpm();
	update_nv_t_count();
}

void nf_import_window::set_bpm() {
	m_bpm_uih.update(ui.bpm->text().toStdString());
	ui.bpm->setToolTip(QString::fromStdString(m_bpm_uih.msg()));
	
	if (!m_bpm_uih.is_valid()) {
		ui.bpm->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.bpm->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}

void nf_import_window::on_err_returnPressed() {
	set_err();
	update_nv_t_count();
}
void nf_import_window::on_err_textEdited() {
	set_err();
	update_nv_t_count();
}

void nf_import_window::on_nf_data_cellChanged(int r, int c) {
	auto cell_data = ui.nf_data->item(r,c)->data(Qt::EditRole).toDouble();
	switch (c) {  // ontime, offtime, pitch, dt
	case 0:
		m_nf_table[r].ontime.setData(Qt::EditRole,x); break;
	case 1:
		m_nf_table[r].offtime.setData(Qt::EditRole,x); break;
	case 2:
		m_nf_table[r].pitch.setData(Qt::EditRole,x); break;
	case 3:
		m_nf_table[r].dt.setData(Qt::EditRole,x); break;
	}
}

void nf_import_window::set_err() {
	m_err_uih.update(ui.err->text().toStdString());
	ui.err->setToolTip(QString::fromStdString(m_err_uih.msg()));
	
	if (!m_err_uih.is_valid()) {
		ui.err->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.err->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}

void nf_import_window::on_cancel_clicked() {
	this->close();
}

void nf_import_window::on_import_clicked() {
	gdp.create(m_nf,m_fname);
	this->close();
}

