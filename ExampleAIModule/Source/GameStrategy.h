#pragma once
#include <BWAPI.h>
#include "GameConst.h"
using namespace BWAPI;
using namespace Filter;


class GameStrategy
{
	private:
	public:
		int getTotalDroneCount();
		int getTotalBaseCount();
		bool needTrainDrone();
		Error trainUnit(const UnitType& unit);
		bool processError(Error lastErr);
};