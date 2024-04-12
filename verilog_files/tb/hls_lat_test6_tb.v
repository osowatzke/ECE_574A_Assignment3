`timescale 1ns/1ns

module HLSM_tb();
    
    wire Clk, Rst;
    
    parameter LATENCY = 5;         // Delay from Start to DOne

    localparam CLK_PERIOD = 10;     // Clock period in timesteps
    localparam RESET_TIME = 100;    // Reset time in timesteps

    localparam START = 0,
               WAIT = 1;
     
    reg State;
    
    reg Start;
    reg signed [15:0] a, b, c, d, e, f, g;
    
    wire Done, DoneRef;
    wire err, DoneErr, kErr, lErr;
    wire signed [15:0] k, l, kRef, lRef;
    
    
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
        .d(d),
        .e(e),
        .f(f),
        .g(g),
        .k(k),
        .l(l));
        
    HLSM_ref #(.LATENCY(LATENCY)) HLSM_ref_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(DoneRef),
        .a(a),
        .b(b),
        .c(c),
        .d(d),
        .e(e),
        .f(f),
        .g(g),
        .k(kRef),
        .l(lRef));
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            a <= 0;
            b <= 0;
            c <= 0;
            d <= 0;
            e <= 0;
            f <= 0;
            g <= 0;
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
                    d <= $random;
                    e <= $random;
                    f <= $random;
                    g <= $random;
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
    error_monitor #(.DATAWIDTH(16))  error_monitor_1(   k,    kRef, Done,    kErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(16))  error_monitor_2(   l,    lRef, Done,    lErr, Clk, Rst);
    
    assign err = DoneErr | kErr| lErr ;
    
endmodule