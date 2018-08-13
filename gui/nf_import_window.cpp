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
	fd.setDirectory(QString().fromStdString(defaults_.init_dir));
	auto fname = fd.getOpenFileName().toStdString();
	m_fname = fname;
	set_nf();
	set_nftable();

	ui.curr_fname->setText(QString().fromStdString(m_fname));
	ui.ts->setText(QString().fromStdString(defaults_.ts));
	set_ts();
	ui.bpm->setText(QString().fromStdString(defaults_.bpm));
	set_bpm();
	ui.err->setText(QString().fromStdString(defaults_.err));
	set_err();

	update_nv_t_count();
}

void nf_import_window::set_nf() {
	m_nf = read_notefile(m_fname, notefileopts::seconds);
	m_dt = notefile2dt(m_nf);
	status.nf = !m_nf.file_error;
}

// Computes m_nf_table from m_nf and populates ui->nf_data with pointers to
// m_nf_table
void nf_import_window::set_nftable() {
	if (!status.nf) {
		status.nf_table = false;
		return;
	}

	bool negative_dt = false;
	m_nf_table.reserve(m_nf.lines.size());
	for (int i=0; i<m_nf.lines.size(); ++i) {
		qt_nf_table_data curr_data;
		curr_data.ontime.setData(Qt::EditRole,m_nf.lines[i].ontime);
		curr_data.offtime.setData(Qt::EditRole,m_nf.lines[i].offtime);
		curr_data.pitch.setData(Qt::EditRole,m_nf.lines[i].pitch);
		curr_data.dt.setData(Qt::DisplayRole,m_nf.lines[i].dt);
		m_nf_table.push_back(curr_data);

		ui.nf_data->setItem(i,0,&(m_nf_table.back().ontime));
		ui.nf_data->setItem(i,1,&(m_nf_table.back().offtime));
		ui.nf_data->setItem(i,2,&(m_nf_table.back().pitch));
		ui.nf_data->setItem(i,3,&(m_nf_table.back().dt));

		if (m_nf.lines[i].dt <= 0) {
			negative_dt = true;
			ui.nf_data->item(i,3)->setBackground(QBrush(QColor(255,153,153)));
		}
	}
	status.nf_table = !negative_dt;
}

void nf_import_window::update_nv_t_count() {
	//bool window_is_valid = status.bpm && status.nf_table && status.err 
	//	&& status.nf && status.ts;
	bool window_is_valid = status.bpm && status.nf_table && status.err 
		&& status.nf && ts_.is_valid();
	if (!window_is_valid) {
		return;
	}

	auto nvs = deltat2rp(m_dt,ts_.get(),m_bpm,m_err);
	auto uq_nvs = unique_n(nvs);

	ui.nv_t_counts->clear();
	for (int i=0; i<uq_nvs.values.size(); ++i) {
		std::string cline {};
		cline += uq_nvs.values[i].print();
		cline += " : ";
		cline += std::to_string(uq_nvs.counts[i]);
		
		ui.nv_t_counts->appendPlainText(QString().fromStdString(cline));
	}
}

void nf_import_window::on_ts_textEdited() {
	set_ts();
	if (ts_.is_valid()) { update_nv_t_count();	}
}

void nf_import_window::on_ts_returnPressed() {
	set_ts();
	if (ts_.is_valid()) { update_nv_t_count();	}
}

void nf_import_window::set_ts() {
	ts_.update(ui.ts->text().toStdString());
	if (!ts_.is_valid()) {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		ui.ts->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
	}
}

void nf_import_window::on_bpm_returnPressed() {
	set_bpm();
	if (status.bpm) { update_nv_t_count(); }
}

void nf_import_window::on_bpm_textEdited() {
	set_bpm();
	if (status.bpm) { update_nv_t_count(); }
}

void nf_import_window::set_bpm() {
	auto usrinput_bpm = validate_bpm_str(ui.bpm->text().toStdString());
	if (!usrinput_bpm.is_valid || usrinput_bpm.bpm <= 0) {
		status.bpm = false;
		ui.bpm->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		status.bpm = true;
		ui.bpm->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
		m_bpm = usrinput_bpm.bpm;
	}
}

void nf_import_window::on_err_returnPressed() {
	set_err();
	if (status.err) { update_nv_t_count(); }
}

void nf_import_window::set_err() {
	auto usrinput_err = ui.err->text().toStdString();
	if (std::stod(usrinput_err) < 0) {
		status.err = false;
		ui.err->setStyleSheet("QLineEdit { background: rgb(255,153,153); }");
	} else {
		status.err = true;
		ui.err->setStyleSheet("QLineEdit { background: rgb(255,255,255); }");
		m_err = std::stod(usrinput_err);
	}
}

void nf_import_window::on_cancel_clicked() {
	this->close();
}

void nf_import_window::on_import_clicked() {
	gdp.create(m_nf,m_fname);
	this->close();
}

