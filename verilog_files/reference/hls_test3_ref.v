`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, num, avg);

    parameter LATENCY = 8;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [7:0] a, b, c, d, e, f, g, h, num;
    output reg signed [7:0] avg;
    
    reg signed [31:0] t1, t2, t3, t4, t5, t6, t7;
    
    always @(posedge Clk) begin
        t1 <= a + b;
        t2 <= t1 + c;
        t3 <= t2 + d; 
        t4 <= t3 + e;
        t5 <= t4 + f;
        t6 <= t5 + g;
        t7 <= t6 + h;
        avg <= t7 / num;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
