#include "addlogocommand.h"

AddLogoCommand::AddLogoCommand()
{

}

AddLogoCommand::AddLogoCommand(cv::Mat logoImage, int logoX, int logoY, double logoAlpha, double logoBeta, double logoGamma)
{
    _logoImage = logoImage;
    _logoX = logoX;
    _logoY = logoY;
    _logoAlpha = logoAlpha;
    _logoBeta = logoBeta;
    _logoGamma = logoGamma;
}

void AddLogoCommand::Execute()
{

}

void AddLogoCommand::Execute(cv::Mat& image)
{
    if(this->_logoImage.rows > image.rows || this->_logoImage.cols > image.cols)
        cv::resize(this->_logoImage, this->_logoImage, image.size());

    cv::Mat imageROI;
    imageROI = image(cv::Rect(_logoX, _logoY, this->_logoImage.cols, this->_logoImage.rows));

    if(imageROI.channels() == 1)
        if(_logoImage.channels() != 1)
            cv::cvtColor(_logoImage, _logoImage, CV_BGR2GRAY);

    cv::addWeighted(imageROI,
                    _logoAlpha,
                    _logoImage,
                    _logoBeta,
                    _logoGamma,
                    imageROI);
}
