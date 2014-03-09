#include "camerawindow.h"
#include "ui_camerawindow.h"

#include <QMessageBox>

#include "saltandpepperdialog.h"

CameraWindow::CameraWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CameraWindow)
{
    ui->setupUi(this);

    timer = new QTimer();

    connect(timer, SIGNAL(timeout()), this, SLOT(cameraImageUpdate()));
}

CameraWindow::~CameraWindow()
{
    if(videoCapture->isOpened())
        videoCapture->release();
    delete videoCapture;

    if(timer->isActive())
        timer->stop();
    delete timer;

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

void CameraWindow::cameraImageUpdate()
{
    try
    {
        videoCapture->operator >>(image);
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

    cv::imshow("Camera Input", image);
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
            cv::namedWindow("Camera Output", CV_WINDOW_NORMAL);

            timer->start(100);
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
    if(timer->isActive())
        timer->stop();

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
            QMessageBox msgBox;
            msgBox.setText("Salt and Pepper added :)");
            msgBox.exec();
            return;
        }
    }
}

void CameraWindow::on_buttonRemoveSelectedOperation_clicked()
{

}
