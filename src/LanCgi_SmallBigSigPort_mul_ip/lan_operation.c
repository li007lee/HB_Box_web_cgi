#include "lan_operation.h"
#include "lan_ip_conf.h"
#include "cgi.h"
#include "common.h"
#include "arping.h"
#include "ipcheck.h"
#include "lan_mul_port.h"
#include "net_api.h"

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


static int sq_cb_show_static_route_table(void * para, int n_column, char ** column_value, char ** column_name)
{
	HB_CHAR *p_static_route_list = (HB_CHAR *) para;
	HB_CHAR c_static_route_ip[512] = { 0 };

	snprintf(c_static_route_ip, sizeof(c_static_route_ip), \
			"{\"Segment\":\"%s\",\"Mask\":\"%s\",\"Gateway\":\"%s\"},", \
			column_value[0], column_value[1], column_value[2]);

	strcat(p_static_route_list, c_static_route_ip);

	return 0;
}

static HB_S32 readata_from_netconf(char *net_file_path, char *dns_file_path, IFCFG_ETHX_ITEM_HANDLE net_conf, NET_CONF_HANDLE dns_conf)
{
	HB_S32 i = 0;
    HB_CHAR line[512] = {0};
    HB_CHAR dns_ip[3][16];
    memset(&dns_ip[0][0], 0, sizeof(dns_ip));
    FILE *fp;
    fp = fopen(net_file_path,"rb");
    if(fp == NULL)
    {
        return -1;//文件打开错误
    }
    while (fgets(line, 511, fp))
    {
    	if (strstr(line, "address"))
    	{
    		line[strlen(line)]='\0';
    		sscanf(line, "%*s%s", net_conf->ipaddr);
    	}
    	else if (strstr(line, "netmask"))
    	{
    		line[strlen(line)]='\0';
    		sscanf(line, "%*s%s", net_conf->netmask);
    	}
    	else if (strstr(line, "gateway"))
    	{
    		line[strlen(line)]='\0';
    		sscanf(line, "%*s%s", net_conf->gateway);
    	}
    	memset(line, 0, 512);
    }
    fclose(fp);

    fp = fopen(dns_file_path, "rb");
    if(fp == NULL)
    {
        return -1;//文件打开错误
    }
    i = 0;
    memset(line, 0, 512);
    while (fgets(line, 511, fp))
    {
    	if (i > 2)
    	{
    		break;
    	}
    	if (strstr(line, "nameserver"))
    	{
    		line[strlen(line)]='\0';
    		memset(&dns_ip[i][0], 0, 16);
    		sscanf(line, "%*s%s", &dns_ip[i][0]);
    		i++;
    	}
    	memset(line, 0, 512);
    }
    if (i >1)
    {
    	memcpy(dns_conf->dns_main_ip, &dns_ip[0][0], 16);
    	memcpy(dns_conf->dns_sub_ip, &dns_ip[1][0], 16);
    }
    else
    {
    	memcpy(dns_conf->dns_main_ip, &dns_ip[0][0], 16);
    }
    fclose(fp);

    return 0;
}


