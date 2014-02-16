#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    ui->label->setBackgroundRole(QPalette::Base);
//    ui->label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
//    ui->label->setScaledContents(true);

    //this->setCentralWidget(ui->horizontalLayoutWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
