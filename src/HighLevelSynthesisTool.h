#ifndef GUARD_HighLevelSynthesisTool_h
#define GUARD_HighLevelSynthesisTool_h

#include "AlapScheduler.h"
#include "AsapScheduler.h"
#include "DataManager.h"
#include "FileParser.h"
#include "FileWriter.h"
#include "FsmGenerator.h"
#include "FDScheduler.h"

#include <string>

#define DEBUG_GRAPH

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
        FDScheduler scheduler;
        FsmGenerator fsmGenerator;
        FileWriter fileWriter;
};

} // namespace HighLevelSynthesis

#endif // GUARD_HighLevelSynthesisTool_h