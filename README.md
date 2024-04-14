# Compiling the Code

From the root directory of the repo, create a build directory with the following command:

`mkdir build`

Then, navigate into the build directory with the following command:

`cd build`

Next, configure CMAKE.

For Visual Studio Code on Windows, type CTRL+SHIFT+P and select "CMake: Configure".

Finally, compile the code with the following commands:

`cmake ..`

and
 
`cmake --build .`

# Running the code

From the newly created build directory, run the following command

`.\src\hlsyn.exe <path_to_in_file> <latency> <path_to_out_file>`

where <path_to_in_file>, <latency>, and <path_to_out_file> are input arguments.

For example,

`.\src\hlsyn.exe ..\testfiles\standard_tests\hls_test1.c 16 ..\verilog_files\autogen\hls_test1.v`

Note that for the above example to work, you must create an autogen folder.

This can be done from the build directory using the following command:

`mkdir ..\verilog_files\autogen`
 
# Testing the code

Open Vivado and navigate to the repo root directory. Then, run

`source ./scripts/run_test.tcl`

followed by:

`run_test <testname> <latency>`

where <testname> and <latency> are input arguments.

For example:

`run_test hls_test1 10

**Note that <testname> is the name of the test only (Ex: "hls_test1" not "hls_test1.v" or "hls_test1.c").**

**Note that this scripts also expects you to save the verilog files to the autogen folder. It will not work if you save the file elsewhere**

# Test Automation Scripts

All of the above steps have been automated in a single script. This enables the code to be rapidly validated.

To run the automation script, navigate to the repo root directory.

Then, run the following command from the terminal:

`python .\scripts\validate_code.py`
