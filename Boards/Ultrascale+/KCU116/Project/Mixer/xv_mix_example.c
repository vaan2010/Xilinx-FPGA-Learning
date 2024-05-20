/*******************************************************************************
* Copyright (C) 2016 - 2022 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
*******************************************************************************/

/*****************************************************************************/
/**
*
* @file main.c
*
* This file demonstrates the example usage of Mixer IP available in catalogue
* Please refer v_mix example design guide for details on HW setup
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  rco   11/24/15   Initial Release
*             02/05/16   Add Logo test
*             03/08/16   Add configuration table for all layers, replace
*                        delay with polling with time out
*             03/14/16   Fix bug startx not multiple of pixels/clk for window
*                        move
*             08/05/16   Add Logo Pixel Alpha test
* 2.00  vyc   10/04/17   Add second buffer pointer for semi-planar formats
* 2.10  vyc   04/04/18   Add support for streaming layers using Frame Buffer
*                        Read for streaming input
*                        Add support for ZCU102, ZCU104, ZCU106
* 4.00  vyc   04/04/18   Add support for streaming layers using Frame Buffer
* 5.00  pv    19/04/18   Added flushing feature support in driver.
*			 flush bit should be set and held (until reset) by
*			 software to flush pending transactions.IP is expecting
*			 a hard reset, when flushing is done.(There is a flush
*			 status bit and is asserted when the flush is done).
* 6.00  pg    01/10/20   Add Colorimetry feature.
*                        Program CSC coefficient registers to do color conversion
*                        from YUV to RGB and RGB to YUV.
*       se    30/05/22	 Added 4K overlay layer support
* </pre>
*
******************************************************************************/

#include "xparameters.h"
#include "platform.h"
#include "sleep.h"
#include "xv_tpg.h"
#include "xv_mix_l2.h"
#include "xvidc.h"
#include "xvtc.h"
#if defined (__MICROBLAZE__)
#include "xintc.h"
#else
#include "xscugic.h"
#endif
#include "xgpio.h"

#include "xiic_l.h"

#define PAGE_SIZE   16
#define IIC_BASE_ADDRESS	XPAR_IIC_0_BASEADDR
#define EEPROM_TEST_START_ADDRESS	0x80
#define IIC_ADV7511_ADDRESS 0x39


#if defined(__MICROBLAZE__)
#define DDR_BASEADDR XPAR_DDR4_0_C0_DDR4_MEMORY_MAP_BASEADDR
#else
#define DDR_BASEADDR XPAR_DDR_MEM_BASEADDR
#endif

#if defined XPAR_PSU_ACPU_GIC_DEVICE_ID
#define PS_ACPU_GIC_DEVICE_ID XPAR_PSU_ACPU_GIC_DEVICE_ID
#elif defined XPAR_SCUGIC_0_DEVICE_ID
#define PS_ACPU_GIC_DEVICE_ID XPAR_SCUGIC_0_DEVICE_ID
#else
#warning No GIC Device ID found
#endif

#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
#include "xv_frmbufrd_l2.h"
extern XV_frmbufrd_Config XV_frmbufrd_ConfigTable[];
#define XVFRMBUFRD_BUFFER_BASEADDR (DDR_BASEADDR + (0x20000000))
#define XVFRMBUFRD_CHROMA_ADDR_OFFSET   (0x01000000U)
#endif

#define NUM_TEST_MODES    (1) //according to main function

/* Memory Layers for Mixer */
#define XVMIX_LAYER1_BASEADDR      (DDR_BASEADDR + (0x21000000))
#define XVMIX_LAYER_ADDR_OFFSET    (0x01000000U)
#define XVMIX_CHROMA_ADDR_OFFSET   (0x01000000U)

#define VIDEO_MONITOR_LOCK_TIMEOUT (2000000)

extern unsigned char Logo_R[];
extern unsigned char Logo_G[];
extern unsigned char Logo_B[];
extern unsigned char Logo_A[];

XV_tpg     tpg_master;
XV_tpg     tpg_stream_1, tpg_stream_2, tpg_stream_3, tpg_stream_4;

XV_Mix_l2  mix;
XVtc       vtc;
#if defined (__MICROBLAZE__)
XIntc      intc;
#else
XScuGic    intc;
#endif
XGpio      vmon;

XVidC_VideoStream VidStream;
XVidC_ColorStd colorStandard;
XVidC_ColorRange colorRange;
u8 colorDepth;

u32 volatile *gpio_hlsIpReset;

#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
typedef struct {
  XV_FrmbufRd_l2 Inst;
  u32            DeviceId;
} FrmbufInst;

FrmbufInst FBLayer[XPAR_XV_FRMBUFRD_NUM_INSTANCES];
#endif

static const XVidC_VideoWindow MixLayerConfig[8] =
{// X   Y     W    H
  {0,  0,  960, 540},//Layer 1
  {960, 0,  960, 540}, //Layer 2
  {0, 540,  960, 540}, //Layer 3
  {960, 540,  960, 540}, //Layer 4
  {800, 100, 128, 128}, //Layer 5
  {1000,  100, 128, 128}, //Layer 6
  {200, 400, 128, 128}, //Layer 7
  {200, 600, 128, 128}  //Layer 8
};
static const XVidC_VideoWindow MixLayerConfig_4K[4] =
{// X   Y     W    H
  {0,  0,  1920, 1080}, 	//Layer 1
  {3840, 0,  1920, 1080}, 	//Layer 2
  {0, 2160,  3840, 2160}, 	//Layer 3
  {3840, 2160,  3840, 2160},//Layer 4
};

/*****************************************************************************/
/**
 * This macro reads GPIO to check video lock status
 *
 * @param  GpioPtr is pointer to the gpio Instance
 * @return T/F
 *
 *****************************************************************************/
#define XVMonitor_IsVideoLocked(GpioPtr)   (XGpio_DiscreteRead(GpioPtr, 1))