HB_VOID GetNetConfInfo()
{
	HB_CHAR mac_str[32] = { 0 };
	HB_CHAR dev_mac[32] = { 0 };
	NET_CONF_OBJ dns_conf;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth;

	HB_CHAR c_wan_ip_info[256] = { 0 };
	//HB_CHAR c_lan_main_ip_info[256] = { 0 };
	HB_CHAR c_sub_ip_list[1024] = { 0 };
	HB_CHAR c_static_route_list[1024] = {0};
	HB_CHAR c_net_set_info[4096] = { 0 };

	/*******************wan口信息*******************/

	memset(&dns_conf, 0, sizeof(NET_CONF_OBJ));
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
#ifdef SMALL_BOX
	readata_from_netconf(BOX_IFCFG_WAN, DNSCONF_SAVE_PATH, &ifcfg_eth, &dns_conf);
#else
	readata_from_dns_conf(DNSCONF_SAVE_PATH, &dns_conf);
	read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
#endif

	get_mac_dev(mac_str, ETH_X);
	sprintf(dev_mac, "%02X:%02X:%02X:%02X:%02X:%02X",
					(unsigned char) mac_str[0], (unsigned char) mac_str[1],
					(unsigned char) mac_str[2], (unsigned char) mac_str[3],
					(unsigned char) mac_str[4], (unsigned char) mac_str[5]);

	snprintf(c_wan_ip_info, sizeof(c_wan_ip_info),
					"\"DevIp\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\",\"DevMainDNS\":\"%s\",\"DevSubDNS\":\"%s\",\"DevMac\":\"%s\"",
					ifcfg_eth.ipaddr, ifcfg_eth.netmask, ifcfg_eth.gateway,
					dns_conf.dns_main_ip, dns_conf.dns_sub_ip, dev_mac);

	WRITE_LOG("c_wan_ip_info:[%s]\n", c_wan_ip_info);

	/*******************wan口信息END*******************/

//	/*******************lan口主ip信息*******************/
//	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
//	//读LAN口网卡配置文件
//	read_eth_config(BOX_IFCFG_LAN, &ifcfg_eth);
//
//	snprintf(c_lan_main_ip_info, sizeof(c_lan_main_ip_info),
//					"\"DevIp\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\"",
//					ifcfg_eth.ipaddr, ifcfg_eth.netmask, ifcfg_eth.gateway);
//
//	/*******************lan口主ip信息END*******************/

	/*******************lan口辅助ip信息*******************/
	HB_CHAR *sql = "select net_ipaddr,net_mask,net_gateway from lan_web_ip_data";
	SqlOperation(sql, BOX_DATA_BASE_NAME, sq_cb_show_lan_ip_table,
					(void *) c_sub_ip_list);

	if (c_sub_ip_list[strlen(c_sub_ip_list) - 1] == ',')
	{
		c_sub_ip_list[strlen(c_sub_ip_list) - 1] = '\0';
	}

//	snprintf(c_net_set_info, sizeof(c_net_set_info),
//					"{\"Result\":\"0\",\"WanIP\":{%s},\"LanMainIP\":{%s},\"LanSubs\":[%s]}",
//					c_wan_ip_info, c_lan_main_ip_info, c_sub_ip_list);
//	snprintf(c_net_set_info, sizeof(c_net_set_info),
//					"{\"Result\":\"0\",\"WanIP\":{%s},\"LanSubs\":[%s]}",
//					c_wan_ip_info, c_sub_ip_list);

	/*******************lan口辅助ip信息END*******************/

	/*******************静态路由信息*******************/
	sql = "select segment,mask,gateway from static_route_data";
	SqlOperation(sql, BOX_DATA_BASE_NAME, sq_cb_show_static_route_table,
					(void *) c_static_route_list);

	if (c_static_route_list[strlen(c_static_route_list) - 1] == ',')
	{
		c_static_route_list[strlen(c_static_route_list) - 1] = '\0';
	}

	snprintf(c_net_set_info, sizeof(c_net_set_info),
					"{\"Result\":\"0\",\"WanIP\":{%s},\"LanSubs\":[%s],\"StaticRoute\":[%s]}",
					c_wan_ip_info, c_sub_ip_list, c_static_route_list);

	/*******************静态路由信息END*******************/

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

	NET_CONF_OBJ dns_conf;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth;

	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
	memset(&dns_conf, 0, sizeof(NET_CONF_OBJ));

	if ((pos = strstr(buf, "DevIp")))
	{
		sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]&DevGateway=%[^&]&DevMainDNS=%[^&]&DevSubDNS=%s", \
						ipaddr, mask_ip, gate_ip, man_dns, sub_dns);
	}
#ifdef SMALL_BOX
	readata_from_netconf(BOX_IFCFG_WAN, DNSCONF_SAVE_PATH, &ifcfg_eth, &dns_conf);
#else
	read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
	readata_from_dns_conf(DNSCONF_SAVE_PATH, &dns_conf);
