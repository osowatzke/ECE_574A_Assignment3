`timescale 1ns/1ns

module HLSM_tb();
    
    wire Clk, Rst;
    
    parameter LATENCY = 12;         // Delay from Start to DOne

    localparam CLK_PERIOD = 10;     // Clock period in timesteps
    localparam RESET_TIME = 100;    // Reset time in timesteps

    localparam START = 0,
               WAIT = 1;
     
    reg State;
    
    reg Start;
    reg signed [31:0] u, x, y, dx, a, three;
    
    wire Done, DoneRef;
    wire err, DoneErr, u1Err, x1Err, y1Err, cErr ;
    wire signed [31:0] u1, x1, y1, c, u1Ref, x1Ref, y1Ref, cRef;
    
    
    clk_gen #(.CLK_PERIOD(CLK_PERIOD)) clk_gen_i(Clk);
    rst_gen #(.RESET_TIME(RESET_TIME)) rst_gen_i(Rst);
    
    HLSM HLSM_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(Done),
        .u(u),
        .x(x),
        .y(y),
        .dx(dx),
        .a(a),
        .three(three),
        .u1(u1),
        .x1(x1),
        .y1(y1),
        .c(c));
        
    HLSM_ref #(.LATENCY(LATENCY)) HLSM_ref_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(DoneRef),
        .u(u),
        .x(x),
        .y(y),
        .dx(dx),
        .a(a),
        .three(three),
        .u1(u1Ref),
        .x1(x1Ref),
        .y1(y1Ref),
        .c(cRef));
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            u <= 0;
            x <= 0;
            y <= 0;
            dx <= 0;
            a <= 0;
            three <= 0;
            Start <= 0;
            State <= START;
        end
        else begin
            Start <= 0;
            case (State)
                START : begin
                    u <= $random;
                    x <= $random;
                    y <= $random;
                    dx <= $random;
                    a <= $random;
                    three <= $random;
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
    error_monitor #(.DATAWIDTH(32))  error_monitor_1(   u1,    u1Ref, Done,    u1Err, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_2(   x1,    x1Ref, Done,    x1Err, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_3(   y1,    y1Ref, Done,    y1Err, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_4(   c,    cRef, Done,    cErr, Clk, Rst);
    
    assign err = DoneErr | u1Err | x1Err | y1Err | cErr ;
    
endmodule