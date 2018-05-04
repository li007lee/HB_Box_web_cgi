/*
 * system_private.h
 *
 *  Created on: 2016年4月21日
 *      Author: root
 */

#ifndef SRC_SYSTEMCGI_SYSTEM_PRIVATE_H_
#define SRC_SYSTEMCGI_SYSTEM_PRIVATE_H_

typedef struct _tagIFCFG_ETH1_ITEM
{
    HB_CHAR device[16];
    HB_CHAR bootproto[16];
    HB_CHAR hwaddr[24];
    HB_CHAR ipaddr[16];
    HB_CHAR netmask[16];
    HB_CHAR gateway[16];
}IFCFG_ETH1_ITEM_OBJ ,*IFCFG_ETH1_ITEM_HANDLE;




////////////////////////////////////////////////////////////////////////////////
// 函数名：write_eth_config
// 描述：  根据结构体 写入文件
// 参数：
//      [IN]   path             文件路径
//       [IN]   ifcfg_eth       结构体指针
//
// 返回值：
//  0   成功
//  -1  参数错误
//  -2  打开文件失败
//
//
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 write_eth_config(HB_CHAR *path,IFCFG_ETH1_ITEM_HANDLE ifcfg_eth);

HB_S32 update_file(HB_CHAR *str, HB_CHAR *file);

//执行cmd_str命令
//返回值:0成功，-1失败
//HB_S32 my_system(HB_CHAR *cmd_str);

#endif /* SRC_SYSTEMCGI_SYSTEM_PRIVATE_H_ */
