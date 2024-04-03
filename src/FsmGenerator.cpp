#include "FsmGenerator.h"
#include "GraphComponents.h"

#include <iostream>
#include <string.h>

using namespace std;

namespace HighLevelSynthesis
{

FsmGenerator::FsmGenerator(DataManager* dataManager)
    : dataManager(dataManager) {}

FsmGenerator::~FsmGenerator()
{
    for (state*& currState : states)
    {
        for (stateTransition* transition : currState->transitions)
        {
            delete(transition);
        }
        delete(currState);
    }
}

void FsmGenerator::run()
{
    /*getHierarchyMap();
    createAllStates();*/
    createStates();
    sortStates();
    printStates();
}

void FsmGenerator::printStates()
{
    for (state* currState : states)
    {
        cout << currState->name << ":" << endl;
        for (vertex* currVertex : currState->vertices)
        {
            if (currVertex->type != VertexType::JOIN)
            {
                cout << "\t" << currVertex->operation << endl;
            }
        }
    }
}

void FsmGenerator::sortStates()
{
    vector<state*> newStates;
    for (int time = 0; time < getEndTime(); ++time)
    {
        for (state* currState : states)
        {
            if (currState->time == time)
            {
                newStates.push_back(currState);
            }
        }
    }
    states = newStates;
}

void FsmGenerator::createStates()
{
    state* initialState = getState({dataManager->graphHierarchy}, 0);
    getNextStates(initialState, 0);
}

void FsmGenerator::getNextStates(state* currState, int time)
{
    vector <hierarchy*> hierUpdate;
    cout << time << " : " << currState->hier.size();
    for (hierarchy* hier : currState->hier)
    {
        bool removeHier = upcomingConditionalEnd(hier, time);
        if (!removeHier)
        {
            hierUpdate.push_back(hier);
        }
    }
    cout << " " << hierUpdate.size() << endl;
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
                newHier.push_back(condHierUpdate[j]->trueHiearchy);
            }
            else
            {
                newHier.push_back(condHierUpdate[j]->falseHiearchy);
            }
        }
        if ((time + 1) < getEndTime())
        {
            state* nextState = getState(newHier, time+1);
            stateTransition* transition = new stateTransition;
            transition->condition = condition;
            transition->isTrue = isTrue;
            transition->nextState = nextState;
            currState->transitions.push_back(transition);
            getNextStates(nextState, time+1);
        }
    }
}

