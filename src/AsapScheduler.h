#ifndef GUARD_AsapScheduler_h
#define GUARD_AsapScheduler_h

#include "DataManager.h"

namespace HighLevelSynthesis
{

class AsapScheduler
{
    public:
        AsapScheduler(DataManager* dataManager);
        void run();
    private:
        DataManager* dataManager;
        bool predecessorsScheduled(vertex* currVertex);
        int getEarliestStartTime(vertex* currVertex);
};

} // namespace HighLevelSynthesis

#endif // GUARD_AsapScheduler_h