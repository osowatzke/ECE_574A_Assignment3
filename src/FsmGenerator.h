#ifndef GUARD_FsmGenerator_h
#define GUARD_FsmGenerator_h

#include "DataManager.h"
#include "GraphComponents.h"

#include <map>

namespace HighLevelSynthesis
{

class FsmGenerator
{
    public:
        FsmGenerator(DataManager* dataManager);
        ~FsmGenerator();
        void run();
    private:
        DataManager* dataManager;
        vector<vector<hierarchy*>> hierarchyMap;
        vector<state*> states;
        int getConditionalStartTime(conditionalHierarchy* condHier);
        int getConditionalEndTime(conditionalHierarchy* condHier);
        int getEndTime();
        void initializeHierarchyMap();
        void getHierarchyMap();
        void getHierarchyMap(hierarchy* hier);
        bool isParentHierarchy(hierarchy* currHier, hierarchy* compHier);
        void createStatesAtTime(int time);
        void createAllStates();
        void printStates();
        void addVerticesRecursive(state* currState, hierarchy* hier, int time);
        void getNextStates(state* currState, int time);
        state* getState(vector<hierarchy*> hier, int time);
        bool upcomingConditionalEnd(hierarchy* hier, int time);
        vector<conditionalHierarchy*> getNewConditionals(state* currState, int time);
        void createStates();
        void sortStates();
};

} // namepsace HighLevelSynthesis

#endif // GUARF_FsmGenerator_h