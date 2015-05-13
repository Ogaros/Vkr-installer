#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define NOMINMAX

#include <QMainWindow>
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <vector>
#include <tuple>
#include <memory>
#include <windows.h>
#include <iostream>
#include <QUrl>
#include <DBManager.h>
#include <QCryptographicHash>
#include <QByteArray>
#include <random>
#include <QFile>
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QMessageBox>
#include "sqlite3.h"
#include "usbSerialAdapter.h"
#include "Gost.h"
#include "progressbar.h"
#include "randomseedwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void refreshDeviceList();
    void openSelectedDir();
    void installEncryptor();

private:
    std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> detectDevices();
    void fillDeviceList(std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> pDevices);
	void copyFiles();	
	void getInstallationSize();
	QByteArray generateKey();    
    QByteArray generateHash(QByteArray &data);
    Ui::MainWindow *ui;
    DBManager db;
	std::unique_ptr<std::seed_seq> seed;
	QByteArray seedArr;
	qint64 installationSize;

private slots:
	void refreshButtons();
	void setupSeed(QByteArray seed);
	void generateKeyFile();
	

signals:
	void fileCopied();
	void filesCounted(int filesCount);

};

#endif // MAINWINDOW_H
