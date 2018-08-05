#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_gui2.h"
#include "g_data_pool.h"

class gui2 : public QMainWindow {
	Q_OBJECT

	public:
		gui2(QWidget *parent = Q_NULLPTR);
	private:
		Ui::gui2 ui;
	private slots:
		//...
};
