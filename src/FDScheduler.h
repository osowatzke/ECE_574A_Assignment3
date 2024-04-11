#ifndef GUARD_FDScheduler_h
#define GUARD_FDScheduler_h

#include "DataManager.h"

namespace HighLevelSynthesis
{

class FDScheduler
{
    public:
        FDScheduler(DataManager* dataManager);
        void run(int Latency);
    private:
        DataManager* dataManager;
        vector<vertex*> vertexes;
        map<VertexType, map<int, float>> probabilityMap;
        float getPredecessorForces(int selfForceTime, vertex* currVertex);
        float getSuccessorForces(int selfForceTime, vertex* currVertex);
        float FDScheduler::getSelfForce(int usedTime, vertex* currVertex);
        void FDScheduler::updateProbabilityMap();
        void FDScheduler::updatePredecessorTiming(int selfForceTime, vertex* currVertex);
        void FDScheduler::updateSuccessorTiming(int selfForceTime, vertex* currVertex);
};

} // namespace HighLevelSynthesis

#endif // GUARD_FDScheduler_h