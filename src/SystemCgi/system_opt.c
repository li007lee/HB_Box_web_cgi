#include "system_opt.h"

SYSTEM_WEB_DATA_OBJ sys_web_data;

/*******************************************************************************
**********************************恢复出厂设置************************************
********************************************************************************/
#ifdef SMALL_BOX
//更新配置文件
static HB_S32 update_conf_file(HB_CHAR *str,const HB_CHAR *file)
{
    //更新配置文件,应该有备份，下面的操作会将文件内容清除
    FILE *fp;
    fp = fopen(file, "wb");
    if(fp == NULL)
    {
    	return -1;
    }
    fprintf(fp,"%s",str);
    fflush(fp);
    fclose(fp);
    return 0;
}
#endif

//恢复出厂
HB_S32 FactroyReset()
{
	HB_S32 ret;

#ifdef N2208C
	IFCFG_ETH1_ITEM_OBJ ifcfg_eth;
	HB_CHAR set_dns_str[128] = {0};
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETH1_ITEM_OBJ));
	strcpy(ifcfg_eth.ipaddr, "192.168.0.99");
	strcpy(ifcfg_eth.netmask, "255.255.255.0");
	strcpy(ifcfg_eth.gateway, "192.168.0.1");
	write_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);//初始化Wan口网卡配置文件

#ifdef MULTIPLE_PORT
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETH1_ITEM_OBJ));
	strcpy(ifcfg_eth.ipaddr, "192.168.100.99");
	strcpy(ifcfg_eth.netmask, "255.255.255.0");
	strcpy(ifcfg_eth.gateway, "192.168.100.1");
	write_eth_config(BOX_IFCFG_LAN, &ifcfg_eth);//初始化Lan口网卡配置文件
#endif

	sprintf(set_dns_str, "nameserver 223.5.5.5");
	update_file(set_dns_str,DNSCONF_SAVE_PATH); //初始化DNS配置文件

	ret = my_system("rm /mnt/mtd/ydt_box -r");

	printf("{\"Result\":\"0\",\"Message\":\"恢复出厂设置成功，设备将重启，登录设备ip为192.168.0.99！\"}");

	return 0;

#endif

#ifdef SMALL_BOX
	HB_CHAR sys_cmd[256] = {0};
	HB_CHAR set_net_str[1024] = {0};
	HB_CHAR set_dns_str[512] = {0};
	HB_S32 factory_reset_flag = 0;
	memset(set_net_str, 0, 1024);
	memset(set_dns_str, 0, 512);
	HB_S32 er = 0;

	ret = my_system("cp /home/default /tmp/ -r");//先复制一份原有配置
	if(ret != 0)
	{
		er=1;
		factory_reset_flag =2;
		goto Err;
	}

	sprintf(set_net_str, "# Configure Loopback\nauto lo\n iface lo inet loopback\n\n\nauto eth0\n iface eth0 inet static\n"
			" address 192.168.0.99\n netmask 255.255.255.0\n gateway 192.168.0.1\n");
	sprintf(set_dns_str, "nameserver 223.5.5.5");
	ret = update_conf_file(set_net_str, BOX_IFCFG_WAN);
	if(ret != 0)
	{
		er=2;
		factory_reset_flag =1;
		goto Err;
	}

	ret = update_conf_file(set_dns_str, DNSCONF_SAVE_PATH);
	if(ret != 0)
	{
		er=3;
		factory_reset_flag =1;
		goto Err;
	}
#if 0
	//system("rm /home/default/shadow");
	sprintf(sys_cmd, "rm %s", BOX_DIRECT_DEV_MAP_FILE);
	ret = my_system(sys_cmd);

	if(ret != 0)
	{
		er=4;
		factory_reset_flag =1;
		break;
	}
