// This module implements an error module which compares measured data to reference
// data when the reset signal is deasserted and the valid is asserted. If there is
// an error, the errOut output will be asserted and stay high and a message will be
// printed to the console. This simplifies error checking as it only requires the user
// to see if the errOut signal goes high. Because this module only validates behavior
// when reset is deasserted it is not meant to validate modules with specific reset
// behavior such as the REG module.

`timescale 1ns/1ns

module error_monitor(measIn,refIn,validIn,errOut,clkIn,rstIn);

    parameter DATAWIDTH = 32;

    input [DATAWIDTH-1:0] measIn;
    input [DATAWIDTH-1:0] refIn;

    output reg errOut;

    input validIn;
    input clkIn;
    input rstIn;

    always @(posedge clkIn) begin
        if (rstIn == 1) begin
            errOut <= 0;
        end
        else begin
            if (validIn == 1) begin
                if (measIn !== refIn) begin
                    errOut <= 1;
                    $error("Error Detected at Time %t: Meas = %d, Ref=%d", $realtime, measIn, refIn);
                end
            end
        end
    end
endmodule