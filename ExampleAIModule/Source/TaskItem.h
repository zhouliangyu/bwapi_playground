#pragma once
#include <BWAPI.h>

using namespace BWAPI;
using namespace Filter;

enum class TaskCategories
{
    TRAIN_UNIT,
    BUILD_UNIT,
    SCOUT_MAP,
    NONE
};

class TaskItem
{
    private:
        TaskCategories m_taskCategory;
        UnitType m_relatedUnit;
        int m_mineralRequired;
    public:
        TaskItem(const TaskCategories& t = TaskCategories::NONE, const UnitType& u = UnitTypes::None);
        TaskCategories getTaskCategory();
        UnitType getRelatedUnit() const;
        int getMineralRequired();
        bool setRelatedUnit(const UnitType& u);
};
