#ifndef GUARD_GraphComponents_h
#define GUARD_GraphComponents_h

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

    enum VertexType{DIV, ADD, MUL, LOGIC};
                                             
    struct vertex
    {
        int time;
        VertexType type;
        string operation; // Operation that will be implemented in HLSM
        vector <edge*> inputs; 
        vector <edge*> outputs;
    };

    const int DIV_TIME = 3;
    const int ADD_TIME = 1;
    const int MUL_TIME = 2;
    const int LOGIC_TIME = 1;

    inline int getVertexRunTime(vertex* currVertex)
    {
        int runTime = 1;
        switch (currVertex->type)
        {
            case VertexType::DIV :
                runTime = 3;
            case VertexType::MUL :
                runTime = 2;
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