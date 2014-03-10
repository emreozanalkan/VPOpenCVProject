#ifndef MORPHCOMMAND_H
#define MORPHCOMMAND_H

#include "ipcommand.h"

class MorphCommand : public IPCommand
{
private:
    int _morphOperation;
    int _kernelType;
    int _imagePaddingMethod;
    int _iterationCount;
    cv::Size *_kernelSize;
    cv::Point *_anchorPoint;
public:
    MorphCommand();

    MorphCommand(int, int, int, int, cv::Size*, cv::Point*);

    void Execute();
    void Execute(cv::Mat&);
};

#endif // MORPHCOMMAND_H
