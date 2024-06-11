# AiCardDemo
## 环境准备
1. Master：650N Demo PCIE-RC emmc Flash (650N默认环境);
   Slave：650N Demo  PCIE-EP NorFlash（需要修改硬件）
2. HDMI线连接Master板的HDMI 0和显示器
3. PCIe线连接Master板的PCIe 1插槽和Slave板的PCIe 0插槽
4. Master配置视频源
   4.1. 若AX内部网络，视频源存储在内部服务器通过NFS挂载，Master启动脚本会自动挂载
   4.2. 若非AX内部网络或者变更视频源，执行以下修改：
        4.2.1. 修改run.sh 将mount_video=0
        4.2.2. 修改aicard_master.conf => [STREAM]streamXX配置，填入板端可访问视频源路径
5. 一拖多(一个Master连接多个Slave)场景
   5.1. Master需在配置文件aicard_master.conf中配置Slave个数（比如，一拖二场景，[PCIE] => slave count = 2）
   5.2. 每个Slave需手动配置解码路数
   5.3. 目前仅支持多Slave平均分配，即：Slave解码路数=Master视频路数 / Slave个数
   5.4. 配置方式：修改/opt/bin/AiCard/slave/aicard_slave.conf中[VDEC] => count为5.2中计算的Slave解码路数

## 如何执行？
### 优先启动Slave
1. cd /opt/bin/AiCard/slave
2. ./run.sh
3. 如有多个Slave，按照上述方式依次启动

### 继次启动Master
1. cd /opt/bin/AiCard/master
2. ./run.sh
