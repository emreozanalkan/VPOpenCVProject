#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include <vector>

#include "histogram.h"

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
    ui->groupBoxHistogramOperations->setEnabled(enabled);
    ui->groupBoxMorphologicalOperations->setEnabled(enabled);
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

void MainWindow::on_pushButtonCalculateHistogram_clicked()
{
    Histogram1D h;

    if(ui->comboBoxHistogramOperationsChannel->currentIndex() > this->imageOutput.channels() - 1)
    {
        QMessageBox msgBox;
        msgBox.setText("Output image has lower number of channel.");
        msgBox.exec();
        return;
    }

    if(this->imageOutput.channels() == 1)
    {
        cv::namedWindow("Histogram - Channel 1", cv::WINDOW_NORMAL);
        cv::imshow("Histogram - Channel 1", h.getHistogramImage(this->imageOutput));
    }
    else
    {
        int channel = ui->comboBoxHistogramOperationsChannel->currentIndex();
        h.setChannel(channel);

        QString title = QString ("Histogram - Channel %1").arg(channel + 1);

        cv::namedWindow(title.toStdString(), cv::WINDOW_NORMAL);

        cv::imshow(title.toStdString(), h.getHistogramImage(this->imageOutput));
    }
}

void MainWindow::on_pushButtonEqualizeHistogram_clicked()
{
    Histogram1D h;

    if(this->imageOutput.channels() == 1)
    {
        this->imageOutput = h.equalize(this->imageOutput);
        this->displayOutputImage();
        return;
    }
    else
    {
        cv::Mat image_eq;
        image_eq.create(this->imageOutput.size(), CV_8UC3);
        std::vector<cv::Mat> channels, channels_eq;
        cv::split(this->imageOutput, channels);

        for(unsigned long i = 0; i < channels.size(); i++)
        {
            cv::Mat eq;
            cv::equalizeHist(channels[i], eq);
            channels_eq.push_back(eq);
        }

        merge(channels_eq, image_eq);

        this->imageOutput = image_eq;

        this->displayOutputImage();
    }

}

void MainWindow::on_buttonMorphologicalOperationsPerform_clicked()
{
    QString morphologicalOperation = ui->comboBoxMorphologicalOperation->currentText();
    QString kernelTypeString = ui->comboBoxMorphologicalKernelType->currentText();
    QString imagePaddingMethodString = ui->comboBoxMorphologicalPaddingMethod->currentText();

    int iterationCount = ui->spinBoxMorphologicalIterationCount->value();

    int kernelSize = ui->spinBoxMorphologicalKernelSize->value();

    int kernelShape = cv::MORPH_RECT;
    if(kernelTypeString == "Rectangle")
        kernelShape = cv::MORPH_RECT;
    else if(kernelTypeString == "Cross")
        kernelShape = cv::MORPH_CROSS;
    else if(kernelTypeString == "Ellipse")
        kernelShape = cv::MORPH_ELLIPSE;
    else
        kernelShape = cv::MORPH_RECT;

    int kernelAnchorX = ui->spinBoxMorphologicalKernelAnchorX->value();
    int kernelAnchorY = ui->spinBoxMorphologicalKernelAnchorY->value();
    cv::Point anchorPoint(kernelAnchorX, kernelAnchorY);

    int imagePaddingMethod = cv::BORDER_REPLICATE;
    if(imagePaddingMethodString == "Border Replicate")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap")
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;

    cv::Mat element(7, 7, CV_8U, cv::Scalar(1));
    cv::Mat element5(kernelSize, kernelSize, CV_8U, cv::Scalar(1));

    cv::Mat structureElement = cv::getStructuringElement(kernelShape, cv::Size(kernelSize, kernelSize), anchorPoint);


    if(morphologicalOperation == "Dilation" || morphologicalOperation == "Erosion")
    {
        if(morphologicalOperation == "Dilation")
            cv::dilate(this->imageOutput, this->imageOutput, structureElement, anchorPoint, iterationCount, imagePaddingMethod);
        else
            cv::erode(this->imageOutput, this->imageOutput, structureElement, anchorPoint, iterationCount, imagePaddingMethod);
    }
    else
    {
        int morphOperationCode = cv::MORPH_OPEN;
        if(morphologicalOperation == "Opening")
            morphOperationCode = cv::MORPH_OPEN;
        else if(morphologicalOperation == "Closing")
            morphOperationCode = cv::MORPH_CLOSE;
        else if(morphologicalOperation == "Morphological Gradient")
            morphOperationCode = cv::MORPH_GRADIENT;
        else if(morphologicalOperation == "Top Hat")
            morphOperationCode = cv::MORPH_TOPHAT;
        else if(morphologicalOperation == "Black Hat")
            morphOperationCode = cv::MORPH_BLACKHAT;
        else
            morphOperationCode = cv::MORPH_OPEN;

        cv::morphologyEx(this->imageOutput, this->imageOutput, morphOperationCode, structureElement, anchorPoint, iterationCount, imagePaddingMethod);
    }

    this->displayOutputImage();
}

void MainWindow::on_buttonBlurringPerform_clicked()
{
    QString blurringOperation = ui->comboBoxBlurringMethod->currentText();
    QString imagePaddingMethodString = ui->comboBoxMorphologicalPaddingMethod->currentText();

    int kernelSize = ui->spinBoxBlurringKernelSize->value();

    int kernelAnchorX = ui->spinBoxBlurringKernelAnchorX->value();
    int kernelAnchorY = ui->spinBoxBlurringKernelAnchorY->value();
    cv::Point anchorPoint(kernelAnchorX, kernelAnchorY);

    int imagePaddingMethod = cv::BORDER_REPLICATE;
    if(imagePaddingMethodString == "Border Replicate")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap")
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;

    if(blurringOperation == "Homogeneous Smoothing")
    {
        cv::blur(this->imageOutput, this->imageOutput, cv::Size(kernelSize, kernelSize), anchorPoint, imagePaddingMethod);
    }
    else if(blurringOperation == "Gaussian Smoothing")
    {
        cv::GaussianBlur(this->imageOutput, this->imageOutput, cv::Size(kernelSize, kernelSize), 0, 0, imagePaddingMethod);
    }
    else if(blurringOperation == "Median Smoothing")
    {
        cv::medianBlur(this->imageOutput, this->imageOutput, kernelSize);
    }
    else if(blurringOperation == "Bilateral Smoothing")
    {
        cv::Mat bilateralTemp;
        cv::bilateralFilter(this->imageOutput, bilateralTemp, 5, 100, 100, imagePaddingMethod);
        this->imageOutput = bilateralTemp;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Can't complete blurrung.");
        msgBox.exec();
        return;
    }

    this->displayOutputImage();
}
