#ifndef GUARD_HighLevelSynthesisTool_h
#define GUARD_HighLevelSynthesisTool_h

#include "AlapScheduler.h"
#include "DataManager.h"
#include "FileParser.h"
#include "FileWriter.h"

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
        AlapScheduler scheduler;
        FileWriter fileWriter;
};

} // namespace HighLevelSynthesis

#endif // GUARD_HighLevelSynthesisTool_h