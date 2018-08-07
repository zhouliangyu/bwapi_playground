#include <BWAPI.h>
#include "BuildQueue.h"

using namespace BWAPI;
using namespace Filter;

BuildQueue::BuildQueue() // constructor
{

}

bool BuildQueue::push(const UnitType& t, int f, bool updateOnScreen, int x, int y)
{

    return true;
}

BuildItemPair BuildQueue::pop(bool updateOnScreen, int x, int y)
{

}

int BuildQueue::size()
{
    return m_queue.size();
}