#endif
	if (!strcmp(ipaddr, ifcfg_eth.ipaddr)
		&& !strcmp(gate_ip, ifcfg_eth.gateway)
		&& !strcmp(man_dns, dns_conf.dns_main_ip)
		&& !strcmp(sub_dns, dns_conf.dns_sub_ip))
	{
		printf("{\"Result\":\"-300\",\"ErrMessage\":\"Wan配置未发生变化!-300\"}");
		goto END;
	}

	if (!strcmp(ipaddr, ifcfg_eth.ipaddr) && !strcmp(mask_ip, ifcfg_eth.netmask)
					&& !strcmp(gate_ip, ifcfg_eth.gateway))
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
		if (strcmp(ipaddr, ifcfg_eth.ipaddr))//ip地址无变化,网关/子网掩码/dns有变化,此时不需要判断ip冲突问题
		{
//			ret = my_arping_check_ip(ETH_X, ipaddr, 3);
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

#ifdef SMALL_BOX
	    HB_CHAR set_net_str[1024] = {0};
		memset(set_net_str, 0, 1024);
		sprintf(set_net_str, "# Configure Loopback\nauto lo\n iface lo inet loopback\n\n\nauto eth0\n iface eth0 inet static\n"
			" address %s\n netmask %s\n gateway %s\n", ipaddr,mask_ip, gate_ip);
		update_network_conf(set_net_str,BOX_IFCFG_WAN);
		update_network_conf(set_dns_str,DNSCONF_SAVE_PATH);
#else
		memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
		strcpy(ifcfg_eth.ipaddr, ipaddr);
		strcpy(ifcfg_eth.netmask, mask_ip);
		strcpy(ifcfg_eth.gateway, gate_ip);

		write_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
		update_network_conf(set_dns_str, DNSCONF_SAVE_PATH);
#endif
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

	printf("{\"Result\":\"0\",\"Message\":\"配置成功！\"}");

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
	HB_CHAR eth_name[16] = {0};
	HB_CHAR c_lan_ip_num[8] = {0};
	HB_CHAR lan_ip[20] = {0};
	HB_CHAR lan_mask[20] = {0};
	HB_CHAR lan_gw[20] = {0};
	HB_CHAR sql[512] = {0};
    IFCFG_ETHX_ITEM_OBJ ifcfg_eth;
    NET_CONF_OBJ dns_conf;


	if ((pos = strstr(buf, "DevIp")))
	{
		memset(sql, 0, sizeof(sql));
		//snprintf(sql, sizeof(sql), "select count (*) as num from lan_web_ip_data");
		strncpy(sql, "select count (*) as num from lan_web_ip_data", sizeof(sql));
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadLanIpListNum, (void *)c_lan_ip_num);
		WRITE_LOG("[%d] sub ip in database!\n", atoi(c_lan_ip_num));

		if (atoi(c_lan_ip_num) >= MAX_LAN_SUB_IP)
		{
			printf("{\"Result\":\"-309\",\"ErrMessage\":\"添加辅助ip的数量达到上限！-309\"}");
			goto END;
		}
#ifdef STATIC_ROUTE_VERSION
		sscanf(pos, "DevIp=%[^&]&DevMask=%s", \
						lan_ip, lan_mask);
		//sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]", lan_ip, lan_mask);
#else
		sscanf(pos, "DevIp=%[^&]&DevMask=%[^&]&DevGateway=%s", \
						lan_ip, lan_mask, lan_gw);
#endif
		WRITE_LOG("lan_ip=[%s], lan_mask=[%s]\n", lan_ip, lan_mask);
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
#ifdef SMALL_BOX
		readata_from_netconf(BOX_IFCFG_WAN, DNSCONF_SAVE_PATH, &ifcfg_eth, &dns_conf);
#else
		read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
#endif
		ret = check_net_segment(ifcfg_eth.ipaddr, ifcfg_eth.netmask,
						lan_ip, lan_mask);
		if (0 == ret) //表示wan口ip和lan口ip在同一网段
		{
			//wan口ip和lan口ip在同一网段
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-305\",\"ErrMessage\":\"辅助ip不可与主ip 在同一网段!-305\"}");
			goto END;
		}

#if 1
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
		WRITE_LOG("DevIp=[%s]&DevMask=[%s]&DevGateway=[%s]", \
						lan_ip, lan_mask, lan_gw);
#endif
	}
	else //地址格式错误
	{
		printf("{\"Result\":\"-304\",\"ErrMessage\":\"添加的LAN口辅助ip网络配置有误！-304\"}");
		goto END;
	}

	ethx = random_number(1, 999);
	snprintf(eth_name, sizeof(eth_name), ETH_LAN ":%d", ethx);
	WRITE_LOG("ethx=%s", eth_name);
#if 0
	//添加lan口辅助ip接口
	ret = add_lan_sub_ip(eth_name, lan_ip, lan_mask, lan_gw);
	if (ret < 0)
	{
		printf("{\"Result\":\"-307\",\"ErrMessage\":\"设置LAN口ip失败(%d)!-307\"}", ret);
		goto END;
	}