#endif

	ret = my_system(RM_SESSION_FILE);
	if(ret != 0)
	{
		er=5;
		factory_reset_flag =1;
		goto Err;
	}

	memset(sys_cmd, 0, sizeof(sys_cmd));
	sprintf(sys_cmd, "rm %s", BOX_DATA_BASE_NAME);
	ret = my_system(sys_cmd);
	if(ret != 0)
	{
		er=6;
		factory_reset_flag = 1;
		goto Err;
	}

	//if (!update_conf_file(set_net_str, NETCONF_SAVE_PATH))
	//恢复出厂设置成功
	if (0 != factory_reset_flag)
	{
		if(1 == factory_reset_flag)
		{
			my_system("rm /home/default/* -r");
			sleep(1);
			my_system("cp /tmp/default/* /home/default/ -r");
			my_system("rm /tmp/default -r");
		}
		printf("{\"Result\":\"-400\",\"ErrMessage\":\"恢复出厂设置失败！-400\"}");
		return -1;
	}
	printf("{\"Result\":\"0\",\"Message\":\"恢复出厂设置成功，设备将重启，登录设备ip为192.168.0.99！\"}");
	return 0;

Err:
	printf("{\"Result\":\"-11\",\"Message\":\"内部错误[%d]！-11\"}", er);
	return er;
#endif
}
/*******************************************************************************
*********************************恢复出厂设置END**********************************
********************************************************************************/


/*******************************************************************************
**********************************tcpdump 抓包***********************************
********************************************************************************/

HB_S32 TcpdumpCapture(HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 status = 0;
	HB_CHAR duration[8] = {0};
	HB_CHAR filepath[32] = {0};
	HB_CHAR cmd[128] = {0};
	HB_CHAR opt[256] = {0};

	if((pos = strstr(buf, "SysData")))
	{
		sscanf(pos, "SysData=%[^&]&DataTime=%s", cmd, duration);
		url_decode(cmd, strlen(cmd));
		WRITE_LOG("tcpdump cmd and time: [%s, %s]\n", cmd, duration);

		if ((pos = strstr(cmd, "tcpdump")) != NULL)
		{
			sprintf(filepath, "%s%s", DOWNLOAD_FILE_PCAP_PATH, DOWNLOAD_FILE_PCAP_NAME);
			//删除上一次的抓包数据
			snprintf(opt, sizeof(opt), "rm %s", filepath);
			system(opt);

			memset(opt, 0, sizeof(opt));
			//snprintf(opt, sizeof(opt), "/ipnc/sbin/%s -w %s &", pos, filepath);
			snprintf(opt, sizeof(opt), "%s -w %s &", pos, filepath);

			status = system(opt);
			if ((-1 != status) && (WIFEXITED(status)) && (0 == WEXITSTATUS(status)))
			{
				sleep(atoi(duration));
				system("killall -9 tcpdump");
				if (access(filepath, F_OK) != -1)
					printf("{\"Result\":\"0\",\"Message\":\"抓包成功！\"}");
				else
					printf("{\"Result\":\"-401\",\"ErrMessage\":\"抓包失败,请检查操作指令！-401\"}");
			}
			else
			{
				printf("{\"Result\":\"-402\",\"ErrMessage\":\"输入的抓包指令有误，请重新确认！-402\"}");
			}
		}
		else
		{
			printf("{\"Result\":\"-403\",\"ErrMessage\":\"输入的抓包指令有误，请重新确认！-403\"}");
		}
	}
	return 0;
}


/*******************************************************************************
********************************tcpdump 抓包END**********************************
********************************************************************************/


