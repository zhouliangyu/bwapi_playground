#include <BWAPI.h>
#include <vector>
#include "BuildQueue.h"

using namespace BWAPI;
using namespace Filter;

BuildQueue::BuildQueue() // constructor
{
}

void BuildQueue::logCurrQueueInfo(int x, int y)
{
    int queueSize = m_queue.size();
    Broodwar->drawTextScreen(x, y, "%s(%d)", m_queue[queueSize-1].s_type.c_str(), queueSize);
}
bool BuildQueue::push(const UnitType& t, int f, bool updateOnScreen, int x, int y)
{
    BuildItemPair itemPushedIn {t, f};
    m_queue.push_back(itemPushedIn);
    if (updateOnScreen) logCurrQueueInfo(x, y);
    return true;
}
BuildItemPair BuildQueue::pop(bool updateOnScreen, int x, int y)
{
    BuildItemPair itemPoppedOut = m_queue.back();
    m_queue.pop_back();
    if (updateOnScreen) logCurrQueueInfo(x, y);
    return itemPoppedOut;
}
int BuildQueue::getQueueSize()
{ return m_queue.size(); }
