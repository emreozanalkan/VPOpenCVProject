#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>

#include "histogram.h"
#include "linefinder.h"
#include "harrisDetector.h"
#include "CameraCalibrator.h"
#include "matcher.h"

#include "camerawindow.h"

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
            this->ui->buttonSaveCurrentImage->setEnabled(true);

            QFileInfo fileInfo(filePath);
            this->addHistory(QString("New Image Loaded: %1").arg(fileInfo.fileName()));

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
    int saltPepperRate = ui->spinBoxSaltAndPepperRate->value();
    if(ui->checkBoxSalt->isChecked())
        this->addSaltNoise(this->imageOutput, saltPepperRate);
    if(ui->checkBoxPepper->isChecked())
        this->addPepperNoise(this->imageOutput, saltPepperRate);

    this->addHistory(QString("Added Salt And Pepper with Rate: %1").arg(saltPepperRate));

    this->displayOutputImage();
}

void MainWindow::setEnabledToolboxes(bool enabled)
{
    ui->groupBoxSaltAndPepper->setEnabled(enabled);
    ui->groupBoxShowLogo->setEnabled(enabled);
    ui->groupBoxConvertColorSpace->setEnabled(enabled);
    ui->groupBoxHistogramOperations->setEnabled(enabled);
    ui->groupBoxMorphologicalOperations->setEnabled(enabled);
    ui->groupBoxBlurring->setEnabled(enabled);
    ui->groupBoxHistory->setEnabled(enabled);
    ui->groupBoxSobelAndLaplacianOperations->setEnabled(enabled);
    ui->groupBoxSharpening->setEnabled(enabled);
    ui->groupBoxCannyEdgeDetect->setEnabled(enabled);
    ui->groupBoxHoughTransform->setEnabled(enabled);
    ui->groupBoxContour->setEnabled(enabled);
    ui->groupBoxHarris->setEnabled(enabled);
    ui->groupBoxFAST->setEnabled(enabled);
    ui->groupBoxSURF->setEnabled(enabled);
    ui->groupBoxSIFT->setEnabled(enabled);
    //ui->groupBoxChessboardCalibration->setEnabled(enabled);
    ui->groupBoxEstimationFindMatches->setEnabled(enabled);
    ui->groupBoxEpipolar->setEnabled(enabled);
    ui->groupBoxHomography->setEnabled(enabled);
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
    this->ui->buttonSaveCurrentImage->setEnabled(false);

    this->imageHistory.clear();
    ui->listWidgetHistory->clear();
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
    imageROI = this->imageOutput(cv::Rect(ui->spinBoxShowLogoX->value(), ui->spinBoxShowLogoY->value(), this->imageLogo.cols, this->imageLogo.rows));

    if(imageROI.channels() == 1)
        if(this->imageLogo.channels() != 1)
            cv::cvtColor(this->imageLogo, this->imageLogo, CV_BGR2GRAY);

    cv::addWeighted(imageROI,
                    ui->doubleSpinBoxShowLogoAlpha->value(),
                    this->imageLogo,
                    ui->doubleSpinBoxShowLogoBeta->value(),
                    ui->doubleSpinBoxShowLogoGamma->value(),
                    imageROI);

    this->addHistory(QString("Logo Added - x:%1 y:%2, alpha: %3").arg(ui->spinBoxShowLogoX->value()).arg(ui->spinBoxShowLogoY->value()).arg(ui->doubleSpinBoxShowLogoAlpha->value()));

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

    this->addHistory(QString("Color Space - from: %2 to %1").arg(ui->comboBoxCurrentColorSpace->currentText()).arg(ui->comboBoxConvertTo->currentText()));

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

        this->addHistory(QString("Histogram Equalized"));

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
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh") // Border Replicate - aaaaaa|abcdefgh|hhhhhhh
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb") // Border Reflect - fedcba|abcdefgh|hgfedcb
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba") // Border Reflect 101 - gfedcb|abcdefgh|gfedcba
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg") // Border Wrap - cdefgh|abcdefgh|abcdefg
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

    this->addHistory(QString("Morph Operation Applied: %1").arg(morphologicalOperation));

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
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg")
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

    this->addHistory(QString("Blurred with: %1").arg(blurringOperation));

    this->displayOutputImage();
}

void MainWindow::addHistory(QString detail)
{
    this->imageHistory.push_back(this->imageOutput.clone());
    ui->listWidgetHistory->addItem(detail);
}

void MainWindow::revertHistory(int index)
{
    if(index <= this->imageHistory.size())
    {
        int historyIndex = index - 1;
        if(historyIndex < 0) historyIndex = 0;
        if(historyIndex >= this->imageHistory.size())
            return;
        this->imageOutput = this->imageHistory.at(historyIndex);
        this->imageHistory.erase(this->imageHistory.begin() + historyIndex, this->imageHistory.end());

        for(int i = ui->listWidgetHistory->count(); i >= index; i--)
        {
            QListWidgetItem* item = ui->listWidgetHistory->takeItem(i);
            ui->listWidgetHistory->removeItemWidget(item);
        }

        this->displayOutputImage();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Can't revert to history.");
        msgBox.exec();
        return;
    }
}