/*******************************************************************************
**********************************修改用户名密码***********************************
********************************************************************************/
HB_S32 ChangeUserPwd(HB_CHAR *src_buf)
{
	HB_CHAR *buf = src_buf;
	HB_CHAR *pos = NULL;
	HB_S32 ret;
	HB_CHAR sql[1024] = {0};

	if((pos = strstr(buf, "NewName")))
	{
		HB_S32 i = 0;
		HB_S32 usr_name_len = 0;
		HB_S32 err_flag = 0;
		HB_CHAR new_name[20] = {0};
		HB_CHAR new_passwd[128] = {0};
		HB_CHAR re_new_passwd[128] = {0};
		HB_CHAR old_passwd[128] = {0};

		sscanf(pos, "NewName=%[^&]&NewPas=%[^&]&AffNewPas=%[^&]&OldPas=%s",
				new_name, new_passwd, re_new_passwd, old_passwd);

		if (strlen(new_name) && strlen(new_passwd) && strlen(re_new_passwd)
				&& strlen(old_passwd) )
		{
			usr_name_len = strlen(new_name);
			for(i = 0; i < usr_name_len; i++)
			{
				if(!isalnum(new_name[i]))//输入的用户名有误
				{
					err_flag = 1;
					break;
				}
			}
			if (err_flag) //新用户名输入错误
			{
				printf("{\"Result\":\"-410\",\"ErrMessage\":\"新用户名输入错误！用户名只能包含字母和数字！-410\"}");
				return -1;
			}
			if (usr_name_len < 5 || usr_name_len >16)
			{
				printf("{\"Result\":\"-411\",\"ErrMessage\":\"用户名长度必须为5到16个字符！-411\"}");
				return -2;
			}

			if (strcmp(sys_web_data.adm_usr_passwd, old_passwd))//原密码错误
			{
				printf("{\"Result\":\"-412\",\"ErrMessage\":\"原密码输入错误！-412\"}");
				return -3;
			}
			else
			{
				memset(sql, 0, 1024);
				sprintf(sql, "UPDATE system_web_data SET adm_usr_name = '%s', adm_usr_passwd = '%s'", \
						new_name, new_passwd);
				ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
				if(ret == -2)
				{
					printf("{\"Result\":\"-413\",\"ErrMessage\":\"新用户名和密码设置失败！-413\"}");
					return -4;
				}
			}
		}
	}
	else
	{
		//json串错误
		printf("{\"Result\":\"-10\",\"ErrMessage\":\"Json解析错误！-10\"}");
		return -5;
	}

	printf("{\"Result\":\"0\",\"Message\":\"密码设置成功，请重新登录！\"}");
	return 0;
}
/*******************************************************************************
*********************************修改用户名密码END*********************************
********************************************************************************/


/*******************************************************************************
************************************获取版本号************************************
********************************************************************************/

#if 0
//获取网卡序列号
//mac_sn 网卡序列号, dev 网卡名
static HB_S32 get_mac_dev(HB_CHAR *mac_sn, HB_CHAR *dev)
{
    struct ifreq tmp;
    HB_S32 sock_mac;
   // HB_CHAR *tmpflag;
    //HB_CHAR mac_addr[30];
    sock_mac = socket(AF_INET, SOCK_STREAM, 0);
    if( sock_mac == -1)
    {
        return -1;
    }
    memset(&tmp,0,sizeof(tmp));
    strncpy(tmp.ifr_name, dev, sizeof(tmp.ifr_name)-1 );
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &tmp)) < 0 )
    {
    	close(sock_mac);
        return -1;
    }

    close(sock_mac);
    memcpy(mac_sn, tmp.ifr_hwaddr.sa_data, 6);
    return 0;
}
#endif

//用于获取是否开启了云端验证
static HB_S32 GetCloudVerifyStatus( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_cloud_Verify = (HB_CHAR *)para;

	strncpy(p_cloud_Verify, column_value[0], strlen(column_value[0]));

	return 0;
}

//用于获取是否开启了广域网
static HB_S32 GetWanConnectionStatus( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *pWanConnection = (HB_CHAR *)para;

	strncpy(pWanConnection, column_value[0], strlen(column_value[0]));

	return 0;
}

HB_S32 GetBoxVersion()
{
	FILE *fp_t;
	HB_CHAR c_cloud_verify[8] = {0};
	HB_CHAR arrcWanConnection[8] = {0};
	HB_CHAR box_version[32] = {0};
	HB_CHAR box_name_type[64]={0};
	HB_CHAR sql[512] = {0};

	fp_t = fopen(BOX_VERSION_FILE, "r");
	if (NULL != fp_t)
	{
		HB_CHAR *ps1 = NULL;
		fgets(box_name_type, 64, fp_t);//盒子型号
		fgets(box_version, 32, fp_t); //盒子版本
		if ((ps1=strchr(box_version,'\r')) != NULL)
		{
			*ps1 = '\0';
		}
		else if ((ps1=strchr(box_version,'\n')) != NULL)
		{
			*ps1 = '\0';
		}
		fclose(fp_t);
	}

	//获取云端验证状态
	memset(sql, 0, sizeof(sql));
	snprintf(sql, sizeof(sql), "select cloud_verify from system_web_data");
	SqlOperation(sql, BOX_DATA_BASE_NAME, GetCloudVerifyStatus, (void *)c_cloud_verify);

	//获取是否开启广域网状态
	memset(sql, 0, sizeof(sql));
	snprintf(sql, sizeof(sql), "select wan_connection from system_web_data");
	SqlOperation(sql, BOX_DATA_BASE_NAME, GetWanConnectionStatus, (void *)arrcWanConnection);

	//printf("Content type: application/json \n\n");
	//printf("{\"Result\":\"0\",\"CurrentNum\":\"%s\",\"Sn\":\"qrClass=mediaGateway;sn=%s\",\"CloudVerify\":\"%s\"}", box_version, c_mac_sn, c_cloud_verify);
	printf("{\"Result\":\"0\",\"CurrentNum\":\"%s\",\"CloudVerify\":\"%s\",\"WanConnection\":\"%s\"}", box_version, c_cloud_verify, arrcWanConnection);

	WRITE_LOG("send to web : [{\"Result\":\"0\",\"CurrentNum\":\"%s\",\"CloudVerify\":\"%s\",\"WanConnection\":\"%s\"}]\n", box_version, c_cloud_verify, arrcWanConnection);

	return 0;
}

