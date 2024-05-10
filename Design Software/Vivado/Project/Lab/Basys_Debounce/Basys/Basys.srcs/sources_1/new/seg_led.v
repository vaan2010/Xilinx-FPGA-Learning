`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2022/06/01 10:31:30
// Design Name: 
// Module Name: seg_led
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


module seg_led(
    input clk, rst, b1, b2,
    input [15:0] seg_switch,
    input start_point, up, down,
    output reg [15:0] led,
    output reg [3:0] led_node,
    output reg [6:0] seven_seg
    );

// LED    
reg [31:0] counter;
reg right_enable;
reg left_enable;
reg enable;

// 7-seg
wire [1:0] seven_seg_display; 
reg [19:0] number_counter;
reg [3:0] LED_number;

parameter ZERO  = 7'b0000001;
parameter ONE   = 7'b1001111;
parameter TWO   = 7'b0010010;
parameter THREE = 7'b0000110;
parameter FOUR  = 7'b1001100; //
parameter FIVE  = 7'b0100100;
parameter SIX   = 7'b0100000;
parameter SEVEN = 7'b0001101;
parameter EIGHT = 7'b0000000;
parameter NINE  = 7'b0000100;

// 99999_999 ---> 1s
// LED left to right, right to left
always@(posedge clk or posedge rst)
begin
    if(rst)
    begin
        counter <= 0;
        led <= 0;
        right_enable <= 0;
        left_enable <= 0;
        enable <= 0;
    end
    else
    begin

        if(enable == 1)begin
        
            if(b1 == 1)begin
                left_enable <= 1;
                right_enable <= 0;
            end
            else if(b2 ==1)begin
                left_enable <= 0;
                right_enable <= 1;
            end
        
            if(left_enable == 1)begin
                if(counter <= 24999_999)
                    counter <= counter+1;
                else begin
                    counter <= 0;
                    if(led == 1) led <= 2**15;
                    else led <= led/2;
                end
            end
            else if(right_enable == 1)begin
                if(counter <= 24999_999)
                    counter <= counter+1;
                else begin
                    counter <= 0;
                    if(led == 2**15) led <= 1;
                    else led <= led*2;
                end
            end
            
        end
        
        else if(b1 == 1 || b2 == 1)begin
            enable <= 1;
            if(b1 == 1)begin
                left_enable <= 1;
                right_enable <= 0;
                led <= 2**15;
            end
            else if(b2 == 1)begin
                left_enable <= 0;
                right_enable <= 1;
                led <= 1;
            end
        end
    end
end    

// debounce
reg flag;
reg key;
reg [31:0] debounce_counter;
reg stop_signal;
reg switch_modify;

always@(posedge clk or posedge rst)
begin
    if(rst) debounce_counter <= 0;
    else if(start_point) debounce_counter <= debounce_counter + 1;
    else debounce_counter <= 0;
end

always@(posedge clk or posedge rst)
begin
    if(rst) flag <= 0;
    else if(debounce_counter == 1999999) flag <= 1;
    else flag <= 0;
end

always@(posedge clk or posedge rst)
begin
    if(rst) key <= 0;
    else if(flag == 0 && debounce_counter == 1999999) key <= 1;
    else key <= 0;
end

// 7-seg display
assign seven_seg_display = number_counter[19:18];
reg [31:0] second_counter;
reg [15:0] current_number;
reg [15:0] seg_number;
reg up_enable;
reg down_enable;

reg s_button;
reg stop;

reg [3:0] th;
reg [3:0] hu;
reg [3:0] te;
reg [3:0] di;

always@(posedge clk or posedge rst)
begin
    if(rst)
    begin
        th <= 0;
        hu <= 0;
        te <= 0;
        di <= 0;
    end
    else
    begin
        if(seg_switch[15:12] >= 9) th <= 9;
        else th <= seg_switch[15:12];
        
        if(seg_switch[11:8] >= 9) hu <= 9;
        else hu <= seg_switch[11:8];
        
        if(seg_switch[7:4] >= 9) te <= 9;
        else te <= seg_switch[7:4];
        
        if(seg_switch[3:0] >= 9) di <= 9;
        else di <= seg_switch[3:0];
    end
end


always@(posedge clk or posedge rst)
begin
if(rst == 1)begin
    led_node <= 4'b0000;
    number_counter <= 0;
    LED_number <= 0;
    
    s_button <= 0;
    
    up_enable <= 0;
    down_enable <= 0;
    
    second_counter <= 0;
    
    stop <= 1;

    seg_number <= 0;
    
    stop_signal <= 0;
    
    switch_modify <= 0;
    
end
else
begin
    number_counter <= number_counter +1;
    

    if(s_button == 1 && up_enable == 1) begin
        second_counter <= second_counter + 1;
        if(second_counter == 99999999)begin
            if(current_number == 9999) current_number <= 9999;
            else current_number <= current_number + 1;
            second_counter <= 0;
        end 
    end
    
    else if(s_button == 1 && down_enable == 1) begin
        second_counter <= second_counter + 1;
        if(second_counter == 99999999)begin
            if(current_number == 0) current_number <= 0;
            else current_number <= current_number - 1;
            second_counter <= 0;
        end 
    end
    
//////
    if(key == 1 && (stop_signal == 1 || switch_modify == 1)) begin
        up_enable <= 0;
        down_enable <= 0;
        s_button <= 1;
        stop_signal <= stop_signal ^ 1;
        current_number <= current_number;
    end
    
    else if(key == 1 && switch_modify == 0) begin
    
        seg_number <= seg_switch;

        stop <= 0;
        
        stop_signal <= stop_signal ^ 1;

        up_enable <= 0;
        down_enable <= 0;
        s_button <= 1;
        
        switch_modify <= 1;
        
        current_number <= th*1000 + hu*100 + te*10 + di;
        
    end

    else if(up) begin
        up_enable <= 1;
        down_enable <= 0;
    end
    else if(down) begin
        up_enable <= 0;
        down_enable <= 1;
    end
    
    else if(seg_number != seg_switch) begin
        stop <= 1;
        up_enable <= 0;
        down_enable <= 0;
        switch_modify <= 0;
        stop_signal <= 0;
    end

    
////// 

    case(seven_seg_display)
    2'b00:begin
        led_node <= 4'b0111;
        if(!stop) LED_number <= current_number/1000;
        else LED_number <= seg_switch[15:12];
        
    end
    
    2'b01:begin
        led_node <= 4'b1011;
        if(!stop) LED_number <= (current_number % 1000)/100;
        else LED_number <= seg_switch[11:8];
    end
    
    2'b10:begin
        led_node <= 4'b1101;
        if(!stop) LED_number <= ((current_number % 1000)%100)/10;
        else LED_number <= seg_switch[7:4];
    end
    
    2'b11:begin
        led_node <= 4'b1110;
        if(!stop) LED_number <= ((current_number % 1000)%100)%10;
        else LED_number <= seg_switch[3:0];
    end
    endcase

end
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
