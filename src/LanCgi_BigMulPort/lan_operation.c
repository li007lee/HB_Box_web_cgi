#include "lan_operation.h"
#include "lan_ip_conf.h"
#include "cgi.h"
#include "common.h"
#include "arping.h"
#include "ipcheck.h"
#include "lan_mul_port.h"

/************************获取网络配置信息***********************/
/************************获取网络配置信息***********************/
/************************获取网络配置信息***********************/

static int sq_cb_show_lan_ip_table(void * para, int n_column, char ** column_value, char ** column_name)
{
	HB_CHAR *p_lan_sub_ip_list = (HB_CHAR *) para;
	HB_CHAR c_lan_sub_ip[512] = { 0 };

	snprintf(c_lan_sub_ip, sizeof(c_lan_sub_ip), "{\"DevIp\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\"},", \
					column_value[0], column_value[1], column_value[2]);

	strcat(p_lan_sub_ip_list, c_lan_sub_ip);

	return 0;
}

HB_VOID GetNetConfInfo()
{
	HB_CHAR mac_str[32] = { 0 };
	HB_CHAR dev_mac[32] = { 0 };
	NET_CONF_OBJ net_conf;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth;

	HB_CHAR c_wan_ip_info[256] = { 0 };
	HB_CHAR c_lan_main_ip_info[256] = { 0 };
	HB_CHAR c_sub_ip_list[1024] = { 0 };
	HB_CHAR c_net_set_info[2048] = { 0 };

	/*******************wan口信息*******************/
	memset(&net_conf, 0, sizeof(NET_CONF_OBJ));
	readata_from_dns_conf(DNSCONF_SAVE_PATH, &net_conf);
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
	read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);

	get_mac_dev(mac_str, ETH_X);
	sprintf(dev_mac, "%02X:%02X:%02X:%02X:%02X:%02X",
					(unsigned char) mac_str[0], (unsigned char) mac_str[1],
					(unsigned char) mac_str[2], (unsigned char) mac_str[3],
					(unsigned char) mac_str[4], (unsigned char) mac_str[5]);

	snprintf(c_wan_ip_info, sizeof(c_wan_ip_info),
					"\"DevIp\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\",\"DevMainDNS\":\"%s\",\"DevSubDNS\":\"%s\",\"DevMac\":\"%s\"",
					ifcfg_eth.ipaddr, ifcfg_eth.netmask, ifcfg_eth.gateway,
					net_conf.dns_main_ip, net_conf.dns_sub_ip, dev_mac);

	/*******************wan口信息END*******************/

	/*******************lan口主ip信息*******************/
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
	//读LAN口网卡配置文件
	read_eth_config(BOX_IFCFG_LAN, &ifcfg_eth);

	snprintf(c_lan_main_ip_info, sizeof(c_lan_main_ip_info),
					"\"DevIp\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\"",
					ifcfg_eth.ipaddr, ifcfg_eth.netmask, ifcfg_eth.gateway);

	/*******************lan口主ip信息END*******************/

	/*******************lan口辅助ip信息END*******************/
	HB_CHAR *sql = "select net_ipaddr,net_mask,net_gateway from lan_web_ip_data";
	SqlOperation(sql, BOX_DATA_BASE_NAME, sq_cb_show_lan_ip_table,
					(void *) c_sub_ip_list);

	if (c_sub_ip_list[strlen(c_sub_ip_list) - 1] == ',')
	{
		c_sub_ip_list[strlen(c_sub_ip_list) - 1] = '\0';
	}

	snprintf(c_net_set_info, sizeof(c_net_set_info),
					"{\"Result\":\"0\",\"WanIP\":{%s},\"LanMainIP\":{%s},\"LanSubs\":[%s]}",
					c_wan_ip_info, c_lan_main_ip_info, c_sub_ip_list);

	/*******************lan口辅助ip信息END*******************/

	printf("%s", c_net_set_info);
	WRITE_LOG("send_to_web:[%s]\n", c_net_set_info);
}

/************************获取网络配置信息END***********************/
/************************获取网络配置信息END***********************/
/************************获取网络配置信息END***********************/

/************************修改Wan口配置信息***********************/
/************************修改Wan口配置信息***********************/
/************************修改Wan口配置信息***********************/
//判断两个ip是否为同一网段
//0:  表示在一个网段
// 1: 表示网段不同
static HB_S32 check_net_segment(HB_CHAR*ip1, HB_CHAR*netmask1, HB_CHAR *ip2, HB_CHAR *netmask2)
{
	unsigned long int min = inet_addr(netmask1);
	if (min > inet_addr(netmask2))
	{
		min = inet_addr(netmask2);
	}
	return (inet_addr(ip1) & min) ^ (inet_addr(ip2) & min) ? 1 : 0;
}

HB_S32 ChangeWanConf(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_S32 restart = 1; //用于标记配置成功后是否需要重启，1重启 0不重启
	HB_CHAR ipaddr[20] = { 0 };
	HB_CHAR mask_ip[20] = { 0 };
	HB_CHAR gate_ip[20] = { 0 };
	HB_CHAR man_dns[20] = { 0 };
	HB_CHAR sub_dns[20] = { 0 };
	HB_CHAR set_dns_str[512] = { 0 };

	NET_CONF_OBJ net_conf;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth0;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth1;

	memset(&ifcfg_eth0, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
	memset(&ifcfg_eth1, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
	memset(&net_conf, 0, sizeof(NET_CONF_OBJ));

	if ((pos = strstr(buf, "DevIp")))
	{
		sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]&DevGateway=%[^&]&DevMainDNS=%[^&]&DevSubDNS=%s", \
						ipaddr, mask_ip, gate_ip, man_dns, sub_dns);
	}

	read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth1);
	readata_from_dns_conf(DNSCONF_SAVE_PATH, &net_conf);
	if (!strcmp(ipaddr, ifcfg_eth1.ipaddr) && !strcmp(mask_ip, ifcfg_eth1.netmask)
					&& !strcmp(gate_ip, ifcfg_eth1.gateway)
					&& !strcmp(man_dns, net_conf.dns_main_ip)
					&& !strcmp(sub_dns, net_conf.dns_sub_ip))
	{
		printf("{\"Result\":\"-300\",\"ErrMessage\":\"Wan配置未发生变化!-300\"}");
		goto END;
	}

	if (!strcmp(ipaddr, ifcfg_eth1.ipaddr) && !strcmp(mask_ip, ifcfg_eth1.netmask)
					&& !strcmp(gate_ip, ifcfg_eth1.gateway))
	{
		restart = 0;
	}

	WRITE_LOG("ip[%s],mask[%s],gateway[%s]!\n", ipaddr, mask_ip, gate_ip);

	if (ip_rightful_check(ipaddr) && mask_rightful_check(mask_ip)
					&& gateway_rightful_check(gate_ip)) //地址格式正确
	{
		if (ip_gate_mask_check(ipaddr, gate_ip, mask_ip) != 1)
		{
			//ip/子网掩码/网关 不匹配
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-301\",\"ErrMessage\":\"ip/子网掩码/网关不匹配!-301\"}");
			goto END;
		}


		read_eth_config(BOX_IFCFG_LAN, &ifcfg_eth0);
		//检测两个ip是否在同一网段
		ret = check_net_segment(ifcfg_eth0.ipaddr, ifcfg_eth0.netmask, ipaddr,
						mask_ip);
		if (0 == ret) //表示wan口ip和lan口ip在同一网段
		{
			//wan口ip和lan口ip在同一网段
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-305\",\"ErrMessage\":\"WAN口 ip和LAN口 ip 不能在同一网段！-305\"}");
			goto END;
		}

		if (0 == strlen(sub_dns))
		{
			if (ip_rightful_check(man_dns))
			{
				memset(set_dns_str, 0, 512);
				sprintf(set_dns_str, "nameserver %s", man_dns);
			}
			else
			{
				goto END;
			}
		}
		else
		{
			if (ip_rightful_check(man_dns) && ip_rightful_check(sub_dns))
			{
				memset(set_dns_str, 0, 512);
				sprintf(set_dns_str, "nameserver %s\r\nnameserver %s", man_dns, sub_dns);
			}
			else
			{
				goto END;
			}
		}

		//WRITE_LOG("local ip[%s]\nconfig ip[%s]-------------[%d]\n", ifcfg_eth1.ipaddr, ipaddr, ret);
		if (strcmp(ipaddr, ifcfg_eth1.ipaddr))//ip地址无变化,网关/子网掩码/dns有变化,此时不需要判断ip冲突问题
		{
//			ret = my_arping_check_ip(ETH_X, ipaddr, 3);
			//ret = arping_check_ip(ipaddr);
			ret = arp_test_ip(ETH_X, ipaddr, 3);
			if (1 == ret)
			{
				//您设置的ip与其他设备ip冲突，请重新设置其他ip！
				//printf("Content type: application/json \n\n");
				printf("{\"Result\":\"-302\",\"ErrMessage\":\"您设置的ip与其他设备ip冲突，请重新设置其他ip！-302\"}");
				goto END;
			}
			else if (ret < 0)
			{
				printf("{\"Result\":\"-303\",\"ErrMessage\":\"ip探测失败，请重试！-303\"}");
				goto END;
			}
		}

		memset(&ifcfg_eth1, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
		strcpy(ifcfg_eth1.ipaddr, ipaddr);
		strcpy(ifcfg_eth1.netmask, mask_ip);
		strcpy(ifcfg_eth1.gateway, gate_ip);

		write_eth_config(BOX_IFCFG_WAN, &ifcfg_eth1);
		update_network_conf(set_dns_str, DNSCONF_SAVE_PATH);

		if (!restart)
		{
			printf("{\"Result\":\"1\",\"ErrMessage\":\"配置成功！\"}");
			goto END;
		}
	}
	else//地址格式错误
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-304\",\"ErrMessage\":\"地址格式错误!-304\"}");
		goto END;
	}

	//printf("{\"Result\":\"0\",\"Message\":\"配置成功，重启盒子，网络配置发生变化，一点通盒子即将重启，等待20秒后重新登录！\"}");
	printf("{\"Result\":\"0\",\"Message\":\"修改成功，请重新登录！\"}");

