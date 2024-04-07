`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, a, b, c, d, e, f, g, j, l);

    parameter LATENCY = 5;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [15:0] a, b, c, d, e, f, g;
    output reg signed [15:0] j, l;
    
    reg signed [15:0] h, i, k;
    
    always @(posedge Clk) begin
        h <= a + b;
        i <= h + c;
        j <= i + d; 
        k <= e * f;
        l <= k * g;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule