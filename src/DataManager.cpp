#include "DataManager.h"

#include <iostream>

using namespace std;

namespace HighLevelSynthesis
{
    DataManager::~DataManager()
    {
        // Delete Nets
        auto netStart = nets.begin();
        auto netEnd = nets.end();
        for (auto it = netStart; it != netEnd; ++it)
        {
            delete(it->second);
        }

        // Delete Vertices
        for (vertex*& currVertex : vertices)
        {
            delete(currVertex);
        }

        // Delete Edges
        for (edge*& currEdge : edges)
        {
            delete(currEdge);
        }
    }

    void DataManager::printGraph()
    {
        printVertices();
        printEdges();
    }

    void DataManager::printVertices()
    {
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            cout << "v" << i << " : " << vertices[i]->operation << endl;
        }
        cout << endl;
    }

    void DataManager::printEdges()
    {
        map<vertex*, string> vertexMap;
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            vertexMap[vertices[i]] = "v" + to_string(i);
        }
        for (edge*& currEdge : edges)
        {
            string currEdgeSrc = "INODE";
            if (currEdge->src != NULL)
            {
                currEdgeSrc = vertexMap[currEdge->src];
            }
            cout << currEdgeSrc << " -> [";
            bool firstDest = true;
            if (currEdge->dest.size() == 0)
            {
                cout << "ONODE";
            }
            else
            {
                for (vertex* dest : currEdge->dest)
                {
                    if (!firstDest)
                    {
                        cout << ", ";
                    }
                    firstDest = false;
                    cout << vertexMap[dest];
                }
            }
            cout << "]" << endl;
        }
        cout << endl;
    }
    /*void DataManager::VisualizeGraph()
    {
        map<edge*, edge*> edgeMap;
        vector<edge*> edgesCopy;
        for (edge*& currEdge : edges)
        {
            edge* newEdge = new edge(*currEdge);
            newEdge->dest = {};
            edgeMap[currEdge] = newEdge;
            edgesCopy.push_back(newEdge);
        }
        vector<vertex*> verticesCopy;
        for (vertex*& currVertex : vertices)
        {
            vertex* newVertex = new vertex(*currVertex);
            auto start = newVertex->inputs.begin();
            auto end = newVertex->inputs.end();
            for (auto it = start; it != end; ++it)
            {
                edge* oldEdge = it->second;
                edge* newEdge = edgeMap[oldEdge];
                it->second = newEdge;
                newEdge->dest.push_back(newVertex);
            }
            start = newVertex->outputs.begin();
            end = newVertex->outputs.end();
            for (auto it = start; it != end; ++it)
            {
                edge* oldEdge = it->second;
                edge* newEdge = edgeMap[oldEdge];
                it->second = newEdge;
                newEdge->src = newVertex;
            }
            verticesCopy.push_back(newVertex);
        }
    }*/
}