void MainWindow::viewHistory(int index)
{
    if(index >= this->imageHistory.size())
        return;
    QListWidgetItem* item = ui->listWidgetHistory->currentItem();
    cv::namedWindow(item->text().toStdString(), cv::WINDOW_NORMAL);
    cv::imshow(item->text().toStdString(), this->imageHistory.at(index));
}

void MainWindow::on_buttonRevert_clicked()
{
    if(!ui->listWidgetHistory->selectedItems().empty())
    {
        this->revertHistory(ui->listWidgetHistory->currentIndex().row());
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No history item selected.");
        msgBox.exec();
        return;
    }
}

void MainWindow::on_buttonView_clicked()
{
    if(!ui->listWidgetHistory->selectedItems().empty())
    {
        this->viewHistory(ui->listWidgetHistory->currentIndex().row());
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No history item selected.");
        msgBox.exec();
        return;
    }
}

void MainWindow::on_buttonSobelLaplacianPerform_clicked()
{
    QString sobelLaplacianOperation = ui->comboBoxBlurringMethod->currentText();
    QString imagePaddingMethodString = ui->comboBoxMorphologicalPaddingMethod->currentText();
    QString outputDepthString = ui->comboBoxSobelLaplacianOutputDepth->currentText();

    int imagePaddingMethod = cv::BORDER_REPLICATE;
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg")
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;

    int outputDepth = CV_8U;
    if(outputDepthString == "CV_8U")
        outputDepth =  CV_8U;
    else if(outputDepthString == "CV_16U/CV_16S")
        outputDepth = CV_16U;
    else if(outputDepthString == "CV_32F")
        outputDepth = CV_32F;
    else if(outputDepthString == "CV_64F")
        outputDepth = CV_64F;
    else
        outputDepth = CV_8U;

    double scaleFactor = ui->doubleSpinBoxSobelLaplacianScaleFactor->value();
    double deltaOffset =  ui->doubleSpinBoxSobelLaplacianDeltaOffset->value();

    if(sobelLaplacianOperation == "Sobel")
    {
        int sobelKernelSize = ui->spinBoxSobelLaplacianSobelKernelSize->value();
        int sobelX = ui->checkBoxSobelLaplacianSobelXOrder->isChecked() ? 1 : 0;
        int sobelY = ui->checkBoxSobelLaplacianSobelYOrder->isChecked() ? 1 : 0;
        cv::Sobel(this->imageOutput, this->imageOutput, outputDepth, sobelX, sobelY, sobelKernelSize, scaleFactor, deltaOffset, imagePaddingMethod);
    }
    else
    {
        int laplacianApertureSize = ui->spinBoxSobelLaplacianLaplacianApertureSize->value();

        cv::Laplacian(this->imageOutput, this->imageOutput, outputDepth, laplacianApertureSize, scaleFactor, deltaOffset, imagePaddingMethod);
    }

    this->addHistory(QString("Derivative filter with: %1").arg(sobelLaplacianOperation));

    this->displayOutputImage();
}

void MainWindow::on_comboBoxSobelLaplacianOperation_currentTextChanged(const QString &arg1)
{
    bool isSobel = (arg1 == "Sobel") ? true : false;

    ui->spinBoxSobelLaplacianSobelKernelSize->setEnabled(isSobel);
    ui->checkBoxSobelLaplacianSobelXOrder->setEnabled(isSobel);
    ui->checkBoxSobelLaplacianSobelYOrder->setEnabled(isSobel);

    ui->spinBoxSobelLaplacianLaplacianApertureSize->setEnabled(!isSobel);
}

void MainWindow::on_buttonSharpeningPerform_clicked()
{
    QString imagePaddingMethodString = ui->comboBoxSharpeningPaddingMethod->currentText();
    int kernelSize = ui->spinBoxSharpeningKernelSize->value();
    double gaussianSigma = ui->doubleSpinBoxSharpeningGaussianSigma->value();

    int imagePaddingMethod = cv::BORDER_REPLICATE;
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg")
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;

    double filterWeight = ui->doubleSpinBoxSharpeningFilterWeight->value();

    cv::Mat temp;

    cv::GaussianBlur(this->imageOutput, temp, cv::Size(kernelSize, kernelSize), gaussianSigma, 0, imagePaddingMethod);
    cv::addWeighted(this->imageOutput, 1.5, temp, filterWeight, 0, this->imageOutput);

    this->addHistory(QString("Sharpened - ksize: %1, sigma: %2, weight: %3").arg(kernelSize).arg(gaussianSigma).arg(filterWeight));

    this->displayOutputImage();
}

void MainWindow::on_buttonCannyEdgeDetectPerform_clicked()
{
    double threshold1 = ui->doubleSpinBoxCannyEdgeDetectThreshold1->value();
    double threshold2 = ui->doubleSpinBoxCannyEdgeDetectThreshold2->value();
    int apertureSize = ui->spinBoxCannyEdgeDetectApertureSize->value();
    bool L2gradient = ui->checkBoxCannyEdgeDetectL2gradient->isChecked();

    cv::Mat temp;
    cv::Canny(this->imageOutput, temp, threshold1, threshold2, apertureSize, L2gradient);

    this->imageOutput = temp;

    this->addHistory(QString("Canny edge detector - t1:%1, t2:%2, ksize:%3").arg(threshold1).arg(threshold2).arg(apertureSize));

    this->displayOutputImage();
}

void MainWindow::on_comboBoxHoughTransformFind_currentTextChanged(const QString &arg1)
{
    bool findLines = (arg1 == "Lines") ? true : false;

    ui->groupBoxFindLinesParameters->setEnabled(findLines);
    ui->groupBoxFindCirclesParameters->setEnabled(!findLines);
}

void MainWindow::on_buttonHoughTransformFind_clicked()
{
    QString houghFindOperation = ui->comboBoxHoughTransformFind->currentText();

    if(houghFindOperation == "Lines")
    {
        double rho = ui->doubleSpinBoxHoughTransformRho->value();
        double theta = ui->doubleSpinBoxHoughTransformTheta->value();
        int threshold = ui->spinBoxHoughTransformThreshold->value();
        double srn = ui->doubleSpinBoxHoughTransformSRN->value();
        double stn = ui->doubleSpinBoxHoughTransformSTN->value();

        // Create LineFinder instance
        LineFinder ld;
        ld.setLineLengthAndGap(srn, stn);
        ld.setMinVote(threshold);
        ld.setAccResolution(rho, theta);

        cv::Mat contours;
        cv::Canny(this->imageOutput, contours, 125, 350);

        // Detect lines
        ld.findLines(contours);
        ld.drawDetectedLines(this->imageOutput);

    }
    else
    {
        int method = CV_HOUGH_GRADIENT;
        double dp = ui->doubleSpinBoxHoughTransformDP->value();
        double minDist = ui->doubleSpinBoxHoughTransformMinDist->value();
        double param1 = ui->doubleSpinBoxHoughTransformParam1->value();
        double param2 = ui->doubleSpinBoxHoughTransformParam2->value();
        int minRadius = ui->spinBoxHoughTransformMinRadius->value();
        int maxRadius = ui->spinBoxHoughTransformMaxRadius->value();

        cv::GaussianBlur(this->imageOutput, this->imageOutput, cv::Size(5,5),1.5);

        std::vector<cv::Vec3f> circles;

        if(this->imageOutput.channels() == 3)
            cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2GRAY);

        cv::HoughCircles(this->imageOutput, circles, method, dp, minDist, param1, param2, minRadius, maxRadius);

        cv::Mat circleImage = this->imageInput.clone();

        std::vector<cv::Vec3f>::const_iterator itc = circles.begin();

        while (itc != circles.end()) {

          cv::circle(circleImage,
              cv::Point((*itc)[0], (*itc)[1]), // circle centre
              (*itc)[2], // circle radius
              cv::Scalar(255), // color
              2); // thickness

          ++itc;
        }

        this->imageOutput = circleImage;
    }

    this->addHistory(QString("Hough Transform: %1 method").arg(houghFindOperation));

    this->displayOutputImage();
}

