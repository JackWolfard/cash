`timescale 1ns/1ns
`include "gcd.v"

`define assert(condition) if (!(condition)) begin $display("assertion FAILED!"); $finish_and_return(1); end

module testbench();

    reg        clk   = 0;
    reg        reset = 0;
    reg        in_valid;
    reg[31:0]  in_data;
    wire       in_ready;
    wire       out_valid;
    wire[15:0] out_data;

    GCD gcd(clk, reset, in_valid, in_data, in_ready, out_valid, out_data);

    always begin
        #1 clk = !clk;
    end

    initial begin
        $dumpfile("testbench.vcd");
        $dumpvars(0, testbench);

        $display ("time\tclk\treset\tout");
        $monitor("%3d\t%b\t%b\t%b\t%h\t%b\t%b\t%h", $time, clk, reset, in_valid, in_data, in_ready, out_valid, out_data);

        #0 reset  = 1;
        #1 reset  = 1;
        #1 reset  = 0;

        #0 in_data  = 32'h0030_0020;
           in_valid = 1;

        #11 `assert(out_valid == 1);
            `assert(out_data == 16);
            $finish;
    end

endmodule
