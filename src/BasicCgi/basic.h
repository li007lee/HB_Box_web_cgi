//
//  basic.h
//  Mapport
//
//  Created by MoK on 15-2-5.
//  Copyright (c) 2015ๅนด MoK. All rights reserved.
//

#ifndef __Mapport__xml_express__
#define __Mapport__xml_express__

typedef enum
{
    ENUM_MSG_CXHZ,          //控件传过来的
    ENUM_MSG_HAND,          //手动注册的设备
    ENUM_MSG_AUTO           //自动扫描添加到设备
}enum_msg_def;

typedef enum _dev_type
{
    ENUM_DEVTYPE_HBDVR  =   23,
    ENUM_DEVTYPE_HBIPC  =   23,
    ENUM_DEVTYPE_HBNVR  =   23,
    ENUM_DEVTYPE_HIKVS  =   2,
    ENUM_DEVTYPE_DH     =   25,
    ENUM_DEVTYPE_YS     =   37, //宇视
    ENUM_DEVTYPE_HH     =   2,  //黄河
    ENUM_DEVTYPE_USB     =  27,
    ENUM_DEVTYPE_OTHER
}enum_dev_type;

//配对盒子下挂设备列表结构
typedef struct _tagBOX_PAIR_DEV_LIST
{
	HB_S32 if_online; //1为在线，0为不在线
	HB_S32 if_online2;//用来标记第二个端口是否联通  1为在线，0为不在线
	HB_S32 i_port_num;
	//HB_S32 i_dev_type;//设备类型
	HB_S32 i_dev_channel;
	HB_S32 i_dev_port1;//设备端口1
	HB_S32 i_dev_port2;//设备端口2
	HB_S32 i_box_port1;
	HB_S32 i_box_port2;
	//HB_S32 i_map_port;//映射端口
	HB_S32 i_dev_status;
	HB_S32 del_flag; //删除标志，0-不删除，1-删除
	HB_CHAR c_dev_ip[16];//设备ip
	HB_CHAR c_dev_login_passwd[32];
	HB_CHAR c_dev_login_name[128];
	HB_CHAR c_dev_type[128];//设备ip
	HB_CHAR c_dev_name[256];//设备名称
	HB_CHAR c_dev_id[64];//设备序列号
    struct _tagBOX_PAIR_DEV_LIST *next;
}BOX_PAIR_DEV_LIST_OBJ, *BOX_PAIR_DEV_LIST_HANDLE;

//配对盒子结构体
typedef struct _tagBOX_PAIR_HEAD
{
	HB_S32 dev_num; //盒子下挂设备个数
	HB_S32 map_port_start;//映射端口起点
	HB_S32 pair_type; //1-LAN，2-WAN ,3-本地直接映射设备
	HB_CHAR box_id[32];//盒子序列号
	HB_CHAR box_ip[16];//盒子ip
    BOX_PAIR_DEV_LIST_HANDLE dev_list_head;
}BOX_PAIR_HEAD_OBJ, *BOX_PAIR_HEAD_HANDLE;


//配对盒子结构体
typedef struct _tagTCP_PAIR_HEAD
{
	HB_S32 i_dev_num; //tcp设备个数
	HB_CHAR c_tcp_dev_ip[16];//tcp设备ip
	HB_S32 i_tcp_dev_port;//tcp设备端口
	HB_CHAR	c_tcp_dev_name[256]; //设备名称
	HB_S32 i_box_port;	//盒子映射端口
	HB_S32 i_if_online;	//连接状态
	struct _tagTCP_PAIR_HEAD *next;
}TCP_PAIR_HEAD_OBJ, *TCP_PAIR_HEAD_HANDLE;






typedef struct _tagRTSP_DEV_LIST
{
	HB_S32 iDevNum;
	HB_S32 iIfOnline; //1为在线，0为不在线
	HB_S32 iDevChannel;
	HB_S32 iDevPort;//设备rtsp端口
	HB_S32 iDevStatus;//设备注册状态
	HB_S32 iDelFlag; //删除标志，0-不删除，1-删除
	HB_CHAR cDevIp[16];//设备ip
	HB_CHAR cDevLoginName[32];
	HB_CHAR cDevLoginPasswd[64];
	HB_CHAR cDevId[128];//设备序列号
	HB_CHAR cDevName[256];//设备名称
    struct _tagRTSP_DEV_LIST *next;
}RTSP_DEV_LIST_OBJ, *RTSP_DEV_LIST_HANDLE;
////////////////////////////////////////////////////////////////////////////////
// 函数名：get_box_direct_config
// 描述：   获取盒子直接映射配置文件中的映射信息，并生成链表
// 参数：［IN］             node    xml节点，该节点对应一个盒子
//	           [IN/OUT]        box_info -   生成盒子配对消息结构体
//
// 返回值：成功：0   解析错误：-1   参数错误：-2
// 说明：
////////////////////////////////////////////////////////////////////////////////
int get_box_direct_config(char *file_path,BOX_PAIR_HEAD_HANDLE box_info);

////////////////////////////////////////////////////////////////////////////////
// 函数名：free_box_direct_config
// 描述：   释放生成链表
// 参数： [IN/OUT]        box_info -   生成盒子配对消息结构体
//
// 返回值：成功：0   解析错误：-1   参数错误：-2
// 说明：
////////////////////////////////////////////////////////////////////////////////
int free_box_direct_config(BOX_PAIR_HEAD_HANDLE box_info);


#endif /* defined(__Mapport__xml_express__) */

