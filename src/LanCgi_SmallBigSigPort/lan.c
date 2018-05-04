#include "my_include.h"
#include "arping.h"
#include "cgi.h"
#include "session.h"
#include "ipcheck.h"
#include "sqlite3.h"
#include "common.h"

typedef struct _tagNET_CONF
{
	HB_CHAR dns_main_ip[16];
	HB_CHAR dns_sub_ip[16];
}DNS_CONF_OBJ, *DNS_CONF_HANDLE;

typedef struct _tagIFCFG_ETH1_ITEM
{
	HB_CHAR device[16];
	HB_CHAR bootproto[16];
	HB_CHAR hwaddr[24];
	HB_CHAR ipaddr[16];
	HB_CHAR netmask[16];
	HB_CHAR gateway[16];
}IFCFG_ETH1_ITEM_OBJ ,*IFCFG_ETH1_ITEM_HANDLE;



static void mystrcpy(char *dest, const char *src)
{
    HB_S32 i=0,j=0;
    while(src[j++]!='=');
    while(src[j]!='\n')
    {
        dest[i++]=src[j++];
    }
    dest[i]='\0';
}

//判断两个ip是否为同一网段
//0:  表示在一个网段
// 1: 表示网段不同
#if 0
static HB_S32  check_net_segment(HB_CHAR*ip1, HB_CHAR*netmask1, HB_CHAR *ip2, HB_CHAR *netmask2)
{
 unsigned long int min=inet_addr(netmask1);
 if(min>inet_addr(netmask2))
 {
     min=inet_addr(netmask2);
 }
    return (inet_addr(ip1)&min)^(inet_addr(ip2)&min)?1:0;
}
#endif
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
HB_S32 read_eth_config(char *path,IFCFG_ETH1_ITEM_HANDLE ifcfg_eth)
{
    HB_CHAR *result;
    HB_CHAR buf[32];

    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }
    FILE *file=fopen(path,"r");
    if(file==NULL)
    {
        printf(" open  file   error\n");
        return -2;
    }

    result=fgets(buf,32,file);
    if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->device,buf);
    result=fgets(buf,32,file);
    if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->bootproto,buf);
    result=fgets(buf,32,file);
    if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->hwaddr,buf);
    result=fgets(buf,32,file);
     if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->ipaddr,buf);

     result=fgets(buf,32,file);
     if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->netmask,buf);

     result=fgets(buf,32,file);
     if(!result)
    {
        return -3;
    }
    mystrcpy(ifcfg_eth->gateway,buf);
    fclose(file);
    return 0;
}

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
HB_S32 write_eth_config(HB_CHAR *path,IFCFG_ETH1_ITEM_HANDLE ifcfg_eth)
{
    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }

    FILE *fp;
    HB_CHAR buf[200];
	IFCFG_ETH1_ITEM_OBJ ifcfg_ethr;
	memset(&ifcfg_ethr,0,sizeof(ifcfg_ethr));
	int result=read_eth_config(path,&ifcfg_ethr);
	if(result!=0)
	{
		return -2;
	}
	strcpy(ifcfg_eth->device,ifcfg_ethr.device);
	strcpy(ifcfg_eth->bootproto,ifcfg_ethr.bootproto);
	strcpy(ifcfg_eth->hwaddr,ifcfg_ethr.hwaddr);
    sprintf(buf,"DEVICE=%s\nBOOTPROTO=%s\nHWADDR=%s\nIPADDR=%s\nNETMASK=%s\nGATEWAY=%s\n",ifcfg_eth->device,ifcfg_eth->bootproto,ifcfg_eth->hwaddr,ifcfg_eth->ipaddr,ifcfg_eth->netmask,ifcfg_eth->gateway);
    //fclose(fp);

    fp = fopen(path, "wb");
    if(fp == NULL)
    {
    	return -1;
    }
    fprintf(fp,"%s",buf);
    fflush(fp);
    fclose(fp);
    return 0;

#if 0
    int fd=open(path,O_RDWR);
    if(fd<0)
    {
        return -2;
    }
    char buf[200];

	IFCFG_ETH1_ITEM_OBJ ifcfg_ethr;
	memset(&ifcfg_ethr,0,sizeof(ifcfg_ethr));
	int result=read_eth_config(path,&ifcfg_ethr);
	if(result!=0)
	{
		return -2;
	}
	strcpy(ifcfg_eth->device,ifcfg_ethr.device);
	strcpy(ifcfg_eth->bootproto,ifcfg_ethr.bootproto);
	strcpy(ifcfg_eth->hwaddr,ifcfg_ethr.hwaddr);
    sprintf(buf,"DEVICE=%s\nBOOTPROTO=%s\nHWADDR=%s\nIPADDR=%s\nNETMASK=%s\nGATEWAY=%s\n",ifcfg_eth->device,ifcfg_eth->bootproto,ifcfg_eth->hwaddr,ifcfg_eth->ipaddr,ifcfg_eth->netmask,ifcfg_eth->gateway);
    close(fd);
	fd=open(path,O_RDWR|O_TRUNC);
	 if(fd<0)
    {
        return -2;
    }
	write(fd,buf,strlen(buf));
    close(fd);
