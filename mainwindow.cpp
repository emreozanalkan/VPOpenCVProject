#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

#include "pch.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonOpenImage_clicked()
{
    cv::Mat image;
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", ".", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    image = cv::imread(fileName.toStdString());
    cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    cv::imshow("Original Image", image);
}

void MainWindow::on_buttonOpenVideo_clicked()
{
    cv::Mat image;
    //QString fileName = QFileDialog::getOpenFileName(this, "Open Image", ".", "Image Files (*.avi *.mov *.jpeg *.bmp)");
    //image = cv::imread(fileName.toStdString());
    //cv::namedWindow("Original Image", cv::WINDOW_NORMAL);
    //cv::imshow("Original Image", image);

    QFileDialog dialog;

    //QString fileName = dialog.getOpenFileName(this, "Open Image", ".", "Image Files (*.avi *.mov *.jpeg *.bmp)");
    //QString fileName = "/Users/emreozanalkan/Dropbox/Camera Uploads/2012-06-26 03.01.56.mov";
    QString fileName = "/Users/emreozanalkan/Desktop/3241_Code/3241OS_images/images/bike.avi";


    cv::VideoCapture capture(fileName.toStdString());

    if(!capture.isOpened())
        return;

    cv::waitKey(2000);

    //cv::namedWindow("video", cv::WINDOW_NORMAL);

    while(capture.grab())
    {
        capture>>image;
        cv::imshow("video", image);

        cv::waitKey(30);
    }
}

void MainWindow::on_buttonOpenCamera_clicked()
{
    cv::VideoCapture capture(CV_CAP_ANY);

    if(!capture.isOpened())
        return;

    cv::Mat image;

    while(capture.grab())
    {
        capture>>image;
        cv::imshow("video", image);

        cv::waitKey(30);
    }

}
