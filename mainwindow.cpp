#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setEnabledToolboxes(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QImage MainWindow::matToQImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type() == CV_8UC1)
    {
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i = 0; i < 256; i++)
            colorTable.push_back(qRgb(i,i,i));
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img;
    }
    else
        return QImage();
}

QString MainWindow::pickImageDialog()
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

void MainWindow::displayInputImage()
{
    cv::namedWindow("Input", cv::WINDOW_NORMAL);
    imshow("Input", this->imageInput);
}

void MainWindow::displayOutputImage()
{
    cv::namedWindow("Output", cv::WINDOW_NORMAL);
    cv::imshow("Output", this->imageOutput);
}

void  MainWindow::displayLogoImage()
{
    cv::namedWindow("Logo", cv::WINDOW_NORMAL);
    cv::imshow("Logo", this->imageLogo);
}

void MainWindow::addSaltNoise(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++) {
        int i = rand() % image.cols;
        int j = rand() % image.rows;

        if (image.channels() == 1) { // gray-level image
            image.at<uchar>(j, i)= 255;
        }
        else if (image.channels() == 3) { // color image
            image.at<cv::Vec3b>(j, i)[0]= 255;
            image.at<cv::Vec3b>(j, i)[1]= 255;
            image.at<cv::Vec3b>(j, i)[2]= 255;
        }
    }
}

void MainWindow::addPepperNoise(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++) {
        int i = rand() % image.cols;
        int j = rand() % image.rows;

        if (image.channels() == 1) { // gray-level image
            image.at<uchar>(j, i)= 0;
        }
        else if (image.channels() == 3) { // color image
            image.at<cv::Vec3b>(j, i)[0]= 0;
            image.at<cv::Vec3b>(j, i)[1]= 0;
            image.at<cv::Vec3b>(j, i)[2]= 0;
        }
    }
}

void MainWindow::on_buttonLoadImage_clicked()
{
    QString filePath = this->pickImageDialog();

        if(filePath != 0)
        {
            imageInput = cv::imread(filePath.toStdString());

            if(!imageInput.data)
            {
                QMessageBox msgBox;
                msgBox.setText("Image you selected didn't loaded.");
                msgBox.exec();
                return;
            }

            this->imageInput.copyTo(this->imageOutput);

            this->setEnabledToolboxes(true);

            this->setCurrentColorSpace("BGR");

            this->ui->buttonClearAndCloseAll->setEnabled(true);
            this->ui->buttonResetOutput->setEnabled(true);

            this->displayInputImage();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You didn't select image.");
            msgBox.exec();
        }
}

void MainWindow::on_buttonAddSaltAndPepper_clicked()
{
    if(ui->checkBoxSalt->isChecked())
        this->addSaltNoise(this->imageOutput, ui->spinBoxSaltAndPepperRate->value());
    if(ui->checkBoxPepper->isChecked())
        this->addPepperNoise(this->imageOutput, ui->spinBoxSaltAndPepperRate->value());

    this->displayOutputImage();
}

void MainWindow::setEnabledToolboxes(bool enabled)
{
    ui->groupBoxSaltAndPepper->setEnabled(enabled);
    ui->groupBoxShowLogo->setEnabled(enabled);
    ui->groupBoxConvertColorSpace->setEnabled(enabled);
}

void MainWindow::on_buttonResetOutput_clicked()
{
    this->imageInput.copyTo(this->imageOutput);
    this->displayOutputImage();
}

void MainWindow::on_buttonClearAndCloseAll_clicked()
{
    cv::destroyAllWindows();

    this->setEnabledToolboxes(false);

    this->ui->buttonClearAndCloseAll->setEnabled(false);
    this->ui->buttonResetOutput->setEnabled(false);
}

void MainWindow::on_buttonLoadLogo_clicked()
{
    QString filePath = this->pickImageDialog();

        if(filePath != 0)
        {
            imageLogo = cv::imread(filePath.toStdString());

            if(!imageLogo.data)
            {
                QMessageBox msgBox;
                msgBox.setText("Logo you selected didn't loaded.");
                msgBox.exec();
                return;
            }

            if(this->imageLogo.rows > this->imageInput.rows || this->imageLogo.cols > this->imageInput.cols)
            {
                QMessageBox msgBox;
                msgBox.setText("Logo you selected is bigger than input image. Please select new logo.");
                msgBox.exec();
                return;
            }

            ui->buttonAddLogo->setEnabled(true);

            ui->spinBoxShowLogoX->setMaximum(this->imageOutput.cols - this->imageLogo.cols);
            ui->spinBoxShowLogoY->setMaximum(this->imageOutput.rows - this->imageLogo.rows);

            this->displayLogoImage();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You didn't select logo.");
            msgBox.exec();
        }
}

void MainWindow::on_buttonAddLogo_clicked()
{
    if(this->imageLogo.rows > this->imageInput.rows || this->imageLogo.cols > this->imageInput.cols)
    {
        QMessageBox msgBox;
        msgBox.setText("Logo you selected is bigger than input image. Please select new logo.");
        msgBox.exec();
        return;
    }

    cv::Mat imageROI;
    imageROI = this->imageOutput(cv::Rect(ui->spinBoxShowLogoX->value(),ui->spinBoxShowLogoY->value(), this->imageLogo.cols, this->imageLogo.rows));

    if(imageROI.channels() == 1)
        if(this->imageLogo.channels() != 1)
            cv::cvtColor(this->imageLogo, this->imageLogo, CV_BGR2GRAY);

    cv::addWeighted(imageROI,
                    ui->doubleSpinBoxShowLogoAlpha->value(),
                    this->imageLogo,
                    ui->doubleSpinBoxShowLogoBeta->value(),
                    ui->doubleSpinBoxShowLogoGamma->value(),
                    imageROI);

    this->displayOutputImage();
}

