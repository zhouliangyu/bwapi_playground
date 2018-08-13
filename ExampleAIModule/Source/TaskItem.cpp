#pragma once
#include <BWAPI.h>
#include "TaskItem.h"

using namespace BWAPI;
using namespace Filter;

TaskItem::TaskItem(const TaskCategories& t, const UnitType& u) :
    m_taskCategory {t}, m_relatedUnit {u}
{
    switch (u)
    {
        case UnitTypes::Zerg_Spawning_Pool:
            m_mineralRequired = 200;
            break;
        default:
            m_mineralRequired = 0;
    }
}
TaskCategories TaskItem::getTaskCategory()
{
    return m_taskCategory;
}
UnitType TaskItem::getRelatedUnit() const
{
    return m_relatedUnit;
}
int TaskItem::getMineralRequired()
{
    return m_mineralRequired;
}

bool TaskItem::setRelatedUnit(const UnitType& u)
{
    m_relatedUnit = u;
    return true;
}