void resetIp(void);
static int DriverInit(void);
static int SetupInterrupts(void);
static void ConfigTpgMaster(XVidC_VideoStream *StreamPtr);
static void ConfigTpgStream(XVidC_VideoStream *StreamPtr);
static void ConfigMixer(XVidC_VideoStream *StreamPtr);
#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
static XVidC_ColorFormat FindMemFormat(XVidC_ColorFormat StreamFmt);
static u32 CalcStride(XVidC_ColorFormat Cfmt,
                      u16 AXIMMDataWidth,
                      XVidC_VideoStream *StreamPtr);
static int ConfigFrmbuf(XV_FrmbufRd_l2 *LayerFrmbuf,
                        u32 StrideInBytes,
                        XVidC_ColorFormat Cfmt,
                        XVidC_VideoStream *LayerStreamPtr);
#endif
static void ConfigVtc(XVidC_VideoStream *StreamPtr);
static int RunMixerFeatureTests(XVidC_VideoStream *StreamPtr);
static int CheckVidoutLock(void);


typedef u8 AddressType;

typedef struct {
	u8 addr;
	u8 data;
	u8 init;
} HDMI_REG;

#define NUMBER_OF_HDMI_REGS  40
HDMI_REG hdmi_iic[NUMBER_OF_HDMI_REGS] = {

		//KCU116 Config
		//power up
		{ 0x41, 0x00, 0x10 },

		//fix reg
		{ 0x98, 0x00, 0x03 },
		{ 0x9A, 0x00, 0xE0 },
		{ 0x9C, 0x00, 0x30 },
		{ 0x9D, 0x00, 0x61 },
		{ 0xA2, 0x00, 0xA4 },
		{ 0xA3, 0x00, 0xA4 },
		{ 0xE0, 0x00, 0xD0 },
		{ 0xF9, 0x00, 0x00 },

		//function
		{ 0x15, 0x00, 0x21 },//[7:4] = 2 is 48 KHz, [3:0] = 1 is YCbCr 422
		{ 0x16, 0x00, 0x38 },
		{ 0x17, 0x00, 0x02 },

		//CSC off

		//CSC Table 39 HDTV YCbCr (Limited Range) to RGB (Limited Range)
		{ 0x18, 0x00, 0xAC },//A
		{ 0x19, 0x00, 0x53 },
		{ 0x1A, 0x00, 0x08 },
		{ 0x1B, 0x00, 0x00 },
		{ 0x1C, 0x00, 0x00 },
		{ 0x1D, 0x00, 0x00 },
		{ 0x1E, 0x00, 0x19 },
		{ 0x1F, 0x00, 0xD6 },

		{ 0x20, 0x00, 0x1C },//B
		{ 0x21, 0x00, 0x56 },
		{ 0x22, 0x00, 0x08 },
		{ 0x23, 0x00, 0x00 },
		{ 0x24, 0x00, 0x1E },
		{ 0x25, 0x00, 0x88 },
		{ 0x26, 0x00, 0x02 },
		{ 0x27, 0x00, 0x91 },

		{ 0x28, 0x00, 0x1F },//C
		{ 0x29, 0x00, 0xFF },
		{ 0x2A, 0x00, 0x08 },
		{ 0x2B, 0x00, 0x00 },
		{ 0x2C, 0x00, 0x0E },
		{ 0x2D, 0x00, 0x85 },
		{ 0x2E, 0x00, 0x18 },
		{ 0x2F, 0x00, 0xBE },

		{ 0x48, 0x00, 0x08 },
		{ 0xAF, 0x00, 0x06 }, //HDMI mode

		//GC enable
		{ 0x40, 0x00, 0x80 },

		//GC update
		{ 0x4A, 0x00, 0x10 },
};


u8 EepromIicAddr;		/* Variable for storing Eeprom IIC address */

int IicLowLevelDynEeprom();

u8 EepromReadByte(AddressType Address, u8 *BufferPtr, u8 ByteCount);
u8 EepromWriteByte(AddressType Address, u8 *BufferPtr, u8 ByteCount);



//HDMI IIC
int IicLowLevelDynEeprom()
{
  u8 BytesRead;
  u32 StatusReg;
  u8 Index;
  int Status;
  u32 i;
//  u8 channel[1] = {0x20};

  Status = XIic_DynInit(IIC_BASE_ADDRESS);
  if (Status != XST_SUCCESS) {
	return XST_FAILURE;
  }
  xil_printf("\r\nAfter XIic_DynInit\r\n");
  while (((StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS,
				XIIC_SR_REG_OFFSET)) &
				(XIIC_SR_RX_FIFO_EMPTY_MASK |
				XIIC_SR_TX_FIFO_EMPTY_MASK |
				XIIC_SR_BUS_BUSY_MASK)) !=
				(XIIC_SR_RX_FIFO_EMPTY_MASK |
				XIIC_SR_TX_FIFO_EMPTY_MASK)) {

  }

//  EepromIicAddr = IIC_SWITCH_ADDRESS;
//  XIic_DynSend(IIC_BASE_ADDRESS, EepromIicAddr,
//		  	  	  channel, sizeof(channel), XIIC_STOP);


  EepromIicAddr = IIC_ADV7511_ADDRESS;
  for ( Index = 0; Index < NUMBER_OF_HDMI_REGS; Index++)
  {
    EepromWriteByte(hdmi_iic[Index].addr, &hdmi_iic[Index].init, 1);
  }

  for ( Index = 0; Index < NUMBER_OF_HDMI_REGS; Index++)
  {
    BytesRead = EepromReadByte(hdmi_iic[Index].addr, &hdmi_iic[Index].data, 1);
    for(i=0;i<1000;i++) {};	// IIC delay
	if (BytesRead != 1) {
      return XST_FAILURE;
	}
  }



  return XST_SUCCESS;

}

