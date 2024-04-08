#include "FileWriter.h"

#include <cmath>
#include <iostream>

namespace HighLevelSynthesis
{

FileWriter::FileWriter(DataManager* dataManager)
    : dataManager(dataManager) {}

// Function writes verilog file for HLSM
int FileWriter::run(string filePath)
{
    // Attempt to open the verilog file
    int retVal = openFile(filePath);

    // If opening the verilog file fails, return with error
    if (retVal)
    {
        return retVal;
    }

    // Declare module name
    declareModule();

    // Declare module nets (inputs, outputs, variables)
    declareNets();

    // Declare the FSM
    declareFsm();

    // End the module
    terminateModule();

    // Close the file
    closeFile();
    return 0;
}

// Function declares the states of the FSM
void FileWriter::declareFsmStates()
{
    // Assume that the FSM will transition from Wait to Final
    string firstStateName = "Final";

    // If the state manager has any states
    if (!dataManager->states.empty())
    {
        // Update the name of teh first state
        firstStateName = dataManager->states[0]->name;
    }

    // Declare else statement (when Rst=0)
    verilogFile << tab(2) << "else begin" << endl;

    // Declare default value for Done signal
    verilogFile << tab(3) << "Done <= 0;" << endl;

    // Create case statement for each state of the FSM
    verilogFile << tab(3) << "case (State)" << endl;

    // Declare the Wait state
    verilogFile << tab(4) << "Wait : begin" << endl;
    verilogFile << tab(5) << "if (Start == 1) begin" << endl;
    verilogFile << tab(6) << "State <= " << firstStateName << ";" << endl;
    if (dataManager->states.empty())
    {
        verilogFile << tab(6) << "Done <= 1;" << endl;
    }
    verilogFile << tab(5) << "end" << endl;
    verilogFile << tab(4) << "end" << endl;

    // Declare the subsequent states in the FSM
    for (state* currState : dataManager->states)
    {
        // Add condition to case statement
        verilogFile << tab(4) << currState->name << " : begin" << endl;

        // Add vertices to state
        for (vertex* currVertex : currState->vertices)
        {
            verilogFile << tab(5) << currVertex->operation << ";" << endl;
        }

        // If state has no transitions, it is the end of the user-defined states
        // Transition to Final State and Assert Done signal
        if (currState->transitions.empty())
        {
            verilogFile << tab(5) << "State <= Final;" << endl;
            verilogFile << tab(5) << "Done <= 1;" << endl;
        }
        // Declare state transition
        else
        {
            declareStateTransition(currState, {}, 0);
        }

        // End condition
        verilogFile << tab(4) << "end" << endl;
    }

    // Declare final state in FSM
    verilogFile << tab(4) << "Final : begin" << endl;
    verilogFile << tab(5) << "State <= Wait;" << endl;
    verilogFile << tab(4) << "end" << endl;

    // Terminate the case statement and if-else statement
    verilogFile << tab(3) << "endcase" << endl;
    verilogFile << tab(2) << "end" << endl;
}

// Function declares a state transition
void FileWriter::declareStateTransition(state* currState, vector<bool> condition, int depth)
{
    // Get the transitions of the current state
    vector<stateTransition*> transitions = currState->transitions;

    // Set the indent
    int indent = depth + 5;

    // Maximum conditional depth reached
    if (transitions.size() == (1 << depth))
    {
        // Determine the next state
        state* nextState = getNextState(currState, condition);

        // Print the next state to the verilog file
        verilogFile << tab(indent) << "State <= " << nextState->name << ";" << endl;
    }
    // Maximum condition depth not reached
    else
    {
        // Print conditional statement
        verilogFile << tab(indent) << currState->transitions[0]->condition[depth] << " begin" << endl;

        // Print true branch of conditional statement
        vector<bool> nextCondition = condition;
        nextCondition.push_back(true);
        declareStateTransition(currState, nextCondition, depth + 1);
        verilogFile << tab(indent) << "end" << endl;

        // Print false branch of conditional statement
        verilogFile << tab(indent) << "else begin" << endl;
        nextCondition = condition;
        nextCondition.push_back(false);
        declareStateTransition(currState, nextCondition, depth + 1);
        verilogFile << tab(indent) << "end" << endl;
    }
}

// Function determines the next state
state* FileWriter::getNextState(state* currState, vector<bool> condition)
{
    // Loop through the transitions of the current state
    for (stateTransition* transition : currState->transitions)
    {
        // Assume that the transition matches the conditions
        bool match = true;

        // Loop through each of the conditions
        for (size_t i = 0; i < condition.size(); ++i)
        {
            // If transition does not match condition
            if (transition->isTrue[i] != condition[i])
            {
                match = false;
            }
        }
        // If transition does not match conditions
        if (match)
        {
            // Return the next state defined by the transition
            return transition->nextState;
        }
    }
    // Return NULL pointer for no next state
    return NULL;
}

// Function opens the verilog file for writing
int FileWriter::openFile(string filePath)
{
     // Attempt to open file
    verilogFile.open(filePath);

    // Return with an error message if unable to open file
    if (!verilogFile.is_open())
    {
        cout << "ERROR: Unable to open output file: \"" << filePath << "\". Ensure output directory exists.";
        return 1;
    }
    return 0;
}

// Function closes the verilog file
void FileWriter::closeFile()
{
    verilogFile.close();
}

// Function declares the FSM reset
void FileWriter::declareFsmReset()
{
    // Add reset if statement to FSM
    verilogFile << tab(2) << "if (Rst == 1) begin" << endl;

    // Loop through all the nets
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; ++netIt)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is a variable or an output, it is driven by the FSM
        // Set net to 0 during reset
        if ((currNet->type == NetType::OUTPUT) || (currNet->type == NetType::VARIABLE))
        {
            verilogFile << tab(3) << netName << " <= 0;" << endl;
        }
    }

    // Set fixed FSM outputs
    verilogFile << tab(3) << "Done <= 0;" << endl;
    verilogFile << tab(3) << "State <= Wait;" << endl;

    // Terminate reset if statement
    verilogFile << tab(2) << "end" << endl;
}