#endif

#if 0
	GN_NET_LAYER_OBJ buf_cmd;
	HB_S32 sockfd = 0;

	memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
	buf_cmd.iProtocolVer = 1;
	buf_cmd.iDataType = 9;
	buf_cmd.iBlockHeadFlag = 1;
	buf_cmd.iBlockEndFlag = 1;

	//sprintf(buf_cmd.cBuffer, "<TYPE>SystemCmd</TYPE><CmdStr>%s</CmdStr>", RESTART_NETWORK);
#ifdef STATIC_ROUTE_VERSION
	sprintf(buf_cmd.cBuffer, \
				"<TYPE>SetIpAddr</TYPE><NetCard>%s</NetCard><IpAddr>%s</IpAddr><NetMask>%s</NetMask>", \
				eth_name, lan_ip, lan_mask);
#else
	sprintf(buf_cmd.cBuffer, "<TYPE>SetIpAddr</TYPE><NetCard>%s</NetCard><IpAddr>%s</IpAddr><NetMask>%s</NetMask><GateWay>%s</GateWay>", \
					eth_name, lan_ip, lan_mask, lan_gw);
#endif
	buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);
	WRITE_LOG("send to main_ctrl [%s]!\n", buf_cmd.cBuffer);
	create_socket_connect_ipaddr(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
	if (sockfd > 0)
	{
		send_data(&sockfd, (HB_VOID *)&buf_cmd, buf_cmd.iActLength, 2);
		memset(&buf_cmd, 0, sizeof(buf_cmd));
		recv_data(&sockfd, (HB_VOID *)&buf_cmd, sizeof(buf_cmd), 2);
		close_sockfd(&sockfd);

		//判断返回值
		if (strncmp(buf_cmd.cBuffer, "<Return>0</Return>", strlen("<Return>0</Return>")))
		{
			WRITE_LOG("recv from main_ctrl=[%s]", buf_cmd.cBuffer);
			printf("{\"Result\":\"-206\",\"ErrMessage\":\"添加辅助ip失败!\"}");
			goto END;
		}
	}
	else
	{
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"连接主控程序失败!\"}");
		goto END;
	}
#endif

	HB_CHAR cmd[512] = {0};
	snprintf(cmd, sizeof(cmd), "ifconfig %s %s netmask %s", eth_name, lan_ip, lan_mask);
	system(cmd);

ReTry:
	snprintf(sql, sizeof(sql), "INSERT INTO lan_web_ip_data (net_ethx,net_ipaddr,net_mask,net_gateway) values ('%d','%s','%s','%s')", \
					ethx, lan_ip, lan_mask, lan_gw);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
	if (ret < 0)
	{
		if (-2 == ret)
		{
			usleep(100*1000);
			goto ReTry;
		}
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
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
#ifdef STATIC_ROUTE_VERSION
		sscanf(pos, "DelIndex=%[^&]&LanSubIp=%[^&]&LanSubMask=%s", \
						line_index, lan_ip, lan_mask);
#else
		sscanf(pos, "DelIndex=%[^&]&LanSubIp=%[^&]&LanSubMask=%[^&]&LanSubGateway=%s", \
						line_index, lan_ip, lan_mask, lan_gw);
#endif
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

	snprintf(eth_name, sizeof(eth_name), ETH_LAN ":%s", ethx);
#ifdef STATIC_ROUTE_VERSION
	ret = del_lan_sub_ip(eth_name, lan_ip, lan_mask, "");
#else
	ret = del_lan_sub_ip(eth_name, lan_ip, lan_mask, lan_gw);
#endif
	if (ret < 0)
	{
		printf("{\"Result\":\"-308\",\"ErrMessage\":\"删除LAN口ip失败(%d)!-308\"}", ret);
		goto END;
	}

	snprintf(sql, sizeof(sql), "Delete from lan_web_ip_data where net_ipaddr='%s'", lan_ip);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, GetEthX, (HB_VOID *)ethx);
	if (ret < 0)
	{
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
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


/************************修改静态路由配置信息***********************/
/************************修改静态路由配置信息***********************/
/************************修改静态路由配置信息***********************/
//获取数据条数回调函数
static HB_S32 LoadListNum( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_para = (HB_CHAR *)para;

	strncpy(p_para, column_value[0], strlen(column_value[0]));

	return 0;
}

//添加静态路由
HB_S32 AddStaticRoute(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_CHAR c_route_num[8] = {0};
	HB_CHAR segment[16] = {0}; //网段
	HB_CHAR mask[16] = {0}; //子网掩码
	HB_CHAR gateway[16] = {0}; //网关
	HB_CHAR sql[512] = {0};

	if ((pos = strstr(buf, "Segment")))
	{
		memset(sql, 0, sizeof(sql));
		strncpy(sql, "select count (*) as num from static_route_data", sizeof(sql));
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadListNum, (void *)c_route_num);
		WRITE_LOG("[%d] route in database!\n", atoi(c_route_num));

		if (atoi(c_route_num) >= MAX_ROUTE)
		{
			printf("{\"Result\":\"-309\",\"ErrMessage\":\"添加静态路由的数量达到上限，最多添加%d个！\"}", MAX_ROUTE);
			goto END;
		}

		sscanf(pos, "Segment=%[^&]&Mask=%[^&]&Gateway=%s", \
						segment, mask, gateway);
		WRITE_LOG("Segment=[%s],Mask=[%s],Gateway=[%s]", \
				segment, mask, gateway);
	}
	else
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"通信错误!-11\"}");
		goto END;
	}

	HB_CHAR *p = &(segment[strlen(segment)]) - 2;
	if (strcmp(p, ".0") != 0)
	{
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"网段有误\"}");
		goto END;
	}

	HB_CHAR cmd[512] = {0};
	snprintf(cmd, sizeof(cmd), "route add -net %s netmask %s gw %s %s", \
			segment, mask, gateway, ETH_LAN);
	system(cmd);

	FILE *file = NULL;
	char line[128] = {0};
	HB_S32 set_succeed = 0;
	file = popen("route -n", "r");
    if (NULL != file)
    {
        while (fgets(line, 128, file) != NULL)
        {
//            WRITE_LOG("line=[%s]\n", line);
            if ((strstr(line, segment) != NULL) && (strstr(line, gateway) != NULL))
            {
            	set_succeed = 1;
            	break;
            }
        }

        if (set_succeed == 0)
        {
			pclose(file);
			file = NULL;
			WRITE_LOG("set route failed!\n");
			printf("{\"Result\":\"-206\",\"ErrMessage\":\"设置静态路由失败\"}");

			return -1;
        }
    }
//	system(cmd);

ReTry:
	snprintf(sql, sizeof(sql), \
			"INSERT INTO static_route_data (segment,mask,gateway) values ('%s','%s','%s')", \
			segment, mask, gateway);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
	if (ret < 0)
	{
		if (-2 == ret)
		{
			usleep(100*1000);
			goto ReTry;
		}
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
	}

	printf("{\"Result\":0,\"Segment\":\"%s\", \"Mask\":\"%s\", \"Gateway\":\"%s\"}", \
					segment, mask, gateway);

END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}


HB_S32 DelStaticRoute(HB_CHAR *buf)
{
	HB_CHAR *pos = buf;
	HB_S32 ret = 0;
	HB_CHAR line_index[8] = {0};
	HB_CHAR segment[20] = {0};
	HB_CHAR mask[20] = {0};
	HB_CHAR gateway[20] = {0};
	HB_CHAR sql[512] = {0};
	HB_CHAR cmd[512] = {0};

	if ((pos = strstr(buf, "DelIndex")))
	{
		sscanf(pos, "DelIndex=%[^&]&Segment=%[^&]&Mask=%[^&]&Gateway=%s", \
						line_index, segment, mask, gateway);
	}
	else
	{
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-11\",\"ErrMessage\":\"通信错误!-11\"}");
		goto END;
	}

	snprintf(cmd, sizeof(cmd), "route del -net %s netmask %s gw %s %s", segment, mask, gateway, ETH_X);
	system(cmd);

	snprintf(sql, sizeof(sql), "Delete from static_route_data where segment='%s'", segment);
	ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
	if (ret < 0)
	{
		printf("{\"Result\":\"-206\",\"ErrMessage\":\"操作数据库失败!-206\"}");
		goto END;
	}


	printf("{\"Result\":0,\"DelIndex\":\"%s\", \"Segment\":\"%s\"}", \
					line_index, segment);
END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;
}
/************************修改静态路由配置信息END***********************/
/************************修改静态路由配置信息END***********************/
/************************修改静态路由配置信息END***********************/