END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}

/************************修改Wan口配置信息END***********************/
/************************修改Wan口配置信息END***********************/
/************************修改Wan口配置信息END***********************/

/************************修改Lan口配置信息***********************/
/************************修改Lan口配置信息***********************/
/************************修改Lan口配置信息***********************/
HB_S32 ChangeMainLanConf(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_CHAR lan_ip[20] = {0};
	HB_CHAR lan_mask[20] = {0};
	HB_CHAR lan_gw[20] = {0};
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth;

	if ((pos = strstr(buf, "DevIp")))
	{
		sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]&DevGateway=%s", \
						lan_ip, lan_mask, lan_gw);
	}
	else
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"通信错误!-11\"}");
		goto END;
	}

	WRITE_LOG("ip[%s],mask[%s],gateway[%s]!", lan_ip, lan_mask, lan_gw);

	read_eth_config(BOX_IFCFG_LAN, &ifcfg_eth);
	if (!strcmp(lan_ip, ifcfg_eth.ipaddr) && !strcmp(lan_mask, ifcfg_eth.netmask) && !strcmp(lan_gw, ifcfg_eth.gateway))
	{

		//printf("Content type: application/json \n\n");
		printf("{\"Result\":-300,\"ErrMessage\":\"LAN口网络配置未发生变化!-300\"}");
		goto END;
	}

	if (ip_rightful_check(lan_ip) && mask_rightful_check(lan_mask)
					&& gateway_rightful_check(lan_gw))				//地址格式正确
	{
		if ((ret = ip_gate_mask_check(lan_ip, lan_gw, lan_mask)) != 1)
		{
			//ip/子网掩码/网关 不匹配
			//printf("Content type: application/json \n\n");
			printf( "{\"Result\":\"-301\",\"ErrMessage\":\"LAN口的 ip/子网掩码/网关  不匹配!-301\"}");
			goto END;
		}

		memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
		read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
		//检测两个ip是否在同一网段
		ret = check_net_segment(ifcfg_eth.ipaddr, ifcfg_eth.netmask, lan_ip, lan_mask);
		if (0 == ret) //表示wan口ip和lan口ip在同一网段
		{
			//wan口ip和lan口ip在同一网段
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-305\",\"ErrMessage\":\"LAN口的 ip和WAN口的 ip 不能在同一网段!-305\"}");
			goto END;
		}

		//ret = arping_check_ip(lan_ip);
