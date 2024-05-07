# ZC702 Test Pattern Generator Flow
+ 此篇主要說明如何針對 ZC702 透過 ADV7511 HDMI 輸出 Test Pattern Generator 的圖像
+ 包含解析度的轉換功能

**Last update: 2024/05/06**

+ Vivado Version: 2021.1

+ 該流程是參照 [Xilinx Video Series](https://support.xilinx.com/s/question/0D52E00006hpsS0SAI/xilinx-video-series-and-blog-posts?language=en_US) 做為修改

## Build ZC702 Block Design on Vivado

### 1. (前略)創建 Block Design，並進行 IP 設計

+ ZYNQ7 Processing System

<img src="Images/TPG1.png"/>

+ Video Test Pattern Generator

<img src="Images/TPG2.png"/>

+ AXI4-Stream to Video Out

<img src="Images/TPG3.png"/>

+ Video Timing Controller

<img src="Images/TPG4.png"/>

<img src="Images/TPG5.png"/>

+ Clocking Wizard

<img src="Images/TPG6.png"/>

<img src="Images/TPG7.png"/>

148.5 𝑀ℎ𝑧=(2200×1125×60)/10^6 

+ AXI IIC

<img src="Images/TPG8.png"/>

+ 將以上 IP 開始進行連接

<img src="Images/TPG9.png"/>

<img src="Images/TPG10.png"/>

<img src="Images/TPG11.png"/>

<img src="Images/TPG12.png"/>

<img src="Images/TPG13.png"/>

<img src="Images/TPG14.png"/>

<img src="Images/TPG15.png"/>

<img src="Images/TPG16.png"/>

<img src="Images/TPG17.png"/>

<img src="Images/TPG18.png"/>

<img src="Images/TPG19.png"/>

Data Width 不一致

<img src="Images/TPG20.png"/>

<img src="Images/TPG21.png"/>

我們透過增加 AXI-4 Stream Subset Converter 來解決

<img src="Images/TPG22.png"/>

<img src="Images/TPG23.png"/>

<img src="Images/TPG24.png"/>

<img src="Images/TPG25.png"/>

<img src="Images/TPG26.png"/>

### 2. 加入 Constraint，內容包括 HDMI 如何連接到 ADV7511 的腳位，可以下載該目錄下的 zip，並在 XVES_0019\src\constr\ZC702.xdc 找到 XDC 檔案

<img src="Images/TPG27.png"/>

+ 修改 Constraint 內容以符合 Block Design 中的 port 命名

<img src="Images/TPG28.png"/>

### 3. Create HDL Wrapper & Generate Bitstream

<img src="Images/TPG29.png"/>

### 4. Export Hardware，這步會輸出 XSA 到 Vitis 中做 C Code 的撰寫來控制 FPGA

<img src="Images/TPG30.png"/>

## Build ZC702 Application on Vitis

### 5. 打開 Vitis，匯入 XSA 建立 Platform

<img src="Images/TPG31.png"/>

+ Platform 建立後要進行 Build 以產生連結檔

<img src="Images/TPG32.png"/>

### 6. 建立 Application，並匯入 zip 內 src 的檔案

<img src="Images/TPG33.png"/>

<img src="Images/TPG34.png"/>

<img src="Images/TPG35.png"/>

### 7. 刪除原先的 helloworld.c，並開啟 tpg_hdmi_zc702.c 加入下圖框選的 Code

<img src="Images/TPG36.png"/>

### 8. ZC702 硬體配置

<img src="Images/TPG37.png"/>

### 9. 右鍵 Application 並選擇 Run as 1 Launch Hardware，查看結果

<img src="Images/TPG38.png"/>

<img src="Images/TPG39.png"/>

# ZC702 Test Pattern Generator Output Multiple Resolution Flow 

## Modify IP on Vivado

### 1. 在前述的 Block Design 中，進行 IP 的設定修改

+ Video Timing Controller－開啟 AXI-4 Lite 是為了讓 Vitis 中的 C Code 能夠控制該 IP

<img src="Images/TPG40.png"/>

+ Clocking Wizard

開啟 Dynamic Reconfig 才能透過 Vitis 從 AXI-4 Lite 進行 Clock 更新

<img src="Images/TPG41.png"/>

勾選想要輸出不同解析度對應的 Clock 數值

25.2 𝑀ℎ𝑧=480𝑃

74.25 𝑀ℎ𝑧=720𝑃

148.5 𝑀ℎ𝑧=1080𝑃

<img src="Images/TPG42.png"/>

開啟 MMCM Override

<img src="Images/TPG43.png"/>

記清楚以下除頻數值，Vitis 會用到

<img src="Images/TPG44.png"/>

回來取消其他兩個 output clock，記得去修改 VTC 對應的解析度

<img src="Images/TPG45.png"/>

+ 將以上 IP 開始進行連接

<img src="Images/TPG46.png"/>

<img src="Images/TPG47.png"/>

### 2. 與先前一樣，產生 Bitstream 與匯出 XSA

## Build ZC702 Application on Vitis

### 3. Update 之前建立好的 Platform，來匯入新的 XSA，一樣要重 Build

<img src="Images/TPG48.png"/>

### 4. 建立 Application，一樣匯入先前修改好的 Code，並在 tpg_hdmi_zc702.c 中再修改以下

<img src="Images/TPG49.png"/>

若未換解析度的話，Clocking Wizard Configuration 那兩行要註解掉

<img src="Images/TPG50.png"/>

若未換解析度的話，VTC Configuration 那三行要註解掉

<img src="Images/TPG51.png"/>

注意，單純不更改原始解析度或 Clock 速度不需要 ReConfig，但有 AXI-4 Lite 就需要在 Vitis 內 Initialize 該 IP 以及 Enable 才會動作

### 5. Run as 1 Launch Hardware，查看結果

+ 480P

<img src="Images/TPG52.png"/>

+ 720P

<img src="Images/TPG53.png"/>

<img src="Images/TPG54.png"/>

<img src="Images/TPG55.png"/>

+ 1080P

<img src="Images/TPG56.png"/>

<img src="Images/TPG57.png"/>

<img src="Images/TPG58.png"/>

## Reference

+ [ZedBoard HDMI1.4 Working #Principle with FPGA](https://www.youtube.com/watch?v=BstMo5OwsjI&ab_channel=Nielfotech)

+ [Video Series 21: TPG Application on ZC702](https://support.xilinx.com/s/article/922324?language=en_US)
