#include "AlapScheduler.h"

#include <limits>
#include <iostream>

using namespace std;

namespace HighLevelSynthesis
{

AlapScheduler::AlapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

int AlapScheduler::run(int latency)
{
    bool unscheduledVertices = true;
    while (unscheduledVertices)
    {
        unscheduledVertices = false;
        for (vertex*& currVertex : dataManager->vertices)
        {
            if (currVertex->alapTime == -1)
            {
                if (successorsScheduled(currVertex))
                {
                    currVertex->alapTime = getLatestStartTime(currVertex, latency);

                    if (currVertex->alapTime < 0) {
                        cout << "Error: Latency constraint too short." << endl;
                        return 1;
                    }
                }
                else
                {
                    unscheduledVertices = true;
                }

            }
        }
    }

    return 0;
}

bool AlapScheduler::successorsScheduled(vertex* currVertex)
{
    for (edge*& output : currVertex->outputs)
    {
        for(vertex*& dest : output->dest)
        {
            if (dest->alapTime == -1)
            {
                return false;
            }
        }
    }
    return true;
}

int AlapScheduler::getLatestStartTime(vertex* currVertex, int latency)
{
    int runTime = getVertexRunTime(currVertex);
    int startTime = latency - runTime;
    for (edge* output : currVertex->outputs)
    {
        for(vertex*& dest : output->dest)
        {
            startTime = min(startTime, dest->alapTime - runTime);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis