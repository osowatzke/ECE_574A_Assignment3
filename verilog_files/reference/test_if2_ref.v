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
        if (Rst == 1) begin
            d <= 0;
            e <= 0;
            f <= 0;
            g <= 0;
            h <= 0;
            dLTe <= 0;
            dEQe <= 0;
            dLTEe <= 0;
            z <= 0;
            x <= 0;
        end
        else
            d <= a + b;
            e <= a + c;
            f <= a - b; 
            dEQe <= d == e;
            dLTe <= d > e;
            dLTEe <= dEQe + dLTe;
            if ( dLTEe ) begin
                if ( dLTe ) begin
                    g <= e + one;
                    h <= f + one;
                end
                g <= d + e;
                h <= f + e;
            end
            x <= h << one;
            z <= h >> one;
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule