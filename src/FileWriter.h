#ifndef GUARD_FileWriter_h
#define GUARD_FileWriter_h

#include "DataManager.h"

#include <fstream>

namespace HighLevelSynthesis
{

struct State
{
    vector<vertex*> vertices;
};

class FileWriter
{
    public:
        FileWriter(DataManager* dataManager);
        void run(string filePath);
    private:
        DataManager* dataManager;
        ofstream verilogFile;
        vector<vector<State>> states; 
        int openFile(string filePath);
        void closeFile();
        void declareModule();
        void terminateModule();
        void declareNets();
        int determineNumUniqueStates();
        void declareStates();
        void declareFsm();
        void declareFsmReset();
        void declareFsmStates();
        string tab();
        string tab(int numTabs);
        void addVerticesToStates();
};

} // namespace HighLevelSynthesis
#endif // GUARD_FileWrite_h