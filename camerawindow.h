#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "pch.h"

namespace Ui {
class CameraWindow;
}

class CameraWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = 0);
    ~CameraWindow();

private slots:
    void on_buttonPlay_clicked();

    void on_buttonPause_clicked();

    void on_buttonAddIPOperation_clicked();

    void on_buttonRemoveSelectedOperation_clicked();

    void cameraImageUpdate();

private:

    void uiPlayMode(bool);

    Ui::CameraWindow *ui;

    cv::VideoCapture *videoCapture;

    cv::Mat image;

    QTimer *timer;
};

#endif // CAMERAWINDOW_H
