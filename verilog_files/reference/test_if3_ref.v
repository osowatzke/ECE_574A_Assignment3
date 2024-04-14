`timescale 1ns/1ns

module HLSM_ref(Clk, Rst, Start, Done, a, b, c, d, e, f, g, h, sa, one, two, four, avg);

    parameter LATENCY = 13;
    
    input Clk, Rst, Start;
    output Done;
    
    input signed [31:0] a, b, c, d, e, f, g, h, sa, one, two, four;
    output reg signed [31:0] avg;
    
    reg signed [31:0] t1, t2, t3, t4, t5, t6, t7, t7div2, t7div4;
    reg signed csa1, csa2, csa3;
    
    localparam State0   = 0,
               State1   = 1,
               State2   = 2,
               State3   = 3,
               State4   = 4,
               State5   = 5,
               State6   = 6,
               State7_0 = 7,
               State7_1 = 8,
               State8_0 = 9,
               State8_1 = 10,
               State8_2 = 11,
               State9_0 = 12,
               State9_1 = 13,
               State9_2 = 14,
               State9_3 = 15;
               
    reg [3:0] State;
               
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
            State <= State0;
        end
        begin
            case (State)
                State0 : begin
                    t1 <= a + b;
                    csa1 <= sa > one;
                    csa2 <= sa > two;
                    csa3 <= sa > four;
                    if (Start == 1) begin
                        State <= State1;
                    end
                end
                State1 : begin
                    t2 <= t1 + c;
                    State <= State2;
                end
                State2 : begin
                    t3 <= t2 + d;
                    State <= State3;
                end
                State3 : begin
                    t4 <= t3 + e;
                    State <= State4;
                end
                State4 : begin
                    t5 <= t4 + f;
                    State <= State5;
                end
                State5 : begin
                    t6 <= t5 + g;
                    State <= State6;
                end
                State6 : begin
                    t7 <= t6 + h;
                    if ( csa1 ) begin
                        State <= State7_0;
                    end
                    else begin
                        State <= State7_1;
                    end
                end
                State7_0 : begin
                    t7div2 <= t7 >> one;
                    if ( csa2 ) begin
                        State <= State8_0;
                    end
                    else begin
                        State <= State8_1;
                    end
                end
                State7_1 : begin
                    State <= State8_2;
                end
                State8_0 : begin
                    t7div4 <= t7div2 >> one;
                    State <= State9_0;
                end
                State8_1 : begin
                    t7div4 <= t7 >> one;
                    if ( csa3 ) begin
                        State <= State9_1;
                    end
                    else begin
                        State <= State9_2;
                    end
                end
                State8_2 : begin
                    State <= State9_3;
                end
                State9_0 : begin
                    State <= State0;
                end
                State9_1 : begin
                    avg <= t7div4 >> one;
                    State <= State0;
                end
                State9_2 : begin
                    State <= State0;
                end
                State9_3 : begin
                    State <= State0;
                end
            endcase
        end
    end
    
    delay_gen #(.DELAY(LATENCY)) delay_i(.qOut(Done), .xIn(Start), .clk(Clk), .rst(Rst));
    
endmodule
