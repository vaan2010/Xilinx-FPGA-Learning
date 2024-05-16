# ZCU104(MPSoC) DP TPG Output Example
+ 此篇主要說明如何針對 ZCU104 透過 PS 端的 Display Port 來輸出 TPG 的畫面
+ PS 端的 Display Port 只支援到 4K@30HZ

**Last update: 2024/05/15**

+ Vivado Version: 2021.1

## Build ZCU104 Block Design on Vivado

### 1. 針對每個 IP 進行設置並連接

+ Zynq MPSoC PS 

<img src="Images/DP1.png"/>

<img src="Images/DP2.png"/>

+ Video Test Pattern Generator

<img src="Images/DP3.png"/>

+ AXI4-Stream to Video Out

<img src="Images/DP6.png"/>

+ Video Timing Controller

<img src="Images/DP4.png"/>

<img src="Images/DP5.png"/>

+ Clocking Wizard

<img src="Images/DP7.png"/>

<img src="Images/DP8.png"/>

+ 將以上 IP 開始進行連接

<img src="Images/DP9.png"/>

<img src="Images/DP10.png"/>

<img src="Images/DP11.png"/>

<img src="Images/DP12.png"/>

<img src="Images/DP13.png"/>

<img src="Images/DP14.png"/>

+ 新增 AXI Interconnect IP

<img src="Images/DP15.png"/>

Run Connection Automation

<img src="Images/DP16.png"/>

<img src="Images/DP17.png"/>

<img src="Images/DP18.png"/>

<img src="Images/DP19.png"/>

<img src="Images/DP20.png"/>

### 2. Create HDL Wrapper & Generate Bitstream

<img src="Images/DP21.png"/>

### 4. Export Hardware，這步會輸出 XSA 到 Vitis 中做 C Code 的撰寫來控制 FPGA

<img src="Images/DP22.png"/>

## Build ZCU104 Application on Vitis

### 5. 打開 Vitis，匯入 XSA 建立 Platform

<img src="Images/DP23.png"/>

+ Platform 建立後要進行 Build 以產生連結檔

<img src="Images/DP24.png"/>

### 6. 這邊要開兩個 Application，一個是 Hello World，一個是 dpdma 的 example code

<img src="Images/DP25.png"/>

<img src="Images/DP26.png"/>

### 7. 到 Hello Word 的專案下，匯入 dpdma example code

<img src="Images/DP27.png"/>

<img src="Images/DP28.png"/>

<img src="Images/DP29.png"/>

### 8. 開啟 xdpdma_video_example.c 和 xdpdma_video_example.h 更改 code

<img src="Images/DP30.png"/>

<img src="Images/DP31.png"/>

<img src="Images/DP32.png"/>

<img src="Images/DP33.png"/>

+ 因為是 1920x1080，記得修改一下 code 如下

<img src="Images/DP34.png"/>

+ Build & Run

<img src="Images/DP35.png"/>

### 9. 右鍵 Application 並選擇 Run as 1 Launch Hardware，查看結果

<img src="Images/DP36.png"/>

<img src="Images/DP37.png"/>

## Reference

+ [4K TPG Video Streaming in Kria KV260](https://www.hackster.io/nikilthapa/4k-tpg-video-streaming-in-kria-kv260-baremetal-part-1-c0c9d6)

+ [MicroZed Chronicles: DisplayPort Controller ](https://www.hackster.io/news/microzed-chronicles-displayport-controller-part-one-25734db13fad)

+ [Ultra96V2_DisplayPort](https://github.com/ATaylorCEngFIET/Ultra96V2_DisplayPort/tree/master/display_port/display_port.sdk/dispport/src)

