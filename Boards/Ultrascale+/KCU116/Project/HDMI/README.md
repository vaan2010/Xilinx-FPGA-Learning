# KCU116 Test Pattern Generator Flow
+ 此篇主要說明如何針對 KCU116 透過 ADV7511 HDMI 輸出 Test Pattern Generator 的圖像

**Last update: 2024/05/20**

+ Vivado Version: 2022.2.2

+ KCU116 IIC 沒有直接對到 ADV7511(HDMI) 腳位，要自己定義，其他開發板因為 IIC 有透過 IIC Switch Chip 或是直接連接到 ADV7511(HDMI) 腳位，所以可以直接用系統預設的 port

+ ADV7511(HDMI) IIC program address 是 8-bit(01110010) 0x72，但 Xilinx IIC 只有 7-bit，轉換過去要除以2 ---> 0111001 = 0x39，這個是在 Vitis 內定義的

+ 關於 ADV7511 register 燒錄的資訊已經寫在 Vitis C Code 內

+ KCU116 HDMI 共有 18-bit，但其實 [16] [17] 是無用資訊，AXI4-Streamout 選擇 YUV 422 8-bit 即可輸出 [15:0]

+ Block Design 步驟與 VC707 一樣

## Build KCU116 Block Design on Vivado

### 1. (前略)創建 Block Design，並進行 IP 設計

+ DDR4 SRAM (MIG)

<img src="Images/K1.png"/>

<img src="Images/K2.png"/>

<img src="Images/K3.png"/>

+ Video Test Pattern Generator

<img src="Images/K5.png"/>

+ AXI4-Stream to Video Out

<img src="Images/K6.png"/>

+ Video Timing Controller

<img src="Images/K7.png"/>

<img src="Images/K8.png"/>

+ Clocking Wizard

<img src="Images/K9.png"/>

<img src="Images/K10.png"/>

148.5 𝑀ℎ𝑧=(2200×1125×60)/10^6 

+ MicroBlaze

更正：Clock 請選擇 clk_wiz ---> clk_out1

<img src="Images/K4.png"/>

+ AXI IIC

<img src="Images/K11.png"/>

+ 將以上 IP 開始進行連接

<img src="Images/K12.png"/>

<img src="Images/K13.png"/>

<img src="Images/K14.png"/>

<img src="Images/K15.png"/>

<img src="Images/K16.png"/>

<img src="Images/K17.png"/>

<img src="Images/K18.png"/>

<img src="Images/K19.png"/>

<img src="Images/K20.png"/>

<img src="Images/K21.png"/>

<img src="Images/K22.png"/>

<img src="Images/K23.png"/>

<img src="Images/K24.png"/>

<img src="Images/K25.png"/>

<img src="Images/K26.png"/>

<img src="Images/K27.png"/>

<img src="Images/U0.png"/>

<img src="Images/U1.png"/>

<img src="Images/U2.png"/>

Data Width 不一致

<img src="Images/K28.png"/>

<img src="Images/K29.png"/>

我們透過增加 AXI-4 Stream Subset Converter 來解決

<img src="Images/K30.png"/>

<img src="Images/K31.png"/>

<img src="Images/K32.png"/>

### 2. 加入 Constraint，內容包括 HDMI 如何連接到 ADV7511 的腳位，可以到[網址](https://www.xilinx.com/products/boards-and-kits/ek-u1-kcu116-g.html#resources)查看 KCU116 所有的 pin assignment，或是參考該目錄下的 kcu116_hdmi.xdc

<img src="Images/K33.png"/>

<img src="Images/C1.png"/>

+ 修改 Constraint 內容以符合 Block Design 中的 port 命名

<img src="Images/K34.png"/>

<img src="Images/K35.png"/>

<img src="Images/K36.png"/>

### 3. Create HDL Wrapper & Generate Bitstream

<img src="Images/K37.png"/>

### 4. Export Hardware，這步會輸出 XSA 到 Vitis 中做 C Code 的撰寫來控制 FPGA

<img src="Images/K38.png"/>

## Build ZC702 Application on Vitis

### 5. 打開 Vitis，匯入 XSA 建立 Platform

<img src="Images/K39.png"/>

+ Platform 建立後要進行 Build 以產生連結檔

### 6. 建立 Application

<img src="Images/K40.png"/>

<img src="Images/K41.png"/>

### 7. 刪除原先的 helloworld.c，並匯入與開啟 tpg_hdmi.c

<img src="Images/K42.png"/>

### 8. ZC702 硬體配置

<img src="Images/K43.jpg"/>

### 9. 右鍵 Application 並選擇 Run as 1 Launch Hardware，查看結果

<img src="Images/K44.png"/>

<img src="Images/K45.png"/>

## Reference

+ [ZedBoard HDMI1.4 Working #Principle with FPGA](https://www.youtube.com/watch?v=BstMo5OwsjI&ab_channel=Nielfotech)

+ [Video Series 21: TPG Application on ZC702](https://support.xilinx.com/s/article/922324?language=en_US)
