#ifndef UTILITIES_H
#define UTILITIES_H

#include "pch.h"

#include <QImage>

// Reference: https://github.com/jayrambhia/qt-opencv-app/blob/master/MatToQImage.cpp
QImage MatToQImage(const cv::Mat& mat);

#endif // UTILITIES_H
