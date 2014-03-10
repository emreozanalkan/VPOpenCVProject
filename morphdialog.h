#ifndef MORPHDIALOG_H
#define MORPHDIALOG_H

#include <QDialog>
#include <QString>
#include "pch.h"

namespace Ui {
class MorphDialog;
}

class MorphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MorphDialog(QWidget *parent = 0);
    ~MorphDialog();

    int morphOperation;
    int kernelType;
    int imagePaddingMethod;
    int iterationCount;
    cv::Size *kernelSize;
    cv::Point *anchorPoint;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::MorphDialog *ui;
};

#endif // MORPHDIALOG_H
