/*
 * data_base_ctrl.c
 *
 *  Created on: 2016年3月21日
 *      Author: root
 */

#include "my_include.h"
#include "sqlite3.h"

#if 0
HB_S32 check_data_base_ip_cb(void * para, int n_column, char ** column_value, char ** column_name)
{
	int i;
	ADD_LAN_IP_HANDLE lan_net =(ADD_LAN_IP_HANDLE)para;
	for(i = 0; i < n_column; i++)
	{
		if(!strcmp(lan_net->ip_addr, ifcfg_eth.ipaddr) && !strcmp(lan_mask, ifcfg_eth.netmask)
				&& !strcmp(lan_gw, ifcfg_eth.gateway))
		printf("字段名:%s     字段值:%s \n", column_name[i], column_value[i]);
	}
	return 0;
}

HB_S32 check_data_base_ip()
{
	sqlite3 *db;
	HB_S32 ret;
	HB_CHAR *sql;
	HB_CHAR *errmsg = NULL;
	HB_CHAR *ret_msg = NULL;
	ret = sqlite3_open("/root/mnt_nfs/database/TM_X04_Data.db", &db);
	if (ret != SQLITE_OK)
	{
		//printf("\n### sqlite3_open failed!\n");
		return -1;
	}

	sql = "SELECT * FROM lan_web_ip_data";
	sqlite3_exec(db, sql, check_data_base_ip_cb, (void*)ret_msg, &errmsg);
}
#endif


