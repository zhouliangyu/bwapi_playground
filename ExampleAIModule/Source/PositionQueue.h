#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace Filter;

class PositionQueue
{
    private:
        std::vector<Point<int, 32>> m_positions;
    public:
        bool push(const Point<int, 32>& p);
        bool addStartingLocations();
        int getPositionQueueSize();
        Point<int, 32> getTilePosition(int i);
        Point<int, 1> getPosition(int i);
};

