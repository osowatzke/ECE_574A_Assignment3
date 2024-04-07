`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, j, k);

    parameter LATENCY = 4;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [15:0] a, b, c, d, e, f;
    output reg signed [15:0] j, k;
    
    reg signed [15:0] h, i;
    
    always @(posedge Clk) begin
        h <= a * b;
        i <= h + c;
        j <= i * d; 
        k <= e * f;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
