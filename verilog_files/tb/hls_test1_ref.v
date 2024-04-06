`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, a, b, c, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input [15:0] a, b, c;
    output reg [7:0] z;
    output reg [15:0] x;
    
    reg [7:0] d, e, f, g;
    
    always @(posedge Clk) begin
        d <= a + b;
        e <= a + c;
        g <= d > e;
        z <= g ? d : e;
        f <= a * c;
        x <= f - d;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
