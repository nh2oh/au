#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_nf_import_window.h"
#include "aulib\types\ts_t.h"
#include "aulib\types\beat_bar_t.h"
#include "aulib\types\nv_t_t.h"
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
		ts_uih ts_;

		struct defaults {
			std::string ts {"4/4"};
			std::string err {"0.250"};
			std::string bpm {"90"};
			std::string curr_fname {""};

			std::string init_dir {"..\\stuff\\"};
		};
		defaults defaults_ {};
		//-------------------------------
		struct status_flags {
			bool curr_fname {false};
			bool nf {false};
			bool nf_table {false};
			bool bpm {false};
			bool err {false};
		};

		status_flags status {};

		struct qt_nf_table_data {
			QTableWidgetItem ontime {0.0};
			QTableWidgetItem offtime {0.0};
			QTableWidgetItem pitch {0};
			QTableWidgetItem dt {0.0};
		};
		std::vector<qt_nf_table_data> m_nf_table {};
			// ui->nf_table holds pointers to this
		std::string m_fname {};
		notefile m_nf {};
		//ts_t m_ts {beat_t{4.0},nv_t{1.0/4.0},false};
		double m_bpm {90.0};
		double m_err {0.25};  // fudge-factor (seconds)
		std::vector<double> m_dt {}; 
			// This is used over and over again so it is convienient to extract
			// it from m_nf.  

		//---------------------------------------------------------------------------
		// Functions
		void set_nftable();
		void update_nv_t_count();
		void set_ts();
		void set_bpm();
		void set_err();
		void set_nf();

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

