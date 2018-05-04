#ifndef STREAM_COMMUNICATE_H
#define STREAM_COMMUNICATE_H

typedef unsigned char BYTE;

#define NET_BUFFER_LEN			(8*1024)
#define FALSE	0
#define TRUE	1

typedef struct
{
	HB_S32		iActLength;		//包实际长度
	BYTE	byProtocolType;	//新增,协议类型,流媒体为0,一点通盒子为1,手机通讯时,此值固定为0
	BYTE	byProtocolVer;	//新增,协议版本,目前固定为9,以后如果有升级,按1增加,可作为C/S端通讯版本匹配提示
	BYTE	byDataType;		//数据类型,手机通讯时,DATA_TYPE_REAL_XML:9:交互命令,DATA_TYPE_SMS_CMD:10:云台控制命令,DATA_TYPE_SMS_MEDIA:13:流媒体数据
	BYTE	byFrameType;	//FRAMETYPE_BP:0:视频非关键帧,FRAMETYPE_KEY:1:视频关键帧,FRAMETYPE_HEAD:2:文件头,FRAMETYPE_SPECIAL:3:特殊帧,收到此帧可直接忽略掉
							//FRAMETYPE_AUDIO:4:音频帧
	HB_S32	iTimeStampHigh;		//音/视频帧时间戳高位,目前保留
	HB_S32	iTimeStampLow;		//音/视频帧时间戳低位,目前保留
	HB_S32		iVodFilePercent;//VOD文件播放进度
	HB_S32		iVodCurFrameNo;//VOD文件当前帧,需要*2,因为最大为65535,视频文件最大可能为25*3600=90000
    BYTE	byBlockHeadFlag;//包头标识,1为头,0为中间包
    BYTE	byBlockEndFlag;//包尾标识,1为尾,0为中间包
	BYTE	byReserved1;	//保留1
	BYTE	byReserved2;	//保留2
	HB_CHAR	cBuffer[NET_BUFFER_LEN];
}NET_LAYER;

#define Net_LAYER_STRUCT_LEN	sizeof(NET_LAYER)
#define PACKAGE_EXTRA_LEN (Net_LAYER_STRUCT_LEN-NET_BUFFER_LEN)

typedef enum
{
	SELECT_ERROR = -1,
	SEND_ERROR = -2,
	RECV_ERROR = -3,
	SOCKET_CLOSED = -4,
	TIME_OUT = -5
}ERR_CODE;

//向流媒体发送数据
HB_S32	SendData(int m_hSocket, char *pSrc, int iLength,int iDataType);
//从流媒体接收数据
HB_S32	RecvData(int m_hSocket, NET_LAYER *m_package, int size, int i_timeout);
//获取gnLan登录信息
HB_S32 get_sys_gnLan(HB_VOID);

#endif
