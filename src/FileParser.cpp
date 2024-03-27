#include "FileParser.h"

#include <fstream>
#include <iostream>
#include <regex>
#include <string>

using namespace std;

namespace HighLevelSynthesis
{

FileParser::FileParser(DataManager* dataManager)
    : dataManager(dataManager) {}

int FileParser::run(string filePath)
{
    // Attempt to read lines from input file
    int retVal = readLines(filePath);

    // Return if read fails (i.e. can't open input file)
    if (retVal)
    {
        return retVal;
    }

    // Strip comments from lines
    removeComments();

    // Read nets (inputs, outputs, registers, and wires in output file)
    readNets();

    // Get starting graph edges (file inputs)
    getInitialGraphEdges();

    // Get graph vertices
    getVertices();

    retVal = checkForUndefinedNets();

    return retVal;
}

// Function reads lines from the input file
int FileParser::readLines(string filePath)
{
    // Attempt to open file
    ifstream file (filePath);

    // If file can be openend
    if (file.is_open())
    {
        string line;
        
        // Read lines from input file
        while (getline(file, line))
        {
            lines.push_back(line);
        }
        file.close();
        return 0;
    }

    // Return with error if file cannot be read
    cout << "ERROR: Unable to open file " << filePath << endl;
    return 1;
}

// Function reads lines from input file
void FileParser::removeComments()
{
    // Regular expression to match comments
    regex commentPattern{"^(.*)//(?:.*)$"};
    smatch commentMatch;

    // Go through each line of the input file
    for (string& line : lines)
    {
        // Attempt to match regular expression
        // Will match if there is a comment
        regex_match(line, commentMatch, commentPattern);

        // If there is a match, strip comment from file
        if(!commentMatch.empty())
        {
            line = commentMatch.str(1);
        }
    }
}

// Function reads nets from input file
void FileParser::readNets()
{
    for (string& line : lines)
    {
        readNetsFromLine(line);
    }
}

// Function reads nets from line in input file
void FileParser::readNetsFromLine(string line)
{
    // Regular expression to match net
    // Nets are the union of all inputs, outputs, and variables
    smatch netsLineMatch;
    const regex netsLinePattern{"^\\s*(input|output|variable)\\s*(U?)Int(\\d+)\\s+(.*)$"};
    regex_match(line, netsLineMatch, netsLinePattern);

    // If line matches regular expression  
    if (!netsLineMatch.empty())
    {
        // Extract sign and width of nets
        NetType type = stringToNetType(netsLineMatch.str(1));
        bool isSigned = (netsLineMatch.str(2) != "U");
        int width = stoi(netsLineMatch.str(3));

        // Regular expression to match individual net names
        string netsSubstring = netsLineMatch.str(4);
        regex netsPattern{"(\\w+)\\s*(?:,|$)"};

        // Create a net structure for each name that is found
        sregex_iterator start = sregex_iterator(netsSubstring.begin(), netsSubstring.end(), netsPattern);
        sregex_iterator end = sregex_iterator();
        for (sregex_iterator i = start; i != end; ++i)
        {
            smatch netsMatch = *i;
            string netName = netsMatch.str(1);
            createNewNet(netName, type, width, isSigned);
        }
    }
}

// Function gets the initial graph edges (input nets)
void FileParser::getInitialGraphEdges()
{
    currHierarchy = dataManager->graphHierarchy;
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto it = netStart; it != netEnd; ++it)
    {
        string netName = it->first;
        net* currNet = it->second;
        if (currNet->type == NetType::INPUT)
        {
            createNewEdge(netName);
        }
    }
}

// Function gets graph vertices
void FileParser::getVertices()
{
    hierarchyUpdatePending = false;
    for (string& line: lines)
    {
        parseConditionalStatements(line);
        getVerticesFromLine(line);
    }
}

