# Versal AI Engine with Vitis Design Flow
+ 此篇主要說明如何透過 Vitis 針對 Versal AI Engine 進行算術運算的設計

**Last update: 2024/03/12**
- [ ] Vitis Versal AIE API
- [ ] Run Model on Versal AIE

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

+ 先從第一部份來說明

first 和 second 分別被定義成了兩個 Kernels

並且還定義了 input port 和 output port

初規劃的 Graph 如下

<img src="Images/AI16.PNG"/>

+ 第二部分則定義了資料流的方向與 Kernel 內所要執行的 Function 是哪一隻(Simple)

<img src="Images/AI17.PNG"/>

+ 第三部分將要 import 的 kernel function 利用 source 代入進來，並設定 AIE 處理時間比例

<img src="Images/AI18.PNG"/>

關於 Runtime Ratio 這塊的說明，在官方原文教學 [AI Engine Series 8](https://support.xilinx.com/s/article/1212243?language=zh_CN) 中有提到以下這段
```
It is important to note that the run-time ratio does not schedule the execution of the kernels.

The kernel will be fired as soon as the data is available and the core is not already running another kernel. Even if the run-time ratio is set to 0.1 for a kernel, the kernel might be running 100% of the time on the AI Engine core (assuming there is only one kernel mapped to the core). In our case, because the two kernels are running the same functions, they might be running 50% of the time with the run-time ratio set to 0.1 or set to 0.4.
```

<img src="Images/AI19.PNG"/>

```
We can now try to increase the run-time ratio for the two kernels to 0.6.

When we look at the graph view we can see that the usage has now changed:

Each kernel is mapped to a different tile.
```

<img src="Images/AI20.PNG"/>

### 11. 開啟 project.cpp
內容敘述了實體 data 的 input 和 output 為何種形式(.txt)

並且定義之前步驟中規畫好的 Graph 需要運行幾次

<img src="Images/AI21.PNG"/>

### 12. Build and Run Project

<img src="Images/AI22.PNG"/>

<img src="Images/AI23.PNG"/>

透過 Vitis 內選項檢驗運算完的結果是否與 Ground Truth 相同

<img src="Images/AI24.PNG"/>

+ 有差異
<img src="Images/AI25.PNG"/>

+ 無差異
<img src="Images/AI26.PNG"/>

### 13. 確認 Kernel 被擺放至 AIE 的位置與實際生成 Graph
我們可以點選 Emulation-AIE 中的 project.aiecompile_summary 這個檔案，會自動生成另一視窗為 Vitis Analyzer

<img src="Images/AI27.PNG"/>

+ Graph

<img src="Images/AI28.PNG"/>

<img src="Images/AI29.PNG"/>

<img src="Images/AI30.PNG"/>

### 14. 確認 AIE 執行時每個動作的執行順序與花費時間
先點選 simple_test 的 C/C++ Build Settings

<img src="Images/AI31.PNG"/>

再將 Kernel optimization 關閉，為了查看最原始的狀態

<img src="Images/AI32.PNG"/>

回到 simple_test，點選 Run Configurations

<img src="Images/AI33.PNG"/>

勾選 Generate Trace，並也把 VCD 打勾

VCD 全名為 Value Change Dump，可以記錄 AIE 運行時的事件

<img src="Images/AI34.PNG"/>

設定結束後點選 Run，會產生出新檔案－default.aierun_summary

<img src="Images/AI35.PNG"/>

雙擊 default.aierun_summary 後一樣會跑出 Vitis Analyzer，可以發現多了 Trace 選項

<img src="Images/AI36.PNG"/>

我們可以透過以下計算式得出單個 Function(Simple) 在運行時所需要的 Frequency 與 Run Cycles

<img src="Images/AI37.PNG"/>

同樣的，我們也可以依照此算出單個 AIE 的 Cycle Budget，等於是 Function 必須在該 Cycle Budget 內執行完畢，不然就必須拆分成兩個 Function 去對應兩個 Kernels，再交由額外的 AIE 來完成運算
```
Run-time ratio = (cycles for one run of the kernel)/(cycle budget)
Cycle Budget = Block Size * (AI Engine Clock Frequency/Sample Frequency)
For example, take a kernel which processes a window of 128 samples and the input samples frequency (for example from an ADC) is 245.76MHz.
Cycle Budget is 128*(1000/245.76) = 520 cycles
```

當然多個 AIE 執行 Function 會直接影響的就是運行時間，下圖左為多個 Kernels 在同一 AIE 內，圖右為多個 Kernels 搭配至多個 AIE

<img src="Images/AI38.PNG"/>

AIE 在讀取資料時分成兩種形式，Window Based Access 與 Stream Based Access

Window Based Access 主要會去讀取 AIE 中的 local memory 資料，或是鄰近 AIE 的 local memory 資料，因此在 Kernel 要進行運算前，會需要等待這些 local memory 將資料填滿，造成初始會有些許 delay 發生

Stream Based Access 則是會透過 AXI-Stream 來去與外部獲取資料，但會有若一 Kernel 處理資料速度沒有達到預期，下游的 Kernel 就會有短暫等待時間發生 

<img src="Images/AI39.PNG"/>

