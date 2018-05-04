/*
 * my_include.h
 *
 *  Created on: 2014年12月11日
 *  Author: root
 */

#ifndef MY_INCLUDE_H_
#define MY_INCLUDE_H_

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/prctl.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <memory.h>
#include <linux/rtc.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <net/if.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <sys/shm.h>
#include <sys/mman.h>


///////////////////////////////////////////////////////////////////////////////////////////
//类型的定义
///////////////////////////////////////////////////////////////////////////////////////////
typedef unsigned char                HB_U8;
typedef unsigned short              HB_U16;
typedef unsigned int                    HB_U32;
typedef unsigned long long     HB_U64;
typedef signed char                      HB_S8;
typedef short                                    HB_S16;
typedef int                                          HB_S32;
typedef long long                           HB_S64;
typedef char                                      HB_CHAR;
typedef float                                     HB_FLOAT;
typedef void                                      HB_VOID;
typedef void *                                   HB_HANDLE;

typedef enum _tagHB_BOOL
{
    HB_FALSE = 0,
    HB_TRUE  = 1
}HB_BOOL;

#ifndef NULL
#define NULL  0L
#endif


#define HB_NULL     0L
#define HB_SUCCESS  0
#define HB_FAILURE  -1

#define MSECOND                 1000
#define USECOND                 (1000*1000)
#define NSECOND                 (1000*1000*1000)
#define SESSION_TIMEOUT 600


#define SMALL_BOX
//#define BIG_BOX_SINGLE_PORT
//#define BIG_BOX_MULTIPLE_PORT
//#define BIG_BOX_ENCRYPT
//#define BIG_BOX_ELEVATOR


#define MAIN_CTRL_IP	"127.0.0.1"
#define MAIN_CTRL_PORT 8101  //主控监听端口

//#define HANDSHARK_PORT 7788 //加密盒子握手服务端口
//#define TEST_SERVER_PORT 8106 //联通性测试端口

#define DOWNLOAD_FILE_PCAP_PATH     "/tmp/"  //抓包文件目录
#define DOWNLOAD_FILE_PCAP_NAME    "data.cap" //抓包文件名
#define LOG_PATH "/tmp/execution.log"
#define RM_LOG_FILE   "rm /tmp/execution.log; rm /tmp/main_ctrl.log"

#define BOX_QRCODE_FILE "/tmp/qrcode_mac.png" //盒子序列号二维码

//大盒子挂载测试用宏定义
#if 0
#define N2208C
#define RM_SESSION_FILE   "rm /mnt/mtd/test/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/test/default/session_files/"
#define BOX_DIRECT_DEV_MAP_FILE   "/mnt/mtd/test/default/box_config.xml"
#define BOX_PAIR_XML_FILE   "/mnt/mtd/test/default/pair_box_config.xml"
#define NETCONF_SAVE_PATH "/mnt/mtd/test/default/interfaces"
#define DNSCONF_SAVE_PATH "/mnt/mtd/etc/local_resolv.conf"
#define BOX_VERSION_FILE "/mnt/mtd/test/default/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/test/default/TM_X02_Data.db"
#define DOWNLOAD_FILE_PCAP_PATH     "/mnt/mtd/test/"
#define DOWNLOAD_FILE_PCAP_NAME    "data.pcapng"
#define DOWNLOAD_FILE_DEVLIST_PATH     "/mnt/mtd/test/default/"
#define DOWNLOAD_FILE_DEVLIST_NAME    "DevList.csv"
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1"   //WAN口配置文件
#define BOX_IFCFG_LAN   "/mnt/mtd/etc/network/ifcfg-eth0"  //LAN口配置文件
#define LOCAL_ACS_CERT   "/mnt/mtd/test/default/AcsCertConf"  //本地证书
#define LOCAL_WHITE_LIST   "/mnt/mtd/test/default/WhiteListConf"   //本地白名单
#define ETH_X  "eth1"

#endif


#ifdef SMALL_BOX

//#define MAX_DEV_NUM   2   //最多挂在的设备数量
//#define MAX_DEV_NUM   5   //最多挂在的设备数量
#define MAX_DEV_NUM   10   //最多挂在的设备数量
//#define MAX_DEV_NUM   20   //最多挂在的设备数量

#define DOUBLE_NET_PORT		//小盒子双网口
#define STATIC_ROUTE_VERSION //可以添加静态路由的定制版本

#define MAX_LAN_SUB_IP 8
#define MAX_ROUTE 8

#define RM_SESSION_FILE   "rm /home/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/home/default/session_files/"
#define BOX_IFCFG_WAN "/home/default/interfaces"
#define BOX_IFCFG_LAN   "/home/default/interfaces"
#define DNSCONF_SAVE_PATH "/home/default/resolv.conf"
#define BOX_VERSION_FILE "/ipnc/config/box_version"
#define BOX_DATA_BASE_NAME    "/home/default/TM_X01_Data.db"

