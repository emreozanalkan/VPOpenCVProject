#ifndef IPCOMMAND_H
#define IPCOMMAND_H

#include "pch.h"

class IPCommand
{
protected:
    cv::Mat _image;
public:
    virtual void Execute() = 0;
};

#endif // IPCOMMAND_H
