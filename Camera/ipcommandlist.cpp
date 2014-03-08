#include "ipcommandlist.h"

IPCommandList::IPCommandList()
{
}

void IPCommandList::Execute()
{
    for(std::vector<IPCommand*>::iterator it = _commandList.begin(); it != _commandList.end(); ++it)
    {
        (*it)->Execute();
    }
}
