`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, one, two, four, avg);

    parameter LATENCY = 14;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, d, e, f, g, h, sa, one, two, four;
    output reg signed [31:0] avg;
    
    reg signed [31:0] t1, t2, t3, t4, t5, t6, t7, t7div2, t7div4;
    reg signed csa1, csa2, csa3;
    
    always @(posedge Clk) begin
        t1 <= a + b;
        t2 <= t1 + c; 
        t3 <= t2 + d; 
        t4 <= t3 + e; 
        t5 <= t4 + f; 
        t6 <= t5 + g; 
        t7 <= t6 + h; 
        csa1 <= sa > one;
        csa2 <= sa > two;
        csa3 <= sa > four;

        if ( csa1 ) {
        t7div2 <= t7 >> one;
   
        if ( csa2 ) {
        t7div4 <= t7div2 >> one;
        }
        else {
        t7div4 <= t7 >> one;
        if ( csa3 ) {
        avg <= t7div4 >> one;
        }
        }
        }
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule