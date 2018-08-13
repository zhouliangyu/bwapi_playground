#include <BWAPI.h>
#include <vector>
#include "PositionQueue.h"

using namespace BWAPI;
using namespace Filter;

bool PositionQueue::push(const Point<int, 32>& p)
{
    m_positions.push_back(p);
    return true;
}

bool PositionQueue::addStartingLocations()
{
    for (const auto& e : Broodwar->getStartLocations())
        push(e);
    return true;
}

int PositionQueue::getPositionQueueSize()
{
    return m_positions.size();
}

Point<int, 32> PositionQueue::getTilePosition(int i)
{
    return m_positions[i];
}

Point<int, 1> PositionQueue::getPosition(int i )
{
    int posX = m_positions[i].x * 32;
    int posY = m_positions[i].y * 32;
    return Position(posX, posY);
}
