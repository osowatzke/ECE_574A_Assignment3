`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, f, g, zrin;
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            d <= 0;
            f <= 0;
            g <= 0;
            zrin <= 0;
            z <= 0;
            x <= 0;
        end 
        else begin
            d <= a + b;
            g <= a < b;
            zrin <= a + c;
            if ( g ) begin
                zrin <= a + b;
            end
            f <= a * c;
            x <= f - d; 
            z <= zrin + f;
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule