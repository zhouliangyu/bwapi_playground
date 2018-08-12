#include <BWAPI.h>
#include <string>
#include <vector>
#include "TaskQueue.h"

using namespace BWAPI;
using namespace Filter;

int TaskQueue::getQueueSize()
{
    return m_taskQueue.size();
}

bool TaskQueue::updateOnScreen()
{
    if (getQueueSize() == 0)
    {
        Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0,
            0, "Stack is empty."); },   // action
            nullptr,    // condition
            Broodwar->getLatencyFrames());  // frames to run
        return false;
    }
    else
    {
        Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0,
            0, "Stack [%d]:[%s]", getQueueSize(), m_taskQueue[getQueueSize()-1].getRelatedUnit().c_str()); },   // action
            nullptr,    // condition
            Broodwar->getLatencyFrames());  // frames to run
        return true;
    }
}

bool TaskQueue::push(const TaskItem& t)
{
    m_taskQueue.push_back(t);
    /* updateOnScreen(); */
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
    /* updateOnScreen(); */
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
