#ifndef GUARD_DataManager_h
#define GUARD_DataManager_h

#include "GraphComponents.h"

#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    class DataManager
    {
        public:
            ~DataManager();
            vector<vertex*> vertices;
            vector<edge*> edges;
    };
}

#endif // GUARD_DataManager_h