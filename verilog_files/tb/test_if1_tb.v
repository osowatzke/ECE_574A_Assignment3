`timescale 1ns/1ns

module HLSM_tb();
    
    wire Clk, Rst;
    
    parameter LATENCY = 6;         // Delay from Start to DOne

    localparam CLK_PERIOD = 10;     // Clock period in timesteps
    localparam RESET_TIME = 100;    // Reset time in timesteps

    localparam START = 0,
               WAIT = 1;
     
    reg State;
    
    reg Start;
    reg signed [31:0] a, b, c;
    
    wire Done, DoneRef;
    wire err, DoneErr, xErr, zErr;
    wire signed [31:0] z, zRef;
    wire signed [31:0] x, xRef;
    
    clk_gen #(.CLK_PERIOD(CLK_PERIOD)) clk_gen_i(Clk);
    rst_gen #(.RESET_TIME(RESET_TIME)) rst_gen_i(Rst);
    
    HLSM HLSM_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(Done),
        .a(a),
        .b(b),
        .c(c),
        .z(z),
        .x(x));
        
    HLSM_ref #(.LATENCY(LATENCY)) HLSM_ref_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(DoneRef),
        .a(a),
        .b(b),
        .c(c),
        .z(zRef),
        .x(xRef));
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            a <= 0;
            b <= 0;
            c <= 0;
            Start <= 0;
            State <= START;
        end
        else begin
            Start <= 0;
            case (State)
                START : begin
                    a <= $random;
                    b <= $random;
                    c <= $random;
                    Start <= 1;
                    State <= WAIT;
                end
                WAIT : begin
                    if (Done == 1) begin
                        State <= Start;
                    end
                end
            endcase
        end
    end
    
    error_monitor #(.DATAWIDTH(1))  error_monitor_0(Done, DoneRef, 1'b1, DoneErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_1(   z,    zRef, Done,    zErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32)) error_monitor_2(   x,    xRef, Done,    xErr, Clk, Rst);
    
    assign err = DoneErr | zErr | xErr;
    
endmodule