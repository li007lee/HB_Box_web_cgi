/*
 * lan_mul_port.h
 *
 *  Created on: 2016-8-17
 *      Author: li007lee
 */

#ifndef RESTART_NETWORK_H
#define RESTART_NETWORK_H

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

#endif /* LAN_MUL_PORT_H_ */
