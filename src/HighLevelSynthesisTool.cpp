#include "HighLevelSynthesisTool.h"

#include <iostream>
#include <string>

using namespace std;

namespace HighLevelSynthesis
{
    int HighLevelSynthesisTool::run(string cFile, int latency, string verilogFile)
    {
        cout << "Generating verilog file \"" << verilogFile << "\" from \"" << cFile << "\" with latency constraint of " << latency << " cycles." << endl;
        return 0;
    }
} // namespace HighLevelSynthesis

int main(int argc, char* argv[])
{
    string cFile = argv[1];
    int latency = stoi(argv[2]);
    string verilogFile = argv[3];
    HighLevelSynthesis::HighLevelSynthesisTool hlsTool;
    hlsTool.run(cFile, latency, verilogFile);
}