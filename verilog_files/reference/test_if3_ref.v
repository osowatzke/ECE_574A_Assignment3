`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, one, two, four, avg);

    parameter LATENCY = 13;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, d, e, f, g, h, sa, one, two, four;
    output reg signed [31:0] avg;
    
    reg signed [31:0] t1, t2, t3, t4, t5, t6, t7, t7div2, t7div4;
    reg signed csa1, csa2, csa3;
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            t1 <= 0;
            t2 <= 0;
            t3 <= 0;
            t4 <= 0;
            t5 <= 0;
            t6 <= 0;
            t7 <= 0;
            t7div2 <= 0;
            t7div4 <= 0;
            csa1 <= 0;
            csa2 <= 0;
            csa3 <= 0;
            avg <= 0;
        end
        begin
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
            if ( csa1 ) begin
                t7div2 <= t7 >> one;
                if ( csa2 ) begin
                    t7div4 <= t7div2 >> one;
                end
                else begin
                    t7div4 <= t7 >> one;
                    if ( csa3 ) begin
                        avg <= t7div4 >> one;
                    end
                end
            end
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
