/*
 * lan_mul_port.h
 *
 *  Created on: 2016-8-17
 *      Author: li007lee
 */

#ifndef LAN_MUL_PORT_H_
#define LAN_MUL_PORT_H_

HB_S32 readata_from_dns_conf(HB_CHAR *dns_file_path,NET_CONF_HANDLE net_conf);

////////////////////////////////////////////////////////////////////////////////
// 函数名：read_eth_config
// 描述：  读取网卡配置文件 生成结构体
// 参数：
//      [IN]   path             文件路径
//       [IN]   ifcfg_eth       结构体指针
//
// 返回值：
//  0   成功
//  -1  参数错误
//  -2  打开文件失败
//  -3  获取信息失败
//
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 read_eth_config(HB_CHAR *path,IFCFG_ETHX_ITEM_HANDLE ifcfg_eth);
//获取网卡序列号
//mac_sn 网卡序列号, dev 网卡名
HB_S32 get_mac_dev(HB_CHAR *mac_sn, HB_CHAR *dev);
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
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 write_eth_config(HB_CHAR *path,IFCFG_ETHX_ITEM_HANDLE ifcfg_eth);

HB_S32 update_network_conf(HB_CHAR *str,const HB_CHAR *file);


#endif /* LAN_MUL_PORT_H_ */
