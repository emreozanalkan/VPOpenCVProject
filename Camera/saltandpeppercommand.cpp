#include "saltandpeppercommand.h"

SaltAndPepperCommand::SaltAndPepperCommand()// : _addSalt(true), _addPepper(true), _rate(5000)
{
    _addSalt = true;
    _addPepper = true;
    _rate = 5000;
}

SaltAndPepperCommand::SaltAndPepperCommand(bool addSalt, bool addPepper, int rate)
{
    _addSalt = addSalt;
    _addPepper = addPepper;
    _rate = rate;
}

void SaltAndPepperCommand::Execute()
{
//    if(_addSalt)
//        this->addSaltNoise(_image, _rate);
//    if(_addPepper)
//        this->addPepperNoise(_image, _rate);
}

void SaltAndPepperCommand::Execute(cv::Mat& image)
{
        if(_addSalt)
            this->addSaltNoise(image, _rate);
        if(_addPepper)
            this->addPepperNoise(image, _rate);
}

void SaltAndPepperCommand::addSaltNoise(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++) {
        int i = rand() % image.cols;
        int j = rand() % image.rows;

        if (image.channels() == 1) { // gray-level image
            image.at<uchar>(j, i)= 255;
        }
        else if (image.channels() == 3) { // color image
            image.at<cv::Vec3b>(j, i)[0]= 255;
            image.at<cv::Vec3b>(j, i)[1]= 255;
            image.at<cv::Vec3b>(j, i)[2]= 255;
        }
    }
}

void SaltAndPepperCommand::addPepperNoise(cv::Mat &image, int n)
{
    for (int k = 0; k < n; k++) {
        int i = rand() % image.cols;
        int j = rand() % image.rows;

        if (image.channels() == 1) { // gray-level image
            image.at<uchar>(j, i)= 0;
        }
        else if (image.channels() == 3) { // color image
            image.at<cv::Vec3b>(j, i)[0]= 0;
            image.at<cv::Vec3b>(j, i)[1]= 0;
            image.at<cv::Vec3b>(j, i)[2]= 0;
        }
    }
}
