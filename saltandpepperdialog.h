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

private:
    Ui::SaltAndPepperDialog *ui;
};

#endif // SALTANDPEPPERDIALOG_H
