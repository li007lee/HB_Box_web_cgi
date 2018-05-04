/*
 * data_base_ctrl.h
 *
 *  Created on: 2016年3月21日
 *      Author: root
 */

#ifndef SRC_LANCGI_DATA_BASE_CTRL_H_
#define SRC_LANCGI_DATA_BASE_CTRL_H_

typedef struct _tagADD_LAN_IP
{
	char ip_name[16];
	char ip_addr[16];
	char ip_mask[16];
	char ip_gw[16];
	int ret_code;
}ADD_LAN_IP_OBJ, *ADD_LAN_IP_HANDLE;



#endif /* SRC_LANCGI_DATA_BASE_CTRL_H_ */
