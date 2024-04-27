1) Names and NetIDs of Group Members:
   Owen Sowatzke           - osowatzke
   Matthew Nolan           - mdnolan
   Kazi Safkat             - safkat
   Muhtasim Alam Chowdhury - mmc7

2) Course Enrolled:
   Owen Sowatzke           - ECE574A
   Matthew Nolan           - ECE474A
   Kazi Safkat             - ECE574A
   Muhtasim Alam Chowdhury - ECE574A

3) Brief Description of the Program:
   Our program produces a Verilog-based high-level state machine (HLSM) from C-like behavioral code. All the sequential
   statements in the resulting file are scheduled using the Force Directed Scheduling (FDS) algorithm. Our program is
   divided into four parts: 1) File Parser, 2) FDS Scheduler, 3) FSM Generator, and 4) File Writer. The File Parser
   validates the input behavioral file and creates a list of nets (inputs, outputs, and variables) and a graph composed
   of edges and vertices. The FDS Scheduler, then, schedules each vertex in the resulting graph. The FDS implementation
   extends the concepts covered in class to multi-cycle and conditional operations. These implementations are consistent
   with the algorithm first presented by Paulin and Knight in 1989. Next, in the FSM Generator each time step is divided
   into a series of mutually exclusive states. These states are captured by structures, which contain the vertices
   scheduled in that state. These states are then connected together with transitional logic to represent the HLSM.
   Finally, the File Writer converts the parsed nets and state structure into a HLSM, which is written to a synthesizeable
   verilog file.

4) Individual Contributions of Group Members:
   All group members collaborated in implementing, testing, and debugging the program. Below are the individual
   contributions of each group member:
    - Owen Sowatzke
        - Contributed to the File Parser, the FSM Generator, and the File Writer
	- Contributed to the overall debugging of the program

    - Matthew Nolan
        - Contributed to the FDS Scheduler
	- Contributed to the overall debugging of the program

    - Kazi Safkat
        - Created Verilog reference files for standard, latency, and if tests 
        - Contributed to the overall debugging of the program

    - Muhtasim Alam Chowdhury
        - Created Verilog testbenches for standard, latency, and if tests
        - Contributed to the overall debugging of the program