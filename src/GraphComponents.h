#ifndef GUARD_GraphComponents_h
#define GUARD_GraphComponents_h

#include <map>
#include <string>
#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    struct vertex;

    enum EdgeType{INPUT,OUTPUT,VARIABLE};

    inline EdgeType stringToEdgeType(string edgeString)
    {
        if (edgeString == "input")
        {
            return EdgeType::INPUT;
        }
        else if (edgeString == "output")
        {
            return EdgeType::OUTPUT;
        }
        else
        {
            return EdgeType::VARIABLE;
        }
    }

    struct edge
    {
        int width;
        bool isSigned;
        EdgeType type;
        vertex* src;
        vector<vertex*> dest;
    };

    enum VertexType{REG,ADD,SUB,MUL,COMP,MUX2x1,SHR,SHL,DIV,MOD,INC,DEC}; 
                                             
    struct vertex
    {
        int width;
        VertexType type;
        map<string, edge*> inputs; 
        map<string, edge*> outputs;
    };
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h