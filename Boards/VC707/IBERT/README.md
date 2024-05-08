# VC707 IBERT Flow
+ 此篇主要說明如何針對 VC707 進行 IBERT 測試並產生眼圖

** Last update: 2024/05/08**

** 注意: VC707 Debug Core 只支援到最高 Vivado 2019.2 版本**

** 可參考目錄下官方文件－xtp210-vc707-ibert-c-2015-1.pdf**

## Vivado 2015.1

### 1. Open Vivado

Create New Project

<img src="Images/IB1.png"/>

Click Next

<img src="Images/IB2.png"/>

Set the Project name and location to ibert_bank_113 and  C:/vc707_ibert; check Create project subdirectory

<img src="Images/IB3.png"/>

Select RTL Project – Select Do not specify sources at this time

<img src="Images/IB4.png"/>

Select the VC707 Board

<img src="Images/IB5.png"/>

Click Finish

<img src="Images/IB6.png"/>

### 2. Build IBERT Design

Click on IP Catalog

<img src="Images/IB7.png"/>

Select IBERT 7 Series GTX, v3.0 under Debug & Verification

<img src="Images/IB8.png"/>

Right click on IBERT 7 Series GTX and select Customize IP

<img src="Images/IB9.png"/>

Set the Component name: ibert_bank_113  

Under the Protocol Definition tab

+ Silicon Version: General ES / Production

+ Protocol: LineRate: 10.000, DataWidth: 40 Refclk: 125.000 Quad Count: 1

<img src="Images/IB10.png"/>

Under the Protocol Selection tab  

Set QUAD_113 to

+ Custom 1 / 10.000 Gbps, and MGTREFCLK0 113

<img src="Images/IB11.png"/>

Under the Clock Settings tab, 

set the System Clock:

+ LVDS, P Pin Location: E19, N Pin Location: E18

<img src="Images/IB12.png"/>

Review the summary and click OK

<img src="Images/IB13.png"/>

Click Generate

<img src="Images/IB14.png"/>

Bank 113 IBERT design appears in Design Sources

<img src="Images/IB15.png"/>

Right click on ibert_bank_113 and select Open IP Example Design

<img src="Images/IB16.png"/>

Set the location to C:/vc707_ibert/ibert_bank_113 and click OK

<img src="Images/IB17.png"/>

A new project is created and Click Generate Bitstream

<img src="Images/IB18.png"/>

Program the generated bitstream and Test IBERT performance

<img src="Images/IB19.png"/>

<img src="Images/IB20.png"/>

<img src="Images/IB21.png"/>

<img src="Images/IB22.png"/>

<img src="Images/IB23.png"/>

<img src="Images/IB24.png"/>

<img src="Images/IB25.png"/>

## APPENDIX

### 1. Follow the board IBERT tutorial on Xilinx Website.

<img src="Images/IB26.png"/>

### 2. Check the board schematic (notice the difference version cause difference pin assignment).

<img src="Images/IB27.png"/>

### 3. Check the correct system clock and MGT Quad for testing.

<img src="Images/IB28.png"/>

### 4. Notice that additional FMC Card if needed for IBERT testing.

<img src="Images/IB29.png"/>