u8 EepromReadByte(AddressType Address, u8 *BufferPtr, u8 ByteCount)
{
  u8 ReceivedByteCount;
  u8 SentByteCount;
  u16 StatusReg;

  /*
   * Position the Read pointer to specific location in the EEPROM.
   */
  do {
	StatusReg = XIic_ReadReg(IIC_BASE_ADDRESS, XIIC_SR_REG_OFFSET);
    if (!(StatusReg & XIIC_SR_BUS_BUSY_MASK)) {
	  SentByteCount = XIic_DynSend(IIC_BASE_ADDRESS, EepromIicAddr,
					  (u8 *) &Address, sizeof(Address), XIIC_REPEATED_START);
    }

  } while (SentByteCount != sizeof(Address));
  /*
   * Receive the data.
   */
  ReceivedByteCount = XIic_DynRecv(IIC_BASE_ADDRESS, EepromIicAddr,
		                                          BufferPtr, ByteCount);

  /*
   * Return the number of bytes received from the EEPROM.
   */

  return ReceivedByteCount;

}


u8 EepromWriteByte(AddressType Address, u8 *BufferPtr, u8 ByteCount)
{
  u8 SentByteCount;
  u8 WriteBuffer[sizeof(Address) + PAGE_SIZE];
  u8 Index;

  /*
   * A temporary write buffer must be used which contains both the address
   * and the data to be written, put the address in first based upon the
   * size of the address for the EEPROM
   */
  if (sizeof(AddressType) == 2) {
	WriteBuffer[0] = (u8) (Address >> 8);
	WriteBuffer[1] = (u8) (Address);
  } else if (sizeof(AddressType) == 1) {
	WriteBuffer[0] = (u8) (Address);
	EepromIicAddr |= (EEPROM_TEST_START_ADDRESS >> 8) & 0x7;
  }

  /*
   * Put the data in the write buffer following the address.
   */
  for (Index = 0; Index < ByteCount; Index++) {
	WriteBuffer[sizeof(Address) + Index] = BufferPtr[Index];
  }

  /*
   * Write a page of data at the specified address to the EEPROM.
   */
  SentByteCount = XIic_DynSend(IIC_BASE_ADDRESS, EepromIicAddr,
				WriteBuffer, sizeof(Address) + ByteCount,
				XIIC_STOP);

  /*
   * Return the number of bytes written to the EEPROM.
   */
  return SentByteCount - sizeof(Address);

}


/*****************************************************************************/
/**
 * This function initializes and configures the system interrupt controller
 *
 * @return XST_SUCCESS if init is OK else XST_FAILURE
 *
 *****************************************************************************/
static int SetupInterrupts(void)
{
#if defined(__MICROBLAZE__)
  int Status;
  XIntc *IntcPtr = &intc;

  /* Initialize the Interrupt controller */
  Status = XIntc_Initialize(IntcPtr,
		  XPAR_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: Interrupt controller device not found\r\n");
    return(XST_FAILURE);
  }

  /* Hook up interrupt service routine */
  Status = XIntc_Connect(IntcPtr,
		  XPAR_MICROBLAZE_0_AXI_INTC_V_MIX_0_INTERRUPT_INTR,
                         (XInterruptHandler)XVMix_InterruptHandler,
                         &mix);
  if (Status != XST_SUCCESS) {
    xil_printf("ERROR:: Mixer interrupt connect failed!\r\n");
    return XST_FAILURE;
  }

  /* Enable the interrupt vector at the interrupt controller */
  XIntc_Enable(IntcPtr,
		  XPAR_MICROBLAZE_0_AXI_INTC_V_MIX_0_INTERRUPT_INTR);

  /*
   * Start the interrupt controller such that interrupts are recognized
   * and handled by the processor
   */
  Status = XIntc_Start(IntcPtr, XIN_REAL_MODE);
  if (Status != XST_SUCCESS) {
    xil_printf("ERROR:: Failed to start interrupt controller\r\n");
    return XST_FAILURE;
  }

#else
  int Status;
  XScuGic *IntcPtr = &intc;

  /* Initialize the Interrupt controller */
  XScuGic_Config *IntcCfgPtr;
  IntcCfgPtr = XScuGic_LookupConfig(PS_ACPU_GIC_DEVICE_ID);
  if(IntcCfgPtr == NULL)
  {
    print("ERR:: Interrupt Controller not found");
    return (XST_DEVICE_NOT_FOUND);
  }
  Status = XScuGic_CfgInitialize(IntcPtr,
                                 IntcCfgPtr,
                                 IntcCfgPtr->CpuBaseAddress);
  if (Status != XST_SUCCESS) {
    xil_printf("Intc initialization failed!\r\n");
    return XST_FAILURE;
  }

  /* Hook up interrupt service routine */
  Status |= XScuGic_Connect(IntcPtr,
                            XPAR_FABRIC_V_MIX_0_INTERRUPT_INTR,
                            (XInterruptHandler)XVMix_InterruptHandler,
                            (void *)&mix);
  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                               (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                               IntcPtr);
  if (Status != XST_SUCCESS) {
    xil_printf("ERR:: Mixer interrupt connect failed!\r\n");
    return XST_FAILURE;
  }

  /* Enable the interrupt vector at the interrupt controller */
  XScuGic_Enable(IntcPtr, XPAR_FABRIC_V_MIX_0_INTERRUPT_INTR);

#endif

  return(XST_SUCCESS);
}

/*****************************************************************************/
/**
 * This function initializes system wide peripherals.
 *
 * @return XST_SUCCESS if init is OK else XST_FAILURE
 *
 *****************************************************************************/
