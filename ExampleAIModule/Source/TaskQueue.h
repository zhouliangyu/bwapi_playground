#pragma once
#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace Filter;

enum class TaskCategories
{
    TRAIN_UNIT,
    NONE
};

struct TaskItem
{
    TaskCategories m_taskCategory;
    UnitType m_relatedUnit;
};

class TaskQueue
{
    private:
        std::vector<TaskItem> m_taskQueue;
    public:
        int getQueueSize();
        bool updateOnScreen();
        bool push(const TaskCategories& c, const UnitType& t); // simple function to build new unit
        TaskItem pop();
};
