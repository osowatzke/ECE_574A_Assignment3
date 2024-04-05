#include "FileWriter.h"

#include <cmath>
#include <iostream>

namespace HighLevelSynthesis
{

FileWriter::FileWriter(DataManager* dataManager)
    : dataManager(dataManager) {}

void FileWriter::run(string filePath)
{
    openFile(filePath);
    declareModule();
    declareNets();
    declareFsm();
    terminateModule();
    closeFile();
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
    /*for (size_t i = 0; i < (depth+1); ++i)
    {
        cout << "    ";
    }*/
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

void FileWriter::declareStates()
{
    verilogFile << tab() << "localparam " << "Wait = 0," << endl;
    int stateIdx = 1;
    for (state* currState : dataManager->states)
    {
        verilogFile << tab(3) << "   " << currState->name << " = " << stateIdx << "," << endl;
        stateIdx++;
    }
    verilogFile << tab(3) << "   " << "Final = " << stateIdx << ";" << endl << endl;
    int numStateBits = static_cast<int>(ceil(log2(stateIdx + 1)));
    verilogFile << tab() << "reg ";
    if (numStateBits > 1)
    {
        verilogFile << "[" << numStateBits - 1 << ":0] ";
    }
    verilogFile << "State;" << endl << endl;
}

void FileWriter::declareNets()
{
    verilogFile << tab() << "input Clk, Rst, Start;" << endl;
    verilogFile << tab() << "output reg Done;" << endl << endl;
    bool uniqueInputs = false;
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if (currNet->type == NetType::INPUT)
        {
            uniqueInputs = true;
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
    if (uniqueInputs)
    {
        verilogFile << endl;
    }

    bool uniqueOutputs = false;
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if (currNet->type == NetType::OUTPUT)
        {
            uniqueOutputs = true;
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
    if (uniqueOutputs)
    {
        verilogFile << endl;
    }

    bool uniqueReg = false;
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if (currNet->type == NetType::VARIABLE)
        {
            uniqueReg = true;
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
    if (uniqueReg)
    {
        verilogFile << endl;
    }
    declareStates();
}

void FileWriter::declareModule()
{
    verilogFile << "`timescale 1ns/1ns" << endl << endl;
    verilogFile << "module HLSM(Clk, Rst, Start, Done";
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if (currNet->type == NetType::INPUT)
        {
            verilogFile << ", " << netName;
        }
    }
    for (auto netIt = netStart; netIt != netEnd; netIt++)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if (currNet->type == NetType::OUTPUT)
        {
            verilogFile << ", " << netName;
        }
    }
    verilogFile << ");" << endl;
}

void FileWriter::terminateModule()
{
    verilogFile << "endmodule" << endl;
}

} // namespace HighLevelSynthesis