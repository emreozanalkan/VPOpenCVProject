#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "pch.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_buttonOpenImage_clicked();

    void on_buttonOpenVideo_clicked();

    void on_buttonOpenCamera_clicked();

    void updateImageFromVideoCapture();

private:
    Ui::MainWindow *ui;

    bool isCameraCapture;

    QMutex mutex1;
    QMutex mutex2;

    QTimer timer;

    cv::VideoCapture videoCapture;

    cv::Mat image;

    // Reference: https://github.com/jayrambhia/qt-opencv-app/blob/master/MatToQImage.cpp
    QImage matToQImage(const cv::Mat& mat);

    QString pickImageDialog();
    QString pickVideoDialog();

    void displayImage();
    void stopDataSources();


};

#endif // MAINWINDOW_H
