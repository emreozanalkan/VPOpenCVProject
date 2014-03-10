#include "camerawindow.h"
#include "ui_camerawindow.h"

#include <QMessageBox>

#include "saltandpepperdialog.h"
#include "Camera/saltandpeppercommand.h"

#include "logodialog.h"
#include "Camera/addlogocommand.h"

#include "histogramdialog.h"
#include "Camera/histogramcommand.h"

#include "morphdialog.h"
#include "Camera/morphcommand.h"

CameraWindow::CameraWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraWindow)
{
    ui->setupUi(this);

    videoCapture = 0;

    commandList = new IPCommandList();

    imageTimer = new QTimer();

    connect(imageTimer, SIGNAL(timeout()), this, SLOT(imageUpdate()));
}

CameraWindow::~CameraWindow()
{
    if(videoCapture->isOpened())
        videoCapture->release();
    delete videoCapture;

    if(imageTimer->isActive())
        imageTimer->stop();
    delete imageTimer;

    delete commandList;

    delete ui;
}

void CameraWindow::uiPlayMode(bool play)
{
    ui->buttonPlay->setEnabled(play);
    ui->spinCameraIndex->setEnabled(play);

    ui->buttonPause->setEnabled(!play);
    ui->comboBoxIPOperation->setEnabled(!play);
    ui->buttonAddIPOperation->setEnabled(!play);
    ui->listIPOperations->setEnabled(!play);
    ui->buttonRemoveSelectedOperation->setEnabled(!play);
}

void CameraWindow::imageUpdate()
{
    try
    {
        videoCapture->operator >>(inputImage);
        inputImage.copyTo(outputImage);
    }
    catch(cv::Exception& e)
    {
        if(videoCapture != 0)
            if(videoCapture->isOpened())
                videoCapture->release();

        this->uiPlayMode(true);

        QMessageBox msgBox;
        msgBox.setText("Camera error");
        msgBox.exec();
        return;


        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
    }

    cv::imshow("Camera Input", inputImage);

    QTimer::singleShot(0, this, SLOT(processUpdate()));
}

void CameraWindow::processUpdate()
{
    commandList->Execute(outputImage);

    cv::imshow("Camera Output", outputImage);
}

void CameraWindow::on_buttonPlay_clicked()
{
    if(videoCapture == 0)
        videoCapture = new cv::VideoCapture();

    if(videoCapture->isOpened())
        videoCapture->release();

    int cameraIndex = ui->spinCameraIndex->value();

    try
    {
        if(videoCapture->open(cameraIndex))
        {
            this->uiPlayMode(false);

            cv::namedWindow("Camera Input", CV_WINDOW_NORMAL);
            cv::namedWindow("Camera Output", CV_WINDOW_KEEPRATIO);

            imageTimer->start(100);
        }
        else
        {
            this->uiPlayMode(true);

            QMessageBox msgBox;
            msgBox.setText("Camera with given index can't be opened");
            msgBox.exec();
            return;
        }
    }
    catch(cv::Exception& e)
    {
        const char* err_msg = e.what();
        std::cout << "exception caught: " << err_msg << std::endl;
    }
}

void CameraWindow::on_buttonPause_clicked()
{
    if(imageTimer->isActive())
        imageTimer->stop();

    if(videoCapture != 0)
        if(videoCapture->isOpened())
            videoCapture->release();

    this->uiPlayMode(true);
}

void CameraWindow::on_buttonAddIPOperation_clicked()
{
    QString IPOperation = ui->comboBoxIPOperation->currentText();

    if(IPOperation == "Add Salt and Pepper Noise")
    {
        SaltAndPepperDialog sapDiag;
        if(sapDiag.exec())
        {
            SaltAndPepperCommand *sapCommand = new SaltAndPepperCommand(sapDiag.addSalt, sapDiag.addPepper, sapDiag.rate);
            commandList->AddCommand(sapCommand);
            ui->listIPOperations->addItem("Salt and Pepper Noise");
        }
    }
    else if(IPOperation == "Add Logo")
    {
        LogoDialog logoDialog;
        if(logoDialog.exec())
        {
            AddLogoCommand *logoCommand = new AddLogoCommand(logoDialog.imageLogo, logoDialog.logoX,
                                                             logoDialog.logoY, logoDialog.logoAlpha,
                                                             logoDialog.logoBeta, logoDialog.logoGamma);
            commandList->AddCommand(logoCommand);
            ui->listIPOperations->addItem("Show Logo");
        }
    }
    else if(IPOperation == "Add Histogram Operation")
    {
        HistogramDialog hisDialog;
        if(hisDialog.exec())
        {
            HistogramCommand *histogramCommand = new HistogramCommand(hisDialog.calculateHistogram, hisDialog.histogramChannel, hisDialog.equalizeHistogram);
            commandList->AddCommand(histogramCommand);
            ui->listIPOperations->addItem("Histogram Operation");
        }
    }
    else if(IPOperation == "Add Morphological Operation")
    {
        MorphDialog morphDialog;
        if(morphDialog.exec())
        {
//            int _morphOperation;
//            int _kernelType;
//            int _imagePaddingMethod;
//            int _iterationCount;
//            cv::Size *_kernelSize;
//            cv::Point *_anchorPoint;
            MorphCommand *morphCommand = new MorphCommand(morphDialog.morphOperation, morphDialog.kernelType,
                                                          morphDialog.imagePaddingMethod, morphDialog.iterationCount,
                                                          morphDialog.kernelSize, morphDialog.anchorPoint);
            commandList->AddCommand(morphCommand);
            ui->listIPOperations->addItem("Morphological Operation");
        }
    }
}

void CameraWindow::on_buttonRemoveSelectedOperation_clicked()
{

}
