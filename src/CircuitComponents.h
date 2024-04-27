#ifndef GUARD_CircuitComponents_h
#define GUARD_CircuitComponents_h

#include <string>

using namespace std;

namespace HighLevelSynthesis
{
    enum NetType{INPUT, OUTPUT, VARIABLE};

    // Inline function converts net type to string
    inline NetType stringToNetType(string netString)
    {
        if (netString == "input")
        {
            return NetType::INPUT;
        }
        else if (netString == "output")
        {
            return NetType::OUTPUT;
        }
        else
        {
            return NetType::VARIABLE;
        }
    }

    // Structure defines an net (input, output, or variable)
    struct net
    {
        NetType type;
        int width;
        bool isSigned;
    };
    
} // namespace HighLevelSynthesis

#endif // GUARD_CircuitComponents_h