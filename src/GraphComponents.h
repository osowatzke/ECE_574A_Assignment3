#ifndef GUARD_GraphComponents_h
#define GUARD_GraphComponents_h

#include <map>
#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    struct vertex;

    // Structure defines an edge
    struct edge
    {
        vertex* src;
        vector<vertex*> dest;
    };

    enum VertexType{DIV, ADD, MUL, LOGIC, FORK, JOIN};

    struct hierarchy;

    // Structure defines a vertex
    struct vertex
    {
        int asapTime = -1;
        int alapTime = -1;
        int asapTestTime = -1;
        int alapTestTime = -1;
        int mobility = 0;
        int time = -1;
        hierarchy* parent;
        VertexType type;
        string operation;
        vector <edge*> inputs; 
        vector <edge*> outputs;
    };

    struct conditionalHierarchy;

    // Structure defines a hierarchy which includes vertices and edges
    // defined at top-level or T/F branch of conditional hierarchy
    struct hierarchy
    {
        conditionalHierarchy* parent;
        vector<conditionalHierarchy*> conditional;
        vector<vertex*> vertices;
        map<string, edge*> edges;
    };

    // Structure defines a conditional hierarchy which includes
    // T/F hierarchies instantianted within conditional hierarchy
    struct conditionalHierarchy
    {
        edge* condition;
        hierarchy* parent;
        hierarchy* trueHierarchy;
        hierarchy* falseHierarchy;
    };

    struct stateTransition;

    // Structure defines a state
    struct state
    {
        int time;
        string name;
        vector<vertex*> vertices;
        vector<hierarchy*> hier;
        vector<stateTransition*> transitions;
    };

    // Structure defines a state transition
    struct stateTransition
    {
        vector<string> condition;
        vector<bool> isTrue;
        state* nextState;
    };

    // Define time of each operation
    const int DIV_TIME = 3;
    const int ADD_TIME = 1;
    const int MUL_TIME = 2;
    const int LOGIC_TIME = 1;

    // Inline function computes the vertex run time
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
    
    // Inline function computes the vertex end time.
    // Note that function uses to the vertex time
    // instead of the ASAP and ALAP time so it cannot
    // be called by the ASAP and ALAP scheduler.
    inline int getVertexEndTime(vertex* currVertex)
    {
        int runTime = getVertexRunTime(currVertex);
        return currVertex->time + runTime - 1;
    }
    
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h