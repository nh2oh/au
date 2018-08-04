#include "nf_import_window.h"
#include "aulib\input\notefile.h"
#include "aulib\util\au_util_all.h"
#include "aulib\types\rp_t.h"
#include <string>
#include <vector>
#include <algorithm>

nf_import_window::nf_import_window(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	m_fname = "C:\\Users\\ben\\Desktop\\cpp\\au\\stuff\\deut1334.notes";
	auto x = m_ts.print();
	ui.ts->setText(QString().fromStdString(m_ts.print()));
	ui.bpm->setText(QString().setNum(m_bpm,'f',2));
	ui.err->setText(QString().setNum(m_err,'f',3));
	
	m_nf = read_notefile(m_fname);
	m_deltat = notefile2dt(m_nf,notefileopts::seconds);
	load_nf();
	update_note_value_count();
}

void nf_import_window::load_nf() {
	qt_nf_data.reserve(m_nf.size());
	for (int i=0; i<m_nf.size(); ++i) {
		qt_nf_table_data curr_data;
		curr_data.ontime.setData(Qt::EditRole,m_nf[i].ontime);
		curr_data.offtime.setData(Qt::EditRole,m_nf[i].offtime);
		curr_data.pitch.setData(Qt::EditRole,m_nf[i].pitch);
		curr_data.tdelta.setData(Qt::DisplayRole,m_deltat[i]);
		qt_nf_data.push_back(curr_data);

		ui.nf_data->setItem(i,0,&(qt_nf_data.back().ontime));
		ui.nf_data->setItem(i,1,&(qt_nf_data.back().offtime));
		ui.nf_data->setItem(i,2,&(qt_nf_data.back().pitch));

		ui.nf_data->setItem(i,3,&(qt_nf_data.back().tdelta));
	}
}

void nf_import_window::update_note_value_count() {
	auto nvs = deltat2rp(m_deltat,m_ts,m_bpm,m_err);
	std::vector<note_value> uq_nvs {};
	std::vector<int> uq_nvs_counts {};
	for (int i=0; i<nvs.size(); ++i) {
		int n_uq = std::count(uq_nvs.begin(),uq_nvs.end(),nvs[i]);
		if (n_uq == 0) {
			int n = std::count(nvs.begin(),nvs.end(),nvs[i]);
			uq_nvs.push_back(nvs[i]);
			uq_nvs_counts.push_back(n);
		}
	}

	ui.note_value_counts->clear();
	for (int i=0; i<uq_nvs.size(); ++i) {
		std::string cline {};
		cline += uq_nvs[i].print(notevalueopts::printapproxifnotexact);
		cline += " : ";
		cline += std::to_string(uq_nvs_counts[i]);
		
		ui.note_value_counts->appendPlainText(QString().fromStdString(cline));
	}
}

void nf_import_window::on_ts_returnPressed() {
	auto userinputts = ui.ts->text().toStdString();
	m_ts = ts_t {userinputts};
	update_note_value_count();
}

void nf_import_window::on_bpm_returnPressed() {
	auto userinputbpm = ui.bpm->text().toStdString();
	m_bpm = std::stod(userinputbpm);
	update_note_value_count();
}

void nf_import_window::on_err_returnPressed() {
	auto userinputbpm = ui.err->text().toStdString();
	m_err = std::stod(userinputbpm);
	update_note_value_count();
}

