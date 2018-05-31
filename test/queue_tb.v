`timescale 1ns/1ns
`include "queue.v"

`define assert(condition) if (!(condition)) begin $display("assertion FAILED!"); $finish_and_return(1); end

module testbench();

    reg       clk = 0;
    reg       rst = 0;
    reg[3:0]  din = 0;
    reg       enq = 0;
    reg       deq = 0;
    wire[3:0] dout;
    wire      enq_ready;
    wire      deq_valid;

    QueueWrapper queue(clk, rst, enq, din, deq, enq_ready, deq_valid, dout);

    always begin
        #1 clk = !clk;
    end

    initial begin
        $dumpfile("testbench.vcd");
        $dumpvars(0, testbench);

        $display ("time\tclk\trst\tdin\tenq\tdeq\tdout\tenq_rdy\tdeq_val");
        $monitor("%3d\t%b\t%b\t%h\t%b\t%b\t%h\t%b\t%b", $time, clk, rst, din, enq, deq, dout, enq_ready, deq_valid);

        #0 rst = 1;
        #1 rst = 1;
        #1 rst = 0;

        #0 din = 1;
           enq = 1;
           `assert(deq_valid == 0);
           `assert(enq_ready == 1);

        #2 din = 2;
           enq = 1;
           `assert(deq_valid == 1);
           `assert(enq_ready == 1);
           `assert(dout == 1);

        #2 din = 0;
           enq = 0;
           `assert(deq_valid == 1);
           `assert(enq_ready == 0);
           `assert(dout == 1);

        #2 deq = 1;
          `assert(deq_valid == 1);
          `assert(enq_ready == 0);
          `assert(dout == 1);

        #2 deq = 1;
           `assert(deq_valid == 1);
           `assert(enq_ready == 1);
           `assert(dout == 2);

        #2 deq = 0;
           `assert(deq_valid == 0);
           `assert(enq_ready == 1);
           `assert(dout == 1);

        #2 $finish;
    end

endmodule
