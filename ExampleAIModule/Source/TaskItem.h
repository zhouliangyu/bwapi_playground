#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace Filter;

enum class TaskCategories
{
    TRAIN_UNIT,
    NONE
};

class TaskItem
{
    private:
        TaskCategories m_taskCategory;
        UnitType m_relatedUnit;
    public:
        TaskItem(const TaskCategories& t = TaskCategories::NONE, const UnitType& u = UnitTypes::None);
        TaskCategories getTaskCategory();
        UnitType getRelatedUnit();
};
