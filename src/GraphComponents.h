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
        VertexType type;
        string operation; // Operation that will be implemented in HLSM
        vector <edge*> inputs; 
        vector <edge*> outputs;
    };
    
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h