#include "GameStrategy.h"
using namespace BWAPI;
using namespace Filter;

int GameStrategy::getTotalDroneCount()
{
	return Broodwar->self()->allUnitCount(UnitTypes::Zerg_Drone);
}

int GameStrategy::getTotalBaseCount()
{
	return Broodwar->self()->allUnitCount(UnitTypes::Zerg_Hatchery) +
		Broodwar->self()->allUnitCount(UnitTypes::Zerg_Lair) +
		Broodwar->self()->allUnitCount(UnitTypes::Zerg_Hive);
}

bool GameStrategy::needTrainDrone()
{
	if (getTotalDroneCount() < DRONE_BASE_RATIO * getTotalBaseCount())
		return true;
	else return false;
}

Error GameStrategy::trainUnit(const UnitType& unit)
{
	Error retValue = Errors::None;
	for (const auto& u : Broodwar->self()->getUnits())
	{
		if (!u->exists()) continue;
		if (u->getType() != UnitTypes::Zerg_Hatchery &&
				u->getType() != UnitTypes::Zerg_Lair &&
				u->getType() != UnitTypes::Zerg_Hive)
			continue;
		for (const auto& l : u->getLarva())
		{
			if (!l->morph(unit))
			{
				retValue = Broodwar->getLastError();
			} else break;
		}
	}
	return retValue;
}

bool GameStrategy::processError(Error)