// Function returns a single tab in spaces
string FileWriter::tab()
{
    return "    ";
}

// Function returns multiple tabs in spaces
string FileWriter::tab(int numTabs)
{
    // Append multiple tabs onto string
    string retVal;
    for (int i = 0; i < numTabs; ++i)
    {
        retVal += tab();
    }
    // Return string containing multiple tabs
    return retVal;
}

// Function declares FSM process
void FileWriter::declareFsm()
{
    // Print start of procedure to file
    verilogFile << tab(1) << "always @(posedge Clk) begin" << endl;

    // Declare the FSM reset logic
    declareFsmReset();

    // Declare the FSM states
    declareFsmStates();

    // Print end of procedure to file
    verilogFile << tab(1) << "end" << endl << endl;
}

// Function defines local parameters for each of the FSM states
void FileWriter::declareStateNet()
{
    // Create local parameter for fixed states
    verilogFile << tab() << "localparam " << "Wait = 0," << endl;

    // Set the current state index
    int stateIdx = 1;

    // Loop through all the states
    for (state* currState : dataManager->states)
    {
        // Print state enumeration to file
        verilogFile << tab(3) << "   " << currState->name << " = " << stateIdx << "," << endl;

        // Increment state index
        stateIdx++;
    }

    // Print file state to file
    verilogFile << tab(3) << "   " << "Final = " << stateIdx << ";" << endl << endl;

    // Determine the number of bits needed to represent the state vector
    int numStateBits = static_cast<int>(ceil(log2(stateIdx + 1)));

    // Print the state register to the file
    verilogFile << tab() << "reg ";
    if (numStateBits > 1)
    {
        verilogFile << "[" << numStateBits - 1 << ":0] ";
    }
    verilogFile << "State;" << endl << endl;
}

// Function declares the nets (inputs, outputs, wires, and registers)
void FileWriter::declareNets()
{
    // Print fixed inputs and outputs to file
    verilogFile << tab() << "input Clk, Rst, Start;" << endl;
    verilogFile << tab() << "output reg Done;" << endl << endl;

    // Assume that there are no unique inputs
    bool uniqueInputs = false;

    // Loop through all the nets
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is an input
        if (currNet->type == NetType::INPUT)
        {
            // Set the unique inputs flag high
            uniqueInputs = true;

            // Write input to the file
            verilogFile << tab() << "input ";
            if (currNet->width > 1)
            {
                if (currNet->isSigned)
                {
                    verilogFile << "signed ";
                }
                verilogFile << "[" << currNet->width - 1 << ":0] ";
            }
            verilogFile << netName << ";" << endl;
        }
    }
    // Add newline between unique inputs and outputs
    if (uniqueInputs)
    {
        verilogFile << endl;
    }

    // Assume that there are no unique outputs
    bool uniqueOutputs = false;

    // Loop through all the nets
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is an output
        if (currNet->type == NetType::OUTPUT)
        {
            // Set the unique outputs flag high
            uniqueOutputs = true;

            // Print output to the file
            verilogFile << tab() << "output reg ";
            if (currNet->width > 1)
            {
                if (currNet->isSigned)
                {
                    verilogFile << "signed ";
                }
                verilogFile << "[" << currNet->width - 1 << ":0] ";
            }
            verilogFile << netName << ";" << endl;
        }
    }
    // Add newline between unique outputs and registers
    if (uniqueOutputs)
    {
        verilogFile << endl;
    }

    // Assume there are no unique registers
    bool uniqueReg = false;

    // Loop through all the nets
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is a variable
        if (currNet->type == NetType::VARIABLE)
        {
            // Set the unique register flag high
            uniqueReg = true;

            // Print register to the file
            verilogFile << tab() << "reg ";
            if (currNet->width > 1)
            {
                if (currNet->isSigned)
                {
                    verilogFile << "signed ";
                }
                verilogFile << "[" << currNet->width - 1 << ":0] ";
            }
            verilogFile << netName << ";" << endl;
        }
    }
    // Add newline between unique outputs and FSM states
    if (uniqueReg)
    {
        verilogFile << endl;
    }

    // Declare FSM states
    declareStateNet();
}

// Function declares module
void FileWriter::declareModule()
{
    // Print timescale to the file
    verilogFile << "`timescale 1ns/1ns" << endl << endl;

    // Print fixed module name and fixed module inputs/outputs
    verilogFile << "module HLSM(Clk, Rst, Start, Done";

    // Loop through all the nets
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is an input
        if (currNet->type == NetType::INPUT)
        {
            // Write input name to the file
            verilogFile << ", " << netName;
        }
    }
    
    // Loop through all the nets
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;

        // If net is an output
        if (currNet->type == NetType::OUTPUT)
        {
            // Write output name to the file
            verilogFile << ", " << netName;
        }
    }

    // End module declaration
    verilogFile << ");" << endl;
}

// Function terminates module
void FileWriter::terminateModule()
{
    verilogFile << "endmodule" << endl;
}

} // namespace HighLevelSynthesis