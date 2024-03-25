#ifndef GUARD_CircuitComponents_h
#define GUARD_CircuitComponents_h

#include <string>

using namespace std;

namespace HighLevelSynthesis
{
    enum NetType{INPUT, OUTPUT, VARIABLE};

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

    struct net
    {
        NetType type;
        int width;
        bool isSigned;
    };
    
} // namespace HighLevelSynthesis

#endif // GUARD_CircuitComponents_h