/*******************************************************************************
***********************************获取版本号END***********************************
********************************************************************************/


/*******************************************************************************
********************************设置是否开启云端验证********************************
********************************************************************************/

HB_S32 SetCloudVerifyStatus(HB_CHAR *src_buf)
{
	HB_CHAR *buf = src_buf;
	HB_CHAR *pos = NULL;
	HB_CHAR c_cloud_verify[8] = {0};
	HB_CHAR arrcWanConnection[8] = {0};
	HB_CHAR sql[512] = {0};

	if((pos = strstr(buf, "CloudVerify")))
	{
		sscanf(pos, "CloudVerify=%s", c_cloud_verify);

		memset(sql, 0, sizeof(sql));
		sprintf(sql, "UPDATE system_web_data SET cloud_verify = '%s'", c_cloud_verify);
		SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
		printf("{\"Result\":\"0\",\"Message\":\"设置成功！\"}");
		return 0;
	}

	printf("{\"Result\":\"-10\",\"Message\":\"Json解析错误！\"}");
	return -1;
}

/*******************************************************************************
******************************设置是否开启云端验证END******************************
********************************************************************************/


/*******************************************************************************
********************************设置是否开启广域网********************************
********************************************************************************/

HB_S32 SetWanConnection(HB_CHAR *src_buf)
{
	HB_CHAR *buf = src_buf;
	HB_CHAR *pos = NULL;
	HB_CHAR arrcWanConnection[8] = {0};
	HB_CHAR arrcCloudyVerify[8] = {0};
	HB_CHAR sql[512] = {0};

	if((pos = strstr(buf, "WanConnection")))
	{
		sscanf(pos, "WanConnection=%s", arrcWanConnection);

		memset(sql, 0, 512);
		sprintf(sql, "UPDATE system_web_data SET wan_connection = '%s'", arrcWanConnection);
		SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);

		//若开启广域网，需要重新获取验证服务器地址
		int iSockFd = -1;
		struct sockaddr_in serverAddr;
		iSockFd=socket(AF_INET,SOCK_STREAM,0);

		memset(&serverAddr,0,sizeof(serverAddr));
		serverAddr.sin_family=AF_INET;
		serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
		serverAddr.sin_port=htons(7879);

		if (atoi(arrcWanConnection) == 0)  //1开启广域网，0关闭广域网
		{
			if(-1==connect(iSockFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))
			{
				WRITE_LOG("Connetc iptable server failed\n");
			}
			else
			{
				WRITE_LOG("send SetWan&Value=0\n");
				send(iSockFd, "SetWan&Value=0", strlen("GetStreamInfo"), 0);
				close(iSockFd);
				iSockFd = -1;
			}
		}
		else
		{
			//若开启广域网，需要重新获取验证服务器地址
			if(-1==connect(iSockFd,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))
			{
				WRITE_LOG("Connetc iptable server failed\n");
			}
			else
			{
				WRITE_LOG("send GetStreamInfo&SetWan&Value=1\n");
				send(iSockFd, "GetStreamInfo&SetWan&Value=1", strlen("GetStreamInfo&SetWan&Value=1"), 0);
				close(iSockFd);
				iSockFd = -1;
			}
		}
	}

	printf("{\"Result\":\"0\",\"Message\":\"设置成功！\"}");
	WRITE_LOG("send to web : [{\"Result\":\"0\",\"Message\":\"设置成功！\"}]\n");
	return 0;
}

/*******************************************************************************
******************************设置是否开启广域网END******************************
********************************************************************************/
