#ifndef GUARD_HighLevelSynthesisTool_h
#define GUARD_HighLevelSynthesisTool_h

#include "DataManager.h"
#include "FileParser.h"

#include <string>

using namespace std;

namespace HighLevelSynthesis
{

class HighLevelSynthesisTool
{
    public:
        HighLevelSynthesisTool();
        int run(string cFile, int latency, string verilogFile);
    private:
        DataManager dataManager;
        FileParser fileParser;
};

} // namespace HighLevelSynthesis

#endif // GUARD_HighLevelSynthesisTool_h