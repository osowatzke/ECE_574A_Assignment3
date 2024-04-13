#include "FDScheduler.h"
#include "AsapScheduler.h"
#include "AlapScheduler.h"

#include <limits>

namespace HighLevelSynthesis
{

FDScheduler::FDScheduler(DataManager* dataManager)
    : dataManager(dataManager)
    , probabilityMap()
    , ASAP(AsapScheduler(dataManager))
    , ALAP(AlapScheduler(dataManager)) {}

int FDScheduler::run(int inlatency)
{
    latency = inlatency;

    // Compute the self-forces, predecessor/successor forces and total forces
    // Schedule the operation with least force and update its time-frame
    for (vertex*& currVertex : dataManager->vertices)
    {
        if (updateTiming() == 1) {
            return 1;
        }
        
        updateProbabilityMap();
        
        if (currVertex->time == -1)
        {
            float currtotalForce;
            float minTotalForce = numeric_limits<float>::max();
            int minSelfForceTime = -1;
            
            for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {
                currtotalForce = getTotalForce(i, currVertex);

                if (minTotalForce > currtotalForce) {
                    minTotalForce = currtotalForce;
                    minSelfForceTime = i;
                }
            }

            currVertex->time = minSelfForceTime;
        }
    }

    return 0;
}

float FDScheduler::getTotalForce(int selfForceTime, vertex* currVertex) {
    int vertexStartTime = currVertex->asapTime;
    int vertexEndTime = currVertex->alapTime;
    float totalForce = 0;
    
    for (vertex* tempVertex : dataManager->vertices) {
        if (tempVertex->time == -1) {
            tempVertex->asapTestTime = tempVertex->asapTime;
            tempVertex->alapTestTime = tempVertex->alapTime;
        }
    }

    currVertex->time = selfForceTime;
    updateTiming();
    
    for (vertex* tempVertex : dataManager->vertices) {
        if (tempVertex->time == -1) {
            int tempASAPTime = tempVertex->asapTestTime;
            int tempALAPTime = tempVertex->alapTestTime;
            tempVertex->asapTestTime = tempVertex->asapTime;
            tempVertex->alapTestTime = tempVertex->alapTime;
            tempVertex->asapTime = tempASAPTime;
            tempVertex->alapTime = tempALAPTime;
        }

        for (int i = tempVertex->asapTestTime; i <= tempVertex->alapTestTime; i++) {
            totalForce += getSelfForce(i, tempVertex);
        }
    }

    currVertex->asapTime = vertexStartTime;
    currVertex->alapTime = vertexEndTime;

    return totalForce;
}

float FDScheduler::getSelfForce(int usedTime, vertex* currVertex) {
    float selfForce = 0;
    float keyAtTime = 0;
    for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {
        if (i != usedTime) {
            keyAtTime = 0;
        } else {
            keyAtTime = 1;
        }

        for (int j = 0; j < getVertexRunTime(currVertex); j++) {
            selfForce += probabilityMap[currVertex->type][usedTime] * (keyAtTime - (1.0f / currVertex->mobility));
        }
    }

    return selfForce;
}

void FDScheduler::updateProbabilityMap() {
    for (vertex*& currVertex : dataManager->vertices)
    {
        // Get vertex mobility (ALAP - ASAP + 1)
        currVertex->mobility = currVertex->alapTime - currVertex->asapTime + 1;

        // Compute the operations and type probabilities
        for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {
            for (int j = 0; j < getVertexRunTime(currVertex); j++) {
                probabilityMap[currVertex->type][i + j] += 1.0f / currVertex->mobility;
            }
        }
    }
}

int FDScheduler::updateTiming() {
    float predecessorForce = 0;
    for (vertex* currVertex : dataManager->vertices) {
        currVertex->alapTime = currVertex->time;
        currVertex->asapTime = currVertex->time;
    }

    // Get ASAP vertex times
    ASAP.run();

    // Get ALAP vertex times
    return ALAP.run(latency);
}

} // namespace HighLevelSynthesis