#ifndef GUARD_HighLevelSynthesisTool_h
#define GUARD_HighLevelSynthesisTool_h

#include "AlapScheduler.h"
#include "AsapScheduler.h"
#include "DataManager.h"
#include "FileParser.h"
#include "FileWriter.h"
#include "FsmGenerator.h"

#include <string>

#define USE_ALAP_SCHEDULER
#define DEBUG_GRAPH

#if (defined(_WIN32) || defined(__CYGWIN__))
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

using namespace std;

namespace HighLevelSynthesis
{

class HighLevelSynthesisTool
{
    public:
        HighLevelSynthesisTool();
        int run(string cFile, int latency, string verilogFile); //, string debugDir);
    private:
        DataManager dataManager;
        FileParser fileParser;
        #ifdef USE_ALAP_SCHEDULER
            AlapScheduler scheduler;
        #endif
        #ifndef USE_ALAP_SCHEDULER
            AsapScheduler scheduler;
        #endif
        FsmGenerator fsmGenerator;
        FileWriter fileWriter;
};

} // namespace HighLevelSynthesis

#endif // GUARD_HighLevelSynthesisTool_h