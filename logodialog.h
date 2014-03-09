#ifndef LOGODIALOG_H
#define LOGODIALOG_H

#include <QDialog>

namespace Ui {
class LogoDialog;
}

class LogoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogoDialog(QWidget *parent = 0);
    ~LogoDialog();

    int logoX;
    int logoY;
    double logoAlpha;
    double logoBeta;
    double logoGamma;
    cv::Mat imageLogo;

private slots:
    void on_buttonBox_accepted();

    void on_buttonLoadLogo_clicked();

private:
    Ui::LogoDialog *ui;

    QString pickImageDialog();
};

#endif // LOGODIALOG_H
