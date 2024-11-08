#pragma once

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <ctime>

#include "BoxConfig.hpp"
#include "IObserver.h"
#include "ax_global_type.h"
#include "AXThread.hpp"
#include "MQTTClient.h"
#include "AXLockQ.hpp"


extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

namespace boxconf {
struct MemoryInfo{
    long totalMem;
    long freeMem;
    long usedMem;
    long availableMem;
    long buffers;
    long cached;
};

struct FlashInfo{
    long total;
    long used;
    long free;
};

struct TpuInfo{
    long total;
    long used;
    long free;
};

struct AlgoInfo{
    std::string name;
    int index;
};

//我希望前端限制传入的channel_id为顺序的，从0-15，而不是乱序的。
struct MediaChanel{
    int id; //通道id是前端传下来的
    std::string url; //流地址
    std::string channel_status; //输出通道信息
};


struct AlgoTask{
    int id;
    std::string url;
    std::string channel_status;
    int algo_index0;
    int algo_index1;
    int algo_index2;
};

class IPStack
{
public:
  IPStack()
  {
		signal(SIGPIPE, SIG_IGN);
  }

  int connect(const char* hostname, int port)
  {
		int type = SOCK_STREAM;
		struct sockaddr_in address;
		int rc = -1;
		sa_family_t family = AF_INET;
		struct addrinfo *result = NULL;
		struct addrinfo hints = {0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL};

		if ((rc = getaddrinfo(hostname, NULL, &hints, &result)) == 0)
		{
			struct addrinfo* res = result;

			/* prefer ip4 addresses */
			while (res)
			{
				if (res->ai_family == AF_INET)
				{
					result = res;
					break;
				}
				res = res->ai_next;
			}

			if (result->ai_family == AF_INET)
			{
				address.sin_port = htons(port);
				address.sin_family = family = AF_INET;
				address.sin_addr = ((struct sockaddr_in*)(result->ai_addr))->sin_addr;
			}
			else
				rc = -1;

			freeaddrinfo(result);
		}

		if (rc == 0)
		{
			mysock = socket(family, type, 0);
			if (mysock != -1)
			{
				rc = ::connect(mysock, (struct sockaddr*)&address, sizeof(address));
			}
		}

        return rc;
    }

  // return -1 on error, or the number of bytes read
  // which could be 0 on a read timeout
  int read(unsigned char* buffer, int len, int timeout_ms)
  {
		struct timeval interval = {timeout_ms / 1000, (timeout_ms % 1000) * 1000};
		if (interval.tv_sec < 0 || (interval.tv_sec == 0 && interval.tv_usec <= 0))
		{
			interval.tv_sec = 0;
			interval.tv_usec = 100;
		}

		setsockopt(mysock, SOL_SOCKET, SO_RCVTIMEO, (char *)&interval, sizeof(struct timeval));

		int bytes = 0;
    int i = 0; const int max_tries = 10;
		while (bytes < len)
		{
			int rc = ::recv(mysock, &buffer[bytes], (size_t)(len - bytes), 0);
			if (rc == -1)
			{
        if (errno != EAGAIN && errno != EWOULDBLOCK)
          bytes = -1;
        break;
			}
			else
				bytes += rc;
      if (++i >= max_tries)
        break;
      if (rc == 0)
        break;
		}
		return bytes;
  }

  int write(unsigned char* buffer, int len, int timeout)
  {
		struct timeval tv;

		tv.tv_sec = 0;  /* 30 Secs Timeout */
		tv.tv_usec = timeout * 1000;  // Not init'ing this can cause strange errors

		setsockopt(mysock, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv,sizeof(struct timeval));
		int	rc = ::write(mysock, buffer, len);
		//printf("write rc %d\n", rc);
		return rc;
  }

	int disconnect()
	{
		return ::close(mysock);
	}

private:

    int mysock;
};

class Countdown
{
public:
  Countdown()
  {

  }

  Countdown(int ms)
  {
		countdown_ms(ms);
  }


  bool expired()
  {
		struct timeval now, res;
		gettimeofday(&now, NULL);
		timersub(&end_time, &now, &res);
		//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
		//if (res.tv_sec > 0 || res.tv_usec > 0)
		//	printf("expired %d %d\n", res.tv_sec, res.tv_usec);
        return res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_usec <= 0);
  }


