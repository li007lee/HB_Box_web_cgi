#ifndef LAN_OPERATION_H
#define LAN_OPERATION_H

#include "my_include.h"
#include "sqlite3.h"

typedef struct _tagNET_CONF
{
	char ip_addr[16];
	char net_mask[16];
	char gate_way[16];
	char dns_main_ip[16];
	char dns_sub_ip[16];
}NET_CONF_OBJ, *NET_CONF_HANDLE;

#define NET_BUFFER_LEN			(8*1024)

typedef struct _tagGN_NET_LAYER
{
	HB_S32 	iActLength;    // 一个TCP包实际长度
	HB_S32		iProtocolVer;  //网络通信协议版本,防止以后升级能识别,目前定为1
	HB_S32 	iDataType;     //传输的数据类型，目前固定为9
	HB_S32 	iBlockHeadFlag;//头标识
    HB_S32 		iBlockEndFlag;//尾标识
    HB_CHAR 	cBuffer[NET_BUFFER_LEN];
}GN_NET_LAYER_OBJ, *GN_NET_LAYER_HANDLE;


//获取网络配置信息
HB_VOID GetNetConfInfo();
//修改Wan口网络配置
HB_S32 ChangeWanConf(HB_CHAR *buf);
//添加辅助ip
HB_S32 AddLanSubIp(HB_CHAR *buf);
//修改Lan口网络配置
HB_S32 ChangeMainLanConf(HB_CHAR *buf);
//删除辅助ip
HB_S32 DelLanSubIp(HB_CHAR *buf);
#endif
