`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, zero, one, t, z, x);

    parameter LATENCY = 6;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, zero, one;
    input signed t;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] e, g, d, f;
    
    localparam State0   = 0,
               State1_0 = 1,
               State1_1 = 2,
               State2_0 = 3,
               State2_1 = 4,
               State3_0 = 5,
               State3_1 = 6;
               
    reg [2:0] State;
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            e <= 0;
            g <= 0;
            d <= 0;
            f <= 0;
            z <= 0;
            x <= 0;
            State <= State0;
        end
        begin
            case (State)
                State0 : begin
                    if (Start == 1) begin
                        if ( t ) begin
                            State <= State1_0;
                        end
                        else begin
                            State <= State1_1;
                        end
                    end
                end
                State1_0 : begin
                    d <= a - one;
                    f <= a + c;
                    State <= State2_0;
                end
                State1_1 : begin
                    d <= a + b;
                    e <= a + c;
                    f <= a * c;
                    State <= State2_1;
                end
                State2_0 : begin
                    x <= f - d;
                    State <= State3_0;
                end
                State2_1 : begin
                    x <= f - d;
                    g <= d > e;
                    State <= State3_1;
                end
                State3_0 : begin
                    State <= State0;
                end
                State3_1 : begin
                    z <= g ? d : e;
                    State <= State0;
                end
            endcase
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule