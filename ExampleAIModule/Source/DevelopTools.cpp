#include <BWAPI.h>
#include "DevelopTools.h"

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
