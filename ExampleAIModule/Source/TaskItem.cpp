#pragma once
#include <BWAPI.h>
#include "TaskItem.h"

using namespace BWAPI;
using namespace Filter;

TaskItem::TaskItem(const TaskCategories& t, const UnitType& u) :
    m_taskCategory {t}, m_relatedUnit {u}
{
}
TaskCategories TaskItem::getTaskCategory()
{
    return m_taskCategory;
}
UnitType TaskItem::getRelatedUnit()
{
    return m_relatedUnit;
}
