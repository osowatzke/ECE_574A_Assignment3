`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, f, g, zrin;
    
    localparam State0   = 0,
               State1   = 1,
               State2_0 = 2,
               State2_1 = 3,
               State3   = 4;
            
    reg [2:0] State;
    
    always @(posedge Clk) begin
        if ( Rst ) begin
            d <= 0;
            f <= 0;
            g <= 0;
            zrin <= 0;
            z <= 0;
            x <= 0;
            State <= State0;
        end
        else begin
            case ( State )
                State0 : begin
                    d <= a + b;
                    g <= a < b;
                    f <= a * c;
                    if ( Start ) begin
                        State <= State1;
                    end
                end
                State1 : begin
                    x <= f - d; 
                    if ( g ) begin
                        State <= State2_0;
                    end
                    else begin
                        State <= State2_1;
                    end
                end
                State2_0 : begin
                    zrin <= a + b;
                    State <= State3;
                end
                State2_1 : begin
                    zrin <= a + c;
                    State <= State3;
                end
                State3 : begin
                    z <= zrin + f;
                    State <= State0;
                end
            endcase;
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule