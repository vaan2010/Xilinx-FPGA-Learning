#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "sleep.h"
#include "xiic_l.h"
#include "xil_io.h"
#include "xil_types.h"


#include "xv_tpg.h"
#include "xvidc.h"

#include "xvtc.h"

//#include "xvprocss.h"

#define PAGE_SIZE   16


#define IIC_BASE_ADDRESS	XPAR_IIC_0_BASEADDR

#define EEPROM_TEST_START_ADDRESS	0x80

//#define IIC_SWITCH_ADDRESS 0x72
#define IIC_ADV7511_ADDRESS 0x39

XVtc       vtc;

XV_tpg tpg;
XV_tpg_Config *tpg_config;

//XVprocSs scaler_new_inst;
//XVprocSs csc_new_inst;

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


//		//ZC702 HDMI Config
//		{ 0x41, 0x00, 0x10 },
//
//		{ 0x98, 0x00, 0x03 },
//		{ 0x9A, 0x00, 0xE0 },
//		{ 0x9C, 0x00, 0x30 },
//		{ 0x9D, 0x00, 0x61 },
//		{ 0xA2, 0x00, 0xA4 },
//		{ 0xA3, 0x00, 0xA4 },
//		{ 0xE0, 0x00, 0xD0 },
//		{ 0xF9, 0x00, 0x00 },
//
//		{ 0x18, 0x00, 0xE7 },
//
//		{ 0x55, 0x00, 0x00 },
//		{ 0x56, 0x00, 0x28 },
//		{ 0xD6, 0x00, 0xC0 },
//		{ 0xAF, 0x00, 0x04 },
//		{ 0xF9, 0x00, 0x00 },
//
//		{ 0x15, 0x00, 0x01 },//[7:4] = 2 is 48 KHz, [3:0] = 1 is YCbCr 422
//		{ 0x16, 0x00, 0x38 },
//
//		{ 0x48, 0x00, 0x08 },
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

XV_tpg tpg;

static int DriverInit(void)
{
  int Status;
  XVtc_Config *vtc_Config;

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

  Status = XV_tpg_Initialize(&tpg, XPAR_V_TPG_0_DEVICE_ID);
  if(Status != XST_SUCCESS) {
    xil_printf("ERROR:: TPG device not found\r\n");
    return(XST_FAILURE);
  }

}




void ConfigTpg() {
	XV_tpg_Initialize(&tpg, 0);
	XV_tpg_DisableAutoRestart(&tpg);
	XV_tpg_Set_height(&tpg, 720);
	XV_tpg_Set_width(&tpg, 1280);
	XV_tpg_Set_colorFormat(&tpg, XVIDC_CSF_YCRCB_422);
	XV_tpg_Set_bckgndId(&tpg, XTPG_BKGND_COLOR_BARS);
	XV_tpg_Set_ovrlayId(&tpg, 1);
	XV_tpg_Set_boxSize(&tpg, 50);
	XV_tpg_Set_motionSpeed(&tpg, 15);
	XV_tpg_EnableAutoRestart(&tpg);
	XV_tpg_Start(&tpg);
}


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

XVidC_VideoStream VidStream;


int main()
{

	XVidC_VideoTiming const *TimingPtr;
	int Status;

    init_platform();

    DriverInit();



    VidStream.PixPerClk     = tpg.Config.PixPerClk;
	VidStream.ColorFormatId = XVIDC_CSF_YCRCB_422;
	VidStream.ColorDepth    = tpg.Config.MaxDataWidth;


	// Get mode to test
	VidStream.VmId = XVIDC_VM_1920x1080_60_P;

	// Get mode timing parameters
	TimingPtr = XVidC_GetTimingInfo(VidStream.VmId);
	VidStream.Timing = *TimingPtr;
	VidStream.FrameRate = XVidC_GetFrameRate(VidStream.VmId);

	xil_printf("\r\n********************************************\r\n");
	xil_printf("Test Input Stream: %s (%s)\r\n",
			XVidC_GetVideoModeStr(VidStream.VmId),
			XVidC_GetColorFormatStr(VidStream.ColorFormatId));
	xil_printf("********************************************\r\n");


    print("Hello World\n\r");
    print("Successfully ran Hello World application");

    Status = IicLowLevelDynEeprom();
    if (Status != XST_SUCCESS) {
    	xil_printf("ADV7511 IIC programming FAILED\r\n");
    	return XST_FAILURE;
      }
    xil_printf("ADV7511 IIC programming PASSED\r\n");



    print("----------------------------------\n\r");
	print(" ADV7511 HDMI Output Demo\n\r");
	print("----------------------------------\n\r");

	print("\n\r");


	ConfigVtc(&VidStream);
	ConfigTpg();

	print("TPG Configuration\n\r");

//	InitVprocSs_CSC();


	return 0;
}
