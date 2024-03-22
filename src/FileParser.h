#ifndef GUARD_FileParser_h
#define GUARD_FileParser_h

#include "DataManager.h"

#include <map>
#include <vector>

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
        int readLines(string filePath);
        void removeComments();
        void getGraphEdges();
        void getGraphEdgesFromLine(string line);
};

} // namespace HighLevelSynthesis

#endif // GUARD_FileParser_h