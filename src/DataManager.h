#ifndef GUARD_DataManager_h
#define GUARD_DataManager_h

#include "CircuitComponents.h"
#include "GraphComponents.h"

#include <map>
#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    class DataManager
    {
        public:
            DataManager();
            ~DataManager();
            map<string, net*> nets;
            vector<vertex*> vertices;
            vector<edge*> edges;
            vector<state*> states;
            hierarchy* graphHierarchy;
            void printGraph();
        private:
            void deleteHierarchy(hierarchy* graphHierarchy);
            void printVertices();
            void printEdges();
    };
} // namespace HighLevelSynthesis

#endif // GUARD_DataManager_h