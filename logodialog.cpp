#include "logodialog.h"
#include "ui_logodialog.h"

#include <QMessageBox>
#include <QFileDialog>

LogoDialog::LogoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogoDialog)
{
    ui->setupUi(this);
}

LogoDialog::~LogoDialog()
{
    delete ui;
}

void LogoDialog::on_buttonBox_accepted()
{
    this->logoX = ui->spinBoxShowLogoX->value();
    this->logoY = ui->spinBoxShowLogoY->value();

    this->logoAlpha = ui->doubleSpinBoxShowLogoAlpha->value();
    this->logoBeta = ui->doubleSpinBoxShowLogoBeta->value();
    this->logoGamma = ui->doubleSpinBoxShowLogoGamma->value();
}

void LogoDialog::on_buttonLoadLogo_clicked()
{
    QString filePath = this->pickImageDialog();

    if(filePath != 0)
    {
        imageLogo = cv::imread(filePath.toStdString());
        ui->buttonBox->setEnabled(true);
        return;

        if(!imageLogo.data)
        {
            QMessageBox msgBox;
            msgBox.setText("Logo you selected didn't loaded.");
            msgBox.exec();
            return;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You didn't select logo.");
        msgBox.exec();
    }
}

QString LogoDialog::pickImageDialog()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::home());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setWindowTitle("Open Image");
    dialog.setNameFilter("Image Files (*.png *.jpg *.jpeg *.bmp)");
    dialog.setViewMode(QFileDialog::Detail);

    if(dialog.exec())
    {
        if(dialog.selectedFiles().empty())
            return 0;

        return dialog.selectedFiles().first();
    }
    else
        return 0;
}
