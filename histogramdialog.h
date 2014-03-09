#ifndef HISTOGRAMDIALOG_H
#define HISTOGRAMDIALOG_H

#include <QDialog>

namespace Ui {
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent = 0);
    ~HistogramDialog();

    bool calculateHistogram;
    int histogramChannel;

    bool equalizeHistogram;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::HistogramDialog *ui;
};

#endif // HISTOGRAMDIALOG_H
