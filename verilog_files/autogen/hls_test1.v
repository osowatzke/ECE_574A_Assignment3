`timescale 1ns/1ns

module HLSM(Clk, Rst, Start, Done, a, b, c, x, z);
    input Clk, Rst, Start;
    output reg Done;

    input [15:0] a;
    input [15:0] b;
    input [15:0] c;

    output reg [15:0] x;
    output reg [7:0] z;

    reg [7:0] d;
    reg [7:0] e;
    reg [7:0] f;
    reg [7:0] g;

    localparam Wait = 0,
               State0_0 = 1,
               State1_0 = 2,
               State2_0 = 3,
               State3_0 = 4,
               State4_0 = 5,
               Final = 6;

    reg [2:0] State;

    always @(posedge Clk) begin
        if (Rst == 1) begin
            d <= 0;
            e <= 0;
            f <= 0;
            g <= 0;
            x <= 0;
            z <= 0;
            Done <= 0;
            State <= Wait;
        end
        else begin
            Done <= 0;
            case (State)
                Wait : begin
                    if (Start == 1) begin
                        State <= State0_0;
                    end
                end
                State0_0 : begin
                    State <= State1_0;
                end
                State1_0 : begin
                    State <= State2_0;
                end
                State2_0 : begin
                    d <= a + b;
                    e <= a + c;
                    f <= a * c;
                    State <= State3_0;
                end
                State3_0 : begin
                    g <= d > e;
                    State <= State4_0;
                end
                State4_0 : begin
                    z <= g ? d : e;
                    x <= f - d;
                    State <= Final;
                    Done <= 1;
                end
                Final : begin
                    State <= Wait;
                end
            endcase
        end
    end

endmodule
