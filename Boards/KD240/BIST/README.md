# Built-In Self Test Flow
+ 此篇主要說明如何針對KD240進行周邊功能上的官方測試

**Last update: 2024/02/29**

## Download Ubuntu 22.04
可以從此網址下載檔案
[Install Ubuntu on AMD | Ubuntu](https://ubuntu.com/download/amd)

​<img src="Images/Ubuntu 22.04.png" width="600" height="360"/>

## Setting Up the SD-Card Image 
使用 [balenaEtcher](https://etcher.balena.io/) 將剛剛下載的檔案燒錄到SD-Card內

也可以參考官方步驟來燒錄

[Setting up the SD Card Image (xilinx.com)](https://www.xilinx.com/products/som/kria/kd240-drives-starter-kit/kd240-getting-started/setting-up-the-sd-card-image.html)

<img src="Images/Program SD Card.png"/>

依照下圖方式插入到 KD240 的 SD Card 槽中

<img src="Images/KD240 Hardware Setting.png"/>

## Power on KD240

依照下圖上電後開啟 [MobaXtern](https://mobaxterm.mobatek.net/download.html) 與 KD240 透過 UART 溝通

<img src="Images/Uart communication.png"/>
