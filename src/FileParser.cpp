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

    readNets();

    getInitialGraphEdges();

    getVertices();

    // getGraphEdges();

    // getVertices();

    /*auto begin = activeEdges.begin();
    auto end = activeEdges.end();

    for (auto& it = begin; it != end; ++it)
    {
        cout << it->first << endl;
    }*/

    return 0;
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

void FileParser::readNets()
{
    for (string& line : lines)
    {
        readNetsFromLine(line);
    }
}

void FileParser::readNetsFromLine(string line)
{
    // Regular expression to match wire
    // Wires are the union of all inputs, outputs, wires, and registers
    smatch netsLineMatch;
    const regex netsLinePattern{"^\\s*(input|output|variable)\\s*(U?)Int(\\d+)\\s+(.*)$"};
    regex_match(line, netsLineMatch, netsLinePattern);

    // If line matches regular expression  
    if (!netsLineMatch.empty())
    {
        // Extract sign and width of wires
        NetType type = stringToNetType(netsLineMatch.str(1));
        bool isSigned = (netsLineMatch.str(2) != "U");
        int width = stoi(netsLineMatch.str(3));

        // Regular expression to match individual wire names
        string netsSubstring = netsLineMatch.str(4);
        regex netsPattern{"(\\w+)\\s*(?:,|$)"};

        // Create a wire structure for each name that is found
        sregex_iterator start = sregex_iterator(netsSubstring.begin(), netsSubstring.end(), netsPattern);
        sregex_iterator end = sregex_iterator();
        for (sregex_iterator i = start; i != end; ++i)
        {
            smatch netsMatch = *i;
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

            // Update wires in data manager
            string netName = netsMatch.str(1);
            dataManager->nets[netName] = newNet; //.push_back(newNet);

            // Add wire to wire table
            // string edgeName = edgeMatch.str(1);
            // activeEdges[edgeName] = newEdge;
            // activeEdges.insert({edgeName, newEdge});
        }
    }
}

