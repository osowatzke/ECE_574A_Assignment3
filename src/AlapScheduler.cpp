#include "AlapScheduler.h"

#include <limits>
#include <iostream>

using namespace std;

namespace HighLevelSynthesis
{

AlapScheduler::AlapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

void AlapScheduler::run(int latency)
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
                }
                else
                {
                    unscheduledVertices = true;
                }

            }
        }
    }
}

bool AlapScheduler::successorsScheduled(vertex* currVertex)
{
    cout << currVertex->inputs.size() << "->" << currVertex->outputs.size() << endl;
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
    cout << currVertex->outputs.size() << endl;
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