void MainWindow::on_buttonContourFind_clicked()
{
    QString contourFindModeString = ui->comboBoxContourMode->currentText();
    QString contourFindMethodString = ui->comboBoxContourMethod->currentText();

    int contourFindMode = CV_RETR_EXTERNAL;
    if(contourFindModeString == "CV_RETR_EXTERNAL")
        contourFindMode = CV_RETR_EXTERNAL;
    else if(contourFindModeString == "CV_RETR_LIST")
        contourFindMode = CV_RETR_LIST;
    else if(contourFindModeString == "CV_RETR_CCOMP")
        contourFindMode = CV_RETR_CCOMP;
    else if(contourFindModeString == "CV_RETR_TREE")
        contourFindMode = CV_RETR_TREE;
    else
        contourFindMode = CV_RETR_EXTERNAL;

    int contourFindMethod = CV_CHAIN_APPROX_NONE;
    if(contourFindMethodString == "CV_CHAIN_APPROX_NONE")
        contourFindMethod = CV_CHAIN_APPROX_NONE;
    else if(contourFindMethodString == "CV_CHAIN_APPROX_SIMPLE")
        contourFindMethod = CV_CHAIN_APPROX_SIMPLE;
    else if(contourFindMethodString == "CV_CHAIN_APPROX_TC89_L1")
        contourFindMethod = CV_CHAIN_APPROX_TC89_L1;
    else if(contourFindMethodString == "CV_CHAIN_APPROX_TC89_KCOS")
        contourFindMethod = CV_CHAIN_APPROX_TC89_KCOS;
    else
        contourFindMethod = CV_RETR_EXTERNAL;

    int offsetX = ui->spinBoxlContourOffsetX->value();
    int offsetY = ui->spinBoxlContourOffsetY->value();

    int threshold = ui->spinBoxlContourBinaryThreshold->value();

    int minContourSize = ui->spinBoxContourMinContourSize->value();
    int maxContourSize = ui->spinBoxContourMaxContourSize->value();
    bool boundingBox = ui->checkBoxContourBoundingBox->isChecked();
    bool boundingMinCircle = ui->checkBoxContourBoundingMinCircle->isChecked();

    if(this->imageOutput.channels() == 3)
        cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2GRAY);

    cv::threshold(this->imageOutput, this->imageOutput, threshold, 255, cv::THRESH_BINARY );

    // Get the contours of the connected components
    std::vector<std::vector<cv::Point> > contours;
    cv::findContours(this->imageOutput,
                     contours, // a vector of contours
                     contourFindMode, // retrieve the external contours
                     contourFindMethod,
                     cv::Size(offsetX, offsetY)); // retrieve all pixels of each contours

    // draw black contours on white image
    //cv::Mat result(this->imageOutput.size(), cv::Scalar(255));
    cv::Mat result(this->imageOutput.rows, this->imageOutput.cols, CV_8UC3);
    //    cv::drawContours(result,contours,
    //        -1, // draw all contours
    //        cv::Scalar(0), // in black
    //        2); // with a thickness of 2


    // Eliminate too short or too long contours
    std::vector<std::vector<cv::Point> >::const_iterator itc= contours.begin();
    while (itc!=contours.end()) {

        if (itc->size() < minContourSize || itc->size() > maxContourSize)
            itc= contours.erase(itc);
        else
            ++itc;
    }

    /// Draw contours
    cv::RNG rng(12345);
    float radius;
    cv::Point2f center;
    //cv::Mat drawing = cv::Mat::zeros( canny_output.size(), CV_8UC3 );
    cv::Scalar color;
    for( int i = 0; i< contours.size(); i++ )
    {
        color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255));
        drawContours(result, contours, i, color, 1);

        if(boundingBox)
        {
            cv::Rect rect= cv::boundingRect(cv::Mat(contours[i]));
            cv::rectangle(result, rect, cv::Scalar(0), 1);
        }
        if(boundingMinCircle)
        {
            cv::minEnclosingCircle(cv::Mat(contours[i]), center, radius);
            cv::circle(result, center, static_cast<int>(radius), cv::Scalar(0), 1);
        }
    }

    this->imageOutput = result;

    this->addHistory(QString("Found contours of connected objects"));

    this->displayOutputImage();
}

void MainWindow::on_buttonHarrisPerform_clicked()
{
    QString imagePaddingMethodString = ui->comboBoxHarrisPaddingMethod->currentText();

    int imagePaddingMethod = cv::BORDER_REPLICATE;
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh")
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb")
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba")
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg")
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;


//    // size of neighbourhood for derivatives smoothing
    int neighbourhood = ui->spinBoxHarrisDerivativeSizeNeighborhood->value();
//    // aperture for gradient computation
    int aperture = ui->spinBoxHarrisAperture->value();
//    // Harris parameter
    double k = ui->doubleSpinBoxHarrisHarrisParameter->value();
//    // maximum strength for threshold computation
    double maxStrength = ui->doubleSpinBoxHarrisThresholdStrength->value();
//    // calculated threshold (internal)
//    double threshold;
//    // size of neighbourhood for non-max suppression
    int nonMaxSize = ui->spinBoxHarrisNonMaxSizeNeighborhood->value();


    if(this->imageOutput.channels() == 3)
        cv::cvtColor(this->imageOutput, this->imageOutput, CV_BGR2GRAY);

    // Create Harris detector instance
    HarrisDetector harris;

    harris.setParams(neighbourhood, aperture, k, maxStrength, nonMaxSize, imagePaddingMethod);

    // Compute Harris values
    harris.detect(this->imageOutput);
    // Detect Harris corners
    std::vector<cv::Point> pts;
    harris.getCorners(pts, 0.01);
    // Draw Harris corners
    harris.drawOnImage(this->imageOutput, pts);

    this->addHistory(QString("Extract corners using Harris and apply non-maximal suppression."));

    this->displayOutputImage();
}

void MainWindow::on_buttonFASTPerform_clicked()
{
    int threshold = ui->spinBoxFASTThreshold->value();
    bool nonMaxSupression = ui->checkBoxFASTNonMaxSupression->isChecked();
    bool keypointColors = ui->checkBoxFASTKeypointColors->isChecked();

    QString keyPointDrawingFlagString = ui->comboBoxFASTKeypointDrawingFlag->currentText();

    int keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    if(keyPointDrawingFlagString == "DEFAULT;")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    else if(keyPointDrawingFlagString == "DRAW_OVER_OUTIMG")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_OVER_OUTIMG;
    else if(keyPointDrawingFlagString == "NOT_DRAW_SINGLE_POINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;
    else if(keyPointDrawingFlagString == "DRAW_RICH_KEYPOINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
    else
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;

    // vector of keypoints
    std::vector<cv::KeyPoint> keypoints;

    cv::FastFeatureDetector fast(threshold, nonMaxSupression);

    fast.detect(this->imageOutput, keypoints);

    if(keypointColors)
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar::all(-1), keyPointDrawingFlag);
    else
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar(255,255,255), keyPointDrawingFlag);

    this->addHistory(QString("FAST - threshold: %1, nonMaxSupression: %2").arg(threshold).arg(nonMaxSupression));

    this->displayOutputImage();
}

