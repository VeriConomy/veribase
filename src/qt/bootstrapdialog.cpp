#include <qt/bootstrapdialog.h>
#include <qt/forms/ui_bootstrapdialog.h>
#include <qt/guiutil.h>
#include <qt/guiconstants.h>
#include <downloader.h>
#include <QDesktopServices>

BootstrapDialog::BootstrapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BootstrapDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setWindowTitle(tr("Chain Bootstrap"));
}

BootstrapDialog::~BootstrapDialog()
{
    delete ui;
}

BootstrapDialog* bootstrap_callback_instance;
static void xfer_callback(curl_off_t now, curl_off_t total)
{
    int percentage = (int)((double)now / (double)total * 100);
    // start at one
    bootstrap_callback_instance->setProgress(percentage+1);
}

void BootstrapDialog::on_startButton_clicked()
{
    extern void set_xferinfo_data(void*);

    bootstrap_callback_instance = this;
    set_xferinfo_data((void*)xfer_callback);

    ui->closeButton->setEnabled(false);
    ui->startButton->setEnabled(false);

    QMessageBox::information(this, "Bootstrap", "The client will now bootstrap the chain. \n\nThe wallet will exit after extracting the bootstrap and need to be restarted.", QMessageBox::Ok, QMessageBox::Ok);
    try {
        downloadBootstrap();
    } catch (const std::runtime_error& e) {
        QMessageBox::critical(this, tr("Bootstrap failed"), e.what());
        this->close();
        return;
    } catch (...) {
        QMessageBox::critical(this, tr("Bootstrap failed"), "Unknown Issue");
        this->close();
        return;
    }

    ui->closeButton->setEnabled(true);
    ui->startButton->setEnabled(true);

    set_xferinfo_data(nullptr);
    bootstrap_callback_instance = nullptr;
    this->close();
    QApplication::quit();
}

void BootstrapDialog::on_closeButton_clicked()
{
    this->close();
}

void BootstrapDialog::setProgress(int now)
{
    if ( now > 100 )
        now = 100;

    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(now);
}
