#include "nf_import_window.h"
#include "aulib\input\notefile.h"
#include "aulib\util\au_util_all.h"
#include "aulib\types\rp_t.h"
#include "data_pool.h"
#include "g_data_pool.h"
#include <string>
#include <vector>
#include <algorithm>
#include <QFileDialog>

nf_import_window::nf_import_window(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	auto fname = QFileDialog::getOpenFileName(this);

	m_fname = fname.toStdString();
	auto x = m_ts.print();
	ui.ts->setText(QString().fromStdString(m_ts.print()));
	ui.bpm->setText(QString().setNum(m_bpm,'f',2));
	ui.err->setText(QString().setNum(m_err,'f',3));
	
	m_nf = read_notefile(m_fname, notefileopts::seconds);
	m_dt = notefile2dt(m_nf);
	load_nftable();
	update_note_value_count();
}

// Computes m_nf_table from m_nf and populates ui->nf_data with pointers to
// m_nf_table
void nf_import_window::load_nftable() {
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
	}
}

void nf_import_window::update_note_value_count() {
	auto nvs = deltat2rp(m_dt,m_ts,m_bpm,m_err);
	auto uq_nvs = unique_n(nvs);

	ui.note_value_counts->clear();
	for (int i=0; i<uq_nvs.values.size(); ++i) {
		std::string cline {};
		cline += uq_nvs.values[i].print(notevalueopts::printapproxifnotexact);
		cline += " : ";
		cline += std::to_string(uq_nvs.counts[i]);
		
		ui.note_value_counts->appendPlainText(QString().fromStdString(cline));
	}
}

void nf_import_window::on_ts_returnPressed() {
	auto userinputts = ui.ts->text().toStdString();
	auto new_ts = ts_t {userinputts};
	if (new_ts != m_ts) {
		m_ts = new_ts;
		update_note_value_count();
	}
}

void nf_import_window::on_bpm_returnPressed() {
	auto userinputbpm = ui.bpm->text().toStdString();
	double new_bpm = std::stod(userinputbpm);
	if (new_bpm != m_bpm) {
		m_bpm = new_bpm;
		update_note_value_count();
	}
}

void nf_import_window::on_err_returnPressed() {
	auto userinputbpm = ui.err->text().toStdString();
	double new_err = std::stod(userinputbpm);
	if (new_err != m_err) {
		m_err = new_err;
		update_note_value_count();
	}
}

void nf_import_window::on_cancel_clicked() {
	this->close();
}

void nf_import_window::on_import_clicked() {
	gdp.create(&m_nf,m_fname);
	this->close();
}

