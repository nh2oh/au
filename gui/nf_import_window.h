#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_nf_import_window.h"
#include "aulib\types\ts_t.h"
#include "aulib\types\beat_bar_t.h"
#include "aulib\types\note_value_t.h"
#include "aulib\input\notefile.h"
#include <string>
#include <vector>

class nf_import_window : public QMainWindow {
	Q_OBJECT

	public:
		nf_import_window(QWidget *parent = Q_NULLPTR);

		void load_nf();
		void update_note_value_count();

	private:
		// Private types
		struct qt_nf_table_data {
			QTableWidgetItem ontime {0.0};
			QTableWidgetItem offtime {0.0};
			QTableWidgetItem pitch {0};

			QTableWidgetItem tdelta {0.0};
		};

		// Private data
		std::string m_fname {};
		ts_t m_ts {beat_t{4.0},note_value{1.0/4.0},false};
		double m_bpm {90.0};
		double m_err {0.125};  // sec fudge-factor

		// All the "Note ..." lines from the notefile parsed
		std::vector<notefile> m_nf {};
		std::vector<double> m_deltat {};

		std::vector<qt_nf_table_data> qt_nf_data {};

		Ui::nf_import_window ui;
	private slots:
		void on_ts_returnPressed();
		void on_bpm_returnPressed();
		void on_err_returnPressed();
};

/*
    QWidget *centralwidget;
    QTableWidget *nf_data;
    QPushButton *cancel;
    QPushButton *import_2;
    QLineEdit *ts;
    QLineEdit *bpm;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *err;
    QPlainTextEdit *note_value_counts;
    QMenuBar *menubar;
    QStatusBar *statusbar;
*/