//		ret = my_arping_check_ip(ETH_LAN, lan_ip, 3);
		ret = arp_test_ip(ETH_X, lan_ip, 3);
		if (1 == ret)
		{
			//您设置的ip与其他设备ip冲突，请重新设置其他ip！
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-302\",\"ErrMessage\":\"您设置的LAN口ip与其他设备ip冲突，请重新设置其他ip！-302\"}");
			goto END;
		}
		else if (ret < 0)
		{
			printf("{\"Result\":\"-303\",\"ErrMessage\":\"ip探测失败，请重试！-303\"}");
			goto END;
		}
	}
	else//地址格式错误
	{
		printf("{\"Result\":\"-304\",\"ErrMessage\":\"LAN口网络配置有误！-304\"}");
		goto END;
	}

	ret = alter_lan_main_ip(ETH_LAN, lan_ip, lan_mask, lan_gw);
	if (ret < 0)
	{
		printf("{\"Result\":\"-306\",\"ErrMessage\":\"设置LAN口ip失败！-306\"}");
		goto END;
	}

	GN_NET_LAYER_OBJ buf_cmd;
	HB_S32 sockfd = 0;
	HB_CHAR recv_buf[128] = {0};

	memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
	buf_cmd.iProtocolVer = 1;
	buf_cmd.iDataType = 9;
	buf_cmd.iBlockHeadFlag = 1;
	buf_cmd.iBlockEndFlag = 1;

	sprintf(buf_cmd.cBuffer, "<TYPE>SystemCmd</TYPE><CmdStr>%s</CmdStr>", RESTART_NETWORK);
	buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);

	create_socket_connect_ipaddr(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
	if (sockfd > 0)
	{
		send_data(&sockfd, (HB_VOID *)&buf_cmd, buf_cmd.iActLength, 2);
		recv_data(&sockfd, recv_buf, sizeof(recv_buf), 2);
		close_sockfd(&sockfd);
	}

#if 0
	if(!setuid(0)){
		WRITE_LOG("setuid successfully!\n");
	}else{
		WRITE_LOG("setuid error!\n");
	}

	FILE *file;
	char buf_cmd[1024] = {0};
	file = popen("RESTART_NETWORK", "r");
	if (file != NULL)
	{
		while(fgets(buf_cmd, 1024, file)!=NULL)
		{
			WRITE_LOG("system recv [%s]\n", buf_cmd);
			memset(buf_cmd, 0, sizeof(buf_cmd));
		}
	}
	pclose(file);
#endif
	//system(RESTART_NETWORK);

	printf("{\"Result\":\"0\",\"Message\":\"配置成功！\"}");

#if 0
	sprintf(msg_str, "<TYPE>SetLanIpAddr</TYPE><MsgData>%s</MsgData>", pos);
	//修改lan口主ip接口
	ret = set_lan_ip("127.0.0.1", MAIN_CTRL_PORT, msg_str);

	if (-1 == ret)
	{
		printf("{\"Result\":\"-306\",\"ErrMessage\":\"设置LAN口ip失败！-306\"}");
		goto END;
	}
#endif
END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}


