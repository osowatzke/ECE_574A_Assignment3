`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, i);

    parameter LATENCY = 4;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [15:0] a, b, c, d, e;
    output reg signed [15:0] i;
    
    reg signed [15:0] f, g, h;
    
    always @(posedge Clk) begin
        f <= a + b;
        g <= f + c;
        h <= g + d;
        i <= h + e;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
