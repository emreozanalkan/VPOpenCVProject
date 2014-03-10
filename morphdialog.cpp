#include "morphdialog.h"
#include "ui_morphdialog.h"
#include "pch.h"

MorphDialog::MorphDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MorphDialog)
{
    ui->setupUi(this);
}

MorphDialog::~MorphDialog()
{
    delete ui;
}

void MorphDialog::on_buttonBox_accepted()
{
    QString morphologicalOperation = ui->comboBoxMorphologicalOperation->currentText();
    if(morphologicalOperation == "Dilation")
        morphOperation = cv::MORPH_DILATE;
    else if(morphologicalOperation == "Erosion")
        morphOperation = cv::MORPH_ERODE;
    else if(morphologicalOperation == "Opening")
        morphOperation = cv::MORPH_OPEN;
    else if(morphologicalOperation == "Closing")
        morphOperation = cv::MORPH_CLOSE;
    else if(morphologicalOperation == "Morphological Gradient")
        morphOperation = cv::MORPH_GRADIENT;
    else if(morphologicalOperation == "Top Hat")
        morphOperation = cv::MORPH_TOPHAT;
    else if(morphologicalOperation == "Black Hat")
        morphOperation = cv::MORPH_BLACKHAT;
    else
        morphOperation = cv::MORPH_DILATE;

    QString kernelTypeString = ui->comboBoxMorphologicalKernelType->currentText();
    if(kernelTypeString == "Rectangle")
        kernelType = cv::MORPH_RECT;
    else if(kernelTypeString == "Cross")
        kernelType = cv::MORPH_CROSS;
    else if(kernelTypeString == "Ellipse")
        kernelType = cv::MORPH_ELLIPSE;
    else
        kernelType = cv::MORPH_RECT;

    QString imagePaddingMethodString = ui->comboBoxMorphologicalPaddingMethod->currentText();
    if(imagePaddingMethodString == "Border Replicate - aaaaaa|abcdefgh|hhhhhhh") // Border Replicate - aaaaaa|abcdefgh|hhhhhhh
        imagePaddingMethod = cv::BORDER_REPLICATE;
    else if(imagePaddingMethodString == "Border Reflect - fedcba|abcdefgh|hgfedcb") // Border Reflect - fedcba|abcdefgh|hgfedcb
        imagePaddingMethod = cv::BORDER_REFLECT;
    else if(imagePaddingMethodString == "Border Reflect 101 - gfedcb|abcdefgh|gfedcba") // Border Reflect 101 - gfedcb|abcdefgh|gfedcba
        imagePaddingMethod = cv::BORDER_REFLECT_101;
    else if(imagePaddingMethodString == "Border Wrap - cdefgh|abcdefgh|abcdefg") // Border Wrap - cdefgh|abcdefgh|abcdefg
        imagePaddingMethod = cv::BORDER_WRAP;
    else
        imagePaddingMethod = cv::BORDER_REPLICATE;

    iterationCount = ui->spinBoxMorphologicalIterationCount->value();

    int kSize = ui->spinBoxMorphologicalKernelSize->value();
    kernelSize = new cv::Size(kSize, kSize);

    anchorPoint = new cv::Point(ui->spinBoxMorphologicalKernelAnchorX->value(), ui->spinBoxMorphologicalKernelAnchorY->value());
}
