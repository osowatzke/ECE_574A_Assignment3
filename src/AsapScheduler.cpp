#include "AsapScheduler.h"

namespace HighLevelSynthesis
{

AsapScheduler::AsapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

void AsapScheduler::run()
{
    bool unscheduledVertices = true;
    while (unscheduledVertices)
    {
        unscheduledVertices = false;
        for (vertex*& currVertex : dataManager->vertices)
        {
            if (currVertex->asapTime == -1)
            {
                if (predecessorsScheduled(currVertex))
                {
                    currVertex->asapTime = getEarliestStartTime(currVertex);
                }
                else
                {
                    unscheduledVertices = true;
                }

            }
        }
    }
}

bool AsapScheduler::predecessorsScheduled(vertex* currVertex)
{
    for (edge* input : currVertex->inputs)
    {
        if (input->src != NULL)
        {
            vertex* src = input->src;
            if (src->asapTime == -1)
            {
                return false;
            }
        }
    }
    return true;
}

int AsapScheduler::getEarliestStartTime(vertex* currVertex)
{
    int startTime = 0;
    for (edge* input : currVertex->inputs)
    {
        if (input->src != NULL)
        {
            vertex* src = input->src;
            int runTime = getVertexRunTime(src);
            int vertexEndTime = src->asapTime + runTime - 1;
            startTime = max(startTime, vertexEndTime + 1);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis