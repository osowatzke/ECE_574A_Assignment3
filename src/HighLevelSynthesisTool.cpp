#include "HighLevelSynthesisTool.h"

#include <iostream>
#include <string>

using namespace std;

namespace HighLevelSynthesis
{
    // Class constructor creates the data manager and provides subclasses
    // with pointer access to the data manager 
    HighLevelSynthesisTool::HighLevelSynthesisTool()
        : dataManager(DataManager())
        , fileParser(FileParser(&dataManager))
        , scheduler(FDScheduler(&dataManager))
        , fsmGenerator(FsmGenerator(&dataManager))
        , fileWriter(FileWriter(&dataManager)) {}

    // Function runs the high level synthesis tool
    int HighLevelSynthesisTool::run(string cFile, int latency, string verilogFile)
    {
        // Attempt to parse the C File, and return with error if parsing fails
        int retVal = fileParser.run(cFile);
        if (retVal)
        {
            return retVal;
        }

        // Attempt to run Force Directed Scheduling. Providing latency - 1 instead
        // of latency to account for transition time out of Wait state.
        retVal = scheduler.run(latency - 1);
        if (retVal)
        {
            return retVal;
        }

        // Run the FSM Generator with the same latency constraint
        fsmGenerator.run(latency - 1);

        // Write the verilog file and return the file writer's error level
        retVal = fileWriter.run(verilogFile);
        return retVal;
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