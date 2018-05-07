/***********************************************
 * @file dev_operation.h
 * @author lijian
 * @date 2016-06-29
************************************************/

#ifndef DEV_OPERATION_H
#define DEV_OPERATION_H

#include "my_include.h"
#include "net_api.h"
#include "cgi.h"
#include "session.h"
#include "sqlite3.h"
#include "common.h"
#include "stream_communicate.h"

#define BUF_LEN_OF_JSON	(1024*30)

typedef struct _tagGN_NET_LAYER
{
	HB_S32 	iActLength;    // 一个TCP包实际长度
	HB_S32		iProtocolVer;  //网络通信协议版本,防止以后升级能识别,目前定为1
	HB_S32 	iDataType;     //传输的数据类型，目前固定为9
	HB_S32 	iBlockHeadFlag;//头标识
    HB_S32 		iBlockEndFlag;//尾标识
    HB_CHAR 	cBuffer[NET_BUFFER_LEN];
}GN_NET_LAYER_OBJ, *GN_NET_LAYER_HANDLE;

typedef struct _tagDEV_PAIR
{
	HB_CHAR dev_type[32];
	HB_CHAR dev_ip[16];
	HB_CHAR dev_id[64];
	HB_CHAR pair_code[64];
}DEV_PAIR_OBJ, *DEV_PAIRHANDLE;

typedef struct _tagSTREAM_INFO
{
	HB_S32 i_server_port;
	HB_CHAR c_server_ip[16];
	struct _tagSTREAM_INFO *next;
}STREAM_OBJ, *STREAM_HANDLE;


typedef struct _tagONVIF_DEV_INFO
{
	HB_CHAR cDevStatus[2];	//设备状态
//	HB_CHAR	cDevPort[8];	//设备端口（Onvif服务端口）
	HB_S32	iDevPort;	//设备端口（rtsp端口）
	HB_CHAR cDevChnls[8];	//设备通道数
	HB_CHAR cDevIp[16];		//设备ip
	HB_CHAR cDevLoginUsr[32];	//设备登录名
	HB_CHAR cDevLoginPasswd[64];	//设备登录密码
	HB_CHAR cDevId[128];		//设备ID号
	HB_CHAR cBasicAuth[32];	//Rtsp基本认证
	HB_CHAR	cDevName[256];		//设备名称（用户设置）
	HB_CHAR cDevServiceUrl[256];	//设备Onvif服务地址
	HB_CHAR cRtspMainUrl[512];		//Rtsp主码流
	HB_CHAR cRtspSubUrl[512];	//Rtsp字码流
}ONVIF_DEV_INFO_OBJ, *ONVIF_DEV_INFO_HANDLE;


typedef struct _tagFIFO
{
	HB_S32 	iVideoSourcesNums;//视频源总数
	HB_S32 	iTotalChnlCount;//获取到的token总数量
	HB_S32	iCurChnlNum;  //当前获取到的token数量
	HB_CHAR cMsg[2048];	//错误消息或列表
}FIFO_OBJ, *FIFO_HANDLE;

typedef enum
{
	GET_ALL_DEV_LIST_PORT=999, //设备映射页面获取所有列表
	GET_ALL_DEV_LIST=1000, //基本信息页面获取所有设备

	ADD_BOX_DEV_AUTO=1001,	//添加一点通盒子设备(自动)
	ADD_BOX_DEV_HAND=1005,	//添加一点通盒子设备(手动)
	DEL_ONE_BOX_DEV=1002,//删除1条一点通盒子数据
	DEL_ALL_BOX_DEV=1003,	//删除全部一点通盒子设备
	GET_BOX_DEV_LIST=1004,	//获取一点通设备列表

	ADD_RTSP_DEV=1011,	//添加rtsp设备
	DEL_ONE_RTSP_DEV=1012,//删除一条rtsp设备
	DEL_ALL_RTSP_DEV=1013,	//删除所有rtsp设备
	GET_RTSP_DEV_LIST=1014,	//获取rtsp设备列表
	DISCOVER_RTSP_DEV=1015, //搜索onvif设备
	REFRESH_RTSP_DEV=1016,	//更新数据库中rtsp设备的信息
	GET_ADD_RTSP_PERSENT=1017,//获取添加进度

	ADD_TCP_DEV=1021,	//添加TCP设备
	DEL_ONE_TCP_DEV=1022,	//删除一条TCP设备
	DEL_ALL_TCP_DEV=1023,	//删除所有TCP设备
	GET_TCP_DEV_LIST=1024	//获取TCP设备列表
}_tagDATACODE;

/************************网络通信************************/
/*
 *	Function: 用于发送数据并接收返回数据（返回错误码）
 *	Para：
 *		sockfd: 未被使用的文件描述符
 *		ip：	服务端的ip地址
 *		port：	服务端的监听端口
 *		buf:	需要发送的数据缓冲区，此缓冲区会被清空并存储服务端返回的数据
 *		buf_len:发送数据的长度
 *		buf_size:发送缓冲区buf的大小
 *		iTimeOut: 数据接收超时
 *	return： 通信成功返回0， 通信失败返回值负值
 */
HB_S32 NetworkCommunicate(HB_S32 *sockfd, HB_CHAR *ip, HB_S32 port, HB_CHAR *buf, HB_S32 buf_len, HB_S32 buf_size, HB_S32 iTimeOut);
/************************网络通信END************************/

//获取所有设备列表
HB_VOID GetALLDevList();
HB_VOID GetALLDevPortList();

/************************************************************/
/************************一点通盒子操作************************/
/************************************************************/
//添加一点通盒子设备
HB_S32 AddBoxDev(HB_S32 i_data_code, HB_CHAR *buf);

//删除一条一点通盒子设备
HB_S32 DelOneBoxDev(HB_CHAR *buf);

//删除全部一点通盒子设备
HB_S32 DelAllBoxDev();

//获取一点通盒子列表
HB_S32 GetBoxDevList(HB_CHAR *packeting_str, HB_S32 size_packeting_str);

//搜索onvif设备
HB_S32 DiscoverRtspDev();

/***************************************************************
*************************一点通盒子操作END*************************
****************************************************************/

/************************Rtsp设备操作************************/
//添加一条Rtsp设备
HB_S32 add_rtsp_dev(HB_CHAR *buf);

//删除一条Rtsp设备
HB_S32 DelOneRtspDev(HB_CHAR *buf);

//删除全部Rtsp设备
HB_S32 DelAllRtspDev();

//搜索rtsp设备（搜索onvif设备）
HB_S32 DiscoverRtspDev();

//获取Rtsp设备列表
HB_S32 GetRtspDevPortList(HB_CHAR *packeting_str, HB_S32 size_packeting_str);

/************************Rtsp设备操作END************************/

/************************TCP设备操作************************/
//添加Tcp设备
HB_S32 AddTcpDev(HB_CHAR *buf);

//删除一条Tcp设备
HB_S32 DelOneTcpDev(HB_CHAR *buf);

//删除所有Tcp设备
HB_S32 DelAllTcpDev();

//获取Tcp设备列表
HB_S32 GetTcpDevList(HB_CHAR *packeting_str, HB_S32 size_packeting_str);

/************************TCP设备操作END************************/

#endif
