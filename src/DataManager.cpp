#include "DataManager.h"

namespace HighLevelSynthesis
{
    DataManager::~DataManager()
    {
        // Delete Vertices
        for (vertex*& currVertex : vertices)
        {
            delete(currVertex);
        }

        // Delete Edges
        for (edge*& currEdge : edges)
        {
            delete(currEdge);
        }
    }
}