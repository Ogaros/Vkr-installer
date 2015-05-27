#include "randomseedwindow.h"

randomSeedWindow::randomSeedWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::WindowSystemMenuHint);
	setAttribute(Qt::WA_DeleteOnClose);
	setFocus();
	timer.start();
	grabKeyboard();	
}

randomSeedWindow::~randomSeedWindow()
{
	releaseKeyboard();	
}

void randomSeedWindow::keyPressEvent(QKeyEvent *e)
{
	quint64 t = timer.restart();		
	if (e->text().isEmpty())
		seed.append(e->key());
	else
		seed.append(e->text());
	for (char &ch : seed)
	{
		ch ^= t;
	}
	int value = ui.progressBar->value();
	value += std::rand() % 4 + 1;
	ui.progressBar->setValue(value);
	if (value >= 100)
	{
		emit generatedSeed(seed);
		close();
	}
}