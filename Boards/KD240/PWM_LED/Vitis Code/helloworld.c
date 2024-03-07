/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "pwm.h"
#include "xil_io.h"
#include "xparameters.h"
#include "sleep.h"

int duty;


int main()
{
    init_platform();

    print("Hello World\n\r");

	//pwm out period = frequency(pwm_out) * (2^N) / frequency(clk);
    PWM_mWriteReg(XPAR_PWM_0_S00_AXI_BASEADDR, PWM_S00_AXI_SLV_REG0_OFFSET,17179);//200hz
	//duty = (2^N) * (1 - (duty cycle)) - 1
	while(1){
		for(duty =17179; duty <999999999999999; duty += 1000000){
			PWM_mWriteReg(XPAR_PWM_0_S00_AXI_BASEADDR, PWM_S00_AXI_SLV_REG1_OFFSET, duty);
			usleep(100);
			printf("%u\n", PWM_mReadReg(XPAR_PWM_0_S00_AXI_BASEADDR, PWM_S00_AXI_SLV_REG0_OFFSET));
		}
	}

//    ZCU104_PWM_mWriteReg(XPAR_ZCU104_PWM_0_S00_AXI_BASEADDR, ZCU104_PWM_S00_AXI_SLV_REG1_OFFSET, 17180);

    cleanup_platform();
return 0;
}
