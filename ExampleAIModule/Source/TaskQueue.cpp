#include <BWAPI.h>
#include <string>
#include "TaskQueue.h"

using namespace BWAPI;
using namespace Filter;

int TaskQueue::getQueueSize()
{
    return m_taskQueue.size();
}

bool TaskQueue::updateOnScreen()
{
    int queueSending = 3;
    Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0,
        0, "Queue Size: %d", getQueueSize()); },   // action
        nullptr,    // condition
        Broodwar->getLatencyFrames());  // frames to run
    Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0,
        10, "%s", "------ Stack ------"); },   // action
        nullptr,    // condition
        Broodwar->getLatencyFrames());  // frames to run
    int printedCounter = 0;
    for (int i = getQueueSize()-1; i>=0; i--)
    {
        Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0,
            20+printedCounter*10, "%d:%s", i, m_taskQueue[i].getRelatedUnit().c_str() ); },   // action
            nullptr,    // condition
        Broodwar->getLatencyFrames());  // frames to run
        ++printedCounter;
        if (printedCounter > queueSending) break;
    }
    return true;
}

bool TaskQueue::push(const TaskItem& t)
{
    m_taskQueue.push_back(t);
    updateOnScreen();
    return true;
}

TaskItem TaskQueue::pop()
{
    if (getQueueSize() == 0)
    {
        TaskItem blankItem(TaskCategories::NONE, UnitTypes::None);
        return blankItem;
    }
    TaskItem poppedItem = m_taskQueue.back();
    m_taskQueue.pop_back();
    updateOnScreen();
    return poppedItem;
}

int TaskQueue::searchTaskRelatedUnit(const UnitType& t)
{
    int indexCounter = 0;
    for (const auto& e : m_taskQueue)
    {
        if (e.getRelatedUnit() == t)
            return indexCounter;
        ++indexCounter;
    }
    return -1;
}
