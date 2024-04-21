#include "FDScheduler.h"
#include "AsapScheduler.h"
#include "AlapScheduler.h"

#include <iostream>
#include <iomanip>
#include <limits>

namespace HighLevelSynthesis
{

FDScheduler::FDScheduler(DataManager* dataManager)
    : dataManager(dataManager)
    , typeDistribution()
    , ASAP(AsapScheduler(dataManager))
    , ALAP(AlapScheduler(dataManager)) {}

int FDScheduler::run(int inlatency)
{
    // Save the input latency
    latency = inlatency;

    // Loop over all vertices
    for (vertex*& currVertex : dataManager->vertices)
    {
        // Update the ASAP and ALAP times
        int retVal = updateTiming();

        // Return if ALAP scheduling fails
        if (retVal)
        {
            return retVal;
        }
        
        // Update the type distributions
        updateTypeDistribution();
        // displayTypeDistribution();
        
        // Schedule all nodes except FORK or JOIN nodes
        if (currVertex->type != VertexType::FORK && currVertex->type != VertexType::JOIN)
        {
            // Initialize loop variables
            double currtotalForce;
            double minTotalForce = numeric_limits<double>::max();
            int minSelfForceTime = -1;
            
            // Loop over all times in which the node could be scheduled
            for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {

                // Get the total force of the node at the given timestep
                currtotalForce = getTotalForce(i, currVertex);

                // Update the minimum total force and time with minimum total force
                if (currtotalForce < minTotalForce) {
                    minTotalForce = currtotalForce;
                    minSelfForceTime = i;
                }
            }

            // Schedule the vertex at the time with the minimum total force
            currVertex->time = minSelfForceTime;
        }
    }

    // Update the ASAP and ALAP times
    updateTiming();

    // Loop over all the vertices
    for (vertex*& currVertex : dataManager->vertices)
    {
        // Schedule all FORK nodes at the latest possible time
        if (currVertex->type == VertexType::FORK)
        {
            currVertex->time = currVertex->alapTime;
        }

        // Schedule all JOIN nodes at the earliest possible time
        else if (currVertex->type == VertexType::JOIN)
        {
            currVertex->time = currVertex->asapTime;
        }
    }

    return 0;
}

// Function computes the total force for scheduling a vertex at a given timestep
double FDScheduler::getTotalForce(int selfForceTime, vertex* currVertex) {

    // Save ASAP and ALAP times of the given node
    int vertexAsapTime = currVertex->asapTime;
    int vertexAlapTime = currVertex->alapTime;

    // Initialize total force
    double totalForce = 0.0;
    
    // Save the ASAP and ALAP times of all nodes. For nodes already
    // scheduled these should match the time the node is scheduled.
    for (vertex* tempVertex : dataManager->vertices) {
        tempVertex->asapTestTime = tempVertex->asapTime;
        tempVertex->alapTestTime = tempVertex->alapTime;
    }

    // Set the time of the selected vertex at the time of interest
    currVertex->time = selfForceTime;

    // Update the ASAP and ALAP times. Determines the ASAP and ALAP times
    // of all nodes assuming that selected node is scheduled at the given timestep.
    updateTiming();
    
    // Loop over all vertices
    for (vertex* tempVertex : dataManager->vertices) {

        // Copy the new ASAP and ALAP times to temporary variables
        // Restore the initial ASAP and ALAP times
        int tempASAPTime = tempVertex->asapTestTime;
        int tempALAPTime = tempVertex->alapTestTime;
        tempVertex->asapTestTime = tempVertex->asapTime;
        tempVertex->alapTestTime = tempVertex->alapTime;
        tempVertex->asapTime = tempASAPTime;
        tempVertex->alapTime = tempALAPTime;

        // Compute the self force, predecessor force, and successor force
        for (int i = tempVertex->asapTestTime; i <= tempVertex->alapTestTime; i++) {
            totalForce += getSelfForce(i, tempVertex);
        }
    }
    cout << "Total Force: " << totalForce << endl << endl;

    // Restore the ASAP and ALAP times for the current node
    currVertex->asapTime = vertexAsapTime;
    currVertex->alapTime = vertexAlapTime;

    return totalForce;
}

// Function computes the self force of a given node
double FDScheduler::getSelfForce(int usedTime, vertex* currVertex) {

    // Reset the self force
    double selfForce = 0.0;

    // Do not consider FORK or JOIN nodes
    if (currVertex->type != VertexType::FORK && currVertex->type != VertexType::JOIN)
    {
        // Flag specifies whether node is running at time
        double keyAtTime = 0.0;

        // Determines the times during which the node could run
        // End time is ALAP time + run time - 1
        // Accounts for multi-cycle operations
        int runTime = getVertexRunTime(currVertex);
        int startTime = currVertex->asapTime;
        int endTime = currVertex->alapTime + runTime - 1;

        // Loop over all times during which the node could run
        for (int time = startTime; time <= endTime; ++time) {

            // Determine if node is running at time
            if (usedTime <= time && time <= (usedTime + runTime - 1)) {
                keyAtTime = 1.0;
            } else {
                keyAtTime = 0.0;
            }

            // Compute the forces for each timestep
            selfForce += typeDistribution[currVertex->type][time] * (keyAtTime - (1.0 / currVertex->mobility));
        }
    }
    return selfForce;
}

// Function computes the type distribution for nodes in a given hierarchy
map<VertexType, vector<double>> FDScheduler::getTypeDistribution(hierarchy* currHierarchy)
{
    // Create empty type distributions
    map<VertexType, vector<double>> hierTypeDistribution;
    hierTypeDistribution[VertexType::DIV] = vector<double>(latency, 0.0);
    hierTypeDistribution[VertexType::ADD] = vector<double>(latency, 0.0);
    hierTypeDistribution[VertexType::MUL] = vector<double>(latency, 0.0);
    hierTypeDistribution[VertexType::LOGIC] = vector<double>(latency, 0.0);

    // Update the type distributions for vertices in the current hierarchy
    for (vertex* currVertex : currHierarchy->vertices)
    {
        // Get vertex mobility (ALAP - ASAP + 1)
        currVertex->mobility = currVertex->alapTime - currVertex->asapTime + 1;

        // Consider all nodes except FORK's and JOIN's
        if (currVertex->type != VertexType::FORK && currVertex->type != VertexType::JOIN)
        {
            // Update type distributions for each valid time [ASAP, ALAP]
            for (int i = currVertex->asapTime; i <= currVertex->alapTime; i++) {

                // Type distribution is 1/mobility for each node in timestep
                // For multi-cycle operations, sum the contributions from each cycle
                // This is consistent with the FDS Algorithm (Paulin & Knight, 1989)
                for (int j = 0; j < getVertexRunTime(currVertex); j++) {
                    hierTypeDistribution[currVertex->type][i + j] += 1.0 / currVertex->mobility;
                }
            }
        }
    }

    // Account for the type distributions of the vertices in the conditional hierarchies
    for (conditionalHierarchy* condHierarchy : currHierarchy->conditional)
    {
        // Get the type distributions for vertices on the true or false branches of the conditional hierarchy
        map<VertexType, vector<double>> trueTypeDistribution = getTypeDistribution(condHierarchy->trueHierarchy);
        map<VertexType, vector<double>> falseTypeDistribution = getTypeDistribution(condHierarchy->falseHierarchy);

        // Loop over vertex types
        auto start = trueTypeDistribution.begin();
        auto end = trueTypeDistribution.end();
        for (auto it = start; it != end; ++it)
        {
            // Loop over times
            for (size_t time = 0; time < latency; ++time)
            {
                // Take the maximum type distributions for either branch of the conditional statement
                // This is consistent with the FDS Algorithm (Paulin & Knight, 1989)
                hierTypeDistribution[it->first][time] += max(
                    trueTypeDistribution[it->first][time],
                    falseTypeDistribution[it->first][time]);
            }
        }
    }

    return hierTypeDistribution;
}

// Function computes the type distribution
void FDScheduler::updateTypeDistribution()
{
    // Get the type distribution for the base hierarchy
    typeDistribution = getTypeDistribution(dataManager->graphHierarchy);
}

void FDScheduler::displayTypeDistribution()
{
    vector <string> typeNames = {"ADD","MUL","DIV","LOGIC"};
    vector <VertexType> types = {VertexType::ADD, VertexType::MUL, VertexType::DIV, VertexType::LOGIC};
    int typeIdx = 0;
    for (string typeName : typeNames)
    {
        VertexType type = types[typeIdx];
        cout << setw(5) << typeName;
        for (int time = 0; time < latency; ++time)
        {
            cout << setw(8) << setprecision(2) << typeDistribution[type][time];
        }
        cout << endl;
        typeIdx++;
    }
    cout << endl;
}

// Function updates the ASAP and ALAP Times
int FDScheduler::updateTiming()
{
    // Fix the ASAP and ALAP times of scheduled nodes
    // Reset the ASAP and ALAP times of other nodes to -1
    for (vertex* currVertex : dataManager->vertices) {
        currVertex->alapTime = currVertex->time;
        currVertex->asapTime = currVertex->time;
    }

    // Get ASAP times
    ASAP.run();

    // Get ALAP times
    // Return with non-zero value if scheduling fails
    return ALAP.run(latency);
}

} // namespace HighLevelSynthesis