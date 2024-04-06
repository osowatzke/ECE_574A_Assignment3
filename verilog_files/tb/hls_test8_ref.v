`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, x0, x1, x2, x3, y0, c0, five, ten, d1, d2, e, f, g, h);

    parameter LATENCY = 13;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] x0, x1, x2, x3, y0, c0, five, ten;
    output reg signed [31:0] d1, d2, e, f, g, h;
    
    reg signed [31:0] t1, t2, t3, vd1, ve, vf, vg;
    
    always @(posedge Clk) begin
        t1 <= x0 + x1;
        d1 <= t1 * c0;
        vd1 <= t1 * c0;
        d2 <= vd1 * five;
        t2 <= x1 + x2;
        t3 <= x3 + c0;
        e <= t2 * t3;
        ve <= t2 * t3;
        f <= ve * y0;
        vf <= ve * y0;
        g <= x0 - ten;
        vg <= x0 - ten;
        h <= vf + vg;
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule