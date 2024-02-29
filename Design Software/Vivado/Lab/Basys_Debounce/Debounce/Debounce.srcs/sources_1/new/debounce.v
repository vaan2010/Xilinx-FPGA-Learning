`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/06/07 10:05:03
// Design Name: 
// Module Name: debounce
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


module debounce(
    input clk, rst, bt1, 
    output reg [6:0] seven_seg
    );
    
    reg [3:0] LED_number;
    reg [31:0] counter;
    reg flag;
    reg key;
    
    parameter ZERO  = 7'b0000001;
    parameter ONE   = 7'b1001111;
    parameter TWO   = 7'b0010010;
    parameter THREE = 7'b0000110;
    parameter FOUR  = 7'b1001100; 
    parameter FIVE  = 7'b0100100;
    parameter SIX   = 7'b0100000;
    parameter SEVEN = 7'b0001101;
    parameter EIGHT = 7'b0000000;
    parameter NINE  = 7'b0000100;
    
    always@(posedge clk or posedge rst)
    begin
        if(rst) counter <= 0;
        else if(bt1) counter <= counter + 1;
        else counter <= 0;
    end
    
    always@(posedge clk or posedge rst)
    begin
        if(rst) flag <= 0;
        else if(bt1) flag <= 0;
        else if(counter == 1999999) flag <= 1;
    end
    
    always@(posedge clk or posedge rst)
    begin
        if(rst) key <= 0;
        else if(flag == 0 && counter == 1999999) key <= 1;
        else key <= 0;
    end
    
    always@(posedge clk or posedge rst)
    begin
        if(rst) LED_number <= 0;
        else if(key) LED_number <= LED_number + 1;
        else LED_number <= LED_number;
    end
    
    always@(*)
    begin
        case(LED_number)
            4'b0000: seven_seg <= ZERO;
            4'b0001: seven_seg <= ONE;
            4'b0010: seven_seg <= TWO;
            4'b0011: seven_seg <= THREE;
            4'b0100: seven_seg <= FOUR;
            4'b0101: seven_seg <= FIVE;
            4'b0110: seven_seg <= SIX;
            4'b0111: seven_seg <= SEVEN;
            4'b1000: seven_seg <= EIGHT;
            4'b1001: seven_seg <= NINE;
            default: seven_seg <= NINE;
        endcase
    end
    
    
    
    
    
    
endmodule
