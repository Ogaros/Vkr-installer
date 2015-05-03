#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define NOMINMAX

#include <QMainWindow>
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
    QByteArray generateKey();
    QByteArray generateHash(QByteArray &data);
    Ui::MainWindow *ui;
    DBManager db;

};

#endif // MAINWINDOW_H
