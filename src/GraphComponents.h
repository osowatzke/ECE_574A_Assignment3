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

    struct vertex
    {
        int asapTime = -1;
        int alapTime = -1;
        int asapTestTime = -1;
        int alapTestTime = -1;
        int mobility = 0;
        int time;
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
        hierarchy* trueHierarchy;
        hierarchy* falseHierarchy;
    };

    struct stateTransition;

    struct state
    {
        int time;
        string name;
        vector<vertex*> vertices;
        vector<hierarchy*> hier;
        vector<stateTransition*> transitions;
    };

    struct stateTransition
    {
        vector<string> condition;
        vector<bool> isTrue;
        state* nextState;
    };

    const int DIV_TIME = 3;
    const int ADD_TIME = 1;
    const int MUL_TIME = 2;
    const int LOGIC_TIME = 1;

    inline int getVertexRunTime(vertex* currVertex)
    {
        switch (currVertex->type)
        {
            case VertexType::DIV:
                return DIV_TIME;
            case VertexType::ADD:
                return ADD_TIME;
            case VertexType::MUL:
                return MUL_TIME;
            case VertexType::LOGIC:
                return LOGIC_TIME;
            case VertexType::FORK:
                return 1;
        }
        return 0;
    }
    
    inline int getVertexEndTime(vertex* currVertex)
    {
        int runTime = getVertexRunTime(currVertex);
        return currVertex->time + runTime - 1;
    }
    
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h