#pragma once
#include <QtWidgets/QMainWindow>
#include "ui_scale_builder.h"

class scale_builder: public QMainWindow {
	Q_OBJECT
public:
	scale_builder(QWidget *parent = Q_NULLPTR);
private:
	Ui::scale_builder ui;
private slots:

};

