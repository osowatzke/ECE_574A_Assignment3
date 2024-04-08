#include "DataManager.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <fstream>

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

        // Delete States
        for (state*& currState : states)
        {
            // Delete State Transitions
            for (stateTransition* transition : currState->transitions)
            {
                delete(transition);
            }
            delete(currState);
        }
        
        deleteHierarchy(graphHierarchy);
    }

    void DataManager::deleteHierarchy(hierarchy* hier)
    {
        for (conditionalHierarchy*& condHierarchy : hier->conditional)
        {
            if (condHierarchy->trueHierarchy != NULL)
            {
                deleteHierarchy(condHierarchy->trueHierarchy);
            }
            if (condHierarchy->falseHierarchy != NULL)
            {
                deleteHierarchy(condHierarchy->falseHierarchy);
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

    void DataManager::visualizeGraph()
    {
        // for inputs
        // {
        //      node [shape=plaintext]; a; b;
        // }
        ofstream digraph;
        digraph.open("../input.dot");
        digraph << "digraph {" << endl;
        map<vertex*, string> vertexMap;
        int idx = 1;
        bool hasFork = false;
        bool hasJoin = false;
        for (vertex* currVertex : vertices)
        {
            vertexMap[currVertex] = "v" + to_string(idx++);
            if (currVertex->type == VertexType::FORK)
            {
                hasFork = true;
            }
            if (currVertex->type == VertexType::FORK)
            {
                hasJoin = true;
            }
        }
        if (hasFork)
        {
            digraph << "{" << endl;
            digraph << "    node [shape = trapezium];" << endl;
            for (vertex* currVertex : vertices)
            {
                if (currVertex->type == VertexType::FORK)
                {
                    digraph << "    " << vertexMap[currVertex] << "[label=\"FORK\"];" << endl;
                }
            }
            digraph << "}" << endl;
        }
        if (hasJoin)
        {
            digraph << "{" << endl;
            digraph << "    node [shape = invtrapezium];" << endl;
            for (vertex* currVertex : vertices)
            {
                if (currVertex->type == VertexType::JOIN)
                {
                    digraph << "    " << vertexMap[currVertex] << "[label=\"JOIN\"];" << endl;
                }
            }
            digraph << "}" << endl;
        }
        string startVertex;
        string endVertex;
        for (vertex* currVertex : vertices)
        {
            startVertex = vertexMap[currVertex];
            for (edge* output : currVertex->outputs)
            {
                for (vertex* dest : output->dest)
                {
                    endVertex = vertexMap[dest];
                    digraph << "    " << startVertex << " -> " << endVertex << endl;
                }
            }
        }
        digraph << "{" << endl;
        // digraph << "    rankdir=LR;" << endl;
        digraph << "    rank=sink;" << endl;
        digraph << "    legend [shape=none, margin=0, label=<" << endl;
        digraph << "    <TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">" << endl;
        for (vertex* currVertex : vertices)
        {
            if ((currVertex->type != VertexType::JOIN) && (currVertex->type != VertexType::FORK))
            {
                string operation = currVertex->operation;

                size_t pos = operation.find("<=");
                while (pos != string::npos)
                {
                    operation.replace(pos, 2, "=");
                    pos = operation.find("<=", pos + 1);
                }

                pos = operation.find("<");
                while (pos != string::npos)
                {
                    operation.replace(pos, 1, "&gt;");
                    pos = operation.find("<", pos + 4);
                }

                pos = operation.find(">");
                while (pos != string::npos)
                {
                    operation.replace(pos, 1, "&lt;");
                    pos = operation.find(">", pos + 4);
                }
                // operation.replace(remove(operation.begin(), operation.end(), '<'), "&gt");
                digraph << "    <TR><TD>" << vertexMap[currVertex] << "</TD><TD>" << operation << "</TD></TR>" << endl;
            }
        }
        digraph << "    </TABLE>>];" << endl;
        digraph << "}" << endl;
        digraph << "}" << endl;
        digraph.close();
    }
} // namespace HighLevelSynthesis