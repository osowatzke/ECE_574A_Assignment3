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

    getGraphEdges();

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

void FileParser::getGraphEdges()
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
}

} // namespace HighLevelSynthesis