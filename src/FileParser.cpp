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
        currHierarchy = condHierarchy->trueHierarchy;
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
        currHierarchy = currHierarchy->parent->falseHierarchy;
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

        // Set flag to leave current conditional hierarchy
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

    // Create true and false hierarchies for either branch of the conditional hierarchy
    condHierarchy->trueHierarchy = new hierarchy;
    condHierarchy->trueHierarchy->parent = condHierarchy;
    condHierarchy->falseHierarchy = new hierarchy;
    condHierarchy->falseHierarchy->parent = condHierarchy;

    // Append conditional hierarchy to array of conditional hierarchies
    currHierarchy->conditional.push_back(condHierarchy);
    return condHierarchy;
}

// Function creates a join vertex
vertex* FileParser::createJoinVertex()
{
    // Function should be called from either true or false hierarchy. Corresponding
    // conditional hierarchy should be the parent of the current hierarchy
    conditionalHierarchy* condHierarchy = currHierarchy->parent;

    // Get the edge names initialized in the true hierarchy
    auto start = condHierarchy->trueHierarchy->edges.begin();
    auto end = condHierarchy->trueHierarchy->edges.end();
    vector<string> edgeNames;
    for (auto it = start; it != end; ++it)
    {
        string edgeName = it->first;
        edgeNames.push_back(edgeName);
    }

    // Get additional edge names initialized in the false hierarchy
    start = condHierarchy->falseHierarchy->edges.begin();
    end = condHierarchy->falseHierarchy->edges.end();
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

    // Get pointers to true and false branches of the conditional hierarchy
    hierarchy* trueHierarchy = condHierarchy->trueHierarchy;
    hierarchy* falseHierarchy = condHierarchy->falseHierarchy;

    // Create array of inputs for the conditional hierarchy
    vector<edge*> inputs;
    for (string edgeName : edgeNames)
    {
        // Get edges from the true hierarchy
        currHierarchy = trueHierarchy;
        edge* inputEdge = getEdge(edgeName);
        inputs.push_back(inputEdge);

        // Get edges from the false hierarchy
        currHierarchy = falseHierarchy;
        inputEdge = getEdge(edgeName);
        inputs.push_back(inputEdge);
    }

    // Set the current hierarchy to the parent hierarchy
    currHierarchy = trueHierarchy->parent->parent;
    vector<edge*> outputs;
    for (string edgeName : edgeNames)
    {
        // Create edges for outputs
        edge* outputEdge = getEdge(edgeName);
        if (outputEdge->src != NULL)
        {
            inputs.push_back(outputEdge);
            outputEdge = createNewEdge(edgeName);
        }
        outputs.push_back(outputEdge);
    }

    // Create join vertex
    return createVertex(VertexType::JOIN, "", inputs, outputs);
}

// Function returns from a conditional hierarchy
void FileParser::returnFromHierarchy()
{
    // If there is a pending hierarchy update, create a join vertex
    if (hierarchyUpdatePending)
    {
        createJoinVertex();
    } 

    // Set the hierarchy update pending flag to false.
    hierarchyUpdatePending = false;
}

// Function gets graph vertices from a line
void FileParser::getVerticesFromLine(string line)
{
    // Use regular expressions to match vertices
    smatch vertexMatch;
    const regex vertexPattern{"^\\s*(\\w+)\\s*=\\s*(\\w+)\\s*(\\S+)?\\s*(\\w+)?\\s*(\\S+)?\\s*(\\w+)?\\s*$"};
    regex_match(line, vertexMatch, vertexPattern);

    // If the line matches the regular expression
    if (!vertexMatch.empty())
    {
        // Return from conditional hierarchy
        returnFromHierarchy();

        // Get a vector of string for edge names and operators
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

        // If there is only one operator 
        if (operators.size() == 1)
        {
            // Create string operation that needs to be written to the verilog file
            string operation = edgeNames[0] + " <= " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2];

            // Determine the operation type
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

            // Create vertex
            createVertex(type, operation, {edgeNames[1], edgeNames[2]}, {edgeNames[0]});
        }

        // If there are two operators (ternary operator)
        else if (operators.size() == 2)
        {
            // Create string operation that needs to be written to the verilog file
            string operation = edgeNames[0] + " <= " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2] + " " + operators[1] + " " + edgeNames[3];
            
            // Create vertex
            createVertex(VertexType::LOGIC, operation, {edgeNames[1], edgeNames[2], edgeNames[3]}, {edgeNames[0]});
        }
    }
}

