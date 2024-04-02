#include "FileWriter.h"

#include <cmath>
#include <iostream>

namespace HighLevelSynthesis
{

FileWriter::FileWriter(DataManager* dataManager)
    : dataManager(dataManager) {}

FileWriter::~FileWriter()
{
    for (vector<state*> stateVector : states)
    {
        for (state* currState : stateVector)
        {
            delete(currState);
        }
    }
}

void FileWriter::run(string filePath)
{
    createStates();
    cout << "DONE" << endl;
    printStates();
    openFile(filePath);
    declareModule();
    declareNets();
    declareFsm();
    terminateModule();
    closeFile();
}

void FileWriter::getNumStatesPerTimestep()
{
    int numTimesteps = getNumTimesteps();
    numStatesPerTimestep = vector<int> (numTimesteps, 0);
    
};

void FileWriter::getStates()
{
    int numTimesteps = getNumTimesteps();
    int maxTime = numTimesteps - 1;
    for (int time = 0; time < maxTime; ++time)
    {
        
    }
}

void FileWriter::createStates()
{
    determineHierarchyMapping();
    int numTimesteps = getNumTimesteps();
    vector<state*> initialStateVector;
    vector<vector<state*>> newStates(numTimesteps, initialStateVector);
    states = newStates;
    for (int time = 0; time < numTimesteps; ++time)
    {
        for (hierarchy* currHierarchy : hierarchyMapping[time])
        {
            state* newState = new state;
            hierarchy* searchHieararchy = currHierarchy;
            for (vertex* currVertex : searchHieararchy->vertices)
            {
                if (currVertex->time == time)
                {    
                    newState->vertices.push_back(currVertex);
                    currVertex->currState = newState;
                }
            }
            while (searchHieararchy->parent != NULL)
            {
                searchHieararchy = searchHieararchy->parent->parent;
                for (vertex* currVertex : searchHieararchy->vertices)
                {
                    if (currVertex->time == time)
                    {    
                        newState->vertices.push_back(currVertex);
                        currVertex->currState = newState;
                    }
                }
            }
            states[time].push_back(newState);
            /*bool addFalseState = currHierarchy->parent != NULL;
            for (hierarchy* compHierarchy : hierarchyMapping[time])
            {
                if ((currHierarchy != compHierarchy) && (currHierarchy->parent != NULL))
                {
                    if (currHierarchy->parent == compHierarchy->parent)
                    {
                        addFalseState = false;
                    }
                }
            }
            if (addFalseState)
            {
                state* newState = new state;
                hierarchy* searchHieararchy = currHierarchy;
                while (searchHieararchy->parent != NULL)
                {
                    searchHieararchy = searchHieararchy->parent->parent;
                    for (vertex* currVertex : searchHieararchy->vertices)
                    {
                        if (currVertex->time == time)
                        {    
                            newState->vertices.push_back(currVertex);
                            currVertex->currState = newState;
                        }
                    }
                }
                states[time].push_back(newState);
            }*/
        }
    }
}

void FileWriter::printStates()
{
    int numTimestamps = states.size();
    for (int time = 0; time < numTimestamps; ++time)
    {
        cout << "Time " << time << ":" << endl;
        int stateIdx = 0;
        for (state* currState : states[time])
        {
            cout << "\tState" << time;
            if (states[time].size() > 1)
            {
                cout << "_" << stateIdx << endl;
            }
            cout << endl;
            for (vertex* currVertex : currState->vertices)
            {
                if (currVertex->type != VertexType::JOIN) // currVertex->type != VertexType::FORK &&
                {
                    cout << "\t\t" << currVertex->operation << endl;
                }
            }
            stateIdx++;
        }
    }
}

void FileWriter::determineHierarchyMapping()
{
    int numTimesteps = getNumTimesteps();
    vector<hierarchy*> initialVector(1, dataManager->graphHierarchy);
    vector<vector<hierarchy*>> newHierarchyMapping(numTimesteps, initialVector);
    hierarchyMapping = newHierarchyMapping;
    determineHierarchyMapping(dataManager->graphHierarchy);
    for (int time = 0; time < numTimesteps; ++time)
    {
        vector<hierarchy*> newHierarchyVector;
        for (hierarchy* currHierarchy : hierarchyMapping[time])
        {
            bool isParent = false;
            for (hierarchy* compHierarchy : hierarchyMapping[time])
            {
                if (!isParent && compHierarchy != currHierarchy)
                {
                    isParent = isParentHierarchy(compHierarchy, currHierarchy);
                }
            }
            if (!isParent)
            {
                newHierarchyVector.push_back(currHierarchy);
            }
        }
        cout << time << " : " << hierarchyMapping[time].size() << endl;
        hierarchyMapping[time] = newHierarchyVector;
        // cout << time << " : " << newHierarchyVector.size() << endl;
    }
}

void FileWriter::determineHierarchyMapping(hierarchy* hierarchy)
{
    for (conditionalHierarchy* condHier : hierarchy->conditional)
    {
        int startTime = getConditionalStartTime(condHier);
        int endTime = getConditionalEndTime(condHier);
        if (condHier->trueHiearchy != NULL)
        {
            for (int time = startTime; time <= endTime; ++time)
            {
                hierarchyMapping[time].push_back(condHier->trueHiearchy);
            }
            determineHierarchyMapping(condHier->trueHiearchy);
        }
        if (condHier->falseHiearchy != NULL)
        {
            for (int time = startTime; time <= endTime; ++time)
            {
                hierarchyMapping[time].push_back(condHier->falseHiearchy);
            }
            determineHierarchyMapping(condHier->falseHiearchy);
        }
    }
}

bool FileWriter::isParentHierarchy(hierarchy* currHierarchy, hierarchy* compHiearchy)
{
    bool isParent = currHierarchy == compHiearchy;
    while (currHierarchy->parent != NULL)
    {
        currHierarchy = currHierarchy->parent->parent;
        if (!isParent)
        {
            isParent = currHierarchy == compHiearchy;
        }
    }
    return isParent;
}

int FileWriter::getConditionalStartTime(conditionalHierarchy* condHier)
{
    int startTime = -1;
    for (vertex* currVertex : condHier->trueHiearchy->vertices)
    {
        int vertexStartTime = currVertex->time;
        if (startTime < 0)
        {
            startTime = vertexStartTime;
        }
        else
        {
            startTime = min(startTime, vertexStartTime);
        }
    }
    for (vertex* currVertex : condHier->falseHiearchy->vertices)
    {
        int vertexStartTime = currVertex->time;
        if (startTime < 0)
        {
            startTime = vertexStartTime;
        }
        else
        {
            startTime = min(startTime, vertexStartTime);
        }
    }
    return startTime;
};

int FileWriter::getConditionalEndTime(conditionalHierarchy* condHier)
{
    int endTime = 0;
    for (vertex* currVertex : condHier->trueHiearchy->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        endTime = max(endTime, vertexEndTime);
    }
    for (vertex* currVertex : condHier->falseHiearchy->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        endTime = max(endTime, vertexEndTime);
    }
    return endTime;
};

void FileWriter::getNumStatesPerTimestep(hierarchy* hier)
{
    vector<bool> vertexInTimestep(numStatesPerTimestep.size(), false);
    for (vertex*& currVertex : hier->vertices)
    {
        int time = currVertex->time;
        if (!vertexInTimestep[time])
        {
            ++numStatesPerTimestep[time]; 
        }
    }
    for (conditionalHierarchy*& conditional : hier->conditional)
    {
        if (conditional != NULL)
        {
            getNumStatesPerTimestep(conditional->trueHiearchy);
            getNumStatesPerTimestep(conditional->falseHiearchy);
        }
    }
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
    int numUniqueStates = getNumTimesteps();
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

/*void FileWriter::addVerticesToStates()
{
    for (vertex*& currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        int numStatesToAdd = vertexEndTime - states.size() + 1;
        for (int i = 0; i < numStatesToAdd; ++i)
        {
            state newState;
            vector<state> stateVector;
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
}*/

int FileWriter::getNumTimesteps()
{
    int numTimesteps = 0;
    for (vertex*& currVertex : dataManager->vertices)
    {
        int vertexEndTime = getVertexEndTime(currVertex);
        numTimesteps = max(numTimesteps, vertexEndTime + 1);
    }
    return numTimesteps;
}

void FileWriter::declareStates()
{
    int numUniqueStates = getNumTimesteps();
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