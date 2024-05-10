`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/06/01 17:06:45
// Design Name: 
// Module Name: segled_test
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module segled_test();

    reg clk, rst, b1;
    wire [3:0] led;

    initial begin
      clk=0;
      forever #1000 clk=~clk;     
    end

    initial begin
      rst=0;
      #20  rst=1;    
      b1 = 1;      //  ¿EÀy
    end

 seg_led u1(                //¨Ò¤ÆÅÞ¿è³]­p¼Ò¶ô
      .clk(clk),
      .rst(rst),
      .b2(b1),
      .led(led));
      
endmodule
