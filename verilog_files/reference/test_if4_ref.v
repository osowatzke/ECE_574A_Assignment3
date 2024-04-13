`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, zero, one, t, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, zero, one;
    input signed t;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] e, g, d, f;
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            e <= 0;
            g <= 0;
            d <= 0;
            f <= 0;
            z <= 0;
            x <= 0;
        end
        begin
            d <= a + b;
            if ( t ) begin
                d <= a - one;
                f <= a + c;
                x <= f - d;
            end
            else begin
                e <= a + c;
                g <= d > e;
                z <= g ? d : e;
                f <= a * c;
                x <= f - d; 
            end
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule