`timescale 1ns / 1ns

module hls_test1_ref_tb;

    // Clock and Reset integration
    wire Clk, Rst;
    clk_gen clk_inst(.clk(Clk));
    rst_gen rst_inst(.rst(Rst));

    parameter START = 0, WAIT = 1;
    reg State;

    reg Start;
    reg [15:0] a, b, c;
    wire Done;
    wire [7:0] z; // Output from the HLS module
    wire [15:0] x; // Output from the HLS module

    // Expected output values
    reg [7:0] expected_z;
    reg [15:0] expected_x;

    // Instantiating the HLS module
    hls_test1_ref HLSM_0(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(Done),
        .a(a),
        .b(b),
        .c(c),
        .z(z),
        .x(x)
    );

    initial begin
        State = START;
        Start = 0;
        // Initialize test input values
        a = 16'd10; // Example values
        b = 16'd20;
        c = 16'd30;
    end

    always @(posedge Clk) begin
        if (Rst) begin
            Start <= 0;
            State <= START;
        end else begin
            case (State)
                START: begin
                    Start <= 1;
                    State <= WAIT;

                    // Compute expected outputs based on the input values
                    expected_z <= (a + b > a + c) ? (a + b)[7:0] : (a + c)[7:0];
                    expected_x <= (a * c) - (a + b);
                end
                WAIT: begin
                    if (Done) begin
                        // Check output against expected values
                        assert(z == expected_z) else $error("Mismatch on z: expected %h, got %h", expected_z, z);
                        assert(x == expected_x) else $error("Mismatch on x: expected %h, got %h", expected_x, x);
                    end
                end
            endcase
        end
    end

    // Instantiate error monitoring for additional validation if needed
    error_monitor #(8) err_monitor_z (
        .expected(expected_z),
        .actual(z),
        .errOut() // Connect to an error indicator or processing logic as needed
    );

    serror_monitor #(16) err_monitor_x (
        .expected(expected_x),
        .actual(x),
        .errOut() // Connect to an error indicator or processing logic as needed
    );

endmodule
