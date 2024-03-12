# Versal AI Engine with Vitis Design Flow
+ 此篇主要說明如何透過 Vitis 針對 Versal AI Engine 進行設計

**Last update: 2024/03/12**

+ 開啟 Vitis，並新建一 Application Project

​<img src="Images/AI1.PNG"/>

+ 選擇要在上面建立 Application 的 Platform

<img src="Images/AI2.PNG"/>

不同的 Vitis 版本有不同的 embedded platforms package，可以至以下下載

[Vitis Embedded Base Platforms](https://www.xilinx.com/support/download/index.html/content/xilinx/en/downloadNav/embedded-platforms.html)

<img src="Images/AI3.PNG"/>

+ 建立 Application Project Name

<img src="Images/AI4.PNG"/>

+ 選擇 AI Engine

<img src="Images/AI5.PNG"/>

## Power on KV(R)260
+ Set Ubuntu environment and download Xilinx package
### 1. 登入帳號和密碼為
```
Account: ubuntu
Password: ubuntu
```

### 2. 接著依序輸入以下指令進行更新和 Xilinx Package 安裝
```
sudo apt update
sudo apt upgrade
sudo add-apt-repository ppa:xilinx-apps
sudo apt update
sudo apt upgrade
sudo snap install xlnx-config --classic
sudo apt search bist
sudo apt install xlnx-firmware-kv260-bist ---> 尋找 BIST 的 firmware
```

### 3. Update the Boot Firmware (KR260 may be different)
可以到官方的 [Setting up the Board and Application Deployment](https://xilinx.github.io/kria-apps-docs/kv260/2022.1/build/html/docs/bist/docs/run.html) 查看各自不同的 Kria 系列需要更新的檔案有哪些

<img src="Images/AI1.PNG"/>

以及到 [Kria Wiki](https://xilinx-wiki.atlassian.net/wiki/spaces/A/pages/1641152513/Kria+SOMs+Starter+Kits#Boot-Firmware-Updates) 去下載相對應的檔案

<img src="Images/AI1.PNG"/>

再來需要將 Temporary failure in name resolution Error 的問題解決
```
sudo systemctl restart systemd-resolved.service ---> this command will solve this problem
```

最後則是處理 Unsupported authorisation protocol 的狀況
```
rm -rf ~/.Xaut*
sudo reboot
```

重開機之後輸入以下指令
```
xauth -v list ---> Using authority file /home/ubuntu/.Xauthority
```

若看到以下畫面就代表成功了

<img src="Images/AI1.PNG"/>

## Load KV(R)260 BIST firmware and Run the Test
### 1. 首先 unload 預先的 firmware 以及 load KV(R)260 BIST firmware
```
sudo reboot ---> to renew the latest kernel version
sudo xmutil listapps
sudo xmutil unloadapp
sudo xmutil loadapp kv260-bist
sudo modprobe ar1335 (KR260 has no ar1335)

sudo xmutil desktop_disable
sudo systemctl stop fancontrol
```

### 2. 安裝 Docker，並下載 Kria BIST 的 Docker Image
```
sudo apt-get install docker.io
sudo docker pull xilinx/kria-bist:2022.2
```

### 3. Docker 啟動
```
sudo docker run \
    --env=DISPLAY \
    --env=XDG_SESSION_TYPE \
    --net=host \
    --privileged \
    --volume=/home/ubuntu/.Xauthority:/root/.Xauthority:rw \
    -v /tmp:/tmp \
    -v /dev:/dev \
    -v /sys:/sys \
    -v /etc/vart.conf:/etc/vart.conf \
    -v /lib/firmware/xilinx:/lib/firmware/xilinx \
    -v /run:/run \
    -it xilinx/kria-bist:2022.2 bash 
```

### 4. 執行測試
```
cd /opt/xilinx/kria-bist/tests
pytest-3 --board kv260 ---> Run the entire BIST test suite for a target board
```

+ 可以個別測試項目，例如
```
pytest-3 -k display --board kv260
```
<img src="Images/AI1.PNG"/>

測試的選項可以參照官方說明的細節
[Setting up the Board and Application Deployment — Kria™ KV260 2022.1 documentation](https://xilinx.github.io/kria-apps-docs/kv260/2022.1/build/html/docs/bist/docs/run.html)

### 5. 測試結果
+ 測試成功項目
<img src="Images/AI1.PNG"/>
<img src="Images/AI1.PNG"/>

## 附錄
### 1. Temporary failure in name resolution Error
[Ubuntu 20.04 server不能ping，提示“Temporary failure in name resolution”的解决方法](https://blog.csdn.net/donaldsy/article/details/119973990)
```
sudo systemctl restart systemd-resolved.service ---> this command will solve this problem
```

### 2. Install v2022.1-09152304_update3 Boot Firmware
```
sudo xmutil bootfw_update -i <path to boot.bin>
```

### 3. Unsupported authorisation protocol

```
xauth -v list ---> Using authority file /home/ubuntu/.Xauthority
```

If incorrect or no authority file present, do the following steps:

```
rm -rf ~/.Xaut*
sudo reboot
```

<img src="Images/AI1.PNG"/>
