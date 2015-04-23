#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopServices>
#include <vector>
#include <tuple>
#include <memory>
#include <windows.h>
#include <iostream>
#include <QUrl>

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

private:
    std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> detectDevices();
    void fillDeviceList(std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> pDevices);
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
