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
            m_gasRequired = 0;
            break;
        case UnitTypes::Zerg_Hatchery:
            m_mineralRequired = 300;
            m_gasRequired = 0;
            break;
        case UnitTypes::Zerg_Hydralisk_Den:
            m_mineralRequired = 100;
            m_gasRequired = 50;
            break;
        case UnitTypes::Zerg_Extractor:
            m_mineralRequired = 50;
            m_gasRequired = 0;
            break;
        default:
            m_mineralRequired = 0;
            m_gasRequired = 0;
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

int TaskItem::getGasRequired()
{
    return m_gasRequired;
}

bool TaskItem::setRelatedUnit(const UnitType& u)
{
    m_relatedUnit = u;
    return true;
}

const char* TaskItem::getTaskCStr()
{
    switch (m_taskCategory)
    {
        case TaskCategories::TRAIN_UNIT:
            return "TRAIN_UNIT";
            break;
        case TaskCategories::BUILD_UNIT:
            return "BUILD_UNIT";
            break;
        case TaskCategories::SCOUT_MAP:
            return "SCOUT_MAP";
            break;
        default:
            return "UNKNOWN";
    }
}
