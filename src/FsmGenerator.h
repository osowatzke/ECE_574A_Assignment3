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
        void createStates();
        void sortStates();
        int getEndTime();
        void getNextStates(state* currState, int time);
        state* createNewState(vector<hierarchy*> hier, int time);
        state* findState(vector<hierarchy*> hier, int time);
        bool upcomingConditionalEnd(hierarchy* hier, int time);
        vector<conditionalHierarchy*> getNewConditionals(state* currState, int time);
};

} // namepsace HighLevelSynthesis

#endif // GUARF_FsmGenerator_h