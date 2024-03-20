#ifndef GUARD_HighLevelSynthesisTool_h
#define GUARD_HighLevelSynthesisTool_h

#include <string>

using namespace std;

namespace HighLevelSynthesis
{

class HighLevelSynthesisTool
{
    public:
        int run(string cFile, int latency, string verilogFile);
};

} // namespace HighLevelSynthesis

#endif // GUARD_HighLevelSynthesisTool_h