#include <BWAPI.h>
#include <vector>
#include "BuildQueue.h"
using namespace BWAPI;
using namespace Filter;

void BuildQueue::updateOnScreen(int screenX, int screenY)
{
    unsigned int queueSize = m_itemPairQueue.size();
    Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(screenX, screenY, "Next:%s Queue:%d", m_itemPairQueue[queueSize-1].m_buildItem.c_str(), queueSize); },   // action
    nullptr,    // condition
    Broodwar->getLatencyFrames());  // frames to run

}

void BuildQueue::push(const UnitType &t, unsigned int currFrame)
{
    BuildItemPair p {t, currFrame};
    m_itemPairQueue.push_back(p);
    updateOnScreen();
}

void BuildQueue::push(const BuildItemPair& p)
{
    m_itemPairQueue.push_back(p);
    updateOnScreen();
}

BuildItemPair BuildQueue::pop()
{
    BuildItemPair p = m_itemPairQueue.back();
    m_itemPairQueue.pop_back();
    updateOnScreen();
    return p;
}

unsigned int BuildQueue::getQueueSize()
{
    return m_itemPairQueue.size();
}