// Function gets a pointer to an edge and creates one if the edge does not exist
edge* FileParser::getEdge(string edgeName)
{
    // Get a pointer to the hierarchy that needs to be searched
    hierarchy* searchHierarchy = currHierarchy;

    // Initialize the found edge to a NULL pointer
    edge* edgeFound = NULL;

    // Continue searching until there are no more parent hierarchies to search
    while (searchHierarchy != NULL)
    {
        // If edge is in current hierarchy
        if (searchHierarchy->edges.find(edgeName) != searchHierarchy->edges.end())
        {
            // Return edge
            edgeFound = searchHierarchy->edges[edgeName];
            break;
        }
        
        // If current hierarchy has no parent, set search hierarchy to NULL
        if (searchHierarchy->parent == NULL)
        {
            searchHierarchy = NULL;
        }
        // Search through parent hierarchy
        else
        {
            searchHierarchy = searchHierarchy->parent->parent;
        }
    }

    // If no edge is found
    if (edgeFound == NULL)
    {

        // Create new edge
        edgeFound = createNewEdge(edgeName);

        // If there is no net to describe an edge
        // Append edge to array of missing edges
        if (dataManager->nets.find(edgeName) == dataManager->nets.end())
        {
             undefinedEdges[edgeName] = edgeFound;
        }
    }

    // Return edge that was found or created
    return edgeFound;
}

// Function creates a vertex from names of inputs/outputs
vertex* FileParser::createVertex(VertexType type, string operation, vector<string> inputNames, vector<string> outputNames)
{
    // Create an array of input edges
    vector<edge*> inputs;
    for (string& inputName : inputNames)
    {
        edge* inputEdge = getEdge(inputName);
        inputs.push_back(inputEdge);
    }

    // Create an array of output edges
    vector<edge*> outputs;
    for (string& outputName : outputNames)
    {
        edge* output = getEdge(outputName);
        
        // If output is already being driven by a source
        if (output->src != NULL)
        {
            // Make pre-existing output an input. New edge must overwrite old edge
            inputs.push_back(output);

            // Create new output edge
            output = createNewEdge(outputName);
        }

        // Append output edge to array of edges
        outputs.push_back(output);
    }

    // Create new vertex
    return createVertex(type, operation, inputs, outputs);
}

// Function creates vertex from pointers to input and output edges
vertex* FileParser::createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs)
{
    // Create new vertex
    vertex* newVertex = new vertex;
    newVertex->time = -1;
    newVertex->type = type;
    newVertex->operation = operation;

    // Connect vertex inputs
    for (edge*& input : inputs)
    {
        newVertex->inputs.push_back(input);
        input->dest.push_back(newVertex);        
    }

    // Add vertex conditional input when inside a conditional hierarchy
    if (currHierarchy->parent != NULL)
    {
        newVertex->inputs.push_back(currHierarchy->parent->condition);
        currHierarchy->parent->condition->dest.push_back(newVertex);
    }

    // Connect vertex outputs
    for (edge*& output : outputs)
    {
        newVertex->outputs.push_back(output);
        output->src = newVertex;        
    }

    // Set parent hierarchy of current vertex
    newVertex->parent = currHierarchy;

    // Append vertex to hierarchy and shared array of vertices
    currHierarchy->vertices.push_back(newVertex);
    dataManager->vertices.push_back(newVertex);

    // Return new vertex
    return newVertex;
}

// Function creates a new edge in the data manager
edge* FileParser::createNewEdge()
{
    edge* newEdge = new edge;
    newEdge->src = NULL;
    dataManager->edges.push_back(newEdge);
    return newEdge;
}

// Function creates a new edge in the data manager and
// adds a key for the edge in the current hierarchy
edge* FileParser::createNewEdge(string edgeName)
{
    edge* newEdge = createNewEdge();
    currHierarchy->edges[edgeName] = newEdge;
    return newEdge;
}

// Function returns a nonzero values if any of the edges were undefined
// (have no corresponding net)
int FileParser::checkForUndefinedEdges()
{
    // If there are undefined edges return 0
    if (undefinedEdges.empty())
    {
        return 0;
    }

    // Get the first undefined edge
    auto it = undefinedEdges.begin();
    string edgeName = it->first;
    edge* missingEdge = it->second;

    // If undefined edge is an input
    if (missingEdge->src == NULL)
    {
        cout << "Undefined input \"" << edgeName << "\"" << endl;
    }
    // If undefined edge is an output
    else if (missingEdge->dest.size() == 0)
    {
        cout << "Undefined output \"" << edgeName << "\"" << endl;
    }
    // If undefined edge is a variable
    else
    {
        cout << "Undefined variable \"" << edgeName << "\"" << endl;
    }

    // Return 1 for undefined edge
    return 1;
}

} // namespace HighLevelSynthesis