static int DriverInit(void)
{
  int Status;
  XVtc_Config *vtc_Config;
  XGpio_Config *GpioCfgPtr;

  vtc_Config = XVtc_LookupConfig(XPAR_V_TC_0_DEVICE_ID);
  if(vtc_Config == NULL) {
    xil_printf("ERROR:: VTC device not found\r\n");
    return(XST_FAILURE);
  }

  Status = XVtc_CfgInitialize(&vtc, vtc_Config, vtc_Config->BaseAddress);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: VTC Initialization failed %d\r\n", Status);
    return(XST_FAILURE);
  }

  Status = XV_tpg_Initialize(&tpg_master, XPAR_V_TPG_0_DEVICE_ID);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: TPG device not found\r\n");
    return(XST_FAILURE);
  }

  Status = XV_tpg_Initialize(&tpg_stream_1, XPAR_V_TPG_1_DEVICE_ID);
  if(Status != XST_SUCCESS) {
  xil_printf("ERROR:: TPG device not found\r\n");
  return(XST_FAILURE);
  }

  Status = XV_tpg_Initialize(&tpg_stream_2, XPAR_V_TPG_2_DEVICE_ID);
  if(Status != XST_SUCCESS) {
  xil_printf("ERROR:: TPG device not found\r\n");
  return(XST_FAILURE);
  }

  Status = XV_tpg_Initialize(&tpg_stream_3, XPAR_V_TPG_3_DEVICE_ID);
  if(Status != XST_SUCCESS) {
  xil_printf("ERROR:: TPG device not found\r\n");
  return(XST_FAILURE);
  }

  Status = XV_tpg_Initialize(&tpg_stream_4, XPAR_V_TPG_4_DEVICE_ID);
  if(Status != XST_SUCCESS) {
  xil_printf("ERROR:: TPG device not found\r\n");
  return(XST_FAILURE);
  }


  Status  = XVMix_Initialize(&mix, XPAR_V_MIX_0_DEVICE_ID);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: Mixer device not found\r\n");
    return(XST_FAILURE);
  }

  //Video Lock Monitor
  GpioCfgPtr = XGpio_LookupConfig(XPAR_AXI_GPIO_1_DEVICE_ID);
  if(GpioCfgPtr == NULL) {
    xil_printf("ERROR:: Video Lock Monitor GPIO device not found\r\n");
    return(XST_FAILURE);
  }

  Status = XGpio_CfgInitialize(&vmon,
                               GpioCfgPtr,
                               GpioCfgPtr->BaseAddress);
  if(Status != XST_SUCCESS)  {
    xil_printf("ERROR:: Video Lock Monitor GPIO Initialization failed %d\r\n", Status);
    return(XST_FAILURE);
  }

  return(XST_SUCCESS);
}

/*****************************************************************************/
/**
 * This function configures TPG for defined mode
 *
 * @return none
 *
 *****************************************************************************/
static void ConfigTpgMaster(XVidC_VideoStream *StreamPtr)
{
  //Stop TPG
  XV_tpg_DisableAutoRestart(&tpg_master);

  XV_tpg_Set_height(&tpg_master, StreamPtr->Timing.VActive);
  XV_tpg_Set_width(&tpg_master, StreamPtr->Timing.HActive);
  XV_tpg_Set_colorFormat(&tpg_master, StreamPtr->ColorFormatId);
  XV_tpg_Set_bckgndId(&tpg_master, XTPG_BKGND_SOLID_WHITE);
  XV_tpg_Set_ovrlayId(&tpg_master, 0);

  //Start TPG
  XV_tpg_EnableAutoRestart(&tpg_master);
  XV_tpg_Start(&tpg_master);
  xil_printf("INFO: Master TPG configured\r\n");
}


static void ConfigTpgStream_1(XVidC_VideoStream *StreamPtr)
{
  //Stop TPG
  XV_tpg_DisableAutoRestart(&tpg_stream_1);

  XV_tpg_Set_height(&tpg_stream_1, 540);
  XV_tpg_Set_width(&tpg_stream_1, 960);
  XV_tpg_Set_colorFormat(&tpg_stream_1, StreamPtr->ColorFormatId);
  XV_tpg_Set_bckgndId(&tpg_stream_1, XTPG_BKGND_COLOR_BARS);
  XV_tpg_Set_ovrlayId(&tpg_stream_1, 0);

  //Start TPG
  XV_tpg_EnableAutoRestart(&tpg_stream_1);
  XV_tpg_Start(&tpg_stream_1);
  xil_printf("INFO: Stream TPG 1 configured\r\n");
}

static void ConfigTpgStream_2(XVidC_VideoStream *StreamPtr)
{
  //Stop TPG
  XV_tpg_DisableAutoRestart(&tpg_stream_2);

  XV_tpg_Set_height(&tpg_stream_2, 540);
  XV_tpg_Set_width(&tpg_stream_2, 960);
  XV_tpg_Set_colorFormat(&tpg_stream_2, StreamPtr->ColorFormatId);
  XV_tpg_Set_bckgndId(&tpg_stream_2, XTPG_BKGND_CROSS_HATCH);
  XV_tpg_Set_ovrlayId(&tpg_stream_2, 0);

  //Start TPG
  XV_tpg_EnableAutoRestart(&tpg_stream_2);
  XV_tpg_Start(&tpg_stream_2);
  xil_printf("INFO: Stream TPG 2 configured\r\n");
}

static void ConfigTpgStream_3(XVidC_VideoStream *StreamPtr)
{
  //Stop TPG
  XV_tpg_DisableAutoRestart(&tpg_stream_3);

  XV_tpg_Set_height(&tpg_stream_3, 540);
  XV_tpg_Set_width(&tpg_stream_3, 960);
  XV_tpg_Set_colorFormat(&tpg_stream_3, StreamPtr->ColorFormatId);
  XV_tpg_Set_bckgndId(&tpg_stream_3, XTPG_BKGND_CHECKER_BOARD);
  XV_tpg_Set_ovrlayId(&tpg_stream_3, 0);

  //Start TPG
  XV_tpg_EnableAutoRestart(&tpg_stream_3);
  XV_tpg_Start(&tpg_stream_3);
  xil_printf("INFO: Stream TPG 3 configured\r\n");
}

static void ConfigTpgStream_4(XVidC_VideoStream *StreamPtr)
{
  //Stop TPG
  XV_tpg_DisableAutoRestart(&tpg_stream_4);

  XV_tpg_Set_height(&tpg_stream_4, 540);
  XV_tpg_Set_width(&tpg_stream_4, 960);
  XV_tpg_Set_colorFormat(&tpg_stream_4, StreamPtr->ColorFormatId);
  XV_tpg_Set_bckgndId(&tpg_stream_4, XTPG_BKGND_PBRS);
  XV_tpg_Set_ovrlayId(&tpg_stream_4, 0);

  //Start TPG
  XV_tpg_EnableAutoRestart(&tpg_stream_4);
  XV_tpg_Start(&tpg_stream_4);
  xil_printf("INFO: Stream TPG 4 configured\r\n");
}

