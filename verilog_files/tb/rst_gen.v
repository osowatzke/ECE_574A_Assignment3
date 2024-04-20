`timescale 1ns/1ns

module rst_gen(rst);

    parameter RESET_TIME = 100;
    output reg rst;

    initial begin
        rst             <= 1;
        #RESET_TIME rst <= 0;
    end

endmodule