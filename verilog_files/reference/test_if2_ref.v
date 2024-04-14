`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, one, z, x);

    parameter LATENCY = 10;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, one;
    output reg signed [31:0] z, x;
    
    reg signed [31:0] d, e, f, g, h;
    reg signed dLTe, dEQe, dLTEe;
    
    localparam State0 = 0,
               State1 = 1,
               State2 = 2,
               State3 = 3,
               State4_0 = 4,
               State4_1 = 5,
               State5 = 6;
               
    reg [2:0] State;
    
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
            State <= State0;
        end
        else begin
            case (State)
                State0 : begin
                    d <= a + b;
                    e <= a + c;
                    f <= a - b;
                    if (Start == 1) begin
                        State <= State1;
                    end
                end
                State1 : begin
                    dEQe <= d == e;
                    dLTe <= d > e;
                    State <= State2;
                end
                State2 : begin
                    dLTEe <= dEQe + dLTe;
                    State <= State3;
                end
                State3 : begin
                    if (dLTEe) begin
                        State <= State4_0;
                    end
                    else begin
                        State <= State4_1;
                    end
                end
                State4_0 : begin
                    g <= d + e;
                    h <= f + e;
                    State <= State5;
                end
                State4_1 : begin
                    State <= State5;
                end
                State5 : begin
                    x <= h << one;
                    z <= h >> one;
                    State <= State0;
                end
            endcase
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule