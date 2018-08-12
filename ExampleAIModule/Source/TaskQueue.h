#pragma once
#include <BWAPI.h>
#include <vector>
#include "TaskItem.h"

using namespace BWAPI;
using namespace Filter;


class TaskQueue
{
    private:
        std::vector<TaskItem> m_taskQueue;
    public:
        int getQueueSize();
        bool updateOnScreen();
        bool push(const TaskItem& t);
        TaskItem pop();
};
