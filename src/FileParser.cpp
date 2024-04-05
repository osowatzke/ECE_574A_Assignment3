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

    // Get graph vertices
    getVertices();

    // Determine if any edges were not defined by a net
    retVal = checkForUndefinedEdges();

    dataManager->visualizeGraph();
    
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
    return newNet;
}

// Function gets graph vertices
void FileParser::getVertices()
{
    // Set initial hierarchy
    currHierarchy = dataManager->graphHierarchy;

    // No pending hierarchy updates (end of if statements)
    hierarchyUpdatePending = false;

    // Parse conditional logic and vertices from each line
    for (string& line: lines)
    {
        parseConditionalStatements(line);
        getVerticesFromLine(line);
    }
}

// Function parses conditional statements
void FileParser::parseConditionalStatements(string line)
{
    // Use regular expression to match start of if statement
    smatch ifMatch;
    const regex ifPattern{"^\\s*if\\s*\\(?\\s*(\\w+)\\s*\\)?\\s*\\{\\s*$"};
    regex_match(line, ifMatch, ifPattern);

    // If line contains start of if statement
    if (!ifMatch.empty())
    {
        // Return from previous conditional hierarchy if applicable
        returnFromHierarchy();

        // Create a fork vertex
        string inputName = ifMatch.str(1);
        string operation = "if (" + inputName + " == 1)";
        edge* input = getEdge(inputName);
        edge* output = createNewEdge();
        createVertex(VertexType::FORK, operation, {input}, {output});

        // Create a new conditional hierarchy and select the true branch of the hierarchy
        conditionalHierarchy* condHierarchy = createNewConditionalHierarchy(output);
        currHierarchy = condHierarchy->trueHiearchy;
    }

   // Use regular expression to match else statement
    smatch elseMatch;
    const regex elsePattern{"^\\s*else\\s*\\{\\s*$"};
    regex_match(line, elseMatch, elsePattern);

    // If line contains else statement
    if (!elseMatch.empty())
    {
        // No longer leaving conditional hieararchy
        hierarchyUpdatePending = false;

        // Select the false branch of the conditional hierarchy
        currHierarchy = currHierarchy->parent->falseHiearchy;
    }

    // Use regular expression to match braces at end of if/else statement
    smatch braceMatch;
    const regex bracePattern{"^\\s*\\}\\s*$"};
    regex_match(line, braceMatch, bracePattern);

    // If line is end of if or else statement
    if (!braceMatch.empty())
    {
        // Return from previous conditional hierarchy if applicable
        returnFromHierarchy();

        // Set flag to leave current conditional hiearchy
        // Must hit another conditional statement or a statement outside
        // of the if-else statement to leave conditional hieararchy
        hierarchyUpdatePending = true;
    }
}

// Function creates a new conditional hierarchy
conditionalHierarchy* FileParser::createNewConditionalHierarchy(edge* condition)
{
    conditionalHierarchy* condHierarchy = new conditionalHierarchy;
    condHierarchy->parent = currHierarchy;
    condHierarchy->condition = condition;
    condHierarchy->trueHiearchy = new hierarchy;
    condHierarchy->trueHiearchy->parent = condHierarchy;
    condHierarchy->falseHiearchy = new hierarchy;
    condHierarchy->falseHiearchy->parent = condHierarchy;
    currHierarchy->conditional.push_back(condHierarchy);
    return condHierarchy;
}

