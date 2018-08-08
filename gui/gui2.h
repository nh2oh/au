#pragma once
#include <QtWidgets/QMainWindow>
//#include <QAbstractItemView>
#include "ui_gui2.h"

class gui2 : public QMainWindow {
	Q_OBJECT

	public:
		gui2(QWidget *parent = Q_NULLPTR);
	private:
		Ui::gui2 ui;
	private slots:
		void on_actionImport_triggered();
		void on_actionrandRp_triggered();
		void on_update_clicked();
};
