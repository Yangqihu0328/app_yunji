# aibox应用（阶段三，问题收敛）

## 问题

| 问题列表                 | 完成情况 | 描述                    |
| ------------------------ | -------- | ----------------------- |
| h265实时画面预览         | done     | 已修复能正常出图        |
| 算法添加                 | done     | 11.23预期完成           |
| 转码卡的需求开发         | done     | 11.23预期完成，下周测试 |
| 内存泄漏问题             | done     | 11.23排查出问题         |
| 前端                     | done     | 问题全部解决            |
| rtsp推流卡顿，画面不稳定 | todo     | 下周中旬开始排查        |
|                          |          |                         |


# aibox应用（阶段二）

## 问题

| 问题列表                                         | 完成情况 | 描述                                                    |
| ------------------------------------------------ | -------- | ------------------------------------------------------- |
| 流媒体服务器库编译成功，完成转发到webrtc         | done     | 编译成功并且提交到仓库                      |
| 整体代码流程是否能跑通，venc，jpeg, ai，mqtt流程 | done     | mqtt和venc流程验证通过,ai和jpeg流程都通过，徐工已验证。 |
| ai没有检测结果                                   | done     | 没有绑定算法通道，已解决                                |
| 开启多通道，其他路没有算法结果                   | done     | 完成                                                    |
| 开启多通道，只有一路编码图片                     | done     | 完成                                                    |
| 动态添加视频流                                   | done     | 完成                                                    |
| mqtt添加算法任务逻辑                             | done     | 完成                                                    |
| 算法任务没有动态开启                             | done     | 主流程没有动态开启                                      |
|                                                  |          |                                                         |
| 杨工                                             |          |                                                         |
| 同步电脑的时间，RTC操作                          | done    | 11.9前完成                                              |
| 重启设备逻辑验证                                 | done    | 11.9前完成                                              |
| 发告警信息验证，前端弹框                         | doing    | 11.16前完成                                             |
| 网络配置页面实现                                 | done    | 11.16前完成                                             |
| 保存成文件名，任务，通道，时间修改               | doing    | 11.20前完成                                             |
|                                                  |          |                                                         |
| 吴工，徐工                                       |          |                                                         |
| 算法告警存储逻辑修改                             | done     | 11.13前完成                                             |
| 任务管理，等aibox底层。                          | done     | 11.13前完成                                             |
| 算法增加两个移植                                 | doing    | 11.23前完成                                             |
| 声音移植                                         | done     | 11.16前完成                                             |



# aibox应用（阶段一）

# demo

### 1.输入（页面一）

搞懂实施流程，参考NVR不需要拉流，直接获取推流地址。原来的SDK功能不需要改动，输入来源网页配置下来的。网页配置流地址通过mqtt下发到板子中。

| 任务进展                                                     | 截止时间 | 进度 |
| ------------------------------------------------------------ | -------- | ---- |
| 基础功能1：同步aicard-master目录的mqtt代码到box，解决编译问题。 | 10.17    | 完成 |
| 基础功能2：mqtt转发服务器搭建，完成多进程的基本的收发通信    | 10.19    |      |
| 业务逻辑1：json数据结构的定义，参考锐景实现                  | 10.23    |      |
| 业务逻辑2：维护动态增加和删除视频流地址                      | 10.26    |      |
| 业务逻辑3：维护每一路视频通路如何绑定1-3个算法的数据结构     | 10.30    |      |
| 业务逻辑4：主界面的获取时间，内存，tpu利用率等信息以及发下控制重启和重启服务器命令给板子 | 11.2     |      |

1.基础功能1-移植master代码到box

已完成编译问题

2.基础功能2-安装转发服务器

```
curl -s https://assets.emqx.com/scripts/install-emqx-deb.sh | sudo bash
sudo apt-get install emqx
sudo systemctl start emqx

sudo systemctl emqx start    #启动
sudo systemctl emqx stop     #停止
sudo systemctl emqx restart  #重启 
可以通过：本机ip/18083进入到网页
用户名：admin，密码：public
```

