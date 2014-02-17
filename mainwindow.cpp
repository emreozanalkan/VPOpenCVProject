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

    ui->labelImage->setScaledContents(true);

    timer.setInterval(25);

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateImageFromVideoCapture()));

    isCameraCapture = false;
}

MainWindow::~MainWindow()
{
    this->stopDataSources();

    delete ui;
}

void MainWindow::on_buttonOpenImage_clicked()
{
    this->stopDataSources();

    QString filePath = this->pickImageDialog();

    if(filePath != 0)
    {
        image = cv::imread(filePath.toStdString());

        if(!image.data)
        {
            QMessageBox msgBox;
            msgBox.setText("Image you selected didn't loaded.");
            msgBox.exec();
            return;
        }

        isCameraCapture = false;

        this->displayImage();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You didn't select image.");
        msgBox.exec();
    }
}

void MainWindow::on_buttonOpenVideo_clicked()
{
    this->stopDataSources();

    QString filePath = this->pickVideoDialog();

    if(filePath != 0)
    {
        this->videoCapture.open(filePath.toStdString());

        if(!videoCapture.isOpened())
        {
            QMessageBox msgBox;
            msgBox.setText("Your video can't be opened.");
            msgBox.exec();
            return;
        }
        else
        {
            isCameraCapture = false;

            if(timer.isActive())
                timer.stop();
            timer.start();
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("You didn't select video.");
        msgBox.exec();
    }
}

void MainWindow::on_buttonOpenCamera_clicked()
{
    this->stopDataSources();

    if(videoCapture.isOpened())
        videoCapture.release();

    if(videoCapture.open(CV_CAP_ANY))
    {
        isCameraCapture = true;

        timer.start();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("No camera found or can't open camera.");
        msgBox.exec();
    }
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

QString MainWindow::pickVideoDialog()
{
    QFileDialog dialog(this);
    dialog.setDirectory(QDir::home());
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setWindowTitle("Open Video");
    dialog.setNameFilter("Video Files (*.mov *.mp4 *.m4v)");
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

void MainWindow::updateImageFromVideoCapture()
{
    mutex1.lock();
    if(videoCapture.isOpened())
        if(videoCapture.grab())
        {
            videoCapture>>image;
            this->displayImage();
        }
    mutex1.unlock();
}

void MainWindow::displayImage()
{
    cv::Mat tempImage;

    cv::cvtColor(image, tempImage, CV_BGR2RGB);

    if(isCameraCapture)
        cv::flip(tempImage, tempImage, 1);

    QImage qImage = this->matToQImage(tempImage);

    ui->labelImage->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::stopDataSources()
{
    mutex2.lock();
    if(timer.isActive())
        timer.stop();
    if(videoCapture.isOpened())
        videoCapture.release();
    mutex2.unlock();
}