#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
/*****************************************************************************/
/**
 * This function calculates the stride
 *
 * @returns stride in bytes
 *
 *****************************************************************************/
static u32 CalcStride(XVidC_ColorFormat Cfmt,
                      u16 AXIMMDataWidth,
                      XVidC_VideoStream *StreamPtr)
{
  u32 stride;
  int width = StreamPtr->Timing.HActive;
  u16 MMWidthBytes = AXIMMDataWidth/8;

  if ((Cfmt == XVIDC_CSF_MEM_Y_UV10) || (Cfmt == XVIDC_CSF_MEM_Y_UV10_420)
      || (Cfmt == XVIDC_CSF_MEM_Y10)) {
    // 4 bytes per 3 pixels (Y_UV10, Y_UV10_420, Y10)
    stride = ((((width*4)/3)+MMWidthBytes-1)/MMWidthBytes)*MMWidthBytes;
  }
  else if ((Cfmt == XVIDC_CSF_MEM_Y_UV8) || (Cfmt == XVIDC_CSF_MEM_Y_UV8_420)
           || (Cfmt == XVIDC_CSF_MEM_Y8)) {
    // 1 byte per pixel (Y_UV8, Y_UV8_420, Y8)
    stride = ((width+MMWidthBytes-1)/MMWidthBytes)*MMWidthBytes;
  }
  else if ((Cfmt == XVIDC_CSF_MEM_RGB8) || (Cfmt == XVIDC_CSF_MEM_YUV8)
           || (Cfmt == XVIDC_CSF_MEM_BGR8)) {
    // 3 bytes per pixel (RGB8, YUV8, BGR8)
     stride = (((width*3)+MMWidthBytes-1)/MMWidthBytes)*MMWidthBytes;
  }
  else {
    // 4 bytes per pixel
    stride = (((width*4)+MMWidthBytes-1)/MMWidthBytes)*MMWidthBytes;
  }

  return(stride);
}

/*****************************************************************************/
/**
 * This function finds a compatible memory format for the frame buffer
 *
 * @returns video color format
 *
 *****************************************************************************/
static XVidC_ColorFormat FindMemFormat(XVidC_ColorFormat StreamFmt)
{
  XVidC_ColorFormat Cfmt;

  switch(StreamFmt) {
    case XVIDC_CSF_RGB :
        Cfmt  = XVIDC_CSF_MEM_RGB8;
        break;
    case XVIDC_CSF_YCRCB_444 :
        Cfmt  = XVIDC_CSF_MEM_YUV8;
        break;
    case XVIDC_CSF_YCRCB_422 :
        Cfmt  = XVIDC_CSF_MEM_Y_UV8;
        break;
    case XVIDC_CSF_YCRCB_420 :
        Cfmt  = XVIDC_CSF_MEM_Y_UV8_420;
        break;
    case XVIDC_CSF_RGBA :
        Cfmt  = XVIDC_CSF_MEM_RGBA8;
        break;
    case XVIDC_CSF_YCRCBA_444 :
        Cfmt  = XVIDC_CSF_MEM_YUVA8;
        break;
    default :
        Cfmt  = XVIDC_CSF_MEM_RGB8;
        break;
  }

  return(Cfmt);
}

/*****************************************************************************/
/**
 * This function configures Frame Buffer for defined mode
 *
 * @return XST_SUCCESS if init is OK else XST_FAILURE
 *
 *****************************************************************************/
static int ConfigFrmbuf(XV_FrmbufRd_l2 *LayerFrmbuf,
                        u32 StrideInBytes,
                        XVidC_ColorFormat Cfmt,
                        XVidC_VideoStream *StreamPtr)
{
  int Status;
  u32 IrqMask;

  /* Configure Frame Buffers */
  Status = XVFrmbufRd_SetMemFormat(LayerFrmbuf, StrideInBytes, Cfmt, StreamPtr);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: Unable to configure Frame Buffer Read\r\n");
    return(XST_FAILURE);
  }

  Status = XVFrmbufRd_SetBufferAddr(LayerFrmbuf, XVFRMBUFRD_BUFFER_BASEADDR);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: Unable to configure Frame Buffer Read buffer address\r\n");
    return(XST_FAILURE);
  }

  /* Set Chroma Buffer Address for semi-planar color formats */
  if ((Cfmt == XVIDC_CSF_MEM_Y_UV8) || (Cfmt == XVIDC_CSF_MEM_Y_UV8_420) ||
      (Cfmt == XVIDC_CSF_MEM_Y_UV10) || (Cfmt == XVIDC_CSF_MEM_Y_UV10_420)) {
    Status = XVFrmbufRd_SetChromaBufferAddr(LayerFrmbuf, XVFRMBUFRD_BUFFER_BASEADDR+XVFRMBUFRD_CHROMA_ADDR_OFFSET);
    if(Status != XST_SUCCESS) {
      xil_printf("ERROR:: Unable to configure Frame Buffer Read chroma buffer address\r\n");
      return(XST_FAILURE);
    }
  }

  /* Start Frame Buffers */
  IrqMask = XVFRMBUFRD_IRQ_DONE_MASK | XVFRMBUFRD_IRQ_READY_MASK;
  XVFrmbufRd_InterruptDisable(LayerFrmbuf, IrqMask);
  XVFrmbufRd_Start(LayerFrmbuf);

  xil_printf("INFO: FRMBUF configured\r\n");
  return(Status);
}
#endif

/*****************************************************************************/
/**
 * This function configures vtc for defined mode
 *
 * @return none
 *
 *****************************************************************************/
