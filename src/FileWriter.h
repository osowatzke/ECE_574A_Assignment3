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
        ~FileWriter();
        void run(string filePath);
    private:
        DataManager* dataManager;
        ofstream verilogFile;
        vector<int> numStatesPerTimestep;
        vector<vector<hierarchy*>> hierarchyMapping;
        vector<vector<state*>> states;
        int openFile(string filePath);
        void closeFile();
        void declareModule();
        void terminateModule();
        void declareNets();
        int getNumTimesteps();
        void getNumStatesPerTimestep();
        void getNumStatesPerTimestep(hierarchy* hier);
        void declareStates();
        void declareFsm();
        void declareFsmReset();
        void declareFsmStates();
        string tab();
        string tab(int numTabs);
        void addVerticesToStates();
        void getStates();
        void updateStates(hierarchy* hier, int time);
        int getConditionalEndTime(conditionalHierarchy* condHier);
        int getConditionalStartTime(conditionalHierarchy* condHier);
        void determineHierarchyMapping();
        void determineHierarchyMapping(hierarchy* hierarchy);
        bool isParentHierarchy(hierarchy* currHierarchy, hierarchy* compHiearchy);
        void createStates();
        void printStates();
        void printStateTransition(state* currState, vector<bool> condition, int depth);
        state* getNextState(state* currState, vector<bool> condition);
};

} // namespace HighLevelSynthesis
#endif // GUARD_FileWrite_h