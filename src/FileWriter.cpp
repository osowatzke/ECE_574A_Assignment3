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

void FileWriter::printStates()
{
    string firstStateName = "Final";
    if (!dataManager->states.empty())
    {
        firstStateName = dataManager->states[0]->name;
    }
    verilogFile << tab(2) << "else begin" << endl;
    verilogFile << tab(3) << "Done <= 0;" << endl;
    verilogFile << tab(3) << "case (State)" << endl;
    verilogFile << tab(4) << "Wait : begin" << endl;
    verilogFile << tab(5) << "if (Start == 1) begin" << endl;
    verilogFile << tab(6) << "State <= " << firstStateName << ";" << endl;
    if (dataManager->states.empty())
    {
        verilogFile << tab(6) << "Done <= 1;" << endl;
    }
    verilogFile << tab(5) << "end" << endl;
    verilogFile << tab(4) << "end" << endl;
    for (state* currState : dataManager->states)
    {
        verilogFile << tab(4) << currState->name << " : begin" << endl;
        for (vertex* currVertex : currState->vertices)
        {
            verilogFile << tab(5) << currVertex->operation << ";" << endl;
        }
        if (currState->transitions.empty())
        {
            verilogFile << tab(5) << "State <= Final;" << endl;
            verilogFile << tab(5) << "Done <= 1;" << endl;
        }
        else
        {
            printStateTransition(currState, {}, 0);
        }
        verilogFile << tab(4) << "end" << endl;
    }
    verilogFile << tab(4) << "Final : begin" << endl;
    verilogFile << tab(5) << "State <= Wait;" << endl;
    verilogFile << tab(4) << "end" << endl;
    verilogFile << tab(3) << "endcase" << endl;
    verilogFile << tab(2) << "end" << endl;
}

void FileWriter::printStateTransition(state* currState, vector<bool> condition, int depth)
{
    vector<stateTransition*> transitions = currState->transitions;
    int indent = depth + 5;
    if (transitions.size() == (1 << depth))
    {
        state* nextState = getNextState(currState, condition);
        verilogFile << tab(indent) << "state <= " << nextState->name << endl;
    }
    else
    {
        verilogFile << tab(indent) << currState->transitions[0]->condition[depth] << " begin" << endl;
        vector<bool> nextCondition = condition;
        nextCondition.push_back(true);
        printStateTransition(currState, nextCondition, depth + 1);
        verilogFile << tab(indent) << "end" << endl;
        verilogFile << tab(indent) << "else begin" << endl;
        nextCondition = {};
        nextCondition = condition;
        nextCondition.push_back(false);
        printStateTransition(currState, nextCondition, depth + 1);
        verilogFile << tab(indent) << "end" << endl;
    }
}

state* FileWriter::getNextState(state* currState, vector<bool> condition)
{
    for (stateTransition* transition : currState->transitions)
    {
        bool match = true;
        for (size_t i = 0; i < condition.size(); ++i)
        {
            if (transition->isTrue[i] != condition[i])
            {
                match = false;
            }
        }
        if (match)
        {
            return transition->nextState;
        }
    }
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

void FileWriter::declareFsmReset()
{
    verilogFile << tab(2) << "if (Rst == 1) begin" << endl;
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; ++netIt)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if ((currNet->type == NetType::OUTPUT) || (currNet->type == NetType::VARIABLE))
        {
            verilogFile << tab(3) << netName << " <= 0;" << endl;
        }
    }
    verilogFile << tab(3) << "Done <= 0;" << endl;
    verilogFile << tab(3) << "State <= Wait;" << endl;
    verilogFile << tab(2) << "end" << endl;
}

string FileWriter::tab()
{
    return "    ";
}

string FileWriter::tab(int numTabs)
{
    string retVal;
    for (int i = 0; i < numTabs; ++i)
    {
        retVal += tab();
    }
    return retVal;
}

void FileWriter::declareFsm()
{
    verilogFile << tab(1) << "always @(posedge Clk) begin" << endl;
    declareFsmReset();
    printStates();
    verilogFile << tab(1) << "end" << endl << endl;
}

// Function defines local parameters for each of the FSM states
void FileWriter::declareStates()
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
    declareStates();
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