static void ConfigVtc(XVidC_VideoStream *StreamPtr)
{
  XVtc_Timing vtc_timing = {0};
  u16 PixelsPerClock = StreamPtr->PixPerClk;

  vtc_timing.HActiveVideo  = StreamPtr->Timing.HActive/PixelsPerClock;
  vtc_timing.HFrontPorch   = StreamPtr->Timing.HFrontPorch/PixelsPerClock;
  vtc_timing.HSyncWidth    = StreamPtr->Timing.HSyncWidth/PixelsPerClock;
  vtc_timing.HBackPorch    = StreamPtr->Timing.HBackPorch/PixelsPerClock;
  vtc_timing.HSyncPolarity = StreamPtr->Timing.HSyncPolarity;
  vtc_timing.VActiveVideo  = StreamPtr->Timing.VActive;
  vtc_timing.V0FrontPorch  = StreamPtr->Timing.F0PVFrontPorch;
  vtc_timing.V0SyncWidth   = StreamPtr->Timing.F0PVSyncWidth;
  vtc_timing.V0BackPorch   = StreamPtr->Timing.F0PVBackPorch;
  vtc_timing.VSyncPolarity = StreamPtr->Timing.VSyncPolarity;
  XVtc_SetGeneratorTiming(&vtc, &vtc_timing);
  XVtc_Enable(&vtc);
  XVtc_EnableGenerator(&vtc);
  XVtc_RegUpdateEnable(&vtc);
  xil_printf("INFO: VTC configured\r\n");
}

/*****************************************************************************/
/**
 * This function configures Mixer for defined mode
 *
 * @return none
 *
 *****************************************************************************/
static void ConfigMixer(XVidC_VideoStream *StreamPtr)
{
  XV_Mix_l2 *MixerPtr = &mix;
  int NumLayers, index, Status;
  u32 MemAddr;
  XVidC_ColorFormat Cfmt;

  /* Setup default config after reset */
  XVMix_LayerDisable(MixerPtr, XVMIX_LAYER_MASTER);
  XVMix_SetVidStream(MixerPtr, StreamPtr);

  /* Set Memory Layer Addresses */
  NumLayers = XVMix_GetNumLayers(MixerPtr);
  MemAddr = XVMIX_LAYER1_BASEADDR;
  for(index = XVMIX_LAYER_1; index < NumLayers; ++index) {
      XVMix_GetLayerColorFormat(MixerPtr, index, &Cfmt);
      if (!(XVMix_IsLayerInterfaceStream(MixerPtr, index))) {
          Status = XVMix_SetLayerBufferAddr(MixerPtr, index, MemAddr);
          if(Status != XST_SUCCESS) {
              xil_printf("MIXER ERROR:: Unable to set layer %d buffer addr to 0x%X\r\n",
                          index, MemAddr);
          } else {
              if ((Cfmt == XVIDC_CSF_MEM_Y_UV8) || (Cfmt == XVIDC_CSF_MEM_Y_UV8_420) ||
                  (Cfmt == XVIDC_CSF_MEM_Y_UV10) || (Cfmt == XVIDC_CSF_MEM_Y_UV10_420)) {
                  MemAddr += XVMIX_CHROMA_ADDR_OFFSET;
                  Status = XVMix_SetLayerChromaBufferAddr(MixerPtr, index, MemAddr);
                  if (Status != XST_SUCCESS) {
                      xil_printf("MIXER ERROR:: Unable to set layer %d chroma buffer2 addr to 0x%X\r\n",
                                  index, MemAddr);
                  }
              }
              MemAddr += XVMIX_LAYER_ADDR_OFFSET;
          }
      }
  }

  if(XVMix_IsLogoEnabled(MixerPtr)) {
    XVidC_VideoWindow Win;

    Win.StartX = 64;
    Win.StartY = 64;
    Win.Width  = 64;
    Win.Height = 64;

    Status = XVMix_LoadLogo(MixerPtr,
                            &Win,
                            Logo_R,
                            Logo_G,
                            Logo_B);


    if(Status != XST_SUCCESS) {
      xil_printf("MIXER ERROR:: Unable to load Logo \r\n");
    }


    if(XVMix_IsLogoPixAlphaEnabled(MixerPtr)) {


      Status = XVMix_LoadLogoPixelAlpha(MixerPtr, &Win, Logo_A);

      if(Status != XST_SUCCESS) {
        xil_printf("MIXER ERROR:: Unable to load Logo pixel alpha \r\n");
      }
    }
  } else {
      xil_printf("INFO: Logo Layer Disabled in HW \r\n");
  }


  XVMix_SetBackgndColor(MixerPtr, XVMIX_BKGND_BLUE, StreamPtr->ColorDepth);


  /*
   * Program CSC coefficients when ENABLE_CSC_COEFFICIENT_REGISTERS
   * is enabled
   */
#if XPAR_XV_MIX_0_ENABLE_CSC_COEFFICIENT_REGISTERS
  colorStandard = XVIDC_BT_709;
  colorRange = XVIDC_CR_16_240;
  colorDepth = MixerPtr->Stream.ColorDepth;

  XVMix_SetCscCoeffs(MixerPtr, colorStandard, colorRange, colorDepth);
#endif

  XVMix_LayerEnable(MixerPtr, XVMIX_LAYER_MASTER);

  XVMix_InterruptDisable(MixerPtr);
  XVMix_Start(MixerPtr);
  xil_printf("INFO: Mixer configured\r\n");
}

/*****************************************************************************/
/**
 * This function checks vidout lock status
 *
 * @return none
 *
 *****************************************************************************/
static int CheckVidoutLock(void)
{
  int Status = FALSE;
  int Lock = FALSE;
  u32 Timeout;

  Timeout = VIDEO_MONITOR_LOCK_TIMEOUT;

  usleep(2000000);         //wait

  while(!Lock && Timeout) {
    if(XVMonitor_IsVideoLocked(&vmon)) {
        xil_printf("Locked\r\n");
        Lock = TRUE;
        Status = TRUE;
    }
    --Timeout;
  }

  if(!Timeout) {
      xil_printf("<ERROR:: Not Locked>\r\n\r\n");
  }
  return(Status);
}

