#include "AsapScheduler.h"

namespace HighLevelSynthesis
{
// Class constructor provides access to shared data manager
AsapScheduler::AsapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

// Function runs the ASAP scheduler
void AsapScheduler::run()
{
    // Assume there are unscheduled vertices
    bool unscheduledVertices = true;

    // Continue looping until there are no unscheduled vertices
    while (unscheduledVertices)
    {
        // Assume no unscheduled vertices until one is found
        unscheduledVertices = false;

        // Loop through all vertices
        for (vertex*& currVertex : dataManager->vertices)
        {
            // If vertex is unscheduled
            if (currVertex->asapTime == -1)
            {
                // If all of its predecessors are scheduled,
                // then schedule the vertex
                if (predecessorsScheduled(currVertex))
                {
                    currVertex->asapTime = getEarliestStartTime(currVertex);
                }
                // Otherwise, indicate that there are still unscheduled vertices
                else
                {
                    unscheduledVertices = true;
                }

            }
        }
    }
}

// Function determines whether a vertex's predecessors have been scheduled
bool AsapScheduler::predecessorsScheduled(vertex* currVertex)
{
    // Loop through all of the vertices inputs
    for (edge* input : currVertex->inputs)
    {
        // If the input is driven by another vertex
        if (input->src != NULL)
        {
            // Return false if the parent vertex is not scheduled
            vertex* src = input->src;
            if (src->asapTime == -1)
            {
                return false;
            }
        }
    }
    return true;
}

// Function determines the earliest start time for a vertex
int AsapScheduler::getEarliestStartTime(vertex* currVertex)
{
    // Assume that the vertex can start at time 0
    int startTime = 0;

    // Loop through all the vertex inputs
    for (edge* input : currVertex->inputs)
    {
        // If the input is driven by another vertex
        if (input->src != NULL)
        {
            // Get a pointer to the parent vertex
            vertex* src = input->src;

            // Determine the end time of the parent vertex
            // Not using getVertexEndTime because it references
            // src->time instead of src->asapTime
            int runTime = getVertexRunTime(src);
            int vertexEndTime = src->asapTime + runTime - 1;

            // Determine the new earliest start time
            startTime = max(startTime, vertexEndTime + 1);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis