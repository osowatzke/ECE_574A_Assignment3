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
            if (currVertex->time == -1)
            {
                if (successorsScheduled(currVertex))
                {
                    currVertex->time = getLatestStartTime(currVertex, latency);
                    if (currVertex->time < 0)
                    {
                        cout << "ERROR : Latency constraint too short" << endl;
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
            if (dest->time == -1)
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
            startTime = min(startTime, dest->time - runTime);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis