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
        map<VertexType, vector<double>> typeDistribution;
        int latency;
        AsapScheduler ASAP;
        AlapScheduler ALAP;
        int updateTiming();
        void updateTypeDistribution();
        void displayTypeDistribution();
        map<VertexType, vector<double>> getTypeDistribution(hierarchy* currHierarchy);
        double getTotalForce(int selfForceTime, vertex* currVertex);
        double getSelfForce(vertex* currVertex);
};

} // namespace HighLevelSynthesis

#endif // GUARD_FDScheduler_h