void FileParser::getInitialGraphEdges()
{
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

/*void FileParser::updateConditionalState(string line)
{
    smatch ifMatch;
    const regex ifPattern{"^\\s*if\\s*\\(?\\s*(\\w+)\\s*\\)?\\s*{\\s*$"};
    regex_match(line, ifMatch, ifPattern);
    if (!ifMatch.empty())
    {
        string inputName = ifMatch.str(0);
        string operation = "if ( " + inputName + " )";
        edge* input = getEdge(ifMatch.str(1));
        edge* output = new edge;
        dataManager->edges.push_back(output);
        vertex* newVertex = createVertex(VertexType::FORK, operation, {input}, {output});
        conditionalStart.push_back(newVertex);
        conditionalState.push_back(true);
        formerEdges.push_back(activeEdges);
        newVertex = new vertex;
        newVertex->type = VertexType::JOIN;
        newVertex->operation = "";
        dataManager->vertices.push_back(newVertex);
        conditionalEnd.push_back(newVertex);
    }
    smatch elseMatch;
    const regex elsePattern{"^\\s*else\\s*{\\s*$"};
    regex_match(line, elseMatch, elsePattern);
    if (!elseMatch.empty())
    {
        conditionalStart.push_back(lastConditionalStart);
        conditionalEnd.push_back(lastConditionalEnd);
        conditionalState.push_back(false);
        formerEdges.push_back(activeEdges);
    }
    smatch braceMatch;
    const regex bracePattern{"^\\s*}\\s*$"};
    regex_match(line, braceMatch, bracePattern);
    if (!braceMatch.empty())
    {
        lastConditionalStart = conditionalStart.back();
        lastConditionalEnd = conditionalEnd.back();
        conditionalStart.pop_back();
        conditionalEnd.pop_back();
        conditionalState.pop_back();
        map<string, edge*> oldEdges = formerEdges.back();
        formerEdges.pop_back();
        auto start = oldEdges.begin();
        auto end = oldEdges.end();
        vector<bool> validInput(lastConditionalEnd->inputs.size(), true);
        for (size_t i = 0; i < lastConditionalEnd->inputs.size(); ++i)
        {
            edge* input = lastConditionalEnd->inputs[i];
            for (auto it = start; it != end; ++it)
            {
                if (it->second == input)
                {
                    validInput[i] = false;
                }
            }
        }
        vector<edge*> inputsCopy = lastConditionalEnd->inputs;
        lastConditionalEnd->inputs = {};
        for (size_t i = 0; i < inputsCopy.size(); ++ i)
        {

        }
        for (size_t i = 0; i < lastConditionalEnd->)
        for (size_t i = 0; i < lastConditionalEnd->inputs.size(); ++i)
        {
            edge* input = lastConditionalEnd->inputs[i];
            for (auto it = start; it != end; ++it)
            {
                if (it->second == input)
                {
                    validInput[i] = false;
                }
            }
        }
    } 
}*/
// Function parses line for graph edges
/*void parseLineForGraphEdges(line)
{
    // Regular expression to match wire
    // Wires are the union of all inputs, outputs, wires, and registers
    smatch multiEdgeMatch;
    const regex multiEdgePattern{"^\\s*(input|output|variable)\\s*(U?)Int(\\d+)\\s+(.*)$"};
    regex_match(line, multiEdgeMatch, multiEdgePattern);

    // If line matches regular expression  
    if (!multiEdgeMatch.empty())
    {
        // Extract sign and width of wires
        WireType type = get_wire_type(multiEdgeMatch.str(1));
        bool sign = (multiEdgeMatch.str(2) != "U");
        int width = stoi(multiEdgeMatch.str(3));

        // Regular expression to match individual wire names
        string edgeSubstring = multiEdgeMatch.str(4);
        regex edgePattern{"(\\w+)\\s*(?:,|$)"};

        // Create a wire structure for each name that is found
        sregex_iterator start = sregex_iterator(edgeSubstring.begin(), edgeSubstring.end(), edgePattern);
        sregex_iterator end = sregex_iterator();
        for (sregex_iterator i = start; i != end; ++i)
        {
            smatch edgeMatch = *i;
            wire* newEdge = new edge;
            newEdge->name = edgeMatch.str(1);
            newEdge->type = type;
            newEdge->width = width;

            // Force single bit wires to be unsigned
            if (newEdge->width > 1)
            {
                newEdge->sign = sign;
            }
            else
            {
                newEdge->sign = false;
            }             
            newEdge->src = NULL;

            // Update wires in data manager
            dataManager->wires.push_back(newEdge);
        }
    }*/

/*void FileParser::getGraphEdges()
{
    for (string& line: lines)
    {
        getGraphEdgesFromLine(line);
    }
}

void FileParser::getGraphEdgesFromLine(string line)
{
    // Regular expression to match wire
    // Wires are the union of all inputs, outputs, wires, and registers
    smatch edgeLineMatch;
    const regex edgeLinePattern{"^\\s*(input|output|variable)\\s*(U?)Int(\\d+)\\s+(.*)$"};
    regex_match(line, edgeLineMatch, edgeLinePattern);

    // If line matches regular expression  
    if (!edgeLineMatch.empty())
    {
        // Extract sign and width of wires
        EdgeType type = stringToEdgeType(edgeLineMatch.str(1));
        bool isSigned = (edgeLineMatch.str(2) != "U");
        int width = stoi(edgeLineMatch.str(3));

        // Regular expression to match individual wire names
        string edgeSubstring = edgeLineMatch.str(4);
        regex edgePattern{"(\\w+)\\s*(?:,|$)"};

        // Create a wire structure for each name that is found
        sregex_iterator start = sregex_iterator(edgeSubstring.begin(), edgeSubstring.end(), edgePattern);
        sregex_iterator end = sregex_iterator();
        for (sregex_iterator i = start; i != end; ++i)
        {
            smatch edgeMatch = *i;
            edge* newEdge = new edge;
            newEdge->type = type;
            newEdge->width = width;

            // Force single bit wires to be unsigned
            if (newEdge->width > 1)
            {
                newEdge->isSigned = isSigned;
            }
            else
            {
                newEdge->isSigned = false;
            }             
            newEdge->src = NULL;

            // Update wires in data manager
            dataManager->edges.push_back(newEdge);

            // Add wire to wire table
            string edgeName = edgeMatch.str(1);
            activeEdges[edgeName] = newEdge;
            // activeEdges.insert({edgeName, newEdge});
        }
    }
}*/

void FileParser::getVertices()
{
    for (string& line: lines)
    {
        // updateConditionalState(line);
        getVerticesFromLine(line);
    }
}

/*void FileParser::updateActiveEdges(string line)
{
    smatch ifMatch;
    const regex ifPattern{"^\\s*if\\s*\\(?\\s*(\\w+)\\s*\\)?\\s*$"};
    regex_match(line, ifMatch, ifPattern);
    if (!ifMatch.empty())
    {
        edge* newEdge = new edge;
        dataManager->edges.push_back(newEdge);
        condEdgeQueue.push(newEdge);
        edgeQueue.push(activeEdges); 
        vertex* newVertex = new vertex;
        dataManager->vertices.push_back(newVertex);
        activeEdge
        newVertex->inputs.push_back()
    }
}*/

void FileParser::getVerticesFromLine(string line)
{
    smatch vertexMatch;
    const regex vertexPattern{"^\\s*(\\w+)\\s*=\\s*(\\w+)\\s*(\\S+)?\\s*(\\w+)?\\s*(\\S+)?\\s*(\\w+)?\\s*$"};
    regex_match(line, vertexMatch, vertexPattern);
    if (!vertexMatch.empty())
    {
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
            string operation = edgeNames[0] + " = " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2];
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
            string operation = edgeNames[0] + " = " + edgeNames[1] + " " + operators[0] + " " + edgeNames[2];
            createVertex(VertexType::LOGIC, operation, {edgeNames[1], edgeNames[2], edgeNames[3]}, {edgeNames[0]});
        }
    }
}

