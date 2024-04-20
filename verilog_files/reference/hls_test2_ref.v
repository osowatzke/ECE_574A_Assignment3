`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, z, x);

    parameter LATENCY = 9;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, e, f, g, h;
    reg dLTe, dEQe;
    
    always @(posedge Clk) begin
        d <= a + b;
        e <= a + c;
        f <= a - b;  
        dEQe <= d == e;
        dLTe <= d < e;
        g <= dLTe ? d : e; 
        h <= dEQe ? g : f; 
        x <= g << dLTe;
        z <= h >> dEQe;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
