#ifndef GUARD_AlapScheduler_h
#define GUARD_AlapScheduler_h

#include "DataManager.h"

namespace HighLevelSynthesis
{

class AlapScheduler
{
    public:
        AlapScheduler(DataManager* dataManager);
        void run();
    private:
        DataManager* dataManager;
        bool predecessorsScheduled(vertex* currVertex);
        int getEarliestStartTime(vertex* currVertex);
};

} // namespace HighLevelSynthesis

#endif // GUARD_AlapScheduler_h