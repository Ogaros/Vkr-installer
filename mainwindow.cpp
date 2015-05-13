#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    refreshDeviceList();
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshDeviceList()));
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openSelectedDir()));
    connect(ui->installButton, SIGNAL(clicked()), this, SLOT(installEncryptor()));
	connect(ui->deviceList, SIGNAL(itemSelectionChanged()), this, SLOT(refreshButtons()));
	refreshButtons();
	getInstallationSize();
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
        // device path (D:\\)
        item->setData(0, Qt::UserRole, std::get<0>(pDevices->at(i)));
        // device capacity
        item->setData(1, Qt::UserRole, std::get<2>(pDevices->at(i)));
		// device availible size
		item->setData(2, Qt::UserRole, std::get<3>(pDevices->at(i)));
        ui->deviceList->addTopLevelItem(item);
    }
}

QByteArray MainWindow::generateKey()
{
    QByteArray key;
    std::mt19937 randGenerator(*seed);
    std::uniform_int_distribution<int> distribution(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());
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
}

void MainWindow::openSelectedDir()
{
    QTreeWidgetItem *item = ui->deviceList->selectedItems().front();
    QDesktopServices::openUrl(QUrl("file:///" + item->data(0, Qt::UserRole).toString()));
}

void MainWindow::installEncryptor()
{	
	if (installationSize >= ui->deviceList->selectedItems().front()->data(2, Qt::UserRole).toLongLong())
	{
		randomSeedWindow *sw = new randomSeedWindow();
		connect(sw, SIGNAL(generatedSeed(QByteArray)), this, SLOT(setupSeed(QByteArray)));
		sw->show();
	}
	else
	{
		QMessageBox box;
		box.setText("Not enough free space on the device!");
		box.setInformativeText("You need at least " + QString::number(installationSize) + " MB of free space to install the encryptor.");
		box.setStandardButtons(QMessageBox::Ok);
		box.setIcon(QMessageBox::Critical);
		box.setWindowModality(Qt::ApplicationModal);
		MessageBeep(MB_OK);
		box.exec();
	}
}

void MainWindow::copyFiles()
{
	QDir dir("./InstallationData");
	QDir dest(ui->deviceList->selectedItems().front()->data(0, Qt::UserRole).toString());
	
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QFileInfoList list = dir.entryInfoList();
	emit filesCounted(list.count());	
	for (QFileInfo f : list)
	{
		QFile::copy(f.absoluteFilePath(), dest.absoluteFilePath(f.fileName()));
		emit fileCopied();
	}
}

void MainWindow::refreshButtons()
{
	if (ui->deviceList->selectedItems().size() == 0)
	{
		ui->installButton->setEnabled(false);
		ui->openButton->setEnabled(false);
	}
	else
	{
		ui->installButton->setEnabled(true);
		ui->openButton->setEnabled(true);
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupSeed(QByteArray seed)
{
	seedArr = seed;
	this->seed = std::make_unique<std::seed_seq>(seedArr.begin(), seedArr.end());
	generateKeyFile();
}

void MainWindow::generateKeyFile()
{
	ProgressBar *bar = new ProgressBar();
	connect(this, SIGNAL(filesCounted(int)), bar, SLOT(setupBar(int)));
	connect(this, SIGNAL(fileCopied()), bar, SLOT(updateBar()));
	bar->show();
	QByteArray key, hash;
	do
	{
		key = generateKey();
		hash = generateHash(key);
		seedArr.append(std::rand());
		this->seed = std::make_unique<std::seed_seq>(seedArr.begin(), seedArr.end());
	} while (db.hashExists(hash));
	seed.reset(nullptr);
	seedArr.fill(0);
	db.addHash(hash);

	auto selectedDevice = ui->deviceList->selectedItems().front();
	QByteArray serialNumber(usbAdapter::getSerialNumber(selectedDevice->data(0, Qt::UserRole).toString().at(0).toLatin1()));

	QFile keyfile(selectedDevice->data(0, Qt::UserRole).toString() + "Vkr.key");
	keyfile.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser);

	Gost g;
	g.setKey("h47skro;,sng89o3sy6ha2qwn89sk.er");
	g.simpleEncrypt(key);
	g.simpleEncrypt(serialNumber);

	keyfile.open(QIODevice::WriteOnly | QIODevice::Unbuffered);
	keyfile.write(key);
	keyfile.write(serialNumber);
	keyfile.close();

	QtConcurrent::run(this, &MainWindow::copyFiles);
}

void MainWindow::getInstallationSize()
{
	installationSize = 0;
	QDir dir("./InstallationData");
	dir.setFilter(QDir::Files | QDir::NoSymLinks);
	QFileInfoList list = dir.entryInfoList();
	for (QFileInfo f : list)
	{
		installationSize += f.size();
	}
	installationSize = (installationSize / 1024) / 1024;  // megabytes
}