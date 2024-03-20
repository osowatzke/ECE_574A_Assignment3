#include "HighLevelSynthesisTool.h"

#include <iostream>
#include <string>

using namespace std;

namespace HighLevelSynthesis
{
    // Dummy run function. Populate with actual function calls once they are created.
    int HighLevelSynthesisTool::run(string cFile, int latency, string verilogFile)
    {
        cout << "Generating verilog file \"" << verilogFile << "\" from \"" << cFile << "\" with latency constraint of " << latency << " cycles." << endl;
        return 0;
    }
} // namespace HighLevelSynthesis

// Main routine
int main(int argc, char* argv[])
{
    // Only 1 additional argument provided
    if (argc == 2)
    {
        // Handle -h flag for function help
        if (string(argv[1]) == "-h")
        {
            cout << "Description:" << endl << endl;
            cout << "Function performs high-level synthesis. It converts a C-like behavioral " << endl;
            cout << "description into a scheduled high-level state machine implemented in Verilog." << endl << endl;
            cout << "Usage:" << endl << endl;
            cout << "hlysn cFile latency verilogFile" << endl << endl;
            return 0;
        } 
    }

    // 3 additional arguments provided
    else if (argc == 4)
    {
        // parse input arguments
        string cFile = argv[1];
        int latency = stoi(argv[2]);
        string verilogFile = argv[3];

        // Run high-level synthesis tool
        HighLevelSynthesis::HighLevelSynthesisTool hlsTool;
        return hlsTool.run(cFile, latency, verilogFile);
    }
    
    // Invalid input arguments. Provide usage for end user
    cout << "ERROR: Invalid Arguments" << endl << endl;
    cout << "Usage:" << endl << endl;
    cout << "hlysn cFile latency verilogFile" << endl << endl;
    return 1;
}