#define ETH_X  "eth0"
#define ETH_LAN "eth0"
#define RESTART_NETWORK "/etc/init.d/S40network restart"

#endif


#ifdef BIG_BOX_MULTIPLE_PORT //大盒子多网口

#define MAX_DEV_NUM 32
#define MAX_LAN_SUB_IP 8
#define MAX_ROUTE 8

#define BIG_BOX
#define N2208C
#define MULTIPLE_PORT
#define RM_SESSION_FILE   "rm /mnt/mtd/ydt_box/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/ydt_box/default/session_files/"
#define BOX_VERSION_FILE "/mnt/ydt_box/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/ydt_box/default/TM_X03_Data.db"
#define ETH_X  "eth1"
#define ETH_LAN "eth0"
#define DNSCONF_SAVE_PATH "/etc/resolv.conf" //DNS配置文件
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1" //WAN口配置文件
#define BOX_IFCFG_LAN   "/mnt/mtd/etc/network/ifcfg-eth0"   //LAN口配置文件
#define RESTART_NETWORK	"/etc/init.d/network restart"

#define BIN_PATH	"/mnt/ydt_box/bin/"


#endif




#ifdef BIG_BOX_SINGLE_PORT //大盒子单网口

#define MAX_DEV_NUM   32   //最多挂载的设备数量 标准版（TM-X02-H200-02）
//#define MAX_DEV_NUM   20	//定制版（TM-X02-H20）
//#define MAX_DEV_NUM   30	//定制版（TM-X02-H30）

#define MAX_LAN_SUB_IP 8
#define MAX_ROUTE 8

#define BIG_BOX

#define STATIC_ROUTE_VERSION //可以添加静态路由的定制版本

#define N2208C
#define RM_SESSION_FILE   "rm /mnt/mtd/ydt_box/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/ydt_box/default/session_files/"
#define BOX_VERSION_FILE "/mnt/ydt_box/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/ydt_box/default/TM_X02_Data.db"
#define ETH_X  "eth1"
#define ETH_LAN "eth1"
#define DNSCONF_SAVE_PATH "/etc/resolv.conf" //DNS配置文件
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1" //WAN口配置文件
#define RESTART_NETWORK	"/etc/init.d/network restart"
#define BIN_PATH	"/mnt/ydt_box/bin/"

#define BOX_IFCFG_LAN   "/mnt/mtd/etc/network/ifcfg-eth1"

#endif


//大盒子加密
#ifdef BIG_BOX_ENCRYPT

#define BIG_BOX
#define N2208C
#define MULTIPLE_PORT
#define RM_SESSION_FILE   "rm /mnt/mtd/ydt_box/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/ydt_box/default/session_files/"
//#define BOX_DIRECT_DEV_MAP_FILE   "/mnt/mtd/ydt_box/default/box_config.xml"
//#define BOX_PAIR_XML_FILE   "/mnt/mtd/ydt_box/default/pair_box_config.xml"
#define BOX_VERSION_FILE "/mnt/ydt_box/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/ydt_box/default/TM_X04_Data.db"
#define DOWNLOAD_FILE_PCAP_PATH     "/mnt/mtd/ydt_box/"
#define DOWNLOAD_FILE_PCAP_NAME    "data.pcapng"
#define DOWNLOAD_FILE_DEVLIST_PATH     "/mnt/mtd/ydt_box/default/"
#define DOWNLOAD_FILE_DEVLIST_NAME    "DevList.csv"

#define ETH_X  "eth1"
#define DNSCONF_SAVE_PATH "/mnt/mtd/etc/local_resolv.conf" //DNS配置文件
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1"   //WAN口配置文件
#define BOX_IFCFG_LAN   "/mnt/mtd/etc/network/ifcfg-eth0"  //LAN口配置文件
#define LOCAL_ACS_CERT   "/mnt/mtd/ydt_box/default/AcsCertConf"  //本地证书
#define LOCAL_WHITE_LIST   "/mnt/mtd/ydt_box/default/WhiteListConf"   //本地白名单


#endif



#ifdef BIG_BOX_ELEVATOR //电梯盒子

