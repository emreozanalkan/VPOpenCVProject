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

    void on_buttonLoadImage_clicked();

    void on_buttonAddSaltAndPepper_clicked();

    void on_buttonResetOutput_clicked();

    void on_buttonClearAndCloseAll_clicked();

    void on_buttonLoadLogo_clicked();

    void on_buttonAddLogo_clicked();

    void on_buttonConvertColorSpaceConvertTo_clicked();

private:
    Ui::MainWindow *ui;

    cv::Mat imageInput;
    cv::Mat imageOutput;

    cv::Mat imageROI;

    cv::Mat imageLogo;

    // Reference: https://github.com/jayrambhia/qt-opencv-app/blob/master/MatToQImage.cpp
    QImage matToQImage(const cv::Mat&);

    void addSaltNoise(cv::Mat&, int);
    void addPepperNoise(cv::Mat&, int);

    void displayInputImage();
    void displayOutputImage();
    void displayLogoImage();

    QString pickImageDialog();

    void setEnabledToolboxes(bool);

    void setCurrentColorSpace(QString);


};

#endif // MAINWINDOW_H
