1. xsct要source vitis

---------------------------------------
self-drive car
1:90

robot_ap
pibot_env_inti
pibot_gmapping
pibot_control
---------------------------------------
change board ---> xpfm regenerated
xpfm cause sd_card.img problem

---------------------------------------
Device Tree
device tree source
device tree generator
device tree compiler
device tree binary

DTG 會從 XSA 檔生成 DTS 文件
而 DTC 會把 DTS 文件編譯成二進制檔 DTB
DTB 主要用途為提供給 Linux kernel 來確保 Linux 可以成功初始化成特定的硬體結構

What is Device Tree?
Device Tree 就是描述硬體數據結構的文件，說明了可由 Linux 等 OS 讀取的硬體，因此不用透過對 computer 的詳細資訊再進行 Hardware encode

petalinux-config --get-hw-description=.xsa
利用petalinux上面的command來吃Vivado設計出來的xsa檔，來讓petalinux自動產生相對應的device tree
但是，device tree generator並不會將custom ip加入到device tree中，必須手動在user dtsi中加入

