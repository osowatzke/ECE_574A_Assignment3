#include "DataManager.h"

#include <iostream>
#include <map>

using namespace std;

namespace HighLevelSynthesis
{
    DataManager::DataManager()
    {
        graphHierarchy = new hierarchy;
        graphHierarchy->parent = NULL;
    }

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

        deleteHierarchy(graphHierarchy);
    }

    void DataManager::deleteHierarchy(hierarchy* hier)
    {
        for (conditionalHierarchy*& condHierarchy : hier->conditional)
        {
            if (condHierarchy->trueHiearchy != NULL)
            {
                deleteHierarchy(condHierarchy->trueHiearchy);
            }
            if (condHierarchy->falseHiearchy != NULL)
            {
                deleteHierarchy(condHierarchy->falseHiearchy);
            }
        }
        delete(hier);
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
} // namespace HighLevelSynthesis