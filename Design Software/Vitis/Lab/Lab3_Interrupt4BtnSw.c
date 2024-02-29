/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
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

#include "xparameters.h"
#include "xgpio.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"

// Parameter Definitions
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID

#define INTC_GPIO_INTERRUPT_ID_SW	XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR
#define INTC_GPIO_INTERRUPT_ID_BTN	XPAR_FABRIC_AXI_GPIO_2_IP2INTC_IRPT_INTR

#define SWS_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define BTNS_DEVICE_ID XPAR_AXI_GPIO_2_DEVICE_ID
#define LEDS_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID

#define SW_INT_CHANNEL 1
#define BTN_INT_CHANNEL 1

XGpio LEDInst, BTNInst, SWInst; // Struct
XScuGic INTCInst;
static int led_data;
static int btn_value;
static int sw_value;
int counter_BTN = 1; //add or minus
int max = 3;

// Prototype Functions
static void BTN_Intr_Handler(void *baseaddr_p);         //interrupt behavior
static int BTN_InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int BTN_IntcInitFunction(u16 DeviceID, XGpio *GpioInstancePtr);

static void SW_Intr_Handler(void *baseaddr_p);          //interrupt behavior
static int SW_InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int SW_IntcInitFunction(u16 DeviceID, XGpio *GpioInstancePtr);

void BTN_Intr_Handler(void *InstancePtr){
	(void)XGpio_InterruptClear(&BTNInst, BTN_INT_CHANNEL);
	XGpio_InterruptDisable(&BTNInst, BTN_INT_CHANNEL);
//	if((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT_CHANNEL) != BTN_INT_CHANNEL){
//		return;
//	}
	btn_value = XGpio_DiscreteRead(&BTNInst, 1); // Read Gpio value
	printf("btn_value = %d\n", btn_value);


	if(btn_value != 0){
		printf("led_data = %d\n", led_data);
		if(led_data == max || led_data == btn_value) counter_BTN = 0;
		else counter_BTN = 1;

		if(counter_BTN == 1) led_data += btn_value;
		else led_data -= btn_value;

	}
	else led_data = led_data;



	printf("led_data = %d\n", led_data);

	printf("=====================\n");


	XGpio_DiscreteWrite(&LEDInst, 1, led_data);

	XGpio_InterruptEnable(&BTNInst, BTN_INT_CHANNEL);
}

int BTN_InterruptSystemSetup(XScuGic *XScuGicInstancePtr){
//	XGpio_InterruptEnable(&BTNInst, BTN_INT_CHANNEL);
//	XGpio_InterruptGlobalEnable(&BTNInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, XScuGicInstancePtr);
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

int BTN_IntcInitFunction(u16 DeviceID, XGpio *GpioInstancePtr){
	XScuGic_Config *IntcConfig;
	int status;

	// XScuGic initialization
	IntcConfig = XScuGic_LookupConfig(DeviceID);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 呼叫中斷設置
	status = BTN_InterruptSystemSetup(&INTCInst);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 將GPIO中斷連接到 handler
	status = XScuGic_Connect(&INTCInst, INTC_GPIO_INTERRUPT_ID_BTN, (Xil_ExceptionHandler) BTN_Intr_Handler, (void *) GpioInstancePtr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 啟用GPIO中斷
	XGpio_InterruptEnable(GpioInstancePtr, 1); // 打開某個 channel 的中斷機制
	XGpio_InterruptGlobalEnable(GpioInstancePtr); // 打開全局中斷機制

	// 在控制器中啟用GPIO和定時器中斷
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID_BTN);

	return XST_SUCCESS;
}
//==================================================================================================


void SW_Intr_Handler(void *InstancePtr){
	(void)XGpio_InterruptClear(&SWInst, SW_INT_CHANNEL);
	XGpio_InterruptDisable(&SWInst, SW_INT_CHANNEL);
//	if((XGpio_InterruptGetStatus(&SWInst) & SW_INT_CHANNEL) != SW_INT_CHANNEL){
//		return;
//	}

	sw_value = XGpio_DiscreteRead(&SWInst, 1); // Read Gpio value (Gpio, channel)
	sw_value ^= 3;

	printf("btn_value = %d\n", sw_value);

	if(sw_value == 0) {
		counter_BTN = 1;
		led_data = 0;
	}
	else{
		led_data = sw_value;
	}



	XGpio_DiscreteWrite(&LEDInst, 1, led_data);

	XGpio_InterruptEnable(&SWInst, SW_INT_CHANNEL);
}

int SW_InterruptSystemSetup(XScuGic *XScuGicInstancePtr){
//	XGpio_InterruptEnable(&SWInst, SW_INT_CHANNEL);
//	XGpio_InterruptGlobalEnable(&SWInst);

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler) XScuGic_InterruptHandler, XScuGicInstancePtr);
	Xil_ExceptionEnable();

	return XST_SUCCESS;
}

int SW_IntcInitFunction(u16 DeviceID, XGpio *GpioInstancePtr){
	XScuGic_Config *IntcConfig;
	int status;

	// XScuGic initialization
	IntcConfig = XScuGic_LookupConfig(DeviceID);
	status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 呼叫中斷設置
	status = SW_InterruptSystemSetup(&INTCInst);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 將GPIO中斷連接到 handler
	status = XScuGic_Connect(&INTCInst, INTC_GPIO_INTERRUPT_ID_SW, (Xil_ExceptionHandler) SW_Intr_Handler, (void *) GpioInstancePtr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	// 啟用GPIO中斷
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// 在控制器中啟用GPIO和定時器中斷
	XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID_SW);

	return XST_SUCCESS;
}
//==================================================================================================

int main()
{
    init_platform();

    int status;

    status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
    if(status != XST_SUCCESS) return XST_FAILURE;

    status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
    if(status != XST_SUCCESS) return XST_FAILURE;

    status = XGpio_Initialize(&SWInst, SWS_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;

    status = BTN_IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
    if(status != XST_SUCCESS) return XST_FAILURE;

    status = SW_IntcInitFunction(INTC_DEVICE_ID, &SWInst);
	if(status != XST_SUCCESS) return XST_FAILURE;


    return 0;
}
