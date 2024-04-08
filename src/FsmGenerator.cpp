#include "FsmGenerator.h"
#include "GraphComponents.h"

#include <iostream>
#include <string.h>

using namespace std;

namespace HighLevelSynthesis
{

FsmGenerator::FsmGenerator(DataManager* dataManager)
    : dataManager(dataManager) {}

// Function generates the HLSM
void FsmGenerator::run()
{
    // Create states in the HLSM
    createStates();

    // Sort states based on their scheduled times
    sortStates();
}

// Function sorts states based on their scheduled times
void FsmGenerator::sortStates()
{
    // Create an empty vector of states
    vector<state*> newStates;

    // Loop through all times
    for (int time = 0; time <= getEndTime(); ++time)
    {
        // Loop through all the states
        for (state* currState : dataManager->states)
        {
            // If state is scheduled at current time
            if (currState->time == time)
            {
                // Add state to new states array
                newStates.push_back(currState);
            }
        }
    }
    // Update array of states in the data manager
    dataManager->states = newStates;
}

void FsmGenerator::createStates()
{
    state* initialState = createNewState({dataManager->graphHierarchy}, 0);
    getNextStates(initialState, 0);
}

// Function gets the next states
void FsmGenerator::getNextStates(state* currState, int time)
{
    // Create an empty vector for the updated hierarchy
    vector <hierarchy*> hierUpdate;

    // Loop through the hierarchies in the current state
    for (hierarchy* hier : currState->hier)
    {
        // Determine whether the hierarchy is coming to an end
        bool removeHier = upcomingConditionalEnd(hier, time);

        // If not, add the hierarchy to the updated list of hierarchies
        if (!removeHier)
        {
            hierUpdate.push_back(hier);
        }
    }

    // Get a vector of new hiearchies from if-else statements
    vector <conditionalHierarchy*> condHierUpdate = getNewConditionals(currState, time);

    // Determine the number of states that the current state transitions to
    // 2^N states for N conditionals 
    int numStates = (1 << condHierUpdate.size());

    // Loop through each of the new states
    for (int i = 0; i < numStates; ++i)
    {
        // Copy all the current hierarchies into the new hierarchy vector
        vector<hierarchy*> newHier = hierUpdate;

        // Create an empty vector of strings for the conditional logic
        vector<string> condition(condHierUpdate.size());

        // Create a vector of booleans for the true/false conditions
        vector<bool> isTrue(condHierUpdate.size());

        // Loop through each of the conditions
        for (int j = 0; j < condHierUpdate.size(); ++j)
        {
            // Get the condition within the conditional 
            edge* condEdge = condHierUpdate[j]->condition;

            // Find the conditional block
            vertex* condVertex = condEdge->src;
            condition[j] = condVertex->operation;
            isTrue[j] = (i & (1 << j)) != 0;
            if (isTrue[j])
            {
                newHier.push_back(condHierUpdate[j]->trueHierarchy);
            }
            else
            {
                newHier.push_back(condHierUpdate[j]->falseHierarchy);
            }
        }
        if ((time + 1) <= getEndTime())
        {
            state* nextState = findState(newHier, time+1);
            if (nextState == NULL)
            {
                nextState = createNewState(newHier, time+1);
                getNextStates(nextState, time+1);
            }
            stateTransition* transition = new stateTransition;
            transition->condition = condition;
            transition->isTrue = isTrue;
            transition->nextState = nextState;
            currState->transitions.push_back(transition);
        }
    }
}

// Function attempts to find a state which matches a vector of hierachies at a given timestep
state* FsmGenerator::findState(vector<hierarchy*> hier, int time)
{
    // Loop through all the states in the data manager
    for (state* currState : dataManager->states)
    {
        // If the current state matches the input time
        if (currState->time == time)
        {
            // Assume hierarchy matches
            bool hierMatch = true;

            // Loop through the input hierarchies
            for (hierarchy* currHier : hier)
            {
                // Assume that the element of the hierarchy has no match
                bool hierElementMatch = false;

                // Loop through the hiearchies in the current state
                for (hierarchy* compHier : currState->hier)
                {
                    // Determine whether the hierachies match
                    if (currHier == compHier)
                    {
                        hierElementMatch = true;
                    }
                }

                // If the hierachies don't match, set the hiearchies
                // match flag to false
                if (!hierElementMatch)
                {
                    hierMatch = false;
                }
            }
            // If the hierachies match, the states are the same
            if (hierMatch)
            {
                // Return current state
                return currState;
            }
        }
    }
    // Return a NULL pointer if there is no matching state 
    return NULL;
}

// Function creates a new state
state* FsmGenerator::createNewState(vector<hierarchy*> hier, int time)
{
    // Determine the number of states scheduled at the current timestep
    int numStatesAtTime = 0;
    for (state* currState : dataManager->states)
    {
        if (currState->time == time)
        {
            numStatesAtTime++;
        }
    }
    
    // Create new state
    state* newState = new state;
    string name = "State" + to_string(time) + "_" + to_string(numStatesAtTime);
    newState->name = name;
    newState->time = time;
    newState->hier = hier;

    // Loop through the hierarchies associated with the current state
    for (hierarchy* currHier : newState->hier)
    {
        // Loop through all the vertices in the given hierarchy
        for (vertex* currVertex : currHier->vertices)
        {
            // If the vertice is scheduled at the same time as the state
            if (currVertex->time == time)
            {
                // If the vertex is not a FORK or JOIN vertex
                if (currVertex->type != VertexType::FORK && currVertex->type != VertexType::JOIN)
                {
                    // Add vertex to state
                    newState->vertices.push_back(currVertex);
                }
            }
        }
    }
    // Append state to array of states
    dataManager->states.push_back(newState);

    // Return new state
    return newState;
}

// Function determines whether the current conditional hierarchy is about to end
bool FsmGenerator::upcomingConditionalEnd(hierarchy* hier, int time)
{
    // If there are no vertices in the current hierarchy
    if (hier->vertices.empty())
    {
        // If the current hierarchy has a parent
        if (hier->parent != NULL)
        {
            // Get the true and false hiearchies that are branches of the current conditon
            hierarchy* trueHier = hier->parent->trueHierarchy;
            hierarchy* falseHier = hier->parent->falseHierarchy;

            // Select the other hiearchy that is part of the current conditonal
            // TRUE => FALSE and FALSE => TRUE
            if (hier == trueHier)
            {
                hier = falseHier;
            }
            else
            {
                hier = trueHier;
            }
        }
    }

    // If the current hierarchy has a parent (it is part of a conditional hierarchy)
    if (hier->parent != NULL)
    {
        // Get the parent hierarchy
        hierarchy* parentHier = hier->parent->parent;

        // Loop through all the vertices in the current hierarchy
        for (vertex* currVertex : hier->vertices)
        {
            // Loop through all the outputs of the current vertex
            for (edge* output : currVertex->outputs)
            {
                // Check the destinations of the current output
                for (vertex* dest : output->dest)
                {
                    // Determine the hierarchy of the current destination
                    hierarchy* destHier = dest->parent;

                    // If the destination vertex is scheduled for the next timestep
                    // and it is a JOIN vertex.
                    if ((dest->time == (time + 1)) && (dest->type == VertexType::JOIN))
                    {
                        // If the hierarchy of the destination vertex, matches the
                        // parent hierarchy return true
                        if (destHier == parentHier)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    // Return false if not end of hierarchy
    return false;
}

// Function gets new conditional hiearchies starting at a given time
vector<conditionalHierarchy*> FsmGenerator::getNewConditionals(state* currState, int time)
{
    // Create empty array for new conditional hiearchies
    vector<conditionalHierarchy*> newCondHier;

    // Loop through all the hierarchies that the state contains
    for (hierarchy* hier : currState->hier)
    {
        // Loop through all vertices in the hierarchy
        for (vertex* currVertex : hier->vertices)
        {
            // If vertex end time matches current time
            if (time == getVertexEndTime(currVertex))
            {
                // If vertex is a FORK
                if (currVertex->type == VertexType::FORK)
                {
                    // Loop through all conditional hiearchies
                    for (conditionalHierarchy* condHier : hier->conditional)
                    {
                        // Find the conditional hierarchy that matches the output of the FORK vertex
                        if (condHier->condition == currVertex->outputs[0])
                        {
                            // Append the conditional hiearchy to the vector of new conditional hiearchies
                            newCondHier.push_back(condHier);
                        }
                    }
                }
            }
        }
    }

    // Return vector of conditional hierarchies
    return newCondHier;
}

// Function gets the end time of the HLSM
int FsmGenerator::getEndTime()
{
    // Get the maximum end time of all the vertices
    int endTime = -1;
    for (vertex* currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        endTime = max(endTime, vertexEndTime);
    }

    // return the end time
    return endTime;
}

} // namespace HighLevelSynthesis