#ifndef IPCOMMAND_H
#define IPCOMMAND_H

#include "pch.h"

class IPCommand
{
public:
    virtual void Execute() = 0;
    virtual void Execute(cv::Mat&) = 0;
};

#endif // IPCOMMAND_H
