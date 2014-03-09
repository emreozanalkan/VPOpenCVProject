#ifndef ADDLOGOCOMMAND_H
#define ADDLOGOCOMMAND_H

#include "ipcommand.h"

class AddLogoCommand : public IPCommand
{
private:
    cv::Mat _logoImage;
    int _logoX;
    int _logoY;
    double _logoAlpha;
    double _logoBeta;
    double _logoGamma;
public:
    AddLogoCommand();
    AddLogoCommand(cv::Mat, int, int, double, double, double);

    void Execute();
    void Execute(cv::Mat&);
};

#endif // ADDLOGOCOMMAND_H
