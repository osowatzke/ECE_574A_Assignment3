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

void FileWriter::declareFsmStates()
{
    int numUniqueStates = determineNumUniqueStates();
    string firstStateName;
    if (numUniqueStates == 0)
    {
        firstStateName = "Final";
    }
    else
    {
        firstStateName = "State0";
    }
    verilogFile << tab(2) << "else begin" << endl;
    verilogFile << tab(3) << "Done <= 0;" << endl;
    verilogFile << tab(3) << "case (State)" << endl;
    verilogFile << tab(4) << "Wait : begin" << endl;
    verilogFile << tab(5) << "if (Start == 1) begin" << endl;
    verilogFile << tab(6) << "State <= " << firstStateName << ";" << endl;
    if (numUniqueStates == 0)
    {
        verilogFile << tab(6) << "Done <= 1;" << endl;
    }
    verilogFile << tab(5) << "end" << endl;
    verilogFile << tab(4) << "end" << endl;
    for (int time = 0; time < numUniqueStates; ++time)
    {
        verilogFile << tab(4) << "State" << time << " : begin" << endl;
        for (vertex*& currVertex : dataManager->vertices)
        {
            if (currVertex->time == time)
            {
                verilogFile << tab(5) << currVertex->operation << ";" << endl;
            }
        }
        verilogFile << tab(5) << "State <= ";
        if (time == (numUniqueStates - 1))
        {
            verilogFile << "Done;" << endl;
        }
        else
        {
             verilogFile << "State" << time + 1 << ";" << endl;
        }
        verilogFile << "                end" << endl;
    }
   verilogFile << tab(4) << "Final : begin" << endl;
   verilogFile << tab(5) << "State <= Wait;" << endl;
   verilogFile << tab(4) << "end" << endl;
   verilogFile << tab(3) << "endcase" << endl;
   verilogFile << tab(2) << "end" << endl;
}

void FileWriter::declareFsm()
{
    verilogFile << tab(1) << "always @(posedge Clk) begin" << endl;
    declareFsmReset();
    declareFsmStates();
    verilogFile << tab(1) << "end" << endl << endl;
}

void FileWriter::addVerticesToStates()
{
    for (vertex*& currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        int numStatesToAdd = vertexEndTime - states.size() + 1;
        for (int i = 0; i < numStatesToAdd; ++i)
        {
            State newState;
            vector<State> stateVector;
            stateVector.push_back(newState);
            states.push_back(stateVector);
        }
        int time = currVertex->time;
        bool stateMatch = false;
        for (State currState : states[time])
        {
            if ((currState.vertices.empty()) || (currVertex->parent == currState.vertices[0]->parent))
            {
                stateMatch = true;
                currState.vertices.push_back(currVertex);
                break;
            }
        }
        if (!stateMatch)
        {
            State newState;
            newState.vertices.push_back(currVertex);
            states[time].push_back(newState);
        }
    }
}

int FileWriter::determineNumUniqueStates()
{
    int numUniqueStates = 0;
    for (vertex*& currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        numUniqueStates = max(numUniqueStates, vertexEndTime + 1);
    }
    return numUniqueStates;
}

void FileWriter::declareStates()
{
    int numUniqueStates = determineNumUniqueStates();
    int numStates = numUniqueStates + 2;
    verilogFile << tab() << "localparam " << "Wait = 0," << endl;
    for (int i = 0; i < numUniqueStates; ++i)
    {
        verilogFile << tab(3) << "   " << "State" << i << " = " << i + 1 << "," << endl;
    }
    verilogFile << tab(3) << "   " << "Final = " << numStates - 1 << ";" << endl << endl;
    int numStateBits = static_cast<int>(ceil(log2(numStates)));
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