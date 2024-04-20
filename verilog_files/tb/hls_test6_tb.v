`timescale 1ns/1ns

module HLSM_tb();
    
    wire Clk, Rst;
    
    parameter LATENCY = 34;         // Delay from Start to DOne

    localparam CLK_PERIOD = 10;     // Clock period in timesteps
    localparam RESET_TIME = 100;    // Reset time in timesteps

    localparam START = 0,
               WAIT = 1;
     
    reg State;
    
    reg Start;
    reg [31:0] a, b, c, d, e, f, g, h, i, j, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, bb, cc, dd, ee, ff, gg, hh, ii;
    
    wire Done, DoneRef;
    wire err, DoneErr, finalErr;
    wire [31:0] final, finalRef;
    
    
    clk_gen #(.CLK_PERIOD(CLK_PERIOD)) clk_gen_i(Clk);
    rst_gen #(.RESET_TIME(RESET_TIME)) rst_gen_i(Rst);
    
    HLSM HLSM_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(Done),
        .a(a),
        .b(b),
        .c(c),
        .d(d),
        .e(e),
        .f(f),
        .g(g),
        .h(h),
        .i(i),
        .j(j),
        .l(l),
        .m(m),
        .n(n),
        .o(o),
        .p(p),
        .q(q),
        .r(r),
        .s(s),
        .t(t),
        .u(u),
        .v(v),
        .w(w),
        .x(x),
        .y(y),
        .z(z),
        .aa(aa),
        .bb(bb),
        .cc(cc),
        .dd(dd),
        .ee(ee),
        .ff(ff),
        .gg(gg),
        .hh(hh),
        .ii(ii),
        .final(final));
    HLSM_ref #(.LATENCY(LATENCY)) HLSM_ref_i(
        .Clk(Clk),
        .Rst(Rst),
        .Start(Start),
        .Done(DoneRef),
        .a(a),
        .b(b),
        .c(c),
        .d(d),
        .e(e),
        .f(f),
        .g(g),
        .h(h),
        .i(i),
        .j(j),
        .l(l),
        .m(m),
        .n(n),
        .o(o),
        .p(p),
        .q(q),
        .r(r),
        .s(s),
        .t(t),
        .u(u),
        .v(v),
        .w(w),
        .x(x),
        .y(y),
        .z(z),
        .aa(aa),
        .bb(bb),
        .cc(cc),
        .dd(dd),
        .ee(ee),
        .ff(ff),
        .gg(gg),
        .hh(hh),
        .ii(ii),
        .final(finalRef));
    
    always @(posedge Clk) begin
        if (Rst == 1) begin
            a <= 0;
            b <= 0;
            c <= 0;
            d <= 0;
            e <= 0;
            f <= 0;
            g <= 0;
            h <= 0;
            i<= 0;
            j<= 0;
            l<= 0;
            m<= 0;
            n<= 0;
            o<= 0;
            p<= 0;
            q<= 0;
            r<= 0;
            s<= 0;
            t<= 0;
            u<= 0;
            v<= 0; 
            w<= 0;
            x<= 0;
            y<= 0;
            z<= 0;
            aa<= 0;
            bb<= 0;
            cc<= 0;
            dd<= 0;
            ee<= 0;
            ff<= 0;
            gg<= 0;
            hh<= 0;
            ii<= 0;
                        
            Start <= 0;
            State <= START;
        end
        else begin
            Start <= 0;
            case (State)
                START : begin
                    a <= $random;
                    b <= $random;
                    c <= $random;
                    d <= $random;
                    e <= $random;
                    f <= $random;
                    g <= $random;
                    h <= $random;
                    i<= $random;
                    j<= $random;
                    l<= $random;
                    m<= $random;
                    n<= $random;
                    o<= $random;
                    p<= $random;
                    q<= $random;
                    r<= $random;
                    s<= $random;
                    t<= $random;
                    u<= $random;
                    v<= $random; 
                    w<= $random;
                    x<= $random;
                    y<= $random;
                    z<= $random;
                    aa<= $random;
                    bb<= $random;
                    cc<= $random;
                    dd<= $random;
                    ee<= $random;
                    ff<= $random;
                    gg<= $random;
                    hh<= $random;
                    ii<= $random;
                    Start <= 1;
                    State <= WAIT;
                end
                WAIT : begin
                    if (Done == 1) begin
                        State <= Start;
                    end
                end
            endcase
        end
    end
    
    error_monitor #(.DATAWIDTH(1))  error_monitor_0(Done, DoneRef, 1'b1, DoneErr, Clk, Rst);
    error_monitor #(.DATAWIDTH(32))  error_monitor_1(   final,    finalRef, Done,    finalErr, Clk, Rst);
    
    assign err = DoneErr | finalErr ;
    
endmodule