//获取start_num与end_num之间的随机数
static HB_S32 random_number(HB_S32 start_num, HB_S32 end_num)
{
	HB_S32 ret_num = 0;
	srand((unsigned)time(0));
	ret_num = rand() % (end_num - start_num) + start_num;
	return ret_num;
}

//获取数据条数回调函数
static HB_S32 LoadLanIpListNum( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_para = (HB_CHAR *)para;

	strncpy(p_para, column_value[0], strlen(column_value[0]));

	return 0;
}

//添加辅助ip
HB_S32 AddLanSubIp(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_S32 ethx = 0;
	HB_CHAR eth_name[8] = {0};
	HB_CHAR c_lan_ip_num[8] = {0};
	HB_CHAR lan_ip[20] = {0};
	HB_CHAR lan_mask[20] = {0};
	HB_CHAR lan_gw[20] = {0};
	HB_CHAR sql[512] = {0};
    IFCFG_ETHX_ITEM_OBJ ifcfg_eth;


	if ((pos = strstr(buf, "DevIp")))
	{
		memset(sql, 0, sizeof(sql));
		//snprintf(sql, sizeof(sql), "select count (*) as num from lan_web_ip_data");
		strncpy(sql, "select count (*) as num from lan_web_ip_data", sizeof(sql));
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadLanIpListNum, (void *)c_lan_ip_num);
		WRITE_LOG("[%d] devices in database!\n", atoi(c_lan_ip_num));

		if (atoi(c_lan_ip_num) >= MAX_LAN_SUB_IP)
		{
			printf("{\"Result\":\"-309\",\"ErrMessage\":\"添加辅助ip的数量达到上限！-309\"}");
			goto END;
		}

		sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]&DevGateway=%s", \
						lan_ip, lan_mask, lan_gw);
	}
	else
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"通信错误!-11\"}");
		goto END;
	}

	if (ip_rightful_check(lan_ip) && mask_rightful_check(lan_mask))	//地址格式正确
	{
		if ((0 != strlen(lan_gw)) && (!gateway_rightful_check(lan_gw)))
		{
			printf("{\"Result\":\"-304\",\"ErrMessage\":\"添加的LAN口辅助ip的网关有误！-304\"}");
			goto END;
		}
		memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
		read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
		ret = check_net_segment(ifcfg_eth.ipaddr, ifcfg_eth.netmask,
						lan_ip, lan_mask);
		if (0 == ret) //表示wan口ip和lan口ip在同一网段
		{
			//wan口ip和lan口ip在同一网段
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-305\",\"ErrMessage\":\"LAN口的 ip和WAN口的 ip 不能在同一网段!-305\"}");
			goto END;
		}

//		ret = arping_check_ip(lan_ip);
		ret = arp_test_ip(ETH_X, lan_ip, 3);
		if (1 == ret)
		{
			//您设置的ip与其他设备ip冲突，请重新设置其他ip！
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-302\",\"ErrMessage\":\"您设置的LAN口ip与其他设备ip冲突，请重新设置其他ip！-302\"}");
			goto END;
		}
		else if (ret < 0)
		{
			printf("{\"Result\":\"-303\",\"ErrMessage\":\"ip探测失败，请重试！-303\"}");
			goto END;
		}
	}
	else //地址格式错误
	{
		printf("{\"Result\":\"-304\",\"ErrMessage\":\"添加的LAN口辅助ip网络配置有误！-304\"}");
		goto END;
	}

	ethx = random_number(1, 999);
	snprintf(eth_name, sizeof(eth_name), ETH_LAN ":%d", ethx);

	//添加lan口辅助ip接口
	ret = add_lan_sub_ip(eth_name, lan_ip, lan_mask, lan_gw);
	if (ret < 0)
	{
		printf("{\"Result\":\"-307\",\"ErrMessage\":\"设置LAN口ip失败(%d)!-307\"}", ret);
		goto END;
	}

