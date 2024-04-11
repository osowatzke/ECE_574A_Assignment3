`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, one, z, x);

    parameter LATENCY = 10;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, one;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, e, f, g, h;
    reg signed dLTe, dEQe, dLTEe;
    
    always @(posedge Clk) begin
        d <= a + b;
        e <= a + c;
        f <= a - b; 
        dEQe <= d == e;
        dLTe <= d > e;
        dLTEe <= dEQe + dLTe;
        if ( dLTEe ) {
	    if ( dLTe ) {
		g <= e + one;
		h <= f + one;
	    }
	    g <= d + e;
	    h <= f + e;
        }
        x <= h << one;
        z <= h >> one;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule