#!/bin/bash
cd 
git clone https://pibot_stm32:pibot-zeus@code.aliyun.com/surewin1102/pibot_ros.git 
cd pibot_ros/
git reset --hard b0b538c76cc33af24765f1a3b3546cc857720e26
sh pibot_install_ros.sh
sudo apt-get -y --allow-unauthenticated install ros-melodic-rviz ros-melodic-rqt-reconfigure ros-melodic-rqt-tf-tree \
    ros-melodic-image-view ros-melodic-cartographer-rviz
sudo apt-get -y
sudo apt-get install -y hostapd util-linux procps iproute2 iw haveged dnsmasq
echo "命令 ： source ~/.bashrc && sh pibot_install2.sh"
