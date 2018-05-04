/*
 * lan_ip_conf.h
 *
 *  Created on: 2016-8-17
 *      Author: li007lee
 */

#ifndef LAN_IP_CONF_H_
#define LAN_IP_CONF_H_

typedef struct _tagIFCFG_ETHX_ITEM
{
	HB_CHAR device[16];
	HB_CHAR bootproto[16];
    HB_CHAR hwaddr[24];
    HB_CHAR ipaddr[16];
    HB_CHAR netmask[16];
    HB_CHAR gateway[16];
}IFCFG_ETHX_ITEM_OBJ ,*IFCFG_ETHX_ITEM_HANDLE;

//添加辅助ip
HB_S32 add_lan_sub_ip(HB_CHAR *eth_name, HB_CHAR *ip, HB_CHAR *mask, HB_CHAR *gate);
//修改lan口主ip
HB_S32 alter_lan_main_ip(HB_CHAR *eth_name, HB_CHAR *lan_ip, HB_CHAR *lan_mask, HB_CHAR *lan_gateway);
//删除辅助ip
HB_S32 del_lan_sub_ip(HB_CHAR *eth_name, HB_CHAR *ip, HB_CHAR *mask,HB_CHAR *gate);

#endif /* LAN_IP_CONF_H_ */
