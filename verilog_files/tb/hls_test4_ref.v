`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, a, b, c, d, zero, z);

    parameter LATENCY = 5;
    
    input Clk, Rst, Start;
    output Done;
    
    input [63:0] a, b, c, d, zero;
    output reg [63:0] z;
    
    reg [63:0] e, f, g;
    reg gEQz;
    
    always @(posedge Clk) begin
        e <= a / b;
        f <= c / d;
        g <= a % b;  
        gEQz <= g == zero;
        z <= gEQz ? e : f; 
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule