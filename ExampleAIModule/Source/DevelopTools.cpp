#include <BWAPI.h>
#include "DevelopTools.h"
#include <cstdlib>
#include <ctime>

using namespace BWAPI;
using namespace Filter;

const bool DevelopTools::IS_DEVELOPING = true;
const int DevelopTools::LOG_MESSEGE_INTERVAL = 50;
int DevelopTools::messegeLastLogged = 0;

void DevelopTools::logMessegeOnScreen(const char* s, int i)
{
    if (!IS_DEVELOPING) return;
    if (Broodwar->getFrameCount() - messegeLastLogged > LOG_MESSEGE_INTERVAL)
    {
        Broodwar->sendTextEx(true,"%s(%d)", s, i);
        messegeLastLogged = Broodwar->getFrameCount();
    }
}

int DevelopTools::randMinMax(int minBoundary, int maxBoundary)
{
    if (minBoundary > maxBoundary) return 0;
    srand(static_cast<int>(time(NULL)));
    return (rand() % (maxBoundary-minBoundary+1))+minBoundary;
}
