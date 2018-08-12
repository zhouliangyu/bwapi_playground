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
    Broodwar->registerEvent([=](Game*){ Broodwar->drawTextScreen(0, 0, "Queue Size: %d", getQueueSize()); },   // action
        nullptr,    // condition
        Broodwar->getLatencyFrames());  // frames to run
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
