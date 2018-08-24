#include "nf_import_window.h"
#include "aulib\input\notefile.h"
#include "aulib\util\au_algs.h"
#include "aulib\util\au_util.h"
#include "aulib\types\rp_t.h"
#include "g_data_pool.h"
#include "aulib\numeric_simple_uih.h" // for qt_table2double()
#include <string>
#include <vector>
#include <algorithm> // remove() to drop a value from m_nf.error_lines
#include <QtWidgets/QFileDialog>
#include <qmessagebox.h> // Error msg if "import" is clicked w/ m_nf.error_lines !empty


// TODO
// - Uniform method of setting background colors for ui widgets

nf_import_window::nf_import_window(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	QFileDialog fd(this);
	fd.setDirectory(QString::fromStdString(m_defaults.init_dir));
	auto fname = fd.getOpenFileName().toStdString();
	m_fname = fname;
	ui.curr_fname->setText(QString::fromStdString(m_fname));

	set_nf_from_file();  // Opens and reads in the note-file into m_nf and m_nf_table_data
	set_nftable();  // Populates the ui.nf_table widget from m_nf_table_data

	ui.ts->setText(QString::fromStdString(m_defaults.ts));
	set_ts();
	ui.bpm->setText(QString::fromStdString(m_defaults.bpm));
	set_bpm();
	ui.err->setText(QString::fromStdString(m_defaults.err));
	set_err();

	update_nv_t_count();
}

// Reads the notefile into m_nf.  From m_nf, populates
// m_nf_table_data, a vector of structs of QTableWidgetItems.  The table
// widget obtains its data by reference to this vector.
// This function should only be called with the notefile is being read
// or re-read, _not_ to update/sync the table widget and m_nf_table_data.  
//
void nf_import_window::set_nf_from_file() {
	if (m_nf.fname != m_fname) {
		// Detect if the user has changed the filename and reload the entire m_nf
		// struct if so.  
		m_nf = read_notefile(m_fname, notefileopts::seconds);
		//m_status.nf = !m_nf.file_error;
		if (m_nf.file_error) {return;}
		m_nf_table_data.clear();
	}
	
	m_nf_table_data.reserve(m_nf.lines.size());
	for (int i=0; i<m_nf.lines.size(); ++i) {
		qt_nf_table_data curr_data;
		curr_data.ontime.setData(Qt::EditRole,m_nf.lines[i].ontime);
		curr_data.offtime.setData(Qt::EditRole,m_nf.lines[i].offtime);
		curr_data.pitch.setData(Qt::EditRole,m_nf.lines[i].pitch);
		curr_data.dt.setData(Qt::EditRole,m_nf.lines[i].dt);
		m_nf_table_data.push_back(curr_data);
	}
}

// Updates the data in row r in the backend datastores m_nf _and_ 
// m_nf_table_data with the data in row r of the nf_table widget.  
// Reads the table widget.  
void nf_import_window::set_nf_from_nftable(int const r) {
	notefileline nf_line {};
	nf_line.file_line_num = r+1;  // NB: r+1 => _file_ line num, not vector index
	nf_line.ontime = ui.nf_table->item(r,0)->data(Qt::EditRole).toDouble();
	nf_line.offtime = ui.nf_table->item(r,1)->data(Qt::EditRole).toDouble();
	nf_line.pitch = ui.nf_table->item(r,2)->data(Qt::EditRole).toInt();
	nf_line.dt = ui.nf_table->item(r,3)->data(Qt::EditRole).toDouble();

	qt_nf_table_data nftable_rowdata;
	nftable_rowdata.ontime.setData(Qt::EditRole,nf_line.ontime);
	nftable_rowdata.offtime.setData(Qt::EditRole,nf_line.offtime);
	nftable_rowdata.pitch.setData(Qt::EditRole,nf_line.pitch);
	nftable_rowdata.dt.setData(Qt::EditRole,nf_line.dt);

	if (r >= m_nf_table_data.size()) {
		m_nf.lines.push_back(nf_line);
		m_nf_table_data.push_back(nftable_rowdata);
	} else {
		m_nf.lines[r] = nf_line;
		m_nf_table_data[r] = nftable_rowdata;
	}

	// NB:  r+1 b/c m_nf.error_lines are _file_ line numbers, not std::vector
	// indices.  
	if (ismember((r+1),m_nf.error_lines) && (nf_line.dt > 0.0)) {
		m_nf.error_lines.erase(
			std::remove(m_nf.error_lines.begin(),m_nf.error_lines.end(),(r+1)),
			m_nf.error_lines.end());
	} else if (!ismember((r+1),m_nf.error_lines) && (nf_line.dt <= 0.0)) {
		m_nf.error_lines.push_back(r+1);
	}

}

