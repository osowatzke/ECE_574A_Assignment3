#ifndef GUARD_FileParser_h
#define GUARD_FileParser_h

#include "DataManager.h"

#include <map>
#include <vector>
#include <queue>
#include <list>

using namespace std;

namespace HighLevelSynthesis
{

class FileParser
{
    public:
        FileParser(DataManager* dataManager);
        int run(string filePath);
    private:
        DataManager* dataManager;
        vector<string> lines;
        map<string, edge*> activeEdges;
        map<string, edge*> missingEdges;
        int readLines(string filePath);
        void removeComments();
        void readNets();
        void readNetsFromLine(string line);
        void getInitialGraphEdges();
        edge* getEdge(string edgeName);
        edge* createNewEdge(string edgeName);
        void getVertices();
        void getVerticesFromLine(string line);
        vertex* createVertex(VertexType type, string operation, vector<string>inputEdgeNames, vector<string>outputEdgeNames);
        vertex* createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs);
        int checkForUndefinedNets();
};

} // namespace HighLevelSynthesis

#endif // GUARD_FileParser_h