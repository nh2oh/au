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
		//---------------------------------------------------------------------------
		// Data
		au::uih_parser m_ts_parser {parse_userinput_ts {},
			"The format of a ts is n/d[c] where n,d are both integers > 0."};
		au::uih m_ts_uih {m_ts_parser};

		uih_parser<parse_userinput_double> m_bpm_parser {parse_userinput_double {},
			"A number > 0 (decimals allowed)."};
		uih_pred bpm_pred_positive {[](double const& inp){return (inp > 0.0);},
			"Value must be > 0"};
		uih m_bpm_uih {m_bpm_parser,bpm_pred_positive};

	struct defaults {
		std::string ts {"4/4"};
		std::string err {"0.250"};
		std::string bpm {"90"};
		std::string curr_fname {""};

		std::string init_dir {"..\\stuff\\"};
	};
	defaults m_defaults {};

	//-------------------------------
	struct status_flags {
		bool curr_fname {false};
		bool nf {false};
		bool nf_table {false};
		bool bpm {false};
		bool err {false};
	};
	status_flags m_status {};

	struct qt_nf_table_data {
		QTableWidgetItem ontime {0.0};
		QTableWidgetItem offtime {0.0};
		QTableWidgetItem pitch {0};
		QTableWidgetItem dt {0.0};
	};
	std::vector<qt_nf_table_data> m_nf_table {};
		// ui->nf_table gets the data it displays by holding pointers
		// to this.  

	std::string m_fname {};

	notefile m_nf {};
	std::vector<double> m_dt {}; 
		// This is used over and over again so it is convienient to extract
		// it from m_nf.  

	double m_bpm {90.0};
	double m_err {0.25};  // fudge-factor (seconds)


	//---------------------------------------------------------------------------
	// Functions
	void set_nftable();
	void update_nv_t_count();
	void set_ts();
	void set_bpm();
	void set_err();
	void set_nf();
	void set_dt(); 

	Ui::nf_import_window ui;
	private slots:
	void on_ts_returnPressed();
	void on_ts_textEdited();
	void on_bpm_returnPressed();
	void on_bpm_textEdited();
	void on_err_returnPressed();

	// "Cancel" and "import" buttons
	void on_cancel_clicked();
	void on_import_clicked();
};

