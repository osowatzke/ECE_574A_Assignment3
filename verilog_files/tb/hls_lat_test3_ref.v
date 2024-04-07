`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, i, j, k, l);

    parameter LATENCY = 4;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [15:0] a, b, c, d, e, f, g, h;
    output reg signed [15:0] i, j, k, l;
    
    always @(posedge Clk) begin
        i <= a * b;
        j <= c * d;
        k <= e * f;
        l <= g * h;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule