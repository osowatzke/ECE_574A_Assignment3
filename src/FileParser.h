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
        map<string, edge*> updatedEdges;
        list<map<string, edge*>> formerEdges;
        list<vertex*> conditionalStart;
        list<vertex*> conditionalEnd;
        list<bool> conditionalState;
        vertex* lastConditionalStart;
        vertex* lastConditionalEnd;
        int readLines(string filePath);
        void removeComments();
        void readNets();
        void readNetsFromLine(string line);
        void getGraphEdges();
        void getGraphEdgesFromLine(string line);
        void updateActiveEdges(string line);
        void getVertices();
        void getVerticesFromLine(string line);
        vertex* createVertex(VertexType type, string operation, vector<string>inputEdgeNames, vector<string>outputEdgeNames);
        void updateConditionalState(string line);
        edge* getEdge(string edgeName);
        vertex* createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs);
        string strtrim(string str);
        edge* createNewEdge(string edgeName);
        vector<edge*> getImplicitEdges(edge* inputEdge);
        void getInitialGraphEdges();
        void reduceGraph();
        vector<edge*> getUnion(vector<edge*> aEdges, vector<edge*> bEdges);
};

} // namespace HighLevelSynthesis

#endif // GUARD_FileParser_h