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

    // Compute the self-forces, predecessor/successor forces and total forces
    // Schedule the operation with least force and update its time-frame
    for (vertex*& currVertex : vertexes)
    {
        updateProbabilityMap();
        
        if (currVertex->time == -1)
        {
            float currSelfForce;
            float currPredecessorForce;
            float currSuccessorForce;
            float currtotalForce;
            float minTotalForce = NULL;
            int minSelfForceTime = -1;
            
            for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {
                currSelfForce = getSelfForce(i, currVertex);
                currSuccessorForce = getSuccessorForces(i + getVertexRunTime(currVertex), currVertex);
                currPredecessorForce = getPredecessorForces(i - getVertexRunTime(currVertex), currVertex);

                currtotalForce = currSelfForce + currPredecessorForce + currSuccessorForce;

                if ((minTotalForce == NULL) || (minTotalForce > currtotalForce)) {
                    minTotalForce = currtotalForce;
                    minSelfForceTime = i;
                }
            }

            currVertex->time = minSelfForceTime;
            currVertex->asapTime = minSelfForceTime;
            currVertex->alapTime = minSelfForceTime;

            updatePredecessorTiming(minSelfForceTime, currVertex);
            updateSuccessorTiming(minSelfForceTime, currVertex);
        }
    }
}

float FDScheduler::getPredecessorForces(int selfForceTime, vertex* currVertex) {
    float predecessorForce = 0;
    for (edge* currEdge : currVertex->inputs) {
        if (selfForceTime == currEdge->src->asapTime) {
            predecessorForce += getSelfForce(selfForceTime, currEdge->src);
            predecessorForce += getPredecessorForces(selfForceTime - getVertexRunTime(currEdge->src), currEdge->src);
        }
    }
    return predecessorForce;
}

float FDScheduler::getSuccessorForces(int selfForceTime, vertex* currVertex) {
    float successorForce = 0;
    for (edge* currEdge : currVertex->outputs) {
        for (vertex* currSuccessor : currEdge->dest) {
            if (selfForceTime == currSuccessor->alapTime) {
                successorForce += getSelfForce(selfForceTime, currSuccessor);
                successorForce += getSuccessorForces(selfForceTime + getVertexRunTime(currVertex), currSuccessor);
            }
        }
    }
    return successorForce;
}

float FDScheduler::getSelfForce(int usedTime, vertex* currVertex) {
    float selfForce = 0;
    for (int j = currVertex->asapTime; j <= currVertex->alapTime; j++) {
        if (j != usedTime) {
            selfForce += probabilityMap[currVertex->type][usedTime] * (0 - (1 / currVertex->mobility));
        } else {
            selfForce += probabilityMap[currVertex->type][usedTime] * (1 - (1 / currVertex->mobility));
        }
    }

    return selfForce;
}

void FDScheduler::updateProbabilityMap() {
    for (vertex*& currVertex : vertexes)
    {
        // Get vertex mobility (ALAP - ASAP + 1)
        currVertex->mobility = currVertex->alapTime - currVertex->asapTime + 1;

        // Compute the operations and type probabilities
        for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {
            probabilityMap[currVertex->type][i] += 1 / currVertex->mobility;
        }
    }
}

void FDScheduler::updatePredecessorTiming(int selfForceTime, vertex* currVertex) {
    float predecessorForce = 0;
    for (edge* currEdge : currVertex->inputs) {
        currEdge->src->alapTime = selfForceTime - getVertexRunTime(currEdge->src);
        updatePredecessorTiming(selfForceTime - getVertexRunTime(currEdge->src), currEdge->src);
    }
}

void FDScheduler::updateSuccessorTiming(int selfForceTime, vertex* currVertex) {
    float successorForce = 0;
    for (edge* currEdge : currVertex->outputs) {
        for (vertex* currSuccessor : currEdge->dest) {
            currSuccessor->asapTime = selfForceTime;
            updateSuccessorTiming(selfForceTime + getVertexRunTime(currVertex), currSuccessor);
        }
    }
}

} // namespace HighLevelSynthesis