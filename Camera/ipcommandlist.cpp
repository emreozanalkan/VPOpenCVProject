#include "ipcommandlist.h"

IPCommandList::IPCommandList()
{
}

IPCommandList::~IPCommandList()
{
    _commandList.clear();
}

void IPCommandList::AddCommand(IPCommand* command)
{
    _commandList.push_back(command);
}

void IPCommandList::RemoveCommand(int index)
{
    _commandList.erase(_commandList.begin() + index);
}

void IPCommandList::Execute(cv::Mat image)
{
    for(std::vector<IPCommand*>::iterator it = _commandList.begin(); it != _commandList.end(); ++it)
    {
        (*it)->Execute(image);
    }
}