//
// Populates ui->nf_table with pointers to m_nf_table_data.  The entire table
// is first cleared, then each cell of each row is set.  Call this only when
// the entire table needs to be refreshed, as when being initially populated
// from a new notefile.  Otherwise, use the overload 
// set_nftable(int row, int col)
//
void nf_import_window::set_nftable() {
	ui.nf_table->blockSignals(true);
	ui.nf_table->clearContents();
	for (int i=0; i<m_nf_table_data.size(); ++i) {
		set_nftable(i);
	}
	ui.nf_table->blockSignals(false);
}
// Overload for updating a particular row of the table widget from the backend
// datastore m_nf/m_nf_table_data.  If thiss is being called in response to a
// user making a change to the table, set_nf_from_nftable(int const) needs to be
// called first to update the backend datastore.  
void nf_import_window::set_nftable(int const r) {
	ui.nf_table->blockSignals(true);
	if (r >= ui.nf_table->rowCount()) {
		ui.nf_table->setRowCount(r+10);
	};
	ui.nf_table->setItem(r,0,&(m_nf_table_data[r].ontime));
	ui.nf_table->setItem(r,1,&(m_nf_table_data[r].offtime));
	ui.nf_table->setItem(r,2,&(m_nf_table_data[r].pitch));
	ui.nf_table->setItem(r,3,&(m_nf_table_data[r].dt));

	auto curr_dt = m_nf_table_data[r].dt.data(Qt::EditRole).toDouble();
	if (curr_dt <= 0.0) {
		ui.nf_table->item(r,3)->setBackground(QBrush(QColor(255,153,153)));
	} else {
		ui.nf_table->item(r,3)->setBackground(QBrush(QColor(255,255,255)));
	}
	ui.nf_table->blockSignals(false);
}

void nf_import_window::update_nv_t_count() {
	bool window_is_valid = m_ts_uih.is_valid() && m_bpm_uih.is_valid() &&
		m_err_uih.is_valid() && (m_nf.error_lines.size()==0) && !m_nf.file_error;
	if (!window_is_valid) {
		return;
	}

	auto bpm = m_bpm_uih.get();
	auto err = m_err_uih.get();
	auto ts = m_ts_uih.get();
	auto dt = notefile2dt(m_nf);

	auto nvs = deltat2rp(dt,ts,bpm,err);
	auto uq_nvs = unique_n(nvs);

	ui.nv_t_counts->clear();
	for (int i=0; i<uq_nvs.size(); ++i) {
		std::string cline {};
		cline += uq_nvs[i].value.print();
		cline += " : ";
		cline += std::to_string(uq_nvs[i].count);
		ui.nv_t_counts->appendPlainText(QString().fromStdString(cline));
	}
	wait();
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

void nf_import_window::on_nf_table_cellChanged(int r, int c) {
	if (r == -1 || c == -1) {
		// TODO:  For some reason, when the window is closed, cancel is clicked, etc,
		// this function is called w/ r == c == -1, triggering an invalid access
		// exception.  
		return;
	}

	ui.nf_table->blockSignals(true);
		// If signals are not blocked, then calls to set_nftable, which alters the
		// ui element, will call back into this.  
	set_nf_from_nftable(r); // update the back-end datastore m_nf and m_nf_table_data
	set_nftable(r);
		// update the table widget from the backend data store; needed only to do
		// the cell coloring
	ui.nf_table->blockSignals(false);
	update_nv_t_count();
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
	if (m_nf.error_lines.size() > 0) {
		QMessageBox *err_msg = new QMessageBox(this);
		err_msg->setText("Can't import if there are errorlines...");
		err_msg->exec();
		return;
	}
	gdp.create(m_nf,m_nf.fname);
	this->close();
}