void FileParser::parseConditionalStatements(string line)
{
    smatch ifMatch;
    const regex ifPattern{"^\\s*if\\s*\\(?\\s*(\\w+)\\s*\\)?\\s*\\{\\s*$"};
    regex_match(line, ifMatch, ifPattern);
    if (!ifMatch.empty())
    {
        /*string inputName = ifMatch.str(0);
        string operation = "if (" + inputName + " == 1)";
        edge* input = getEdge(ifMatch.str(1));
        edge* output = new edge;
        dataManager->edges.push_back(output);
        vertex* newVertex = createVertex(VertexType::FORK, operation, {input}, {output});*/
        conditionalHierarchy* condHierarchy = new conditionalHierarchy;
        condHierarchy->parent = currHierarchy;
        condHierarchy->trueHiearchy = new hierarchy;
        condHierarchy->falseHiearchy = NULL;
        currHierarchy->conditional.push_back(condHierarchy);
        currHierarchy = condHierarchy->trueHiearchy;
        currHierarchy->parent = condHierarchy;
    }
    smatch elseMatch;
    const regex elsePattern{"^\\s*else\\s*\\{\\s*$"};
    regex_match(line, elseMatch, elsePattern);
    if (!elseMatch.empty())
    {
        hierarchyUpdatePending = false;
        conditionalHierarchy* condHiearchy = currHierarchy->parent;
        condHiearchy->falseHiearchy = new hierarchy;
        currHierarchy = condHiearchy->falseHiearchy;
        currHierarchy->parent = condHiearchy;
    }
    smatch braceMatch;
    const regex bracePattern{"^\\s*\\}\\s*$"};
    regex_match(line, braceMatch, bracePattern);
    if (!braceMatch.empty())
    {
        if (hierarchyUpdatePending)
        {
            currHierarchy = currHierarchy->parent->parent;
        }
        hierarchyUpdatePending = true;
    }
}
// Function gets graph vertices from a line
void FileParser::getVerticesFromLine(string line)
{
    smatch vertexMatch;
    const regex vertexPattern{"^\\s*(\\w+)\\s*=\\s*(\\w+)\\s*(\\S+)?\\s*(\\w+)?\\s*(\\S+)?\\s*(\\w+)?\\s*$"};
    regex_match(line, vertexMatch, vertexPattern);
    if (!vertexMatch.empty())
    {
        if (hierarchyUpdatePending)
        {
            currHierarchy = currHierarchy->parent->parent;
            hierarchyUpdatePending = false;
        }
        vector <string> edgeNames;
        vector <string> operators;
        edgeNames.push_back(vertexMatch.str(1));
        for (size_t i = 2; i < vertexMatch.size(); ++i)
        {
            if (vertexMatch.str(i) != "")
            {
                if ((i % 2) == 0)
                {
                    edgeNames.push_back(vertexMatch.str(i));
                }
                else
                {
                    operators.push_back(vertexMatch.str(i));
                }
            }
        }
        if (operators.size() == 1)
        {
            string operation = edgeNames[0] + " <= " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2];
            VertexType type;
            if ((operators[0] == "+") ||  (operators[0] == "-"))
            {
                type = VertexType::ADD;
            }
            else if (operators[0] == "*")
            {
                type = VertexType::MUL;
            }
            else if ((operators[0] == ">") || (operators[0] == "<") || (operators[0] == "==") || (operators[0] == ">>") || (operators[0] == "<<"))
            {
                type = VertexType::LOGIC;
            }
            else if ((operators[0] == "/") || (operators[0] == "%"))
            {
                type = VertexType::DIV;
            }
            createVertex(type, operation, {edgeNames[1], edgeNames[2]}, {edgeNames[0]});
        }
        else if (operators.size() == 2)
        {
            string operation = edgeNames[0] + " <= " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2] + " " + operators[1] + " " + edgeNames[3];
            createVertex(VertexType::LOGIC, operation, {edgeNames[1], edgeNames[2], edgeNames[3]}, {edgeNames[0]});
        }
    }
}

// Function gets a pointer to an edge and creates one if a pointer does not exist
edge* FileParser::getEdge(string edgeName)
{
    hierarchy* searchHierarchy = currHierarchy;
    edge* edgeFound = NULL;
    while (searchHierarchy != NULL)
    {
        if (searchHierarchy->edges.find(edgeName) != searchHierarchy->edges.end())
        {
            edgeFound = searchHierarchy->edges[edgeName];
            break;
        }
        if (searchHierarchy->parent == NULL)
        {
            searchHierarchy = NULL;
        }
        else
        {
            searchHierarchy = searchHierarchy->parent->parent;
        }
    }
    if (edgeFound == NULL)
    {
        edgeFound = createNewEdge(edgeName);
        if (dataManager->nets.find(edgeName) == dataManager->nets.end())
        {
             missingEdges[edgeName] = edgeFound;
        }
    }
    return edgeFound;
}

// Function creates a vertex from names of inputs/outputs
vertex* FileParser::createVertex(VertexType type, string operation, vector<string> inputNames, vector<string> outputNames)
{
    vector<edge*> inputs;
    for (string& inputName : inputNames)
    {
        inputs.push_back(getEdge(inputName));
    }
    vector<edge*> outputs;
    for (string& outputName : outputNames)
    {
        edge* output = getEdge(outputName);
        if (output->src != NULL)
        {
            output = createNewEdge(outputName);
        }
        outputs.push_back(output);
    }
    return createVertex(type, operation, inputs, outputs);
}

// Function creates vertex from pointers to input and output edges
vertex* FileParser::createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs)
{
    vertex* newVertex = new vertex;
    newVertex->time = -1;
    newVertex->type = type;
    newVertex->operation = operation;
    for (edge*& input : inputs)
    {
        newVertex->inputs.push_back(input);
        input->dest.push_back(newVertex);        
    }
    for (edge*& output : outputs)
    {
        newVertex->outputs.push_back(output);
        output->src = newVertex;        
    }
    currHierarchy->vertices.push_back(newVertex);
    dataManager->vertices.push_back(newVertex);
    return newVertex;
}

// Function creates a new edge
edge* FileParser::createNewEdge(string edgeName)
{
    edge* newEdge = new edge;
    newEdge->src = NULL;
    dataManager->edges.push_back(newEdge);
    currHierarchy->edges[edgeName] = newEdge;
    return newEdge;
}

// Function creates a new net
net* FileParser::createNewNet(string netName, NetType type, int width, bool isSigned)
{
    net* newNet = new net;
    newNet->type = type;
    newNet->width = width;

    // Force single bit wires to be unsigned
    if (newNet->width > 1)
    {
        newNet->isSigned = isSigned;
    }
    else
    {
        newNet->isSigned = false;
    }

    // Update nets in data manager
    dataManager->nets[netName] = newNet;
}

int FileParser::checkForUndefinedNets()
{
    if (missingEdges.empty())
    {
        return 0;
    }
    auto it = missingEdges.begin();
    string edgeName = it->first;
    edge* missingEdge = it->second;
    if (missingEdge->src == NULL)
    {
        cout << "Undefined input \"" << edgeName << "\"" << endl;
    }
    else if (missingEdge->dest.size() == 0)
    {
        cout << "Undefined output \"" << edgeName << "\"" << endl;
    }
    else
    {
        cout << "Undefined variable \"" << edgeName << "\"" << endl;
    }
    return 1;
}

} // namespace HighLevelSynthesis