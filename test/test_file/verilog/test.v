// Verilog test file for tree-sitter symbol extraction
// Should extract: module, function, task, generate block, parameter

`timescale 1ns / 1ps

// ---- Module ----
module counter #(
    parameter WIDTH = 8,
    parameter INIT_VALUE = 0
) (
    input  wire             clk,
    input  wire             rst_n,
    input  wire             enable,
    output reg  [WIDTH-1:0] count
);
    // Internal signals
    wire overflow;
    
    // Always block
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            count <= INIT_VALUE;
        else if (enable)
            count <= count + 1;
    end
    
    assign overflow = &count;
    
endmodule

// ---- Module with multiple ports ----
module alu #(
    parameter DATA_WIDTH = 32
) (
    input  wire [DATA_WIDTH-1:0] a,
    input  wire [DATA_WIDTH-1:0] b,
    input  wire [2:0]            opcode,
    output reg  [DATA_WIDTH-1:0] result,
    output reg                   zero,
    output reg                   overflow
);
    
    always @(*) begin
        case (opcode)
            3'b000: result = a + b;
            3'b001: result = a - b;
            3'b010: result = a & b;
            3'b011: result = a | b;
            3'b100: result = a ^ b;
            default: result = 0;
        endcase
        zero = (result == 0);
    end
    
endmodule

// ---- Generate block ----
module register_file #(
    parameter NUM_REGS = 32,
    parameter DATA_WIDTH = 32
) (
    input  wire                    clk,
    input  wire [4:0]              read_addr1,
    input  wire [4:0]              read_addr2,
    input  wire [4:0]              write_addr,
    input  wire [DATA_WIDTH-1:0]   write_data,
    input  wire                    write_en,
    output wire [DATA_WIDTH-1:0]   read_data1,
    output wire [DATA_WIDTH-1:0]   read_data2
);
    
    reg [DATA_WIDTH-1:0] registers [0:NUM_REGS-1];
    
    assign read_data1 = registers[read_addr1];
    assign read_data2 = registers[read_addr2];
    
    always @(posedge clk) begin
        if (write_en && write_addr != 0)
            registers[write_addr] <= write_data;
    end
    
endmodule

// ---- Testbench module ----
module tb_counter;
    reg  clk;
    reg  rst_n;
    reg  enable;
    wire [7:0] count;
    
    counter #(.WIDTH(8), .INIT_VALUE(0)) uut (
        .clk(clk),
        .rst_n(rst_n),
        .enable(enable),
        .count(count)
    );
    
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end
    
    initial begin
        rst_n = 0;
        enable = 0;
        #20 rst_n = 1;
        #10 enable = 1;
        #100 enable = 0;
        #50 $finish;
    end
    
    initial begin
        $monitor("Time=%0t count=%d", $time, count);
    end
    
endmodule
