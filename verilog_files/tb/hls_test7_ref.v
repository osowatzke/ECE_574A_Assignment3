`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, u, x, y, dx, a, three, u1, x1, y1, c);

    parameter LATENCY = 12;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] u, x, y, dx, a, three;
    output reg signed [31:0] u1, x1, y1, c;
    
    reg signed [31:0] t1, t2, t3, t4, t5, t6, t7, vx1;
    
    always @(posedge Clk) begin
        x1 <= x + dx;
        vx1 <= x + dx;
        t1 <= three * x;
        t2 <= u * dx;
        t3 <= t1 * t2;
        t4 <= u - t3;
        t5 <= three * y;
        t6 <= t5 * dx;
        u1 <= t4 - t6;
        t7 <= u * dx;
        y1 <= y + t7;
        c <= vx1 < a;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule