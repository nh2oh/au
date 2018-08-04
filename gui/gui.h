#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_gui.h"
#include <string>

class gui : public QMainWindow {
	Q_OBJECT

public:
	gui(QWidget *parent = Q_NULLPTR);
	gui(std::string, QWidget *parent = Q_NULLPTR);

	std::string m_str {"default"};
private:
	Ui::guiClass ui;

private slots:
	void on_btn_make_clicked();
	void on_actionImport_triggered();
};
