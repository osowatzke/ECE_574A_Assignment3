`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, k, l);

    parameter LATENCY = 5;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [15:0] a, b, c, d, e, f, g;
    output reg signed [15:0] k, l;
    
    reg signed [15:0] h, i, j;
    
    always @(posedge Clk) begin
        h <= a * b;
        i <= c * d;
        j <= h + i;
        k <= j / e;
        l <= f / g;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