state* FsmGenerator::getState(vector<hierarchy*> hier, int time)
{
    int numStatesAtTime = 0;
    for (state* currState : states)
    {
        if (currState->time == time)
        {
            numStatesAtTime++;
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
    states.push_back(newState);
    return newState;
}

bool FsmGenerator::upcomingConditionalEnd(hierarchy* hier, int time)
{
    if (hier->vertices.empty())
    {
        if (hier->parent != NULL)
        {
            hierarchy* trueHier = hier->parent->trueHiearchy;
            hierarchy* falseHier = hier->parent->falseHiearchy;
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
            // if ((time >= currVertex->time) && (time <= getVertexEndTime(currVertex)))
            // {
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
            // }
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
                /*for (edge* output : currVertex->outputs)
                {
                    for (vertex* dest : output->dest)
                    {
                        hierarchy* destHier = dest->parent;
                        if ((dest->time == (time + 1)) && (dest->type == VertexType::FORK) && (destHier == hier))
                        {
                            if ((destHier == hier) && (dest->type == VertexType::FORK))
                            {
                                for (conditionalHierarchy* condHier : hier->conditional)
                                {
                                    if (condHier->condition == dest->outputs[0])
                                    {
                                        newCondHier.push_back(condHier);
                                    }
                                }
                            }
                        }
                    }
                }*/
            }
        }
    }
    return newCondHier;
}

void FsmGenerator::createAllStates()
{
    int endTime = getEndTime();
    for (int time = 0; time < endTime; ++time)
    {
        createStatesAtTime(time);
    }
}

void FsmGenerator::createStatesAtTime(int time)
{
    vector<conditionalHierarchy*> condHierVec;
    vector<bool> inConditional(hierarchyMap[time].size(), false);
    cout << inConditional.size() << endl;
    cout << hierarchyMap[time].size() << endl;
    for (int i = 0; i < hierarchyMap[time].size(); ++i)
    {
        hierarchy* currHier = hierarchyMap[time][i];
        for (int j = 0; j < hierarchyMap[time].size(); ++j)
        {
            hierarchy* compHier = hierarchyMap[time][j];
            cout << "\t" << j << " : " << inConditional[j] << endl;
            if ((i != j) && (currHier->parent != NULL) && !inConditional[i] && !inConditional[j])
            {
                inConditional[i] = true;
                inConditional[j] = true;
                if (currHier->parent == compHier->parent)
                {
                    condHierVec.push_back(currHier->parent);
                }
            }
        }
    }
    cout << condHierVec.size() << endl;
    int stateCount = (1 << condHierVec.size());
    //cout << stateCount << endl;
    for (int i = 0; i < stateCount; ++i)
    {
        state* newState = new state;
        int j = 0;
        for (conditionalHierarchy* condHier : condHierVec)
        {
            int selectTrueCond = (i & (1 << j)) >> j;
            if (selectTrueCond)
            {
                newState->hier.push_back(condHier->trueHiearchy);
            }
            else
            {
                newState->hier.push_back(condHier->falseHiearchy);
            }
            ++j;
        }
        newState->name = "State" + to_string(time) + "_" + to_string(i);
        if (stateCount == 1)
        {
            newState->name = "State" + to_string(time);
            newState->hier = hierarchyMap[time];
        }
        for (hierarchy* currHier : newState->hier)
        {
            addVerticesRecursive(newState, currHier, time);
        }
        states.push_back(newState);
    }
};

void FsmGenerator::addVerticesRecursive(state* currState, hierarchy* hier, int time)
{
    for (vertex* currVertex : hier->vertices)
    {
        if (currVertex->time == time)
        {
            currState->vertices.push_back(currVertex);
        }
    }
    if (hier->parent != NULL)
    {
        addVerticesRecursive(currState, hier->parent->parent, time);
    }
}

bool FsmGenerator::isParentHierarchy(hierarchy* currHier, hierarchy* compHier)
{
    hierarchy* searchHier = currHier;
    bool isParent = searchHier == compHier;
    while (searchHier->parent != NULL)
    {
        searchHier = searchHier->parent->parent;
        if ((!isParent) && (searchHier == compHier))
        {
            isParent = true;
        }
    }
    return isParent;
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

void FsmGenerator::initializeHierarchyMap()
{
    int endTime = getEndTime();
    int numTimesteps = endTime; // + 1;
    vector<hierarchy*> emptyHierarchyVector;
    vector<vector<hierarchy*>> emptyHierarchyMap (numTimesteps, {dataManager->graphHierarchy});
    hierarchyMap = emptyHierarchyMap;
}

void FsmGenerator::getHierarchyMap()
{
    initializeHierarchyMap();
    getHierarchyMap(dataManager->graphHierarchy);
    for (int time = 0; time < hierarchyMap.size(); ++time)
    {
        vector<hierarchy*> newHierVector;
        for (hierarchy* currHier : hierarchyMap[time])
        {
            bool isParent = false;
            for (hierarchy* compHier : hierarchyMap[time])
            {
                if (currHier!=compHier)
                {
                    if (!isParent)
                    {
                        isParent = isParentHierarchy(compHier, currHier);
                    }
                }
            }
            if (!isParent)
            {
                newHierVector.push_back(currHier);
            }
        }
        hierarchyMap[time] = newHierVector;
        // cout <<  hierarchyMap[time].size() << endl;
    }
}

void FsmGenerator::getHierarchyMap(hierarchy* hier)
{
    for (conditionalHierarchy* condHier : hier->conditional)
    {
        int startTime = getConditionalStartTime(condHier);
        int endTime = getConditionalEndTime(condHier);
        for (int time = startTime; time < endTime; ++time)
        {
            hierarchyMap[time].push_back(condHier->trueHiearchy);
            hierarchyMap[time].push_back(condHier->falseHiearchy);
        }
        getHierarchyMap(condHier->trueHiearchy);
        getHierarchyMap(condHier->falseHiearchy);
    }
}

int FsmGenerator::getConditionalStartTime(conditionalHierarchy* condHier)
{
    vertex* condVertex = condHier->condition->src;
    return condVertex->time + 1;
};

int FsmGenerator::getConditionalEndTime(conditionalHierarchy* condHier)
{
    int endTime = -1;
    for (vertex* currVertex : condHier->trueHiearchy->vertices)
    {
        for (edge* currEdge : currVertex->outputs)
        {
            for (vertex* destVertex : currEdge->dest)
            {
                endTime = max(endTime, destVertex->time);
            }
        }
    }
    for (vertex* currVertex : condHier->falseHiearchy->vertices)
    {
        for (edge* currEdge : currVertex->outputs)
        {
            for (vertex* destVertex : currEdge->dest)
            {
                endTime = max(endTime, destVertex->time);
            }
        }
    }
    return endTime;
}

} // namespace HighLevelSynthesis