  void countdown_ms(int ms)
  {
		struct timeval now;
		gettimeofday(&now, NULL);
		struct timeval interval = {ms / 1000, (ms % 1000) * 1000};
		//printf("interval %d %d\n", interval.tv_sec, interval.tv_usec);
		timeradd(&now, &interval, &end_time);
  }


  void countdown(int seconds)
  {
		struct timeval now;
		gettimeofday(&now, NULL);
		struct timeval interval = {seconds, 0};
		timeradd(&now, &interval, &end_time);
  }


  int left_ms()
  {
		struct timeval now, res;
		gettimeofday(&now, NULL);
		timersub(&end_time, &now, &res);
		//printf("left %d ms\n", (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000);
        return (res.tv_sec < 0) ? 0 : res.tv_sec * 1000 + res.tv_usec / 1000;
  }

private:

	struct timeval end_time;
};

typedef enum {
    JPEG_TYPE_BODY = 0,
    JPEG_TYPE_VEHICLE,
    JPEG_TYPE_CYCLE,
    JPEG_TYPE_FACE,
    JPEG_TYPE_PLATE,
    JPEG_TYPE_CAPTURE,
    JPE_TYPE_FLASH,
    JPEG_TYPE_BUTT
} JPEG_TYPE_E;

typedef struct {
    AX_U8 nSnsSrc;
    AX_U8 nChannel;
    AX_U32 nWidth;
    AX_U32 nHeight;
	AX_CHAR szTimestamp[16];
	AX_CHAR szImgPath[256];
} JPEG_HEAD_INFO_T;

typedef struct _JPEG_CAPTURE_INFO_T {
    JPEG_HEAD_INFO_T tHeaderInfo;
    AX_VOID* pData;

    _JPEG_CAPTURE_INFO_T() {
        memset(this, 0, sizeof(_JPEG_CAPTURE_INFO_T));
    }
} JPEG_CAPTURE_INFO_T;

typedef struct {
    AX_U8 nGender; /* 0-female, 1-male */
    AX_U8 nAge;
    AX_CHAR szMask[32];
    AX_CHAR szInfo[32];
} JPEG_FACE_INFO_T;

typedef struct {
    AX_CHAR szNum[16];
    AX_CHAR szColor[32];
} JPEG_PLATE_INFO_T;

typedef struct _JPEG_DATA_INFO_T {
    JPEG_TYPE_E eType; /* JPEG_TYPE_E */
    union {
        JPEG_CAPTURE_INFO_T tCaptureInfo;
        JPEG_FACE_INFO_T tFaceInfo;
        JPEG_PLATE_INFO_T tPlateInfo;
    };

    _JPEG_DATA_INFO_T() {
        eType = JPEG_TYPE_BUTT;
    }
} JPEG_DATA_INFO_T;

#define MAX_BUF_LENGTH (1920*1080*3/2)
typedef struct _QUEUE_T {
	JPEG_DATA_INFO_T tJpegInfo;
	AX_U8 *jpg_buf;
	AX_U32 buf_length;
	AX_U64 u64UserData; // 标识数据来源哪个通道
} QUEUE_T;


class MqttClient final: public IObserver {
public:
    MqttClient() = default;
    virtual ~MqttClient(AX_VOID) = default;

	AX_BOOL Init(MQTT_CONFIG_T &mqtt_config);
    AX_BOOL DeInit(AX_VOID);

    AX_BOOL Start(AX_VOID);
    AX_BOOL Stop(AX_VOID);

	// AX_VOID BindTransfer(CTransferHelper* pInstance);
	virtual AX_BOOL OnRecvData(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, AX_VOID* pData) override;
	AX_BOOL OnRegisterObserver(OBS_TARGET_TYPE_E eTarget, AX_U32 nGrp, AX_U32 nChn, OBS_TRANS_ATTR_PTR pParams) override {
        return AX_TRUE;
    }

private:
    AX_VOID WorkThread(AX_VOID* pArg);
	AX_BOOL SaveJpgFile(QUEUE_T* jpg_info);
	AX_VOID SendAlarmMsg();

protected:
	std::mutex m_mtxConnStatus;
	std::unique_ptr<CAXLockQ<QUEUE_T>> arrjpegQ;

    CAXThread m_threadWork;
	std::string topic;
	
};
};