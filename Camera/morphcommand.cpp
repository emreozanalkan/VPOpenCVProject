#include "morphcommand.h"

MorphCommand::MorphCommand()
{

}

MorphCommand::MorphCommand(int morphOperation, int kernelType, int imagePaddingMethod, int iterationCount, cv::Size* kernelSize, cv::Point* anchorPoint)
{
    _morphOperation = morphOperation;
    _kernelType = kernelType;
    _imagePaddingMethod = imagePaddingMethod;
    _iterationCount = iterationCount;
    _kernelSize = kernelSize;
    _anchorPoint = anchorPoint;
}

void MorphCommand::Execute()
{

}

void MorphCommand::Execute(cv::Mat& image)
{
    cv::Mat structureElement = cv::getStructuringElement(_kernelType, *_kernelSize, *_anchorPoint);

    if(_morphOperation == cv::MORPH_DILATE)
        cv::dilate(image, image, structureElement, *_anchorPoint, _iterationCount, _imagePaddingMethod);
    else if(_morphOperation == cv::MORPH_ERODE)
        cv::erode(image, image, structureElement, *_anchorPoint, _iterationCount, _imagePaddingMethod);
    else
        cv::morphologyEx(image, image, _morphOperation, structureElement, *_anchorPoint, _iterationCount, _imagePaddingMethod);
}
