#ifndef GUARD_FileWriter_h
#define GUARD_FileWriter_h

#include "DataManager.h"

#include <fstream>

namespace HighLevelSynthesis
{

class FileWriter
{
    public:
        FileWriter(DataManager* dataManager);
        int run(string filePath);
    private:
        DataManager* dataManager;
        ofstream verilogFile;
        int openFile(string filePath);
        void closeFile();
        void declareModule();
        void terminateModule();
        void declareNets();
        void declareStates();
        void declareFsm();
        void declareFsmReset();
        string tab();
        string tab(int numTabs);
        void printStates();
        void printStateTransition(state* currState, vector<bool> condition, int depth);
        state* getNextState(state* currState, vector<bool> condition);
};

} // namespace HighLevelSynthesis
#endif // GUARD_FileWrite_h