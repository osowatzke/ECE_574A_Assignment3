#include "AlapScheduler.h"

namespace HighLevelSynthesis
{

AlapScheduler::AlapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

void AlapScheduler::run()
{
    bool unscheduledVertices = true;
    while (unscheduledVertices)
    {
        unscheduledVertices = false;
        for (vertex*& currVertex : dataManager->vertices)
        {
            if ((currVertex->time == -1) && (predecessorsScheduled(currVertex)))
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

bool AlapScheduler::predecessorsScheduled(vertex* currVertex)
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

int AlapScheduler::getEarliestStartTime(vertex* currVertex)
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