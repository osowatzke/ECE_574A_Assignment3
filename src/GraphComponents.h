#ifndef GUARD_GraphComponents_h
#define GUARD_GraphComponents_h

#include <map>
#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    struct vertex;

    struct edge
    {
        vertex* src;
        vector<vertex*> dest;
    };

    enum VertexType{DIV, ADD, MUL, LOGIC, FORK, JOIN};

    struct hierarchy;
    struct state;

    struct vertex
    {
        int time;
        state* currState;
        hierarchy* parent;
        VertexType type;
        string operation; // Operation that will be implemented in HLSM
        vector <edge*> inputs; 
        vector <edge*> outputs;
    };

    struct conditionalHierarchy;

    struct hierarchy
    {
        conditionalHierarchy* parent;
        vector<conditionalHierarchy*> conditional;
        vector<vertex*> vertices;
        map<string, edge*> edges;
    };

    struct conditionalHierarchy
    {
        edge* condition;
        hierarchy* parent;
        hierarchy* trueHiearchy;
        hierarchy* falseHiearchy;
    };

    
    struct conditionalState;

    struct state
    {
        vector<vertex*> vertices;
        state* prevState;
        state* nextState;
        conditionalState* nextCondState;
    };

    struct conditionalState
    {
        string condition;
        state* trueState;
        state* falseState;
        conditionalState* trueCondState;
        conditionalState* falseCondState;
    };

    const int DIV_TIME = 3;
    const int ADD_TIME = 1;
    const int MUL_TIME = 2;
    const int LOGIC_TIME = 1;

    inline int getVertexRunTime(vertex* currVertex)
    {
        int runTime = 0;
        switch (currVertex->type)
        {
            case VertexType::DIV:
                runTime = 3;
            case VertexType::ADD:
                runTime = 1;
            case VertexType::MUL:
                runTime = 2;
            case VertexType::LOGIC:
                runTime = 1;
            case VertexType::FORK:
                runTime = 1;
        }
        return runTime;
    }
    
    inline int getVertexEndTime(vertex* currVertex)
    {
        int runTime = getVertexRunTime(currVertex);
        return currVertex->time + runTime - 1;
    }
    
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h