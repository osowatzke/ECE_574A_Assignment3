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
        map<string, edge*> undefinedEdges;
        hierarchy* currHierarchy;
        bool hierarchyUpdatePending;
        int readLines(string filePath);
        void removeComments();
        void readNets();
        void readNetsFromLine(string line);
        net* createNewNet(string netName, NetType type, int width, bool isSigned);
        void createInitialEdges();
        void getVertices();
        void parseConditionalStatements(string line);
        edge* getEdge(string edgeName);
        edge* createNewEdge();
        edge* createNewEdge(string edgeName);
        vertex* createVertex(VertexType type, string operation, vector<string>inputEdgeNames, vector<string>outputEdgeNames);
        vertex* createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs);
        conditionalHierarchy* createNewConditionalHierarchy(edge* condition);
        void returnFromHierarchy();
        vertex* createJoinVertex();
        void getVerticesFromLine(string line);
        int checkForUndefinedEdges();
        int validateGraph();
};

} // namespace HighLevelSynthesis

#endif // GUARD_FileParser_h