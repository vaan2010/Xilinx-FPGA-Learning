# Vitis AI Lab 1 & 2
+ 此篇主要介紹官方對於 Vitis AI 提出的 Lab1 與 LAB2

**Last update: 2024/03/14**

## Environment Setup
### 1. 可以安裝 VMware 或是在獨立電腦上安裝 Ubuntu 20.04
 
### 2. 安裝 Docker

+ Install Docker
```
sudo apt-get install docker.io
```

+ Check Docker installation
```
sudo docker --version
```

+ Remove typing ‘sudo’ before docker
```
sudo chmod 666 /var/run/docker.sock
```

### 3. 安裝 Vitis AI 2.0

+ Download Vitis AI v2.0 from github
```
git clone https://github.com/Xilinx/Vitis-AI.git -b v2.0
```

+ Download Vitis AI Docker Image
```
docker pull xilinx/vitis-ai:2.0.0
```

+ Activate Vitis AI Docker Environment
```
./docker_run.sh 2.0.0
```

<img src="Images/vitis1.png"/>

## Lab 1: AI Quantizer and AI Compiler – Caffe

check the tutorial on “AI Quantizer and AI Compiler – Caffe.docx”

### Result

<img src="Images/vitis2.png"/>

## Lab 2: AI Quantizer and AI Compiler – TensorFlow2 and PyTorch

check the tutorial on “AI Quantizer and AI Compiler – TensorFlow2 and PyTorch”

### Result

<img src="Images/vitis3.png"/>

<img src="Images/vitis4.png"/>
