#include "saltandpepperdialog.h"
#include "ui_saltandpepperdialog.h"

SaltAndPepperDialog::SaltAndPepperDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SaltAndPepperDialog)
{
    ui->setupUi(this);

    addSalt = false;
    addPepper = false;
    rate = 0;
}

SaltAndPepperDialog::~SaltAndPepperDialog()
{
    delete ui;
}

void SaltAndPepperDialog::on_buttonBox_accepted()
{
    addSalt =  ui->checkBoxSalt->isChecked();

    addPepper = ui->checkBoxPepper->isChecked();

    rate = ui->spinBoxSaltAndPepperRate->value();
}