/*****************************************************************************/
/**
 * This function runs defined tests on Mixer core
 *
 * @return none
 *
 *****************************************************************************/
static int RunMixerFeatureTests(XVidC_VideoStream *StreamPtr)
{
	int layerIndex, Status;
	int ErrorCount = 0;
	XVidC_VideoWindow Win;
	XVidC_ColorFormat Cfmt;
	u32 baseaddr, Stride;
	XV_Mix_l2 *MixerPtr = &mix;
	u16 ScaleFactor[XVMIX_SCALE_FACTOR_NUM_SUPPORTED] = {1,2,4};
	XVMix_Scalefactor Scale = XVMIX_SCALE_FACTOR_2X ;

	xil_printf("\r\n****Running Mixer Feature Tests****\r\n");
	/* Test 1: Master Layer Enable/Disable
		- Disable layer 0
		- Check video lock
		- Enable layer 0
		- Check video lock
	*/
	xil_printf("Disable Master Layer: ");
	Status = XVMix_LayerDisable(MixerPtr, XVMIX_LAYER_MASTER);
	if(Status == XST_SUCCESS) {
		ErrorCount += (!CheckVidoutLock() ? 1 : 0);
	} else {
		xil_printf("<ERROR:: Command Failed>\r\n");
		++ErrorCount;
	}

	xil_printf("Enable  Master Layer: ");
	Status = XVMix_LayerEnable(MixerPtr, XVMIX_LAYER_MASTER);
	if(Status == XST_SUCCESS) {
		ErrorCount += (!CheckVidoutLock() ? 1 : 0);
	} else {
		xil_printf("<ERROR:: Command Failed>\r\n");
		++ErrorCount;
	}


	/* Test 2: Layer Enable
		  - Set layer window
		  - Set layer Alpha, if available
		  - Set layer scaling, if available
		  - Enable layer
		  - Check video lock
		  - Move layer window
		  - Check video lock
		  - Disable layer
		  - Check video lock
	*/

	xil_printf("Layer number: %d\r\n", XVMix_GetNumLayers(MixerPtr));

	for(layerIndex=XVMIX_LAYER_1; layerIndex<XVMix_GetNumLayers(MixerPtr); ++layerIndex) {

		xil_printf("\r\n--> Test Mixer Layer %d <--\r\n", layerIndex);
		if (!(XVMix_IsLayerInterfaceStream(MixerPtr, layerIndex))) {
		  baseaddr = XVMix_GetLayerBufferAddr(MixerPtr, layerIndex);
		  xil_printf("   Layer Buffer Addr: 0x%X\r\n", baseaddr);
		}
		if(StreamPtr->VmId  <= XVIDC_VM_4096x2160_24_P ) {
		Win = MixLayerConfig[layerIndex-1];
		}
		else {
		Win = MixLayerConfig_4K[layerIndex-1];
		}

		XVMix_GetLayerColorFormat(MixerPtr, layerIndex, &Cfmt);

		xil_printf("   Layer Color Format: %s\r\n", XVidC_GetColorFormatStr(Cfmt));
		Stride = ((Cfmt == XVIDC_CSF_YCRCB_422) ? 2: 4); //BytesPerPixel
		Stride *= Win.Width;

		xil_printf("   Set Layer Window (%3d, %3d, %3d, %3d): ",
				Win.StartX, Win.StartY, Win.Width, Win.Height);
		Status = XVMix_SetLayerWindow(MixerPtr, layerIndex, &Win, Stride);
		if(Status != XST_SUCCESS) {
			xil_printf("<ERROR:: Command Failed>\r\n");
			++ErrorCount;
		} else {
			xil_printf("Done\r\n");
		}

		xil_printf("   Set Layer Alpha to %d: ", XVMIX_ALPHA_MAX);
		if(XVMix_IsAlphaEnabled(MixerPtr, layerIndex)) {
		  Status = XVMix_SetLayerAlpha(MixerPtr, layerIndex, XVMIX_ALPHA_MAX);
		  if(Status != XST_SUCCESS) {
			xil_printf("<ERROR:: Command Failed>\r\n");
			++ErrorCount;
		  } else {
			xil_printf("Done\r\n");
		  }
		} else {
			xil_printf("(Disabled in HW)\r\n");
		}

		xil_printf("   Set Layer Scale Factor to 2x: ");
		if(XVMix_IsScalingEnabled(MixerPtr, layerIndex)) {
		if(((Win.StartX + (Win.Width * ScaleFactor[Scale]) ) <= (MixerPtr->Stream.Timing.HActive)) &&
				((Win.StartY + ( Win.Height * ScaleFactor[Scale]) ) <= (MixerPtr->Stream.Timing.VActive))){
		  Status = XVMix_SetLayerScaleFactor(MixerPtr,
											 layerIndex,
											 Scale );
		  if(Status != XST_SUCCESS) {
			xil_printf("<ERROR:: Command Failed>\r\n");
			++ErrorCount;
		  } else {
			xil_printf("Done\r\n");
			}
		}
		else{
			 xil_printf(" <Check Window Position and Configuration>\r\n");
		  }
		} else {
			xil_printf("(Disabled in HW)\r\n");
		}

		xil_printf("   Enable Layer: ");
		Status = XVMix_LayerEnable(MixerPtr, layerIndex);
		if(Status != XST_SUCCESS) {
			xil_printf("<ERROR:: Command Failed>\r\n");
			++ErrorCount;
		} else {
			xil_printf("Done\r\n");
		}

		//Check for vidout lock
		xil_printf("   Check Vidout State: ");
		ErrorCount += (!CheckVidoutLock() ? 1 : 0);

//		xil_printf("   Move window (x+32), (y+50): ");
//		if(((Win.StartX + (Win.Width * ScaleFactor[Scale]) + 32 ) <= (MixerPtr->Stream.Timing.HActive)) &&
//			((Win.StartY + (Win.Height * ScaleFactor[Scale]) + 50) <= (MixerPtr->Stream.Timing.VActive))){
//		Status = XVMix_MoveLayerWindow(MixerPtr,
//									   layerIndex,
//									   (Win.StartX+32),
//									   (Win.StartY+50));
//		if(Status != XST_SUCCESS) {
//		  xil_printf("<ERROR:: Command Failed>\r\n");
//		  ++ErrorCount;
//		} else {
//		  xil_printf("Done\r\n");
//		}
//		}else {
//		xil_printf(" <Check Window Position and Configuration>\r\n");
//		}

		//Check for vidout lock
//		xil_printf("   Check Vidout State: ");
//		ErrorCount += (!CheckVidoutLock() ? 1 : 0);
	}

  return(ErrorCount);
}