#define BIG_BOX
#define N2208C
#define MULTIPLE_PORT
#define RM_SESSION_FILE   "rm /mnt/mtd/ydt_box/default/session_files/*"
#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/ydt_box/default/session_files/"
//#define BOX_DIRECT_DEV_MAP_FILE   "/mnt/mtd/ydt_box/default/box_config.xml" //盒子直接映射配置文件
//#define BOX_PAIR_XML_FILE   "/mnt/mtd/ydt_box/default/pair_box_config.xml" //盒子配对配置文件
#define BOX_VERSION_FILE "/mnt/ydt_box/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/ydt_box/default/TM_X05_Data.db"
#define DOWNLOAD_FILE_PCAP_PATH     "/mnt/mtd/ydt_box/default/"
#define DOWNLOAD_FILE_PCAP_NAME    "data.pcapng"
#define DOWNLOAD_FILE_DEVLIST_PATH     "/mnt/mtd/ydt_box/default/"
#define DOWNLOAD_FILE_DEVLIST_NAME    "DevList.csv"
#define ETH_X  "eth1"
#define DNSCONF_SAVE_PATH "/mnt/mtd/etc/local_resolv.conf" //DNS配置文件
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1" //WAN口配置文件
#define BOX_IFCFG_LAN   "/mnt/mtd/etc/network/ifcfg-eth0"   //LAN口配置文件

#endif



//正式版定义的宏
#if 0
#define RM_SESSION_FILE   "rm /mnt/mtd/ydt_box/default/session_files/*"
#define RM_SHADOW   "rm /mnt/mtd/ydt_box/default/shadow"
#define RM_VIRTUAL_IP   "rm /mnt/mtd/ydt_box/default/vitual_ip.xml"
#define RM_BOX_INFO   "rm /mnt/mtd/ydt_box/default/box_info.xml"

#define CGI_SESSION_SAVE_PATH  "/mnt/mtd/ydt_box/default/session_files/"
#define SHADOW_SAVE_PATH "/mnt/mtd/ydt_box/default/shadow"
#define CONFIG_XML_SAVE_PATH   "/mnt/mtd/ydt_box/default/box_config.xml"
#define BOX_PAIR_XML_FILE   "/mnt/mtd/ydt_box/default/pair_box_config.xml"
#define BOX_ENCRYPT_XML_FILE  "/mnt/mtd/test/default/box_encrypt.conf"
//#define NETCONF_SAVE_PATH "/mnt/mtd/ydt_box/default/interfaces"
#define DNSCONF_SAVE_PATH "/mnt/mtd/etc/local_resolv.conf"
#define VIRTUAL_IP_SAVE_PATH "/mnt/mtd/ydt_box/default/vitual_ip.xml"
#define BOX_VERSION_FILE  "/mnt/mtd/ydt_box/default/box_version"
#define BOX_DATA_BASE_NAME    "/mnt/mtd/ydt_box/default/TM_X02_Data.db"

//WAN口配置文件
#define BOX_IFCFG_WAN   "/mnt/mtd/etc/network/ifcfg-eth1"
//LAN口配置文件
#define BOX_IFCFG_LAN   "/etc/network/ifcfg-eth0"
#define ETH_X  "eth1"

#define DOWNLOAD_FILE_PCAP_PATH     "/mnt/"
#define DOWNLOAD_FILE_PCAP_NAME    "data.pcapng"

#define DOWNLOAD_FILE_DEVLIST_PATH     "/mnt/mtd/ydt_box/default/"
#define DOWNLOAD_FILE_DEVLIST_NAME    "DevList.csv"
#endif



#define DEBUG
#ifdef DEBUG
#define COLOR_STR_NONE          "\033[0m"
#define COLOR_STR_RED              "\033[1;31m"
#define COLOR_STR_GREEN         "\033[1;32m"
#define COLOR_STR_YELLOW      "\033[1;33m"
#define COLOR_STR_BLUE		     "\033[0;32;34m"

#define TRACE_LOG(str, args...)  printf(COLOR_STR_GREEN  "\n########   FILE:%s  FUNCTION: %s "str "\n" COLOR_STR_NONE,__FILE__, __FUNCTION__,## args);
#define TRACE_ERR(str, args...)   printf(COLOR_STR_RED "\n########   FILE:%s  FUNCTION: %s "str "\n" COLOR_STR_NONE,__FILE__, __FUNCTION__,## args);
#define TRACE_DBG(str, args...)  printf(COLOR_STR_YELLOW  str COLOR_STR_NONE, ## args);
#define WRITE_LOG(str, args...)	 WriteLog("\n****************FILE:%s  FUNCTION: %s LINE:%d****************\n"str ,__FILE__, __FUNCTION__, __LINE__,## args);
#else
#define TRACE_LOG(str, args...)   do{} while(0)
#define TRACE_ERR(str, args...)    do{} while(0)
#define TRACE_DBG(str, args...)   do{} while(0)
#define WRITE_LOG(str, args...) do{} while(0)
#endif /* ERR_DEBUG */


#endif /* MY_INCLUDE_H_ */
