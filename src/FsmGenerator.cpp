#include "FsmGenerator.h"
#include "GraphComponents.h"

#include <iostream>
#include <string.h>

using namespace std;

namespace HighLevelSynthesis
{

FsmGenerator::FsmGenerator(DataManager* dataManager)
    : dataManager(dataManager) {}

void FsmGenerator::run()
{
    createStates();
    sortStates();
}

void FsmGenerator::sortStates()
{
    vector<state*> newStates;
    for (int time = 0; time < getEndTime(); ++time)
    {
        for (state* currState : dataManager->states)
        {
            if (currState->time == time)
            {
                newStates.push_back(currState);
            }
        }
    }
    dataManager->states = newStates;
}

void FsmGenerator::createStates()
{
    state* initialState = createState({dataManager->graphHierarchy}, 0);
    getNextStates(initialState, 0);
}

void FsmGenerator::getNextStates(state* currState, int time)
{
    vector <hierarchy*> hierUpdate;
    for (hierarchy* hier : currState->hier)
    {
        bool removeHier = upcomingConditionalEnd(hier, time);
        if (!removeHier)
        {
            hierUpdate.push_back(hier);
        }
    }
    vector <conditionalHierarchy*> condHierUpdate = getNewConditionals(currState, time);
    int numStates = (1 << condHierUpdate.size());
    for (int i = 0; i < numStates; ++i)
    {
        vector<hierarchy*> newHier = hierUpdate;
        vector<string> condition(condHierUpdate.size());
        vector<bool> isTrue(condHierUpdate.size());
        for (int j = 0; j < condHierUpdate.size(); ++j)
        {
            edge* condEdge = condHierUpdate[j]->condition;
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
        if ((time + 1) < getEndTime())
        {
            state* nextState = findState(newHier, time+1);
            if (nextState == NULL)
            {
                nextState = createState(newHier, time+1);
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

state* FsmGenerator::findState(vector<hierarchy*> hier, int time)
{
    for (state* currState : dataManager->states)
    {
        if (currState->time == time)
        {
            bool hierMatch = true;
            for (hierarchy* currHier : hier)
            {
                bool hierElementMatch = false;
                for (hierarchy* compHier : currState->hier)
                {
                    if (currHier == compHier)
                    {
                        hierElementMatch = true;
                    }
                }
                if (!hierElementMatch)
                {
                    hierMatch = false;
                }
            }
            if (hierMatch)
            {
                return currState;
            }
        }
    }
    return NULL;
}

state* FsmGenerator::createState(vector<hierarchy*> hier, int time)
{
    int numStatesAtTime = 0;
    for (state* currState : dataManager->states)
    {
        if (currState->time == time)
        {
            numStatesAtTime++;
        }
    }
    state* newState = new state;
    string name = "State" + to_string(time) + "_" + to_string(numStatesAtTime);
    newState->name = name;
    newState->time = time;
    newState->hier = hier;
    for (hierarchy* currHier : newState->hier)
    {
        for (vertex* currVertex : currHier->vertices)
        {
            if (currVertex->time == time)
            {
                if (currVertex->type != VertexType::FORK && currVertex->type != VertexType::JOIN)
                {
                    newState->vertices.push_back(currVertex);
                }
            }
        }
    }
    dataManager->states.push_back(newState);
    return newState;
}

bool FsmGenerator::upcomingConditionalEnd(hierarchy* hier, int time)
{
    if (hier->vertices.empty())
    {
        if (hier->parent != NULL)
        {
            hierarchy* trueHier = hier->parent->trueHierarchy;
            hierarchy* falseHier = hier->parent->falseHierarchy;
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
    if (hier->parent != NULL)
    {
        hierarchy* parentHier = hier->parent->parent;
        for (vertex* currVertex : hier->vertices)
        {
            for (edge* output : currVertex->outputs)
            {
                for (vertex* dest : output->dest)
                {
                    hierarchy* destHier = dest->parent;
                    if ((dest->time == (time + 1)) && (dest->type == VertexType::JOIN))
                    {
                        if (destHier == parentHier)
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

vector<conditionalHierarchy*> FsmGenerator::getNewConditionals(state* currState, int time)
{
    vector<conditionalHierarchy*> newCondHier;
    for (hierarchy* hier : currState->hier)
    {
        for (vertex* currVertex : hier->vertices)
        {
            if (time == getVertexEndTime(currVertex))
            {
                if (currVertex->type == VertexType::FORK)
                {
                    for (conditionalHierarchy* condHier : hier->conditional)
                    {
                        if (condHier->condition == currVertex->outputs[0])
                        {
                            newCondHier.push_back(condHier);
                        }
                    }
                }
            }
        }
    }
    return newCondHier;
}

int FsmGenerator::getEndTime()
{
    int endTime = -1;
    for (vertex* currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        endTime = max(endTime, vertexEndTime + 1);
    }
    return endTime;
}

} // namespace HighLevelSynthesis