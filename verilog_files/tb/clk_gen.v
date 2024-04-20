`timescale 1ns/1ns

module clk_gen(clk);

    parameter CLK_PERIOD = 10;
    output reg clk;

    initial
        clk <= 0;

    always
        #(CLK_PERIOD/2) clk <= ~clk;

endmodule