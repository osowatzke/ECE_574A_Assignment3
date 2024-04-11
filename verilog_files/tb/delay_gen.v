`timescale 1ns/1ns

module delay_gen(qOut, xIn, clk, rst);

    parameter DELAY = 1;
    
    input xIn;
    input clk;
    input rst;

    output qOut;
    
    generate
        if (DELAY == 0) begin
            assign qOut = xIn;
        end
        if (DELAY > 0) begin
            reg [(DELAY-1):0] xDlyR;
            always @(posedge clk) begin
                if (rst == 1'b1)
                    xDlyR <= 0;
                else
                    if (DELAY == 1)
                        xDlyR[0] <= xIn;
                    else
                        xDlyR <= {xDlyR[(DELAY-2):0], xIn};
            end
            assign qOut = xDlyR[DELAY-1];
        end
    endgenerate

endmodule