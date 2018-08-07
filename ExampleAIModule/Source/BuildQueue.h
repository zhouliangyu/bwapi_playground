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
        bool push(const BuildItemPair& p, int f, bool updateOnScreen=true, int x=120, int y=10);
        BuildItemPair pop(bool updateOnScreen=true, int x=120, int y=10);
        int size();
};