void MainWindow::on_buttonSURFPerform_clicked()
{
    int minHessian = ui->spinBoxSURFMinHessian->value();
    bool keypointColors = ui->checkBoxSURFKeypointColors->isChecked();

    QString keyPointDrawingFlagString = ui->comboBoxFASTKeypointDrawingFlag->currentText();

    int keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    if(keyPointDrawingFlagString == "DEFAULT;")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    else if(keyPointDrawingFlagString == "DRAW_OVER_OUTIMG")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_OVER_OUTIMG;
    else if(keyPointDrawingFlagString == "NOT_DRAW_SINGLE_POINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;
    else if(keyPointDrawingFlagString == "DRAW_RICH_KEYPOINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
    else
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;

    // vector of keypoints
    std::vector<cv::KeyPoint> keypoints;

    // Construct the SURF feature detector object
    cv::SurfFeatureDetector surf(minHessian);
    // Detect the SURF features
    surf.detect(this->imageOutput, keypoints);

    //cv::Mat featureImage;

    if(keypointColors)
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar::all(-1), keyPointDrawingFlag);
    else
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar(255,255,255), keyPointDrawingFlag);

    this->addHistory(QString("SURF - minHessian: %1").arg(minHessian));

    this->displayOutputImage();
}

void MainWindow::on_buttonSIFTPerform_clicked()
{
    double featureThreshold = ui->doubleSpinBoxSIFTFeatureThreshold->value();
    double edgeThreshold = ui->doubleSpinBoxSIFTEdgeThreshold->value();

    bool keypointColors = ui->checkBoxSURFKeypointColors->isChecked();

    QString keyPointDrawingFlagString = ui->comboBoxFASTKeypointDrawingFlag->currentText();

    int keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    if(keyPointDrawingFlagString == "DEFAULT;")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    else if(keyPointDrawingFlagString == "DRAW_OVER_OUTIMG")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_OVER_OUTIMG;
    else if(keyPointDrawingFlagString == "NOT_DRAW_SINGLE_POINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;
    else if(keyPointDrawingFlagString == "DRAW_RICH_KEYPOINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
    else
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;

    // vector of keypoints
    std::vector<cv::KeyPoint> keypoints;

    cv::SiftFeatureDetector sift(featureThreshold, edgeThreshold);

    // Detect the SURF features
    sift.detect(this->imageOutput, keypoints);

    if(keypointColors)
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar::all(-1), keyPointDrawingFlag);
    else
        cv::drawKeypoints(this->imageOutput, keypoints, this->imageOutput, cv::Scalar(255,255,255), keyPointDrawingFlag);

    this->addHistory(QString("SIFT - featureThreshold: %1, edgeThreshold: %2").arg(featureThreshold).arg(edgeThreshold));

    this->displayOutputImage();
}

void MainWindow::on_buttonSaveCurrentImage_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save File",
                                                     QDir::homePath().append("/untitled.png"),
                                                    "Image Files (*.png *.jpg *.jpeg *.bmp)");


    if(!fileName.isEmpty())
        cv::imwrite(fileName.toStdString(), this->imageOutput);
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Can't save image.");
        msgBox.exec();
        return;
    }
}

void MainWindow::on_buttonEstimationCalibrate_clicked()
{
    int numberOfCornersX = ui->spinBoxNumberCornersX->value();
    int numberOfCornersY = ui->spinBoxNumberCornerY->value();

    cv::Mat image;
    std::vector<std::string> filelist;

    // generate list of chessboard image filename
    for (int i = 1; i <= 20; i++) {

        std::stringstream str;
        str << "/Users/emreozanalkan/Desktop/3241_Code/3241OS_images/images/chessboards/chessboard" << std::setw(2) << std::setfill('0') << i << ".jpg";
        std::cout << str.str() << std::endl;

        filelist.push_back(str.str());
        image= cv::imread(str.str(),0);
        cv::imshow("Image",image);
        cv::waitKey(300);
    }

    cv::destroyWindow("Image");


    // Create calibrator object
    CameraCalibrator cameraCalibrator;
    // add the corners from the chessboard
    cv::Size boardSize(numberOfCornersX, numberOfCornersY);
    cameraCalibrator.addChessboardPoints(
        filelist,	// filenames of chessboard image
        boardSize);	// size of chessboard
        // calibrate the camera
    //	cameraCalibrator.setCalibrationFlag(true,true);

    cv::Size imageSize;
    imageSize = image.size();
    cameraCalibrator.calibrate(imageSize);

    // Image Undistortion
//    image = cv::imread(filelist[6]);
//    cv::Mat uImage= cameraCalibrator.remap(image);

    // generate list of chessboard image filename
    for (int i = 0; i < 20; i++) {
        image = cv::imread(filelist[i]);
        cv::Mat uImage = cameraCalibrator.remap(image);

        cv::imshow("Undistorted Image",uImage);
        cv::waitKey(300);
    }

    // display camera matrix
    cv::Mat cameraMatrix= cameraCalibrator.getCameraMatrix();
    std::cout << " Camera intrinsic: " << cameraMatrix.rows << "x" << cameraMatrix.cols << std::endl;
    std::cout << cameraMatrix.at<double>(0,0) << " " << cameraMatrix.at<double>(0,1) << " " << cameraMatrix.at<double>(0,2) << std::endl;
    std::cout << cameraMatrix.at<double>(1,0) << " " << cameraMatrix.at<double>(1,1) << " " << cameraMatrix.at<double>(1,2) << std::endl;
    std::cout << cameraMatrix.at<double>(2,0) << " " << cameraMatrix.at<double>(2,1) << " " << cameraMatrix.at<double>(2,2) << std::endl;
}

