#pragma once
#include <BWAPI.h>
#include <vector>
using namespace BWAPI;
using namespace Filter;

struct BuildItemPair
{
    UnitType m_buildItem;
    unsigned int m_pushFrame;
};

class BuildQueue
{
    private:
        std::vector<BuildItemPair> m_itemPairQueue;
    public:
        void updateOnScreen(int screenX=0, int screenY=0);
        void push(const UnitType& t, unsigned int currFrame=Broodwar->getFrameCount());
        void push(const BuildItemPair& p);
        BuildItemPair pop();
};

