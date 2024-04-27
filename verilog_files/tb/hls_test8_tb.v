`timescale 1ns/1ns

module HLSM_tb();
    
    wire Clk, Rst;
    
    parameter LATENCY = 13;         // Delay from Start to DOne

    localparam CLK_PERIOD = 10;     // Clock period in timesteps
    localparam RESET_TIME = 100;    // Reset time in timesteps

    localparam START = 0,
               WAIT = 1;
     
    reg State;
    
    reg Start;
    reg signed [31:0] x0, x1, x2, x3, y0, c0, five, ten;
    
    wire Done, DoneRef;
    wire err, DoneErr, u1Err, d1Err, d2Err, eErr, fErr, gErr, hErr ;
    wire signed [31:0] d1, d2, e, f, g, h, d1Ref, d2Ref, eRef, fRef, gRef, hRef;
    
    
    clk_gen #(.CLK_PERIOD(CLK_PERIOD)) clk_gen_i(Clk);
    rst_gen #(.RESET_TIME(RESET_TIME)) rst_gen_i(Rst);
    
    HLSM HLSM_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(Done),
        .x0(x0),
        .x1(x1),
        .x2(x2),
        .x3(x3),
        .y0(y0),
        .c0(c0),
        .five(five),
        .ten(ten),
        .d1(d1),
        .d2(d2),
        .e(e),
        .f(f),
        .g(g),
        .h(h)); 
        
    HLSM_ref #(.LATENCY(LATENCY)) HLSM_ref_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(DoneRef),
        .x0(x0),
        .x1(x1),
        .x2(x2),
        .x3(x3),
        .y0(y0),
        .c0(c0),
        .five(five),
        .ten(ten), 
        .d1(d1Ref),
        .d2(d2Ref),
        .e(eRef),
        .f(fRef),
        .g(gRef),
        .h(hRef));
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            x0 <= 0;
            x1 <= 0;
            x2 <= 0;
            x3 <= 0;
            y0 <= 0;
            c0 <= 0;
            five <= 0;
            ten <= 0;
            Start <= 0;
            State <= START;
        end
        else begin
            Start <= 0;
            case (State)
                START : begin
                    x0 <= $random;
                    x1 <= $random;
                    x2 <= $random;
                    x3 <= $random;
                    y0 <= $random;
                    c0 <= $random;
                    five <= $random;
                    ten <= $random;
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
    error_monitor #(.DATAWIDTH(32))  error_monitor_1(   d1,   d1Ref, Done,   d1Err, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_2(   d2,   d2Ref, Done,   d2Err, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_3(   e,    eRef, Done,    eErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_4(   f,    fRef, Done,    fErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_5(   g,    gRef, Done,    gErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_6(   h,    hRef, Done,    hErr, Clk, Rst);
    
    assign err = DoneErr | d1Err | d2Err | eErr | fErr | gErr | hErr ;
    
endmodule