void MainWindow::on_buttonConvertColorSpaceConvertTo_clicked()
{
    // CV_BGR2RGB
    // CV_BGR2GRAY
    // CV_BGR2HSV
    // CV_BGR2HLS
    if(ui->comboBoxCurrentColorSpace->currentText() == "BGR")
    {
        if(ui->comboBoxConvertTo->currentText() == "RGB")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2RGB);
            this->setCurrentColorSpace("RGB");
        }
        else if(ui->comboBoxConvertTo->currentText() == "GRAY")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2GRAY);
            this->setCurrentColorSpace("GRAY");
        }
        else if(ui->comboBoxConvertTo->currentText() == "HSV")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2HSV);
            this->setCurrentColorSpace("HSV");
        }
        else if(ui->comboBoxConvertTo->currentText() == "HLS")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2HLS);
            this->setCurrentColorSpace("HLS");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Can't convert color space.");
            msgBox.exec();
            return;
        }
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "RGB")
    {
        //CV_RGB2BGR
        //CV_RGB2GRAY
        //CV_RGB2HSV
        //CV_RGB2HLS
        if(ui->comboBoxConvertTo->currentText() == "BGR")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_RGB2BGR);
            this->setCurrentColorSpace("BGR");
        }
        else if(ui->comboBoxConvertTo->currentText() == "GRAY")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_RGB2GRAY);
            this->setCurrentColorSpace("GRAY");
        }
        else if(ui->comboBoxConvertTo->currentText() == "HSV")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_RGB2HSV);
            this->setCurrentColorSpace("HSV");
        }
        else if(ui->comboBoxConvertTo->currentText() == "HLS")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_RGB2HLS);
            this->setCurrentColorSpace("HLS");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Can't convert color space.");
            msgBox.exec();
            return;
        }
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "GRAY")
    {
        //CV_GRAY2BGR
        //CV_GRAY2RGB
        if(ui->comboBoxConvertTo->currentText() == "BGR")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_GRAY2BGR);
            this->setCurrentColorSpace("BGR");
        }
        else if(ui->comboBoxConvertTo->currentText() == "RGB")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_GRAY2RGB);
            this->setCurrentColorSpace("RGB");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Can't convert color space.");
            msgBox.exec();
            return;
        }
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "HSV")
    {
        //CV_HSV2BGR
        //CV_HSV2RGB
        if(ui->comboBoxConvertTo->currentText() == "BGR")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_HSV2BGR);
            this->setCurrentColorSpace("BGR");
        }
        else if(ui->comboBoxConvertTo->currentText() == "RGB")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_HSV2RGB);
            this->setCurrentColorSpace("RGB");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Can't convert color space.");
            msgBox.exec();
            return;
        }
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "HLS")
    {
        //CV_HLS2BGR
        //CV_HLS2RGB
        if(ui->comboBoxConvertTo->currentText() == "BGR")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_HLS2BGR);
            this->setCurrentColorSpace("BGR");
        }
        else if(ui->comboBoxConvertTo->currentText() == "RGB")
        {
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_HLS2RGB);
            this->setCurrentColorSpace("RGB");
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("Can't convert color space.");
            msgBox.exec();
            return;
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Can't convert color space.");
        msgBox.exec();
        return;
    }

    this->displayOutputImage();
}

void MainWindow::setCurrentColorSpace(QString colorSpace)
{
    ui->comboBoxCurrentColorSpace->setCurrentText(colorSpace);

    QStringList avaliableColorSpace;
    // Fill combo boxes
    //threadPriorities<<"Idle"<<"Lowest"<<"Low"<<"Normal"<<"High"<<"Highest"<<"Time Critical"<<"Inherit";
    //capturePrioComboBox->addItems(threadPriorities);

    if(colorSpace == "BGR")
    {
        // CV_BGR2RGB
        // CV_BGR2GRAY
        // CV_BGR2HSV
        // CV_BGR2HLS
        avaliableColorSpace<< "RGB" <<
                              "GRAY" <<
                              "HSV" <<
                              "HLS";

        ui->comboBoxConvertTo->clear();
        ui->comboBoxConvertTo->addItems(avaliableColorSpace);
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "RGB")
    {
        //CV_RGB2BGR
        //CV_RGB2GRAY
        //CV_RGB2HSV
        //CV_RGB2HLS

        avaliableColorSpace<< "BGR" <<
                              "GRAY" <<
                              "HSV" <<
                              "HLS";

        ui->comboBoxConvertTo->clear();
        ui->comboBoxConvertTo->addItems(avaliableColorSpace);
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "GRAY")
    {
        //CV_GRAY2BGR
        //CV_GRAY2RGB

        avaliableColorSpace<< "BGR" <<
                              "RGB";

        ui->comboBoxConvertTo->clear();
        ui->comboBoxConvertTo->addItems(avaliableColorSpace);
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "HSV")
    {
        //CV_HSV2BGR
        //CV_HSV2RGB

        avaliableColorSpace<< "BGR" <<
                              "RGB";

        ui->comboBoxConvertTo->clear();
        ui->comboBoxConvertTo->addItems(avaliableColorSpace);
    }
    else if(ui->comboBoxCurrentColorSpace->currentText() == "HLS")
    {
        //CV_HLS2BGR
        //CV_HLS2RGB

        avaliableColorSpace<< "BGR" <<
                              "RGB";

        ui->comboBoxConvertTo->clear();
        ui->comboBoxConvertTo->addItems(avaliableColorSpace);
    }
    else
        return;
}