// Function creates a join vertex
vertex* FileParser::createJoinVertex()
{
    conditionalHierarchy* condHierarchy = currHierarchy->parent;
    auto start = condHierarchy->trueHiearchy->edges.begin();
    auto end = condHierarchy->trueHiearchy->edges.end();
    vector<string> edgeNames;
    for (auto it = start; it != end; ++it)
    {
        string edgeName = it->first;
        edgeNames.push_back(edgeName);
    }
    start = condHierarchy->falseHiearchy->edges.begin();
    end = condHierarchy->falseHiearchy->edges.end();
    vector<string> initialEdgeNames = edgeNames;
    for (auto it = start; it != end; ++it)
    {
        string edgeName = it->first;
        bool nameMatch = false;
        for (string& initialEdgeName : initialEdgeNames)
        {
            if (edgeName == initialEdgeName)
            {
                nameMatch = true;
            }
        }
        if (!nameMatch)
        {
            edgeNames.push_back(edgeName);
        }
    }
    hierarchy* trueHierarchy = condHierarchy->trueHiearchy;
    hierarchy* falseHierarchy = condHierarchy->falseHiearchy;
    vector<edge*> inputs;
    cout << "Inputs: " << endl;
    for (string edgeName : edgeNames)
    {
        currHierarchy = trueHierarchy;
        edge* inputEdge = getEdge(edgeName);
        inputs.push_back(inputEdge);        
        cout << edgeName << " = " << inputEdge << endl;
        // cout << inputs.size() << endl;
        currHierarchy = falseHierarchy;
        inputEdge = getEdge(edgeName);
        inputs.push_back(inputEdge);        
        cout << edgeName << " = " << inputEdge << endl;
        // cout << inputs.size() << endl;
    }
    currHierarchy = trueHierarchy->parent->parent;
    vector<edge*> outputs;
    cout << "Outputs: " << endl;
    for (string edgeName : edgeNames)
    {
        // cout << "Size Pre: " << currHierarchy->edges.size() << endl;
        edge* outputEdge = getEdge(edgeName);
        if (outputEdge->src != NULL)
        {
            inputs.push_back(outputEdge);
            outputEdge = createNewEdge(edgeName);
        }
        // cout << "Size Post: " << currHierarchy->edges.size() << endl;
        cout << edgeName << " = " << outputEdge << endl;
        outputs.push_back(outputEdge);
    }
    return createVertex(VertexType::JOIN, "", inputs, outputs);
}

void FileParser::returnFromHierarchy()
{
    if (hierarchyUpdatePending)
    {
        vertex* joinVertex = createJoinVertex();
        cout << "JOIN = " << joinVertex << endl;
        // cout << "JOIN : IN=" << joinVertex->inputs.size() << " : OUT="  << joinVertex->outputs.size() << endl;
    } 
    hierarchyUpdatePending = false;
}

// Function gets graph vertices from a line
void FileParser::getVerticesFromLine(string line)
{
    smatch vertexMatch;
    const regex vertexPattern{"^\\s*(\\w+)\\s*=\\s*(\\w+)\\s*(\\S+)?\\s*(\\w+)?\\s*(\\S+)?\\s*(\\w+)?\\s*$"};
    regex_match(line, vertexMatch, vertexPattern);
    if (!vertexMatch.empty())
    {
        returnFromHierarchy();
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
        edge* inputEdge = getEdge(inputName);
        inputs.push_back(inputEdge);
    }
    vector<edge*> outputs;
    for (string& outputName : outputNames)
    {
        edge* output = getEdge(outputName);
        if (output->src != NULL)
        {
            inputs.push_back(output);
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
    if (currHierarchy->parent != NULL)
    {
        newVertex->inputs.push_back(currHierarchy->parent->condition);
        currHierarchy->parent->condition->dest.push_back(newVertex);
    }
    for (edge*& output : outputs)
    {
        newVertex->outputs.push_back(output);
        output->src = newVertex;        
    }
    newVertex->parent = currHierarchy;
    currHierarchy->vertices.push_back(newVertex);
    dataManager->vertices.push_back(newVertex);
    return newVertex;
}

edge* FileParser::createNewEdge()
{
    edge* newEdge = new edge;
    newEdge->src = NULL;
    dataManager->edges.push_back(newEdge);
    return newEdge;
}

// Function creates a new edge
edge* FileParser::createNewEdge(string edgeName)
{
    edge* newEdge = createNewEdge();
    currHierarchy->edges[edgeName] = newEdge;
    return newEdge;
}

int FileParser::checkForUndefinedEdges()
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