#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_nf_import_window.h"
#include "aulib\uih.h"
#include "aulib\types\ts_t_uih.h"
#include "aulib\numeric_simple_uih.h"
#include "aulib\input\notefile.h"
#include <string>
#include <vector>

class nf_import_window : public QMainWindow {
	Q_OBJECT

	public:
	nf_import_window(QWidget *parent = Q_NULLPTR);

	private:
	//----------------------------------------------------------------------------
	// Data
	struct defaults {
		std::string ts {"4/4"};
		std::string err {"0.250"};
		std::string bpm {"90"};
		std::string curr_fname {""};

		std::string init_dir {"..\\stuff\\"};
	};
	defaults m_defaults {};


	au::uih_parser<parse_userinput_ts> m_ts_parser {parse_userinput_ts {},
		"The time signature (format: n/d[c] where c means \"compound\")"};
	au::uih<decltype(m_ts_parser)> m_ts_uih {m_ts_parser};

	au::uih_parser<parse_userinput_double> m_bpm_parser {parse_userinput_double {},
		"The tempo in beats-per-minute"};
	au::uih_pred<ftr_gt> p_gtzero {ftr_gt{0.0},"A tempo is always > 0"};
	au::uih<decltype(m_bpm_parser),decltype(p_gtzero)> m_bpm_uih {m_bpm_parser,p_gtzero};

	au::uih_parser<parse_userinput_double> m_err_parser {parse_userinput_double {},
		"A number >= 0 (decimals allowed)."};
	au::uih_pred<ftr_geq> p_geqzero {ftr_geq{0.0},"Value must be >= 0."};
	au::uih<decltype(m_err_parser),decltype(p_geqzero)> m_err_uih {m_err_parser,p_geqzero};

	std::string m_fname {};

	notefile m_nf {};
	struct qt_nf_table_data {
		QTableWidgetItem ontime {0.0};
		QTableWidgetItem offtime {0.0};
		QTableWidgetItem pitch {0};
		QTableWidgetItem dt {0.0};
	};
	std::vector<qt_nf_table_data> m_nf_table_data {};
		// ui->nf_table gets the data it displays by holding pointers
		// to this.  Populated by set_nf_table_data()

	//---------------------------------------------------------------------------
	// Functions
	//
	// set_* functions (1) update the internal data structures, (2) check the
	//   status of the data and set window properties accordingly.  They do 
	//   _not_ call other set_* functions.  set_* function chaining logic is
	//   encoded in on_* functions.  
	//
	// on_* functions call whatever series of set_* functions is needed to 
	//   keep the window consistent with itself and with the internal data.  
	//   They do _not_ set window properties.  
	//
	void set_nf_from_file(); 
	void set_nf_from_nftable(int const);
		// Both populate the internal datastores m_nf/m_nf_table_data
		// without altering the table widget.  Overload 
		// set_nf_from_nftable(int const) _reads_ the table widget.  
	
	void set_nftable();
	void set_nftable(int const r);
		// Populates the gui widget from m_nf/m_nf_table_data.  Overload 
		// set_nftable(int const r) does only one row of the table,
		// overload set_nftable() clears the entire table widget then
		// calls overload set_nftable(int const r) for every row in the
		// backend datastore m_nf/m_nf_table_data.  
		// Overload m_nf_table_data(int const r) is responsible for 
		// setting cell colors for valid/invalid cells.  
		//
		// TODO:  This should blank out all rows below 
		// m_nf_table.data.size() as a part of syncing w/ m_nf_table_data.  

	void update_nv_t_count();
	void set_ts();
	void set_bpm();
	void set_err();

	Ui::nf_import_window ui;

	private slots:
	void on_ts_returnPressed();
	void on_ts_textEdited();
	void on_bpm_returnPressed();
	void on_bpm_textEdited();
	void on_err_returnPressed();
	void on_err_textEdited();
	void on_nf_table_cellChanged(int, int);
	void on_cancel_clicked();
	void on_import_clicked();
};

