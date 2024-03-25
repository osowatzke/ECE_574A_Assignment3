#ifndef GUARD_GraphComponents_h
#define GUARD_GraphComponents_h

#include <map>
#include <string>
#include <vector>

using namespace std;

namespace HighLevelSynthesis
{

    /*enum EdgeType{INPUT,OUTPUT,VARIABLE};

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
    }*/

    struct vertex;

    struct edge
    {
        vertex* src;
        vector<vertex*> dest;
    };

    enum VertexType{DIV, ADD, MUL, LOGIC}; //, FORK, JOIN};
                                             
    struct vertex
    {
        VertexType type;
        string operation;
        // vertex* branch;
        // bool cond;
        vector <edge*> inputs; 
        vector <edge*> outputs;
    };

    /*inline string vertexToString(vertex* currVertex)
    {
        switch (currVertex->type) {
            case VertexType::REG:
                return "U";
            case VertexType::ADD:
                return "+";
            case VertexType::SUB:
                return "-";
            case VertexType::MUL:
                return "*";
            case VertexType::COMP:
                if (currVertex->outputs.find("gt") != currVertex->outputs.end())
                {
                    return ">";
                }
                else if (currVertex->outputs.find("lt") != currVertex->outputs.end())
                {
                    return "<";
                }
                else
                {
                    return "==";
                }
            case VertexType::MUX2x1:
                return "M";
            case VertexType::SHR:
                return "<<";
            case VertexType::SHL:
                return ">>";
            case VertexType::DIV:
                return "/";
            case VertexType::MOD:
                return "%";
            case VertexType::INC:
                return "+";
            case VertexType::DEC:
                return "-";
        }
        return "U";
    }*/
} // namespace HighLevelSynthesis

#endif // GUARD_GraphComponents_h