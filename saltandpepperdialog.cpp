#include "saltandpepperdialog.h"
#include "ui_saltandpepperdialog.h"

SaltAndPepperDialog::SaltAndPepperDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaltAndPepperDialog)
{
    ui->setupUi(this);
}

SaltAndPepperDialog::~SaltAndPepperDialog()
{
    delete ui;
}
