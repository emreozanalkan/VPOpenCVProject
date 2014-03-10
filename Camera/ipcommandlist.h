#ifndef IPCOMMANDLIST_H
#define IPCOMMANDLIST_H

#include "ipcommand.h"
#include <vector>

class IPCommandList
{
private:
    std::vector<IPCommand*> _commandList;
public:
    IPCommandList();
    ~IPCommandList();

    void AddCommand(IPCommand*);
    void RemoveCommand(int);

    void Execute(cv::Mat);
};

#endif // IPCOMMANDLIST_H
