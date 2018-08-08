#pragma once // BuildQueue.h
#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace Filter;

struct BuildItemPair
{
    UnitType s_type; // unit type in the queue
    int s_frame;     // absolute frame count when the item was added
};

class BuildQueue
{
    private:
        std::vector<BuildItemPair> m_queue;
    public:
        BuildQueue(); // constructor
        void logCurrQueueInfo(int x=120, int y=10);
        bool push(const UnitType& t, int f=Broodwar->getFrameCount(), bool updateOnScreen=true, int x=120, int y=10);
        BuildItemPair pop(bool updateOnScreen=true, int x=120, int y=10);
        int getQueueSize();
};
