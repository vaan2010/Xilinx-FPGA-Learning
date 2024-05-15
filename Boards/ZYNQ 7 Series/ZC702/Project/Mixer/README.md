# ZC702 Video Mixer Output Flow
+ 此篇主要說明如何針對 ZC702 透過 ADV7511 HDMI 輸出 Video Mixer 的官方 Demo

**Last update: 2024/05/13**

+ Vivado Version: 2021.1

+ Vivado 2020.1, 2020,2, and 2021.1 有相關影像處理 IP 的 Bug 存在，像是 VPSS, Video Mixer, and Video Multi-Scaler 都可能有卡在 synthesis 不過的問題，可以參考以下連結排除：

[Vivado HLS - 2021.1 - Why does synthesis stall when using Windows OS?] (https://support.xilinx.com/s/article/Patch-AR-for-HLS-IP-patch?language=en_US)

### 0. 下載上述連結的 zip 檔案後將其解壓縮到 C 槽，並增加一環境變數後重啟 Vivado 即可解決

<img src="Images/M1.png"/>

## Build ZC702 Block Design on Vivado

### 1. (前略)參照 ZC702 HDMI Block Design，並加入 Video Mixer 與 x2 GPIO

+ ZYNQ7 Processing System

<img src="Images/M2.png"/>

<img src="Images/M3.png"/>

<img src="Images/M4.png"/>

+ Video Test Pattern Generator

<img src="Images/M5.png"/>

+ AXI4-Stream to Video Out

<img src="Images/M6.png"/>

+ Video Timing Controller

<img src="Images/M7.png"/>

<img src="Images/M8.png"/>

+ Clocking Wizard

<img src="Images/M9.png"/>

<img src="Images/M10.png"/>

+ AXI IIC

<img src="Images/M11.png"/>

+ AXI4-Stream Subset Converter

<img src="Images/M12.png"/>

+ GPIO-0

<img src="Images/M13.png"/>

+ GPIO-1

<img src="Images/M14.png"/>

+ Video Mixer

<img src="Images/M15.png"/>

+ AXI Interconnect

<img src="Images/M16.png"/>

+ 將以上 IP 開始進行連接 – 不要接 sof_state_out

<img src="Images/M17.png"/>

<img src="Images/M18.png"/>

Run Connection Automation – Clock Source 全部選 148MHz

<img src="Images/M19.png"/>

<img src="Images/M20.png"/>

<img src="Images/M21.png"/>

<img src="Images/M22.png"/>

<img src="Images/M23.png"/>

### 2. 添加 XDC 內容，可以從此[網址](https://support.xilinx.com/s/feed/0D52E00007IPcI2SAL?language=en_US)前往下載

或是你可以在 XVES_0019\src\constr\ZC702.xdc 找到檔案

記得 HDMI 輸出 Port 要跟 XDC Port 名稱一樣

<img src="Images/M24.png"/>

修改 XDC 內容

<img src="Images/M25.png"/>

<img src="Images/M26.png"/>

<img src="Images/M27.png"/>

### 3. Create HDL Wrapper & Generate Bitstream

<img src="Images/M28.png"/>

### 4. Export Hardware，這步會輸出 XSA 到 Vitis 中做 C Code 的撰寫來控制 FPGA

<img src="Images/M29.png"/>

## Build ZC702 Application on Vitis

### 5. 打開 Vitis，匯入 XSA 建立 Platform

<img src="Images/M30.png"/>

+ Platform 建立後要進行 Build 以產生連結檔

<img src="Images/M31.png"/>

### 6. 選擇 BSP 並透過官方提供的 Video Mixer Example Code 建立 Application

<img src="Images/M32.png"/>

<img src="Images/M33.png"/>

### 7. 匯入先前 ZC702 HDMI 寫入 ADI HDMI Chip 的 code

<img src="Images/M34.png"/>

### 8. 開啟 xv_mix_example.c 增加部分 code

<img src="Images/M35.png"/>

<img src="Images/M36.png"/>

+ main function

<img src="Images/M37.png"/>

<img src="Images/M38.png"/>

+ Build 有 error，發現是 GPIO 名稱跟 Vivado 不一樣，要改

<img src="Images/M39.png"/>

<img src="Images/M40.png"/>

<img src="Images/M41.png"/>

### 9. 右鍵 Application 並選擇 Run as 1 Launch Hardware，查看結果

<img src="Images/M42.png"/>

+ ZC702 硬體配置

<img src="Images/M43.png"/>

+ Result

<img src="Images/M44.png"/>

<img src="Images/M45.png"/>

<img src="Images/result.gif"/>