/*****************************************************************************/
/**
 * This function toggles HW reset line for all IP's
 *
 * @return None
 *
 *****************************************************************************/
void resetIp(void)
{
#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
  /* Stop Frame Buffer and wait for IDLE */
  for(int i=0; i<XPAR_XV_FRMBUFRD_NUM_INSTANCES; ++i) {
    XVFrmbufRd_Stop(&FBLayer[i].Inst);
  }
#endif

  xil_printf("\r\nReset HLS IP \r\n");
  *gpio_hlsIpReset = 0; //reset IPs
  usleep(1000);         //hold reset line
  *gpio_hlsIpReset = 1; //release reset
  usleep(1000);         //wait
}

/***************************************************************************
*  This is the main loop of the application
***************************************************************************/
int main(void)
{
  int Status, index;
  int FailCount = 0;
  int Lock = FALSE;
#ifdef XPAR_XV_FRMBUFRD_NUM_INSTANCES
  XV_Mix_l2 *MixerPtr = &mix;
  XVidC_VideoStream LayerStream;
  XVidC_ColorFormat StreamFmt, MemFmt;
#endif
  XVidC_ColorFormat Cfmt;
  XVidC_VideoTiming const *TimingPtr;
  XVidC_VideoMode TestModes[NUM_TEST_MODES] =
  {
#if XPAR_V_MIX_0_MAX_COLS >= 7680
	XVIDC_VM_3840x2160_60_P,
    XVIDC_VM_7680x4320_30_P
#endif
#if ((XPAR_V_MIX_0_MAX_COLS  < 7680) && (XPAR_V_MIX_0_MAX_COLS  >=3840))
	XVIDC_VM_1080_60_P ,
//    XVIDC_VM_UHD_30_P
#endif
#if XPAR_V_MIX_0_MAX_COLS  <= 2048
	XVIDC_VM_1080_30_P ,
	XVIDC_VM_1080_60_P
#endif
  };

  init_platform();

  xil_printf("Start Mixer Example Design Test\r\n");


  Status = IicLowLevelDynEeprom();
  if (Status != XST_SUCCESS) {
	xil_printf("ADV7511 IIC programming FAILED\r\n");
	return XST_FAILURE;
	}
  xil_printf("ADV7511 IIC programming PASSED\r\n");


  /* Setup Reset line and video lock monitor */
  gpio_hlsIpReset = (u32*)XPAR_AXI_GPIO_0_BASEADDR;

  //Release reset line
  *gpio_hlsIpReset = 1;

  /* Initialize IRQ */
  Status = SetupInterrupts();
  if (Status == XST_FAILURE) {
    xil_printf("ERROR:: Interrupt Setup Failed\r\n");
    xil_printf("ERROR:: Test could not be completed\r\n");
    while(1);
  }

  Status = DriverInit();
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: Driver Init. Failed\r\n");
    xil_printf("ERROR:: Test could not be completed\r\n");
    while(1);
  }

  /* Enable exceptions. */
  Xil_ExceptionEnable();

  /* Setup a default stream */
  /* Color Format for the default stream is set to the mixer master stream
   * color format
   */
  XVMix_GetLayerColorFormat(&mix, XVMIX_LAYER_MASTER, &Cfmt);

  VidStream.PixPerClk     = tpg_master.Config.PixPerClk;
  VidStream.ColorFormatId = Cfmt;
  VidStream.ColorDepth    = tpg_master.Config.MaxDataWidth;

  resetIp();

  /* sanity check */
  if(XVMonitor_IsVideoLocked(&vmon)) {
    xil_printf("ERROR:: Video should not be locked\r\n");
    xil_printf("ERROR:: Test could not be completed\r\n");
    while(1);
  }

  for(index=0; index<NUM_TEST_MODES; ++index)
  {

    // Get mode to test
    VidStream.VmId = TestModes[index];

    // Get mode timing parameters
    TimingPtr = XVidC_GetTimingInfo(VidStream.VmId);
    VidStream.Timing = *TimingPtr;
    VidStream.FrameRate = XVidC_GetFrameRate(VidStream.VmId);

    xil_printf("\r\n********************************************\r\n");
    xil_printf("Test Input Stream: %s (%s)\r\n",
            XVidC_GetVideoModeStr(VidStream.VmId),
            XVidC_GetColorFormatStr(VidStream.ColorFormatId));
    xil_printf("********************************************\r\n");

    ConfigVtc(&VidStream);
    ConfigMixer(&VidStream);
    ConfigTpgMaster(&VidStream);
    ConfigTpgStream_1(&VidStream);
    ConfigTpgStream_2(&VidStream);
    ConfigTpgStream_3(&VidStream);
    ConfigTpgStream_4(&VidStream);

    xil_printf("Wait for vid out lock: ");

    Lock = CheckVidoutLock();
    if(Lock) {
      Status = RunMixerFeatureTests(&VidStream);
      if(Status != 0) { //problems encountered in feature test
        ++FailCount;
      }
    } else {
      ++FailCount;
    }

//    resetIp();
  }

  if(FailCount) {
    xil_printf("\r\n\r\nINFO: Test completed. %d/%d tests failed\r\n", FailCount, NUM_TEST_MODES);
  } else {
    xil_printf("\r\n\r\nINFO: Test completed successfully\r\n");
  }

  while(1);
}
