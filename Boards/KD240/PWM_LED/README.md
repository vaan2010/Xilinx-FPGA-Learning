# KD240 PWM LED
+ 此篇主要說明如何在 KD240 上實現呼吸燈

**Last update: 2024/03/07**
- [x] **使用 Vivado & Vitis Design Flow 達成**
- [x] **使用 PYNQ Design Flow 達成**

## Vivado Part
## 自定義 PWM IP
+ Vivado 內部在頂端選單的 Tools 擁有新建及打包 IP 的選項，接下來此段落會介紹如何將 PWM IP 加入到 IP Catalog 中供日後使用
1. 於 Vivado 內選取 Create and Package New IP
​<img src="Images/IP1.png"/>

2. 前面設定都直接點選 Next 即可
<img src="Images/IP2.png"/>

3. 設定 PWM IP，會開一個暫時的 project 來設定 IP
<img src="Images/IP3.png"/>

4. 加入 PWM RTL Code，Source Code－ax_pwm.v
<img src="Images/IP4.png"/>
<img src="Images/IP5.png"/>

5. 修改 AXI Top RTL Code 來引入剛剛的 PWM RTL Code
+ pwm_v1_0_S00_AXI.v
<img src="Images/IP6.png"/>
<img src="Images/IP7.png"/>

+ pwm_v1_0.v
<img src="Images/IP8.png"/>
<img src="Images/IP9.png"/>

+ 須注意 AXI Reset 是為 0 時 initial register value，這點要跟 PWM Code 一致
<img src="Images/IP10.png"/>

6. Merge Changes and Re-Package IP
<img src="Images/IP11.png"/>

## Create Block Design－開始於 Vivado 內的 Block Diagram 中將 IP 以 GUI 形式 import 進來
1. 呼叫 ZYNQ IP
<img src="Images/IP12.png"/>

2. 呼叫 PWM IP
<img src="Images/IP13.png"/>

3. 將 PWM IP 輸出 port 定義為外部接口
<img src="Images/IP14.png"/>

4. 透過 Vivado 的 Connection Automation 自動將 IP 透過 AXI Bus 連接在一起
<img src="Images/IP15.png"/>

5. 驗證設計，確定沒有問題
<img src="Images/IP16.png"/>

6. 添加 XDC 內容
<img src="Images/IP17.png"/>
<img src="Images/IP18.png"/>
<img src="Images/IP19.png"/>

XDC 內容可與 KD240 電路圖對照
<img src="Images/IP20.png"/>

## Generate Bitstream－產生燒錄檔並 Export 硬體資訊給 Vitis
1. Generate Bitstream，處理時間依電腦性能不同而有所差異
<img src="Images/IP21.png"/>

2. Export Hardware，會生成 .xsa 檔案提供 Vitis import
<img src="Images/IP22.png"/>

3. 自定義 IP 要改 xsa 內容，透過 winrar 開啟 xsa 並點選以下路徑開啟 Makefile
<img src="Images/IP23.png"/>

4. 修改以下內容後儲存
<img src="Images/IP24.png"/>




