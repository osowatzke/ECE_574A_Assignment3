#ifndef GUARD_FDScheduler_h
#define GUARD_FDScheduler_h

#include "DataManager.h"
#include "AsapScheduler.h"
#include "AlapScheduler.h"

namespace HighLevelSynthesis
{

class FDScheduler
{
    public:
        FDScheduler(DataManager* dataManager);
        int run(int Latency);
    private:
        DataManager* dataManager;
        map<VertexType, map<int, float>> probabilityMap;
        int latency;
        AsapScheduler ASAP;
        AlapScheduler ALAP;
        float getTotalForce(int selfForceTime, vertex* currVertex);
        float getSelfForce(int usedTime, vertex* currVertex);
        void updateProbabilityMap();
        int updateTiming();
};

} // namespace HighLevelSynthesis

#endif // GUARD_FDScheduler_h