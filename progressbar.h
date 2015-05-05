#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include "ui_progressbar.h"
#include <Windows.h>

class ProgressBar : public QWidget
{
	Q_OBJECT

public:
	ProgressBar(QWidget *parent = 0);
	~ProgressBar();

public slots:
	void setupBar(int filesCount);
	void updateBar();

private:
	Ui::ProgressBar ui;
};

#endif // PROGRESSBAR_H