ReTry:
	snprintf(sql, sizeof(sql), "INSERT INTO lan_web_ip_data (net_ethx,net_ipaddr,net_mask,net_gateway) values ('%d','%s','%s','%s')", \
					ethx, lan_ip, lan_mask, lan_gw);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
	if (ret < 0)
	{
		if (-2 == ret)
		{
			goto ReTry;
		}
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
	}

	GN_NET_LAYER_OBJ buf_cmd;
	HB_S32 sockfd = 0;
	HB_CHAR recv_buf[128] = {0};

	memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
	buf_cmd.iProtocolVer = 1;
	buf_cmd.iDataType = 9;
	buf_cmd.iBlockHeadFlag = 1;
	buf_cmd.iBlockEndFlag = 1;

	sprintf(buf_cmd.cBuffer, "<TYPE>SystemCmd</TYPE><CmdStr>%s</CmdStr>", RESTART_NETWORK);
	buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);

	create_socket_connect_ipaddr(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
	if (sockfd > 0)
	{
		send_data(&sockfd, (HB_VOID *)&buf_cmd, buf_cmd.iActLength, 2);
		recv_data(&sockfd, recv_buf, sizeof(recv_buf), 2);
		close_sockfd(&sockfd);
	}

	printf("{\"Result\":0,\"DevIp\":\"%s\", \"DevMask\":\"%s\", \"DevGateway\":\"%s\"}", \
					lan_ip, lan_mask, lan_gw);

END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}



static HB_S32 GetEthX( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_ethx = (HB_CHAR *)para;

	strncpy(p_ethx, column_value[0], strlen(column_value[0]));

	return 0;
}


HB_S32 DelLanSubIp(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_CHAR line_index[8] = {0};
	HB_CHAR ethx[8] = {0};
	HB_CHAR eth_name[16] = {0};
	HB_CHAR lan_ip[20] = {0};
	HB_CHAR lan_mask[20] = {0};
	HB_CHAR lan_gw[20] = {0};
	HB_CHAR sql[512] = {0};

	if ((pos = strstr(buf, "DelIndex")))
	{
		sscanf(pos, "DelIndex=%[^&]&LanSubIp=%[^&]&LanSubMask=%[^&]&LanSbuGateway=%s", \
						line_index, lan_ip, lan_mask, lan_gw);
	}
	else
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"通信错误!-11\"}");
		goto END;
	}

	snprintf(sql, sizeof(sql), "select net_ethx from lan_web_ip_data where net_ipaddr='%s'", lan_ip);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, GetEthX, (HB_VOID *)ethx);
	if (ret < 0)
	{
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
	}

	snprintf(sql, sizeof(sql), "Delete from lan_web_ip_data where net_ipaddr='%s'", lan_ip);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, GetEthX, (HB_VOID *)ethx);
	if (ret < 0)
	{
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
	}

	snprintf(eth_name, sizeof(eth_name), ETH_LAN ":%s", ethx);
	ret = del_lan_sub_ip(eth_name, lan_ip, lan_mask, lan_gw);
	if (ret < 0)
	{
		printf("{\"Result\":\"-308\",\"ErrMessage\":\"删除LAN口ip失败(%d)!-308\"}", ret);
		goto END;
	}

	printf("{\"Result\":0,\"DelIndex\":\"%s\", \"LanSubIp\":\"%s\"}", \
					line_index, lan_ip);
END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}

/************************修改Lan口配置信息END***********************/
/************************修改Lan口配置信息END***********************/
/************************修改Lan口配置信息END***********************/
