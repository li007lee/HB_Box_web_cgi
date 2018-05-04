//
//  Header.h
//  Mapport
//
//  Created by MoK on 15-3-4.
//  Copyright (c) 2015骞� MoK. All rights reserved.
//

#ifndef Mapport_Header_h
#define Mapport_Header_h
#define LOG_FILE    "/tmp/tmp_root/error.log"

#define MAX_CONN_NUM            128
#define MAPPORT_ARRAY           256
#define LISTTEN_SRV_PORT        8888

#define HBGK_DEV_MAPPORT        10000
#define HKVIS_DEV_MAPPORT       20000
#define DH_DEV_MAPPORT          30000
#define CXHZ_MAPPORT            40000

#define MSECOND                 1000
#define USECOND                 (1000*1000)
#define NSECOND                 (1000*1000*1000)

#include <stdio.h>
#include <pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<signal.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include<signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


typedef struct
{
    size_t tick;
    int dev_ch;
    int accpt_sock[32];
    int accept_cli_sock;
    int connect_dev_sock;
}sock_party;

typedef struct
{
    pthread_t   lpthid;
    pthread_t   cpthid;
    int         if_active;
    int         dev_type;
    int         mapport;
    int         dev_port;
    int         listen_sockfd;
    char        dev_ip[16];
    sock_party  sockpt[MAX_CONN_NUM];
}MapPort_stru;

#pragma pack (push,1)
typedef struct _tagSEARCH_DEVICE
{
    char    Header[18];         //卤每鈥撁幝犅Ｂ�"HBneTautOf8inDp_@"
    char    PcIdNum[6];         //艗楼鈭偮�溍�
    char    Cmd;                //卤每鈥撁幝犅Ｂ�0x8a
    char    MacAddress[6];      //艗脭驴脤碌每梅鈭�
    char    IpAddress[4];       //IPv4碌每梅鈭�
    int     DeviceType;         //鈥γ嬄扁垙驴鈥♀�撁�
    char    Port[2];               //鈭偯�酶鈦�
    char    HostId[32];         //Host ID
    char    HostType[16];
}SEARCH_DEVICE_OBJ,*SEARCH_DEVICE_HANDLE;

#pragma pack(pop) 

#define IPADDR_LEN_SIZE 16
#define MAX_IPADDR_SIZE 255
#define XML_FILE_NAME   "./config.xml"
typedef enum
{
    ENUM_MSG_CONN_ERR = 1,      //璁惧杩炴帴澶辫触
    ENUM_MSG_SOURCE_IP_DEFINE   //璁惧宸茬粡瀹氫箟
}enum_msg_def;

typedef struct
{
    char lan_ip[16];
    int app_port;
    int map_port;
    enum_msg_def dev_info;
    char dev_serial[32];
}map_xml_def;
#define NET_BUFFER_LEN			(8*1024)



typedef struct _tagGN_NET_LAYER
{
    int     iActLength;    // 鈥溌垙藛TCP鈭灺嘎犅德郝犫墺搂鈭偮�
    int     iProtocolVer;  //脮炉卢脕脮庐鈥撯増鈥撯墵鈥溍堚垶脢卤忙,鈭懧棵废�鈥溾�樷埆脹鈥λ澛衡垈茠鈥孤犫垈卤铮�,茠酶芦鈭炩垈庐艗鈩�1
    int     iDataType;     //楼麓聽鈥奥灯捖犓澝︹�郝库�♀�撁暵Ｂㄆ捗嘎垶蟺脙鈭偮掆劉9
    int     iBlockHeadFlag;//脮鈭懧泵嵚犫垈
    int     iBlockEndFlag;//艗鈮ぢ泵嵚犫垈
    char    cBuffer[NET_BUFFER_LEN];
}GN_NET_LAYER_OBJ, *GN_NET_LAYER_HANDLE;


typedef struct _tagNET_ADD_PORT_CMD
{
    char device_type[32];
    char device_serial[32];
    char device_local_ip[16];
    char device_local_port[8];
    char device_mapp_port[8];
    char rtsp_local_port[8];
    char rtsp_mapp_port[8];
}NET_ADD_PORT_CMD_OBJ, *NET_ADD_PORT_CMD_HANDLE;


#define MAXLINE 1024
/* define a get if info struct */
struct if_ipv4_info
{
    char if_name[10];
    unsigned char ip_addr[20];
    unsigned char hd_addr[20];
    
    struct if_ipv4_info *next;
};

typedef struct item_t
{
    char *key;
    char *value;
}ITEM;

#endif
