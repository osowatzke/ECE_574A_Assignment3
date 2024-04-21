#include "AlapScheduler.h"

#include <limits>
#include <iostream>

using namespace std;

namespace HighLevelSynthesis
{

AlapScheduler::AlapScheduler(DataManager* dataManager)
    : dataManager(dataManager) {}

// Function runs the ALAP scheduler
int AlapScheduler::run(int latency)
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
            if (currVertex->alapTime == -1)
            {
                // If all of its successors are scheduled,
                // then attempt to schedule the vertex
                if (successorsScheduled(currVertex))
                {
                    currVertex->alapTime = getLatestStartTime(currVertex, latency);
                    
                    // Print an error message and return a non-zero value
                    // if vertex cannot be scheduled
                    if (currVertex->alapTime < 0) {
                        cout << "Error: Latency constraint too short." << endl;
                        return 1;
                    }
                }
                // Otherwise, indicate that there are still unscheduled vertices
                else
                {
                    unscheduledVertices = true;
                }

            }
        }
    }
    return 0;
}

// Function determines whether a vertex's successors have been scheduled
bool AlapScheduler::successorsScheduled(vertex* currVertex)
{
    // Loop through all of the vertices outputs
    for (edge*& output : currVertex->outputs)
    {
        // Loop through the destinations of the output
        for(vertex*& dest : output->dest)
        {
            // Return false if any successors are not scheduled
            if (dest->alapTime == -1)
            {
                return false;
            }
        }
    }
    return true;
}

// Function determines the latest start time for a vertex
int AlapScheduler::getLatestStartTime(vertex* currVertex, int latency)
{
    // Get the run time of the current vertex
    int runTime = getVertexRunTime(currVertex);

    // Initialize the start time of the vertex with the latest possible start time
    int startTime = latency - runTime;

    // Loop through each of the vertex outputs
    for (edge* output : currVertex->outputs)
    {
        // Loop through the destinations of the output
        for(vertex*& dest : output->dest)
        {
            // Determine the new latest start time
            startTime = min(startTime, dest->alapTime - runTime);
        }
    }
    return startTime;
}

} // namespace HighLevelSynthesis