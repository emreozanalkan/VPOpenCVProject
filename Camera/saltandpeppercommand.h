#ifndef SALTANDPEPPERCOMMAND_H
#define SALTANDPEPPERCOMMAND_H

#include "ipcommand.h"

class SaltAndPepperCommand : public IPCommand
{
private:
    bool _addSalt;
    bool _addPepper;
    int _rate;
public:
    SaltAndPepperCommand();
    SaltAndPepperCommand(bool, bool, int);

    void addSaltNoise(cv::Mat&, int);
    void addPepperNoise(cv::Mat&, int);

    void Execute();
    void Execute(cv::Mat&);
};

#endif // SALTANDPEPPERCOMMAND_H
