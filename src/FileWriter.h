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
        void run(string filePath);
    private:
        DataManager* dataManager;
        ofstream verilogFile;
        int openFile(string filePath);
        void closeFile();
        void declareModule();
        void terminateModule();
        void declareNets();
        int determineNumUniqueStates();
        void declareStates();
};

} // namespace HighLevelSynthesis
#endif // GUARD_FileWrite_h