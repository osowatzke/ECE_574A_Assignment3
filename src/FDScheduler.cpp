#include "FDScheduler.h"
#include "AsapScheduler.h"
#include "AlapScheduler.h"

#include <limits>

namespace HighLevelSynthesis
{

FDScheduler::FDScheduler(DataManager* dataManager)
    : dataManager(dataManager)
    , probabilityMap() {}

void FDScheduler::run(int latency)
{
    AsapScheduler ASAP = AsapScheduler(dataManager);
    AlapScheduler ALAP = AlapScheduler(dataManager);

    // Get ASAP vertex times
    ASAP.run();

    // Get ALAP vertex times
    ALAP.run(latency);

    // Get vertex mobility (ALAP - ASAP + 1)
    for (vertex*& currVertex : vertexes)
    {
        currVertex->mobility = currVertex->alapTime - currVertex->asapTime + 1;
    }

    // Compute the operations and type probabilities
    for (vertex*& currVertex : vertexes)
    {
        for (int i = currVertex->asapTime - 1; i < currVertex->alapTime; i++) {
            probabilityMap[currVertex->type][i] += 1 / currVertex->mobility;
        }
    }

    // Compute the self-forces, predecessor/successor forces and total forces
    // Schedule the operation with least force and update its time-frame
    for (vertex*& currVertex : vertexes)
    {
        if (currVertex->time == -1)
        {
            float currSelfForce;
            float currPredecessorForce;
            float currSuccessorForce;
            float currtotalForce;
            float minTotalForce = NULL;
            int minSelfForceTime = 0;
            
            for (int i = currVertex->asapTime - 1; i < currVertex->alapTime; i++) {
                currSelfForce = getSelfForce(i, currVertex);
                currSuccessorForce = getSuccessorForces(i + (getVertexRunTime(currVertex) - 1), currVertex);
                currPredecessorForce = getPredecessorForces(i - (getVertexRunTime(currVertex) - 1), currVertex);

                currtotalForce = currSelfForce + currPredecessorForce + currSuccessorForce;

                if ((minTotalForce == NULL) || (minTotalForce > currtotalForce)) {
                    minTotalForce = currtotalForce;
                    minSelfForceTime = i;
                }
            }

            currVertex->time = minSelfForceTime;
        }
    }
}

float FDScheduler::getPredecessorForces(int selfForceTime, vertex* currVertex) {
    float predecessorForce = 0;
    for (edge* currEdge : currVertex->inputs) {
        predecessorForce += getSelfForce(selfForceTime - (getVertexRunTime(currVertex) - 1), currEdge->src);
        predecessorForce += getPredecessorForces(selfForceTime - (getVertexRunTime(currVertex) - 1), currEdge->src);
    }
    return predecessorForce;
}

float FDScheduler::getSuccessorForces(int selfForceTime, vertex* currVertex) {
    float successorForce = 0;
    for (edge* currEdge : currVertex->outputs) {
        for (vertex* currSuccessor : currEdge->dest) {
            successorForce += getSelfForce(selfForceTime + (getVertexRunTime(currVertex) - 1), currSuccessor);
            successorForce += getSuccessorForces(selfForceTime + (getVertexRunTime(currVertex) - 1), currSuccessor);
        }
    }
    return successorForce;
}

float FDScheduler::getSelfForce(int usedTime, vertex* currVertex) {
    float selfForce = 0;
    for (int j = usedTime; j < currVertex->alapTime; j++) {
        if (usedTime != j) {
            selfForce += probabilityMap[currVertex->type][usedTime] * (0 - (1 / currVertex->mobility));
        } else {
            selfForce += probabilityMap[currVertex->type][usedTime] * (1 - (1 / currVertex->mobility));
        }
    }

    return selfForce;
}

} // namespace HighLevelSynthesis