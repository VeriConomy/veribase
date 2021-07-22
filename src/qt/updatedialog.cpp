#include <qt/updatedialog.h>
#include <downloader.h>
#include <util/system.h>
#include <boost/algorithm/string.hpp>
#include <clientversion.h>
#include <qt/forms/ui_updatedialog.h>
#include <qt/guiutil.h>
#include <qt/guiconstants.h>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QDir>

UpdateDialog::UpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowTitle(tr("Check for Update"));
    ui->updateButton->setVisible(false);
    ui->progressBar->setVisible(false);

    try {
        downloadVersionFile();
    } catch(const std::runtime_error& re) {
        ui->label->setText("The following error happened while retrieving update information: " + QString::fromStdString(re.what()));
        return;
    } catch(...) {
        ui->label->setText("An error happened while retrieving update information");
        return;
    }

    clientName = getUpdatedClient();
    if (clientName.compare(" ") != 0){

        ui->updateButton->setVisible(true);
        QString qstr = QString(QObject::tr("Update is available! Click update to download %1")).arg(QString::fromStdString(clientName));
        ui->progressBar->setVisible(true);
        ui->label->setText(qstr);
    }
    else {
        ui->label->setText(tr("Wallet is the newest version.  No update at this time."));
    }
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

UpdateDialog* update_callback_instance;
static void xfer_callback(curl_off_t total, curl_off_t now)
{
    update_callback_instance->setProgress(total, now);
}

void UpdateDialog::on_updateButton_clicked()
{
    extern void set_xferinfo_data(void*);

    update_callback_instance = this;
    set_xferinfo_data((void*)xfer_callback);

    QMessageBox::information(this, "Update", "The wallet will now be updated. \n\nThe wallet will exit after update starts and will need to be restarted.", QMessageBox::Ok, QMessageBox::Ok);
    try {
        downloadClient(clientName);
    } catch (const std::runtime_error& e) {
        QMessageBox::critical(this, tr("Update failed"), e.what());
    }
    set_xferinfo_data(nullptr);
    update_callback_instance = nullptr;
    this->close();
    QString qClientName = QString::fromStdString(clientName);
    processUpdate(qClientName);
}

void UpdateDialog::on_closeButton_clicked()
{
    this->close();
}

void UpdateDialog::setProgress(curl_off_t total, curl_off_t now)
{
    ui->progressBar->setMinimum(0);
    ui->progressBar->setMaximum(total - 1);
    ui->progressBar->setValue(now);
}

bool needClientUpdate()
{
    try {
        downloadVersionFile();
    } catch (...) {
        throw;
    }

    std::string clientName = getUpdatedClient();
    return clientName.compare(" ") != 0;
}
// Reads the version file and maps it to the current configuration.
std::string getUpdatedClient()
{
    std::string clientFileName = " ";
    QString versionData;
    std::string line;
    std::string version;
    boost::filesystem::path pathVersionFile = GetDataDir() / "VERSION.json";
    boost::filesystem::ifstream streamVersion(pathVersionFile);

    // Read version file to get update status
    if (streamVersion && streamVersion.is_open())
    {
        while (getline(streamVersion,line))
        {
            versionData.append(line.c_str());
        }
        streamVersion.close();

        if (versionData.isEmpty())
        {
            LogPrintf("Error: Version data is empty.\n");
            return clientFileName;
        }

        // get details of update and system
        QJsonDocument versionDoc = QJsonDocument::fromJson(versionData.toUtf8());
        QJsonObject versionObj = versionDoc.object();
        QString vTitle = versionObj.value(QString("title")).toString();
        QString vDescription = versionObj.value(QString("description")).toString();
        QString vFileName = versionObj.value(QString("prefix")).toString();
        QString vVersion = versionObj.value(QString("version")).toString();
        vFileName.append(vVersion);
        QString vArch("");
        if (getArchitecture() == 64)
            vArch = versionObj.value(QString("arch64")).toString();
        else
            vArch = versionObj.value(QString("arch32")).toString();
        vFileName.append(vArch);
#ifdef WIN32
        vFileName.append(versionObj.value(QString("windows")).toString());
#else
#ifdef MAC_OSX
        vFileName.append(versionObj.value(QString("mac")).toString());
#else
        vFileName.append(versionObj.value(QString("linux")).toString());
#endif
#endif
        // determine available version
        version = vVersion.toStdString();
        int maj = 0; int min = 0; int rev = 0; int bld = 0;
        typedef std::vector<std::string> parts_type;
        parts_type parts;
        boost::split(parts, version, boost::is_any_of(".,"), boost::token_compress_on); // catch those fat-fingers ;)
        long unsigned int i = 0;
        for (std::vector<std::string>::iterator it = parts.begin(); it != parts.end() && i++ < parts.size(); ++it)
        {
            switch (i)
                {
                case 1: maj = stoi(*it); break;
                case 2: min = stoi(*it); break;
                case 3: rev = stoi(*it); break;
                case 4: bld = stoi(*it); break;
                }
        }
        if( bld == 0 && CLIENT_VERSION_BUILD != 0)
            bld = 99; // when no build, we are on release. release is stronger than anything
        int nVersion = (1000000 * maj) + (10000 * min) + (100 * rev) + (1 * bld);

        // Check whether client is out of date
        if (nVersion > CLIENT_VERSION)
        {
            clientFileName = vFileName.toStdString();
        }
    }
    else
    {
        LogPrintf("Error: Unable to read version file.\n");
    }
    return clientFileName;
}

void processUpdate(QString qClientName)
{
    QStringList newArgv(QApplication::instance()->arguments());
    QString command;

#ifdef WIN32
    // If Windows, replace argv[0] with the exe installer and restart.
    newArgv.clear();
    // Installer created by Inno Setup
    command = QString(GetDataDir().string().c_str()) + QString("/") + qClientName;

#else
#ifdef MAC_OSX
    // If Mac, replace argv[0] with Finder and pass the location of the pkg file.
    newArgv.clear();
    // Installer created by pkgbuild or Package MakerGetArg
    command = QString("/usr/bin/open");
    newArgv.append(QString(GetDataDir().c_str()) + QString("/") + qClientName);

#else
    // If Linux, we can just put it in Download directory
    boost::filesystem::path downloadDir(GetDataPathForAppName("") / "Downloads");
    boost::filesystem::path downloadFile(GetDataPathForAppName("") / "Downloads" / qClientName.toStdString());
    boost::filesystem::path tmpFile(GetDataDir() / qClientName.toStdString());
    if (!fs::is_directory(downloadDir)) {
        QMessageBox::information(nullptr, QObject::tr("Update failed"), QObject::tr("Impossible to put the installer in your Download Directory. \n\n Please download the new version from our website."), QMessageBox::Ok, QMessageBox::Ok);
    }
    else {
        boost::filesystem::permissions(tmpFile, status(tmpFile).permissions() | boost::filesystem::owner_exe | boost::filesystem::group_exe);
        boost::filesystem::rename(tmpFile, downloadFile);

        QMessageBox::information(nullptr, QObject::tr("Successfully Updated"), QObject::tr("The new wallet has been downloaded and put in your Download directory. \n\n Path: %1").arg(downloadFile.string().c_str()), QMessageBox::Ok, QMessageBox::Ok);

        // We cannot launch it because of the lock and the X minutes of turning off
        // // If Linux, just restart (already extracted verium-qt from the zip in downloader.cpp).
        // newArgv.clear();
        // // Installer created by makeself.sh
        // command = QString::fromStdString(downloadFile.string());
    }
#endif
#endif

    // run installer and quit
    QProcess::startDetached(command, newArgv);
    QApplication::quit();
}