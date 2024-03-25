#ifndef GUARD_DataManager_h
#define GUARD_DataManager_h

#include "CircuitComponents.h"
#include "GraphComponents.h"

#include <vector>

using namespace std;

namespace HighLevelSynthesis
{
    class DataManager
    {
        public:
            ~DataManager();
            // void VisualizeGraph();
            // vertex* inode;
            // vertex* onode;
            map<string, net*> nets;
            vector<vertex*> vertices;
            vector<edge*> edges;
            void printGraph();
        private:
            void printVertices();
            void printEdges();
    };
}

#endif // GUARD_DataManager_h