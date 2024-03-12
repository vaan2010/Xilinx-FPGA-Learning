# Versal AI Engine with Vitis Design Flow
+ 此篇主要說明如何透過 Vitis 針對 Versal AI Engine 進行算術運算的設計

**Last update: 2024/03/12**

### 1. 開啟 Vitis，並新建一 Application Project

​<img src="Images/AI1.PNG"/>

### 2. 選擇要在上面建立 Application 的 Platform

<img src="Images/AI2.PNG"/>

不同的 Vitis 版本有不同的 embedded platforms package，可以至以下下載

[Vitis Embedded Base Platforms](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)

<img src="Images/AI3.PNG"/>

### 3. 建立 Application Project Name

<img src="Images/AI4.PNG"/>

### 4. 選擇 AI Engine

<img src="Images/AI5.PNG"/>

### 5. Application 的 Templates 選擇 Simple
可以看到該 Templates 說明 works only for AIE Emulation and SW(x86) Emulation

主要原因為 the template does not include any I/O to feed the AIE application or any PS application which will be required for a full system running in hardware.

因此只能做軟體模擬

<img src="Images/AI6.PNG"/>

### 6. 建立好的 Application 可以看到結構如下

<img src="Images/AI7.PNG"/>

### 7. 開啟 input.txt
此份文件為 input 到 AIE 內的資料

<img src="Images/AI8.PNG"/>

### 8. 開啟 golden.txt
此份文件等於是運算完後的正確結果，可以當成是 ground truth

<img src="Images/AI9.PNG"/>

兩者放在一起比對如下

<img src="Images/AI10.PNG"/>

### 9. 接著到 src 底下，可以看到 kernels 內部檔案包含對運算過程的描述
NUM_SAMPLES 代表一次抓取來運算的資料個數

<img src="Images/AI11.PNG"/>

kernels.cc 描述了運算過程，基本上是複數的運算

<img src="Images/AI12.PNG"/>

一個被描述的 Kernel 只能在同一個 AIE 內被執行

一個 AIE 可以同時執行多個 Kernels

CINT16 代表 16-bit Real number with 16-bit Imaginary number

<img src="Images/AI13.PNG"/>

因此該算術過程整體需要的頻寬為 128 bytes

<img src="Images/AI14.PNG"/>

### 10. 開啟 project.h
裏頭定義了 kernel 之間的執行順序、設定 kernel 節點以及單個 AIE 的處理時間

<img src="Images/AI15.PNG"/>





