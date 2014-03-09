#include "histogramdialog.h"
#include "ui_histogramdialog.h"

HistogramDialog::HistogramDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramDialog)
{
    ui->setupUi(this);

    calculateHistogram = true;
    histogramChannel = 0;

    equalizeHistogram = false;
}

HistogramDialog::~HistogramDialog()
{
    delete ui;
}

void HistogramDialog::on_buttonBox_accepted()
{
    if(ui->radioCalculateHistogram->isChecked())
    {
        calculateHistogram = true;
        equalizeHistogram = false;

        histogramChannel = ui->comboBoxHistogramOperationsChannel->currentIndex();
    }
    else
    {
        calculateHistogram = false;
        equalizeHistogram = true;
        histogramChannel = 0;
    }
}
