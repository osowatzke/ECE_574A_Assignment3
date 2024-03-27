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
            if (currVertex->time == -1)
            {
                if (predecessorsScheduled(currVertex))
                {
                    currVertex->time = getEarliestStartTime(currVertex);
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
            if (src->time == -1)
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
            int vertexEndTime = getVertexEndTime(src);
            startTime = max(startTime, vertexEndTime + 1);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis