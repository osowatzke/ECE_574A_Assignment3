#include "DataManager.h"

#ifndef GraphSimplifier_h
#define GraphSimplifier_h

namespace HighLevelSynthesis
{

class GraphSimplifier
{
    public:
        GraphSimplifier(DataManager* dataManager);
        void run();
    private:
        DataManager* dataManager;
        bool isVertexRequired(vertex* currVertex);
        bool isHierarchyEmpty(hierarchy* currHierarchy);
        bool isConditionalHierarchyEmpty(conditionalHierarchy* condHierarchy);
        void removeHierarchy(hierarchy* currHierarchy);
        void removeConditionalHierarchy(conditionalHierarchy* condHierarchy);
        void removeVertex(vertex* currVertex);
        void removeEdge(edge* currEdge);
        void removeUnusedVertices();
        void removeUnusedConditionalHierarchies();
        void removeUnusedConditionalHierarchies(hierarchy* currHierarchy);
        bool isOutputRequired(edge* output);
        bool removeUnusedOutputs(vertex* currVertex);
        void removeUnusedOutputs();
};

} // namespace HighLevelSynthesis

#endif // #ifndef GraphSimplifier_h