void MainWindow::on_buttonEstimationFindMatches_clicked()
{
    if(!this->imageMatching.data)
    {
        QMessageBox msgBox;
        msgBox.setText("Matching image is empty. Please select matching image.");
        msgBox.exec();
        return;
    }

    int minHessian = ui->spinBoxEstimationSURFMinHessian->value();

    QString keyPointDrawingFlagString = ui->comboBoxFASTKeypointDrawingFlag->currentText();
    QString fundementalMatrixMethodString = ui->comboBoxEstimationFindMatchesFundemental->currentText();

    int keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    if(keyPointDrawingFlagString == "DEFAULT;")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;
    else if(keyPointDrawingFlagString == "DRAW_OVER_OUTIMG")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_OVER_OUTIMG;
    else if(keyPointDrawingFlagString == "NOT_DRAW_SINGLE_POINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS;
    else if(keyPointDrawingFlagString == "DRAW_RICH_KEYPOINTS")
        keyPointDrawingFlag = cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS;
    else
        keyPointDrawingFlag = cv::DrawMatchesFlags::DEFAULT;


    int fundementalMatrixMethod = CV_FM_7POINT;
    if(fundementalMatrixMethodString == "7-point;")
        fundementalMatrixMethod = CV_FM_7POINT;
    else if(fundementalMatrixMethodString == "8-point")
        fundementalMatrixMethod = CV_FM_8POINT;
    else if(fundementalMatrixMethodString == "RANSAC")
        fundementalMatrixMethod = CV_FM_RANSAC;
    else
        fundementalMatrixMethod = CV_FM_7POINT;



    // vector of keypoints
    std::vector<cv::KeyPoint> keypoints1;
    std::vector<cv::KeyPoint> keypoints2;

    // Construction of the SURF feature detector
    cv::SurfFeatureDetector surf(minHessian);

    // Detection of the SURF features
    surf.detect(this->imageOutput,keypoints1);
    surf.detect(this->imageMatching,keypoints2);

    std::cout << "Number of SURF points (1): " << keypoints1.size() << std::endl;
    std::cout << "Number of SURF points (2): " << keypoints2.size() << std::endl;

    // Draw the kepoints
    cv::Mat imageKP;
    cv::drawKeypoints(this->imageOutput,keypoints1,imageKP,cv::Scalar(255,255,255), keyPointDrawingFlag);
    cv::namedWindow("Right SURF Features", cv::WINDOW_NORMAL);
    cv::imshow("Right SURF Features",imageKP);
    cv::drawKeypoints(this->imageMatching,keypoints2,imageKP,cv::Scalar(255,255,255), keyPointDrawingFlag);
    cv::namedWindow("Left SURF Features", cv::WINDOW_NORMAL);
    cv::imshow("Left SURF Features",imageKP);


    // Construction of the SURF descriptor extractor
    cv::SurfDescriptorExtractor surfDesc;

    // Extraction of the SURF descriptors
    cv::Mat descriptors1, descriptors2;
    surfDesc.compute(this->imageOutput, keypoints1, descriptors1);
    surfDesc.compute(this->imageMatching, keypoints2, descriptors2);


    // Construction of the matcher
    cv::BruteForceMatcher<cv::L2<float> > matcher;

    // Match the two image descriptors
    std::vector<cv::DMatch> matches;
    matcher.match(descriptors1,descriptors2, matches);


    // Draw the selected matches
    cv::Mat imageMatches;
    cv::drawMatches(this->imageOutput,keypoints1,  // 1st image and its keypoints
                    this->imageMatching,keypoints2,  // 2nd image and its keypoints
//					selMatches,			// the matches
                    matches,			// the matches
                    imageMatches,		// the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::namedWindow("Matches", cv::WINDOW_NORMAL);
    cv::imshow("Matches",imageMatches);

    // Convert 1 vector of keypoints into
    // 2 vectors of Point2f
    std::vector<int> pointIndexes1;
    std::vector<int> pointIndexes2;
    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
         it!= matches.end(); ++it) {

             // Get the indexes of the selected matched keypoints
             pointIndexes1.push_back(it->queryIdx);
             pointIndexes2.push_back(it->trainIdx);
    }


    // Convert keypoints into Point2f
    std::vector<cv::Point2f> selPoints1, selPoints2;
    cv::KeyPoint::convert(keypoints1,selPoints1,pointIndexes1);
    cv::KeyPoint::convert(keypoints2,selPoints2,pointIndexes2);

    // check by drawing the points
    std::vector<cv::Point2f>::const_iterator it= selPoints1.begin();
    while (it!=selPoints1.end()) {

        // draw a circle at each corner location
        cv::circle(this->imageOutput,*it,3,cv::Scalar(255,255,255),2);
        ++it;
    }

    it= selPoints2.begin();
    while (it!=selPoints2.end()) {

        // draw a circle at each corner location
        cv::circle(this->imageMatching,*it,3,cv::Scalar(255,255,255),2);
        ++it;
    }

    // Compute F matrix
    cv::Mat fundemental= cv::findFundamentalMat(
        cv::Mat(selPoints1), // points in first image
        cv::Mat(selPoints2), // points in second image
        fundementalMatrixMethod);       // 7-point method

        std::cout << "F-Matrix size: " << fundemental.rows << "," << fundemental.cols << std::endl;
}

void MainWindow::on_buttonEstimationLoadMatchingImage_clicked()
{
    QString filePath = this->pickImageDialog();

        if(filePath != 0)
        {
            this->imageMatching = cv::imread(filePath.toStdString());

            if(!this->imageMatching.data)
            {
                QMessageBox msgBox;
                msgBox.setText("Image you selected didn't loaded.");
                msgBox.exec();
                return;
            }

            this->displayOutputImage();
            namedWindow("Matching Image", cv::WINDOW_NORMAL);
            imshow("Matching Image", this->imageMatching);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("You didn't select image.");
            msgBox.exec();
        }
}

void MainWindow::on_buttonEpipolarLoadMatchingImage_clicked()
{
    this->on_buttonEstimationLoadMatchingImage_clicked();
}

void MainWindow::on_buttonEpipolarFindEpipolarLines_clicked()
{
    if(!this->imageMatching.data)
    {
        QMessageBox msgBox;
        msgBox.setText("Matching image is empty. Please select matching image.");
        msgBox.exec();
        return;
    }

    double epipolarRatio = ui->doubleSpinBoxEpipolarRatio->value();
    int minHessian = ui->spinBoxEpipolarSURFMinHessian->value();

    double confidenceLevel = ui->doubleSpinBoxEpipolarConfidenceLevel->value();
    double minDistance = ui->doubleSpinBoxEpipolarMinDistanceToEpipolar->value();


    // Prepare the matcher
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(confidenceLevel);
    rmatcher.setMinDistanceToEpipolar(minDistance);
    rmatcher.setRatio(epipolarRatio);
    cv::Ptr<cv::FeatureDetector> pfd= new cv::SurfFeatureDetector(minHessian);
    rmatcher.setFeatureDetector(pfd);

    // Match the two images
    std::vector<cv::DMatch> matches;
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat fundemental = rmatcher.match(this->imageOutput, this->imageMatching, matches, keypoints1, keypoints2);

    // draw the matches
    cv::Mat imageMatches;
    cv::drawMatches(this->imageOutput,keypoints1,  // 1st image and its keypoints
                    this->imageMatching,keypoints2,  // 2nd image and its keypoints
                    matches,			// the matches
                    imageMatches,		// the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::namedWindow("Matches", cv::WINDOW_NORMAL);
    cv::imshow("Matches",imageMatches);


    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;

    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
             it!= matches.end(); ++it) {

             // Get the position of left keypoints
             float x= keypoints1[it->queryIdx].pt.x;
             float y= keypoints1[it->queryIdx].pt.y;
             points1.push_back(cv::Point2f(x,y));
             cv::circle(this->imageOutput,cv::Point(x,y),3,cv::Scalar(255,255,255),3);
             // Get the position of right keypoints
             x= keypoints2[it->trainIdx].pt.x;
             y= keypoints2[it->trainIdx].pt.y;
             cv::circle(this->imageMatching,cv::Point(x,y),3,cv::Scalar(255,255,255),3);
             points2.push_back(cv::Point2f(x,y));
    }

    // Draw the epipolar lines
    std::vector<cv::Vec3f> lines1;
    cv::computeCorrespondEpilines(cv::Mat(points1),1,fundemental,lines1);

    for (std::vector<cv::Vec3f>::const_iterator it= lines1.begin();
             it!=lines1.end(); ++it) {

             cv::line(this->imageMatching,cv::Point(0,-(*it)[2]/(*it)[1]),
                             cv::Point(this->imageMatching.cols,-((*it)[2]+(*it)[0]*this->imageMatching.cols)/(*it)[1]),
                             cv::Scalar(255,255,255));
    }

    std::vector<cv::Vec3f> lines2;
    cv::computeCorrespondEpilines(cv::Mat(points2),2,fundemental,lines2);

    for (std::vector<cv::Vec3f>::const_iterator it= lines2.begin();
             it!=lines2.end(); ++it) {

             cv::line(this->imageOutput,cv::Point(0,-(*it)[2]/(*it)[1]),
                             cv::Point(this->imageOutput.cols,-((*it)[2]+(*it)[0]*this->imageOutput.cols)/(*it)[1]),
                             cv::Scalar(255,255,255));
    }

    // Display the images with epipolar lines
    cv::namedWindow("Right Image Epilines (RANSAC)", cv::WINDOW_NORMAL);
    cv::imshow("Right Image Epilines (RANSAC)",this->imageOutput);
    cv::namedWindow("Left Image Epilines (RANSAC)", cv::WINDOW_NORMAL);
    cv::imshow("Left Image Epilines (RANSAC)",this->imageMatching);

    this->addHistory(QString("Epipolar Lines"));
}

void MainWindow::on_buttonHomographyLoadMatchingImage_clicked()
{
    this->on_buttonEstimationLoadMatchingImage_clicked();
}

void MainWindow::on_buttonHomographyCalculateHomography_clicked()
{
    if(!this->imageMatching.data)
    {
        QMessageBox msgBox;
        msgBox.setText("Matching image is empty. Please select matching image.");
        msgBox.exec();
        return;
    }


    double epipolarRatio = ui->doubleSpinBoxHomographyRatio->value();
    int minHessian = ui->spinBoxHomographySURFMinHessian->value();

    double confidenceLevel = ui->doubleSpinBoxHomographyConfidenceLevel->value();
    double minDistance = ui->doubleSpinBoxHomographyMinDistanceToEpipolar->value();


    // Prepare the matcher
    RobustMatcher rmatcher;
    rmatcher.setConfidenceLevel(0.98);
    rmatcher.setMinDistanceToEpipolar(1.0);
    rmatcher.setRatio(0.65f);
    cv::Ptr<cv::FeatureDetector> pfd= new cv::SurfFeatureDetector(10);
    rmatcher.setFeatureDetector(pfd);

    // Match the two images
    std::vector<cv::DMatch> matches;
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat fundemental = rmatcher.match(this->imageOutput, this->imageMatching,matches, keypoints1, keypoints2);

    // draw the matches
    cv::Mat imageMatches;
    cv::drawMatches(this->imageOutput,keypoints1,  // 1st image and its keypoints
                    this->imageMatching,keypoints2,  // 2nd image and its keypoints
                    matches,			// the matches
                    imageMatches,		// the image produced
                    cv::Scalar(255,255,255)); // color of the lines
    cv::namedWindow("Matches", CV_WINDOW_NORMAL);
    cv::imshow("Matches",imageMatches);

    // Convert keypoints into Point2f
    std::vector<cv::Point2f> points1, points2;
    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
         it!= matches.end(); ++it) {

             // Get the position of left keypoints
             float x= keypoints1[it->queryIdx].pt.x;
             float y= keypoints1[it->queryIdx].pt.y;
             points1.push_back(cv::Point2f(x,y));
             // Get the position of right keypoints
             x= keypoints2[it->trainIdx].pt.x;
             y= keypoints2[it->trainIdx].pt.y;
             points2.push_back(cv::Point2f(x,y));
    }

    std::cout << points1.size() << " " << points2.size() << std::endl;

    // Find the homography between image 1 and image 2
    std::vector<uchar> inliers(points1.size(),0);
    cv::Mat homography= cv::findHomography(
        cv::Mat(points1),cv::Mat(points2), // corresponding points
        inliers,	// outputed inliers matches
        CV_RANSAC,	// RANSAC method
        1.);	    // max distance to reprojection point

    // Draw the inlier points
    std::vector<cv::Point2f>::const_iterator itPts= points1.begin();
    std::vector<uchar>::const_iterator itIn= inliers.begin();
    while (itPts!=points1.end()) {

        // draw a circle at each inlier location
        if (*itIn)
            cv::circle(this->imageOutput,*itPts,3,cv::Scalar(255,255,255),2);

        ++itPts;
        ++itIn;
    }

    itPts= points2.begin();
    itIn= inliers.begin();
    while (itPts!=points2.end()) {

        // draw a circle at each inlier location
        if (*itIn)
            cv::circle(this->imageMatching,*itPts,3,cv::Scalar(255,255,255),2);

        ++itPts;
        ++itIn;
    }

    // Display the images with points
    cv::namedWindow("Image 1 Homography Points", CV_WINDOW_NORMAL);
    cv::imshow("Image 1 Homography Points",this->imageOutput);
    cv::namedWindow("Image 2 Homography Points", CV_WINDOW_NORMAL);
    cv::imshow("Image 2 Homography Points",this->imageMatching);

    // Warp image 1 to image 2
    cv::Mat result;
    cv::warpPerspective(this->imageOutput, // input image
        result,			// output image
        homography,		// homography
        cv::Size(2*this->imageOutput.cols,this->imageOutput.rows)); // size of output image

    // Copy image 1 on the first half of full image
    cv::Mat half(result,cv::Rect(0,0,this->imageMatching.cols,this->imageMatching.rows));
    this->imageMatching.copyTo(half);

    // Display the warp image
    cv::namedWindow("After warping", CV_WINDOW_NORMAL);
    cv::imshow("After warping",result);

}

void MainWindow::on_buttonCameraToolbox_clicked()
{
    CameraWindow *cameraWindow = new CameraWindow();

    cameraWindow->move(0, 0);

    cameraWindow->show();
}