edge* FileParser::getEdge(string edgeName)
{
    if (activeEdges.find(edgeName) == activeEdges.end())
    {
        cout << edgeName << endl;
        edge* newEdge = createNewEdge(edgeName);
        if (dataManager->nets.find(edgeName) == dataManager->nets.end())
        {
            missingEdges[edgeName] = newEdge;
        }
    }
    return activeEdges[edgeName];
}

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
            inputs.push_back(output);
            output = createNewEdge(outputName);
        }
        outputs.push_back(output);
    }
    return createVertex(type, operation, inputs, outputs);
}

vertex* FileParser::createVertex(VertexType type, string operation, vector<edge*> inputs, vector<edge*> outputs)
{
    vertex* newVertex = new vertex;
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
    /*if (conditionalStart.empty())
    {
        newVertex->branch = NULL;
        newVertex->cond = false;
    }
    else
    {
        newVertex->branch = conditionalStart.back();
        newVertex->cond = conditionalState.back();
        newVertex->inputs.push_back(newVertex->branch->outputs[0]);
    }*/
    dataManager->vertices.push_back(newVertex);
    return newVertex;
}

edge* FileParser::createNewEdge(string edgeName)
{
    edge* newEdge = new edge;
    cout << edgeName << endl;
    newEdge->src = NULL;
    dataManager->edges.push_back(newEdge);
    activeEdges[edgeName] = newEdge;
    dataManager->printGraph();
    return newEdge;
}

/*void FileParser::reduceGraph()
{
    for (vertex*& currVertex : dataManager->vertices)
    {
        vector<edge*> edgesCopy = currVertex->inputs;
        for (edge*& input : currVertex->inputs)
        {
            vector<edge*> implicitEdges = getImplicitEdges(input);
            getUnion(implicitEdges, currVertex->inputs);
            for (edge*& input : currVertex->inputs)
            {

            }
            for (edge*& implicitEdge : implicitEdges)
            {
                for (edge*& currEdge : edgesCopy)
                {
                    if (implicitEdge == currEdge)
                    {

                    }
                }
            }
        }
    }
}

vector<edge*> FileParser::getImplicitEdges(edge* inputEdge)
{
    if (inputEdge->src == NULL)
    {
        return {};
    }
    else
    {
        vertex* vertexSrc = inputEdge->src;
        vector<edge*> implicitEdges;
        for (edge*& input : vertexSrc->inputs)
        {
            vector<edge*> newEdges = getImplicitEdges(input);
            implicitEdges.insert(implicitEdges.end(), newEdges.begin(), newEdges.end());
        }
        return implicitEdges;
    }
}

vector<edge*> FileParser::getUnion(vector<edge*> aEdges, vector<edge*> bEdges)
{
    vector<edge*> retEdges;
    for (edge*& aEdge : aEdges)
    {
        for (edge*& bEdge : bEdges)
        {
            if (aEdge == bEdge)
            {
                retEdges.push_back(aEdge);
            }
        }
    }
    return retEdges;
}*/

string FileParser::strtrim(string str)
{
    string whitespace = " \t\n\v\f\r";
    int endIdx = str.find_last_not_of(whitespace);
    int startIdx = str.find_first_not_of(whitespace);
    return str.substr(startIdx, endIdx - startIdx + 1);
};

} // namespace HighLevelSynthesis