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


