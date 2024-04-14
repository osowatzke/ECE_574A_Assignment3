#ifndef GUARD_AlapScheduler_h
#define GUARD_AlapScheduler_h

#include "DataManager.h"

namespace HighLevelSynthesis
{

class AlapScheduler
{
    public:
        AlapScheduler(DataManager* dataManager);
        int run(int Latency);
    private:
        DataManager* dataManager;
        bool successorsScheduled(vertex* currVertex);
        int getLatestStartTime(vertex* currVertex, int Latency);
};

} // namespace HighLevelSynthesis

#endif // GUARD_AlapScheduler_h