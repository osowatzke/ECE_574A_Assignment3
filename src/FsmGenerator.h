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
        state* createState(vector<hierarchy*> hier, int time);
        bool upcomingConditionalEnd(hierarchy* hier, int time);
        vector<conditionalHierarchy*> getNewConditionals(state* currState, int time);
        void createStates();
        void sortStates();
        void printStateTransition(state* currState, vector<bool> condition, int depth);
        state* findNextState(state* currState, vector<bool> condition);
        state* findState(vector<hierarchy*> hier, int time);
};

} // namepsace HighLevelSynthesis

#endif // GUARF_FsmGenerator_h