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
    verilogFile << "        if (Rst = 1) begin" << endl;
    auto netStart = dataManager->nets.begin();
    auto netEnd = dataManager->nets.end();
    for (auto netIt = netStart; netIt != netEnd; ++netIt)
    {
        string netName = netIt->first;
        net* currNet = netIt->second;
        if ((currNet->type == NetType::OUTPUT) || (currNet->type == NetType::VARIABLE))
        {
            verilogFile << "            " << netName << " <= 0;" << endl;
        }
    }
    verilogFile << "        end" << endl;
}

void FileWriter::declareFsm()
{
    verilogFile << "    always @(posedge clk) begin" << endl;
    declareFsmReset();
    verilogFile << "    end" << endl;
}

int FileWriter::determineNumUniqueStates()
{
    int numUniqueStates = 0;
    for (vertex*& currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        numUniqueStates = max(numUniqueStates, vertexEndTime);
    }
    return numUniqueStates;
}

void FileWriter::declareStates()
{
    int numUniqueStates = determineNumUniqueStates();
    int numStates = numUniqueStates + 2;
    verilogFile << "    localparam " << "Wait = 0," << endl;
    for (int i = 0; i < numUniqueStates; ++i)
    {
        verilogFile << "               " << "State" << i << " = " << i + 1 << "," << endl;
    }
    verilogFile << "               " << "Final = " << numStates - 1 << ";" << endl << endl;
    int numStateBits = static_cast<int>(ceil(log2(numStates - 1)));
    verilogFile << "    reg ";
    if (numStateBits > 1)
    {
        verilogFile << "[" << numStateBits - 1 << ":0] ";
    }
    verilogFile << "state;" << endl << endl; 
}

void FileWriter::declareNets()
{
    verilogFile << "    input Clk, Rst, Start;" << endl;
    verilogFile << "    output reg Done;" << endl << endl;
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
            verilogFile << "    input ";
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
            verilogFile << "    output reg ";
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
            verilogFile << "    reg ";
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
    verilogFile << "timescale 1ns/1ns" << endl << endl;
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