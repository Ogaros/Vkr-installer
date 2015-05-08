#ifndef RANDOMSEEDWINDOW_H
#define RANDOMSEEDWINDOW_H

#include <QWidget>
#include "ui_randomseedwindow.h"
#include <QKeyEvent>
#include <QElapsedTimer>

class randomSeedWindow : public QWidget
{
	Q_OBJECT

public:
	randomSeedWindow(QWidget *parent = 0);
	~randomSeedWindow();
	void keyPressEvent(QKeyEvent *e);

private:
	Ui::randomSeedWindow ui;
	QByteArray seed;
	QElapsedTimer timer;

signals:
	void generatedSeed(QByteArray);
};

#endif // RANDOMSEEDWINDOW_H