mqtt通过tcp连接总是失败。原来是ip地址写错了，写多了双引号

发送的消息无法接收，需要重启程序，用户名的账号和密码不需要填

但是客户端的ID不能修改，否则无法使用。

3.业务逻辑1：json数据结构的定义，参考锐景实现

如果没开启mqtt转发服务器，定时器无法使用，已修复

4和5业务逻辑：已完成代码编写，准备实现编译

使用媒体通道表示绑定的通道号，流地址，流状态

使用算法任务表示绑定的任务号，流地址，算法个数和类型

暂时使用16路算法，因为最大编码数量为16。锐景能做到18路视频流，说明20路以上也可以。

但是要考虑到一些内存的限制和算法数量问题。



mount -t nfs -o nolock 192.168.3.39:/home/zhenjie/complier/nfs_work /mnt

export LD_LIBRARY_PATH=/mnt/:$LD_LIBRARY_PATH



### 2.AI分析（页面二）

网页要配置任务通道，流地址确认ok之后就开启算法分析。

| 任务进展                                                     | 截止时间 |
| ------------------------------------------------------------ | -------- |
| 基础功能1：18路实现视频流动态开始停止以及增加和删除          | 11.9     |
| 业务逻辑1：每路视频根据算法的配置实现开启多个算法串行运算（可以同时1-3个算法使用） | 11.13    |
| 业务逻辑2：算法sdk实现多个算法都有跟踪效果。                 | 11.16    |
| 业务逻辑3：每路的每个算法都要保存相应的结果以及绑定对应的视频帧，维护相应的数据结构 | 11.23    |

基础功能1：

已经延续奇虎大哥的写法，先把系统启动，再通过mqtt，添加和删除视频流。也是保存文件，下次启动可用

新知识：

如果确定链接相应的库，并且通过nm -D命令确定库里面有函数定义。但还是编译报错提示找不到函数。

因为是C++调用C来实现，可以用extern "C" 来包含C文件，来确保能否正常调用。成功解决问题。



业务逻辑1和2已经完成，需要针对具体的算法实现。

业务逻辑3也是已经完成。



### 3.输出

算法分析出结果进行画框，画框之后，SDK能输出到HDMI中。

web预览功能，配合流媒体服务器和本地rtsp转发，实现web预览。

| 任务进展                                                     | 截止时间 |
| ------------------------------------------------------------ | -------- |
| 基础功能1：完善多算法画框结果输出到显示屏中                  | 11.27    |
| 基础功能2：在完善流媒体服务器能正常转发webrtc流              | 11.30    |
| 业务逻辑1：移植视频编码，能正常对画框之后的视频流进行编码    | 12.4     |
| 业务逻辑2：完成图片编码，对告警图片进行编码并且传到web中     | 12.7     |
| 业务逻辑3：音频输出：视频音频硬件驱动控制，保存音频文件然后进行播放 | 12.11    |

1.画完框之后需要进行编码和保存图片。完成。

适配编码任务完成，还需要移植保存图片以及RTSP流发送。

2.搭建流媒体服务器。未完成。

这个比较简单，就是编译运行即可。

3.多算法结果正常画框。完成



遇到问题：

1.没看懂rtsp是如何取流并且推流

原来是在CAXRtspObserver注册和接受数据以及调用类的instance进行注册。

2.输出按按照屏幕输出

stream -- ffmpeg(de-muxer) -- vdec -- ai/dispatchO  -- dispO -- hw_bingding vo -- venc  -- muxer

按照这个pipeline，所有视频都会整合在输出，而不是能单个输出，虽然节省带宽。

但是无法实现对每一路视频进行推理预览显示。正确的方法应该是按照每一路都进行编码。

但是这个RTSP流是否输出可以通过前端来控制，流媒体服务器是不需要控制的。
