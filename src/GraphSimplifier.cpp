#include "GraphSimplifier.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace HighLevelSynthesis
{

GraphSimplifier::GraphSimplifier(DataManager* dataManager)
    : dataManager(dataManager) {}

void GraphSimplifier::run()
{
    removeUnusedVertices();
    removeUnusedConditionalHierarchies();
}

void GraphSimplifier::isOutputRequired(vertex* currVertex)
{
    for (edge* output : currVertex->outputs)
    {
        for (bool isImplicit : output->isImplicit)
        {

        }
    }
}

bool GraphSimplifier::isOutputRequired(edge* output)
{
    bool retVal = false;
    if (output->type == NetType::OUTPUT)
    {
        retVal = true;
    }
    else
    {
        for (bool isImplict : output->isImplicit)
        {
            if (!isImplict)
            {
                retVal = true;
                break;
            }
        }
    }
    return retVal;
}

void GraphSimplifier::removeUnusedOutputs()
{
    bool check = true;
    while (check)
    {
        for (vertex* currVertex : dataManager->vertices)
        {
            removeUnusedOutputs();
        }
    }
}

bool GraphSimplifier::removeUnusedOutputs(vertex* currVertex)
{
    bool retVal;
    vector<edge*> edgesToRemove;
    int outputIdx = 0;
    vector<bool> removeOutput(currVertex->outputs.size(), false);
    for (edge* output : currVertex->outputs)
    {
        if (!isOutputRequired(output))
        {
            retVal = true;
            edgesToRemove.push_back(output);
            removeOutput[outputIdx] = true;
        }
        outputIdx++;
    }
    vector<edge*> inputsToDisconnect;
    for (edge* input : currVertex->inputs)
    {
        bool disconnectInput = true;
        int checkIdx = 0;
        for (vector<edge*> dependencies : currVertex->dependencies)
        {
            if (!removeOutput[checkIdx])
            {
                for (edge* dependency : dependencies)
                {
                    if (dependency == input)
                    {
                        disconnectInput = false;
                    }
                }
            }
            checkIdx++;
        }
        if (disconnectInput)
        {
            inputsToDisconnect.push_back(input);
        }
    }
    for (edge* inputToDisconnect : inputsToDisconnect)
    {
        auto edgeStart = currVertex->inputs.begin();
        auto edgeEnd = currVertex->inputs.end();
        auto edgeIt = find(edgeStart, edgeEnd, inputToDisconnect);
        currVertex->inputs.erase(edgeIt);

        auto vertexStart = inputToDisconnect->dest.begin();
        auto vertexEnd = inputToDisconnect->dest.end();
        auto vertexIt = find(vertexStart,vertexEnd,currVertex);
        inputToDisconnect->dest.erase(vertexIt);
    }
    for (edge* edgeToRemove : edgesToRemove)
    {
        auto edgeStart = currVertex->outputs.begin();
        auto edgeEnd = currVertex->outputs.end();
        auto edgeIt = find(edgeStart, edgeEnd, edgeToRemove);
        size_t idx = distance(edgeStart, edgeIt);
        auto dependencyStart = currVertex->dependencies.begin();
        auto dependancyEnd = currVertex->dependencies.end();
        auto dependencyIt = dependencyStart + idx;
        currVertex->dependencies.erase(dependencyIt);
        removeEdge(edgeToRemove);
    }
    return retVal;
}

bool GraphSimplifier::isVertexRequired(vertex* currVertex)
{
    bool retVal = false;
    for (edge* output : currVertex->outputs)
    {
        if (output->type == NetType::OUTPUT)
        {
            retVal = true;
        }
        else
        {
            for (bool isImplict : output->isImplicit)
            {
                if (!isImplict)
                {
                    retVal = true;
                    break;
                }
            }
        }
    }
    return retVal;
}

void GraphSimplifier::removeUnusedVertices()
{
    bool check = true;
    while (check)
    {
        check = false;
        for (vertex* currVertex : dataManager->vertices)
        {
            if (!isVertexRequired(currVertex))
            {
                removeVertex(currVertex);
                check = true;
                break;
            }
        }
    }
}

void GraphSimplifier::removeUnusedConditionalHierarchies()
{
    removeUnusedConditionalHierarchies(dataManager->graphHierarchy);
}

void GraphSimplifier::removeUnusedConditionalHierarchies(hierarchy* hierarchy)
{
    bool check = true;
    while (check)
    {
        check = false;
        for (conditionalHierarchy* condHierarchy : hierarchy->conditional)
        {
            removeUnusedConditionalHierarchies(condHierarchy->trueHierarchy);
            removeUnusedConditionalHierarchies(condHierarchy->falseHierarchy);
            if (isConditionalHierarchyEmpty(condHierarchy))
            {
                removeConditionalHierarchy(condHierarchy);
                check = true;
                break;
            }
        }
    }
}

bool GraphSimplifier::isConditionalHierarchyEmpty(conditionalHierarchy* condHierarchy)
{
    return isHierarchyEmpty(condHierarchy->trueHierarchy)
        &  isHierarchyEmpty(condHierarchy->falseHierarchy);
}

bool GraphSimplifier::isHierarchyEmpty(hierarchy* currHierarchy)
{
    return currHierarchy->vertices.empty()
        &  currHierarchy->conditional.empty();
}

void GraphSimplifier::removeHierarchy(hierarchy* currHierarchy)
{
    auto start = currHierarchy->edges.begin();
    auto end = currHierarchy->edges.end();
    vector<edge*> edgesToRemove;
    for (auto it = start; it != end; ++it)
    {
        edgesToRemove.push_back(it->second);
    }
    for (edge* edgeToRemove : edgesToRemove)
    {
        removeEdge(edgeToRemove);
    }
    delete(currHierarchy);
}

void GraphSimplifier::removeConditionalHierarchy(conditionalHierarchy* condHierarchy)
{
    removeHierarchy(condHierarchy->trueHierarchy);
    removeHierarchy(condHierarchy->falseHierarchy);
    auto start = condHierarchy->parent->conditional.begin();
    auto end = condHierarchy->parent->conditional.begin();
    auto it = find(start,end,condHierarchy);
    condHierarchy->parent->conditional.erase(it);
    delete(condHierarchy);
}

void GraphSimplifier::removeVertex(vertex* currVertex)
{
    // Remove vertex from vector in data manager
    auto start = dataManager->vertices.begin();
    auto end = dataManager->vertices.end();
    auto it = find(start,end,currVertex);
    dataManager->vertices.erase(it);

    // Remove vertex from vector in hierarchy
    hierarchy* currHierarchy = currVertex->parent;
    start = currHierarchy->vertices.begin();
    end = currHierarchy->vertices.end();
    it = find(start,end,currVertex);
    currHierarchy->vertices.erase(it);
    //cout << currVertex->operation << endl;
    // cout << "Num Vertices: " << currHierarchy->vertices.size() << endl;

    // Remove all edges output by vertex
    for (edge* output : currVertex->outputs)
    {
        removeEdge(output);
    }

    int idx = 0;
    for (edge* input : currVertex->inputs)
    {
        start = input->dest.begin();
        end = input->dest.end();
        it = find(start, end, currVertex);
        idx = distance(start, it);
        input->dest.erase(it);
        auto startImplicit = input->isImplicit.begin();
        auto itImplicit = startImplicit + idx;
        input->isImplicit.erase(itImplicit);
    }

    // Delete current vertex
    delete(currVertex);
}

void GraphSimplifier::removeEdge(edge* currEdge)
{
    auto vecStart = dataManager->edges.begin();
    auto vecEnd = dataManager->edges.end();
    auto vecIt = find(vecStart,vecEnd,currEdge);
    dataManager->edges.erase(vecIt);

    hierarchy* currHierarchy = currEdge->parent;
    auto mapStart = currHierarchy->edges.begin();
    auto mapEnd = currHierarchy->edges.end();
    cout << currHierarchy->edges.size() << endl;
    for (auto it = mapStart; it != mapEnd; it++)
    {
        if (it->second == currEdge)
        {
            currHierarchy->edges.erase(it->first);
            break;
        }
    }
    cout << currHierarchy->edges.size() << endl;

    for (vertex* dest : currEdge->dest)
    {
        auto start = dest->inputs.begin();
        auto end = dest->inputs.end();
        auto it = find(start,end,currEdge);
        dest->inputs.erase(it);
    }

    delete(currEdge);
}

} // namespace HighLevelSynthesis