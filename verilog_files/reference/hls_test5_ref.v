`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, avg);

    parameter LATENCY = 10;
    
    input Clk, Rst, Start;
    output Done;
    
    input [31:0] a, b, c, d, e, f, g, h, sa;
    output reg [31:0] avg;
    
    reg [31:0] t1, t2, t3, t4, t5, t6, t7, t7div2, t7div4;
    
    always @(posedge Clk) begin
        t1 <= a + b;
        t2 <= t1 + c; 
        t3 <= t2 + d;
        t4 <= t3 + e;
        t5 <= t4 + f;
        t6 <= t5 + g;
        t7 <= t6 + h;
        t7div2 <= t7 >> sa;
        t7div4 <= t7div2 >> sa;
        avg <= t7div4 >> sa;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
