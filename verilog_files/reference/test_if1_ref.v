`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, f, g, zrin;
    
    always @(posedge Clk) begin
       d <= a + b;
       g <= a < b;
       zrin <= a + c;
       if ( g ) {
	   zrin <= a + b;
       }
       f <= a * c;
       x <= f - d; 
       z <= zrin + f;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule