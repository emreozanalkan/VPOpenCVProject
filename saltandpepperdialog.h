#ifndef SALTANDPEPPERDIALOG_H
#define SALTANDPEPPERDIALOG_H

#include <QDialog>

namespace Ui {
class SaltAndPepperDialog;
}

class SaltAndPepperDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaltAndPepperDialog(QWidget *parent = 0);
    ~SaltAndPepperDialog();

    bool addSalt;
    bool addPepper;
    int rate;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SaltAndPepperDialog *ui;
};

#endif // SALTANDPEPPERDIALOG_H
