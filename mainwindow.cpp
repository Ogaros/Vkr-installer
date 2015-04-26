#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sqlite3.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    refreshDeviceList();
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshDeviceList()));
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openSelectedDir()));
    connect(ui->installButton, SIGNAL(clicked()), this, SLOT(installEncryptor()));
}

std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> MainWindow::detectDevices()
{
    //                                     letter   name   capacity  availible
    std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> pDevices(new std::vector<std::tuple<QString, QString, size_t, size_t>>);
    DWORD dwDrives = GetLogicalDrives();
    std::wstring path = L"A:\\";
    while(dwDrives)
        {
            if(dwDrives & 1 && GetDriveType(path.c_str()) == DRIVE_REMOVABLE)
            {
                wchar_t label[MAX_PATH];
                if(!GetVolumeInformationW(path.c_str(), label, sizeof(label), nullptr, nullptr, nullptr, nullptr, 0))
                {
                    std::wcerr << L"GetVolumeInformationW(" << path << L") error: " << GetLastError() << std::endl;
                    //TODO:: add exception
                }
                else
                {
                    ULARGE_INTEGER capacity, availible;
                    GetDiskFreeSpaceExW(path.c_str(), nullptr, &capacity, &availible);
                    pDevices->emplace_back(QString::fromStdWString(path), QString::fromWCharArray(label),
                                           capacity.QuadPart / (1024 * 1024), availible.QuadPart / (1024 * 1024));
                }
            }
            dwDrives = dwDrives >> 1;
            path[0]++;
        }
    pDevices->emplace_back("D:\\Users\\Ogare\\Desktop\\InstallTest\\", "Test folder", 0, 0);
    return pDevices;
}

void MainWindow::fillDeviceList(std::unique_ptr<std::vector<std::tuple<QString, QString, size_t, size_t>>> pDevices)
{
    for(size_t i = 0; i < pDevices->size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        QString name = std::get<1>(pDevices->at(i));
        if(name.isEmpty())
            name = "Съемный диск";
        // device name
        item->setData(0, Qt::DisplayRole, name + " (" + static_cast<QString>(std::get<0>(pDevices->at(i))).left(2) + ")");
        // device capacity(availible size)
        item->setData(1, Qt::DisplayRole, QString::number(std::get<2>(pDevices->at(i))) + "(" + QString::number(std::get<3>(pDevices->at(i))) + ") MB");
        // device letter (D:\\)
        item->setData(0, Qt::UserRole, std::get<0>(pDevices->at(i)));
        // device capacity
        item->setData(1, Qt::UserRole, std::get<2>(pDevices->at(i)));
        ui->deviceList->addTopLevelItem(item);
    }
}

QByteArray MainWindow::generateKey()
{
    QByteArray key;
    std::mt19937 randGenerator(std::rand());
    std::uniform_int_distribution<char> distribution(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
    for(int i = 0; i < 32; i++)
    {
        key.append(distribution(randGenerator));
    }
    return key;
}

QByteArray MainWindow::generateHash(QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha512);
}

void MainWindow::refreshDeviceList()
{
    ui->deviceList->clear();
    fillDeviceList(detectDevices());
    ui->deviceList->resizeColumnToContents(0);
    // Add some space between device name and capacity
    ui->deviceList->header()->resizeSection(0, ui->deviceList->header()->sectionSize(0) + 20);
//    ui->encryptButton->setEnabled(false);
//    ui->decryptButton->setEnabled(false);
//    ui->openButton->setEnabled(false);
}

void MainWindow::openSelectedDir()
{
    QTreeWidgetItem *item = ui->deviceList->selectedItems().front();
    QDesktopServices::openUrl(QUrl("file:///" + item->data(0, Qt::UserRole).toString()));
}

void MainWindow::installEncryptor()
{
    QByteArray key, hash;
    do
    {
        key = generateKey();
        hash = generateHash(key);
    }
    while(db.hashExists(hash));
    db.addHash(hash);
}

MainWindow::~MainWindow()
{
    delete ui;
}
