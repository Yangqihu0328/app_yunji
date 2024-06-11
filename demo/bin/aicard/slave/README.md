# AiCardDemo
## 如何执行？
1. cd /opt/bin/AiCard/slave
2. vi aicard_slave.conf 配置解码通道数以及智能算法检测开关等。[参数说明](#配置参数)
3. 执行./run.sh

## 如何编译？
1. cd app/demo/src/ppl/aicard
2. make p=xxx clean
3. make p=xxx
4. make p=xxx install
> p=xxx 指定编译项目名，示例：make p=AX650_emmc

# <a href="#配置参数">配置参数</a>

|   #   |             参数       |   参数范围   |                          说明           |
| ----- | ----------------------| ------------ | --------------------------------------- |
|   1   | [VDEC]count           |              | 码流数量                                 |
|   2   | [DETECT]enable        | [1 - 32]     | 0：不检测 1:检测                         |