#endif
    return 0;
}


//获取网卡序列号
//mac_sn 网卡序列号, dev 网卡名
HB_S32 get_mac_dev(char *mac_sn, char *dev)
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


HB_S32 read_conf_value(char *file_path, char *outbuf)
{
    FILE *fp;
    fp = fopen(file_path,"r");
    if(fp == NULL)
    {
        return -1;//文件打开错误
    }
    fgets(outbuf, 1023, fp);
    fclose(fp);
    return 0;
}

HB_S32 update_network_conf(char *str,const char *file)
{
    // 更新配置文件,应该有备份，下面的操作会将文件内容清除
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

HB_S32 readata_from_netconf(char *net_file_path, char *dns_file_path, IFCFG_ETH1_ITEM_HANDLE net_conf, DNS_CONF_HANDLE dns_conf)
{
	//int ret = 0;
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


HB_S32 readata_from_dns_conf(char *dns_file_path,DNS_CONF_HANDLE dns_conf)
{
	//HB_S32 ret = 0;
	HB_S32 i = 0;
	HB_CHAR line[512] = {0};
	HB_CHAR dns_ip[3][16];
    memset(&dns_ip[0][0], 0, sizeof(dns_ip));
    FILE *fp;
#if 0
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
    		sscanf(line, "%*s%s", net_conf->ip_addr);
    	}
    	else if (strstr(line, "netmask"))
    	{
    		line[strlen(line)]='\0';
    		sscanf(line, "%*s%s", net_conf->net_mask);
    	}
    	else if (strstr(line, "gateway"))
    	{
    		line[strlen(line)]='\0';
    		sscanf(line, "%*s%s", net_conf->gate_way);
    	}
    	memset(line, 0, 512);
    }
    fclose(fp);
#endif
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


HB_S32 main()
{
	HB_CHAR *buf=NULL;
	HB_S32 ret = 0;
    HB_S32 DataCode = 0;
    HB_CHAR ipaddr[16]= {0};
    HB_CHAR mask_ip[20]= {0};
    HB_CHAR gate_ip[20]= {0};
    HB_CHAR man_dns[20] = {0};
    HB_CHAR sub_dns[20] = {0};
    HB_CHAR mac_str[32] = {0};
    HB_CHAR dev_mac[32] = {0};
    HB_CHAR set_net_str[1024] = {0};
    HB_CHAR set_dns_str[512] = {0};

    DNS_CONF_OBJ dns_conf;
	IFCFG_ETH1_ITEM_OBJ ifcfg_eth;
	memset(&ifcfg_eth, 0, sizeof(IFCFG_ETH1_ITEM_OBJ));
    memset(&dns_conf, 0, sizeof(DNS_CONF_OBJ));

    CgiInitAndConfig(&buf, CGI_SESSION_SAVE_PATH);
	if (cgi_session_var_exists("USER_NAME") && cgi_session_var_exists("PASSWORD")
			&& cgi_session_var_exists("LAST_ACCESS_TIME")) //服务器端有session文件
	{
		if(CheckUsrPasswd())//session和配置文件匹配
		{
#if 1
			if (CalcTimeOut())
			{
				printf("Content type: text/xml \n\n");
				printf("{\"Result\":\"-1\",\"ErrMessage\":\"用户登录超时,请重新登录!\"}");
				goto END;
			}
#endif
			HB_CHAR *pos = NULL;
			DataCode = GetDataCode(buf);
			if(1030 == DataCode)//修改网络配置信息
			{
				HB_S32 restart = 1; //用于标记配置成功后是否需要重启，1重启 0不重启
				if((pos = strstr(buf, "DevIP")))
				{
					sscanf(pos, "DevIP=%[^&]&DevMask=%[^&]&DevGateway=%[^&]&DevMainDNS=%[^&]&DevSubDNS=%s",
								ipaddr, mask_ip, gate_ip, man_dns, sub_dns);
				}
#ifdef SMALL_BOX
				readata_from_netconf(BOX_IFCFG_WAN, DNSCONF_SAVE_PATH, &ifcfg_eth, &dns_conf);
#endif

#ifdef BIG_BOX_SINGLE_PORT
				read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
				readata_from_dns_conf(DNSCONF_SAVE_PATH, &dns_conf);
#endif
				if(!strcmp(ipaddr, ifcfg_eth.ipaddr) && !strcmp(mask_ip, ifcfg_eth.netmask)
						&& !strcmp(gate_ip, ifcfg_eth.gateway) && !strcmp(man_dns, dns_conf.dns_main_ip)
						&& !strcmp(sub_dns, dns_conf.dns_sub_ip))
				{
					//网络配置未发生变化
					//printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-300\",\"ErrMessage\":\"网络配置未发生变化!-300\"}");

					goto END;
				}

				if(!strcmp(ipaddr, ifcfg_eth.ipaddr) && !strcmp(mask_ip, ifcfg_eth.netmask)
						&& !strcmp(gate_ip, ifcfg_eth.gateway))
				{
					//只修改了dns,不需要重启盒子
					restart = 0;
				}

				if(ip_rightful_check(ipaddr) && mask_rightful_check(mask_ip) && gateway_rightful_check(gate_ip))//地址格式正确
				{
					if((ret = ip_gate_mask_check(ipaddr, gate_ip, mask_ip)) != 1)
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
					}
					else
					{
						if (ip_rightful_check(man_dns) && ip_rightful_check(sub_dns))
						{
							memset(set_dns_str, 0, 512);
							sprintf(set_dns_str, "nameserver %s\r\nnameserver %s", man_dns, sub_dns);
						}
					}

					//WRITE_LOG("local ip[%s]\nconfig ip[%s]\n", ifcfg_eth.ipaddr, ipaddr);
					//ip地址无变化,网关/子网掩码/dns有变化,此时不需要判断ip冲突问题
					if(strcmp(ipaddr, ifcfg_eth.ipaddr))
					{
						//ret = arping_check_ip(ipaddr);
//						ret = my_arping_check_ip(ETH_X, ipaddr, 3);
						ret = arp_test_ip(ETH_X, ipaddr, 3);
						WRITE_LOG("arping [%s] return val=[%d]\n", ipaddr, ret);
						if(1 == ret)
						{
							//您设置的ip与其他设备ip冲突，请重新设置其他ip！
							//printf("Content type: application/json \n\n");
							printf("{\"Result\":\"-302\",\"ErrMessage\":\"您设置的ip与其他设备ip冲突，请重新设置其他ip！-302\"}");
							goto END;
						}
						else if (ret < 0)
						{
							printf("{\"Result\":\"-303\",\"ErrMessage\":\"ip探测失败,请重试！-303\"}");
							goto END;
						}
					}
#ifdef SMALL_BOX
					memset(set_net_str, 0, 1024);
					sprintf(set_net_str, "# Configure Loopback\nauto lo\n iface lo inet loopback\n\n\nauto eth0\n iface eth0 inet static\n"
						" address %s\n netmask %s\n gateway %s\n", ipaddr,mask_ip, gate_ip);
					update_network_conf(set_net_str,BOX_IFCFG_WAN);
					update_network_conf(set_dns_str,DNSCONF_SAVE_PATH);

#endif

#ifdef BIG_BOX_SINGLE_PORT
					memset(&ifcfg_eth, 0, sizeof(IFCFG_ETH1_ITEM_OBJ));
					strcpy(ifcfg_eth.ipaddr, ipaddr);
					strcpy(ifcfg_eth.netmask, mask_ip);
					strcpy(ifcfg_eth.gateway, gate_ip);

					write_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
					update_network_conf(set_dns_str,DNSCONF_SAVE_PATH);
#endif
					if (!restart)
					{
						printf("{\"Result\":\"1\",\"ErrMessage\":\"配置成功！\"}");
						goto END;
					}
				}
				else
				{
					//printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-304\",\"ErrMessage\":\"地址格式错误!-304\"}");
					goto END;
				}

				//printf("{\"Result\":\"0\",\"Message\":\"配置成功，重启盒子，网络配置发生变化，一点通盒子即将重启，等待20秒后重新登录！\"}");
				printf("{\"Result\":\"0\",\"Message\":\"修改成功，重新登录！\"}");
			}
			else if (1029 == DataCode) //获取当前网络配置信息
			{
#ifdef SMALL_BOX
				readata_from_netconf(BOX_IFCFG_WAN, DNSCONF_SAVE_PATH, &ifcfg_eth, &dns_conf);
#endif

#ifdef BIG_BOX_SINGLE_PORT
				read_eth_config(BOX_IFCFG_WAN, &ifcfg_eth);
				readata_from_dns_conf(DNSCONF_SAVE_PATH, &dns_conf);
#endif
				get_mac_dev(mac_str, ETH_X);
			    sprintf(dev_mac, "%02X:%02X:%02X:%02X:%02X:%02X",
			    		(unsigned char)mac_str[0],
						(unsigned char)mac_str[1],
			            (unsigned char)mac_str[2],
			            (unsigned char)mac_str[3],
			            (unsigned char)mac_str[4],
			            (unsigned char)mac_str[5]
			            );
				//printf("Content type: application/json \n\n");
				printf("{\"Result\":\"0\",\"DevIP\":\"%s\",\"DevMask\":\"%s\",\"DevGateway\":\"%s\",\"DevMainDNS\":\"%s\",\"DevSubDNS\":\"%s\",\"DevMAC\":\"%s\"}", \
						ifcfg_eth.ipaddr, ifcfg_eth.netmask, ifcfg_eth.gateway, dns_conf.dns_main_ip, dns_conf.dns_sub_ip, dev_mac);
			}
			else
			{
				printf("{\"Result\":\"-2\",\"ErrMessage\":\"通信错误！-2\"}");
			}
		}
		else//session和配置文件不匹配， 意味着用户名和密码输入错误
		{
			printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-3\"}");
		}
	}
	else
	{
		printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-4\"}");
	}

END:
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	cgi_end();
	return 0;

}
