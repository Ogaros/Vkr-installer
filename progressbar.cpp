#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.finishButton->setEnabled(false);
	setWindowFlags(Qt::WindowSystemMenuHint);
	setAttribute(Qt::WA_DeleteOnClose);
	connect(ui.finishButton, SIGNAL(clicked()), this, SLOT(close()));
}

ProgressBar::~ProgressBar()
{

}

void ProgressBar::setupBar(int filesCount)
{
	ui.progressBar->setMaximum(filesCount - 1);
	ui.progressBar->reset();	
}

void ProgressBar::updateBar()
{
	ui.progressBar->setValue(ui.progressBar->value() + 1);
	if (ui.progressBar->value() == ui.progressBar->maximum())
	{
		ui.finishButton->setEnabled(true);
		MessageBeep(MB_OK);
	}
}