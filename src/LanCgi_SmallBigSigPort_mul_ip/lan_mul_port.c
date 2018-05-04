#include "my_include.h"

#include "data_base_ctrl.h"
#include "multi_virtual_ip.h"
#include "arping.h"
#include "cgi.h"
#include "session.h"
#include "ipcheck.h"
#include "sqlite3.h"
#include "lan_operation.h"
#include "lan_ip_conf.h"
#include "common.h"
#include "lan_mul_port.h"


typedef struct _tagSYSTEM_WEB_DATA
{
	HB_CHAR adm_usr_name[32];//用户名
	HB_CHAR adm_usr_passwd[64];//密码，md5加密过
}SYSTEM_WEB_DATA_OBJ, *SYSTEM_WEB_DATA_HANDLE;

SYSTEM_WEB_DATA_OBJ sys_web_data;

HB_S32 load_sys_web_usr_passwd( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	memset(&sys_web_data, 0, sizeof(SYSTEM_WEB_DATA_OBJ));
	strncpy(sys_web_data.adm_usr_name, column_value[0], strlen(column_value[0]));
	strncpy(sys_web_data.adm_usr_passwd, column_value[1], strlen(column_value[1]));
	return 0;
}


static HB_VOID mystrcpy(HB_CHAR *dest, const HB_CHAR *src)
{
	HB_S32 i=0,j=0;
    while(src[j++]!='=');
    while(src[j]!='\n')
    {
        dest[i++]=src[j++];
    }
    dest[i]='\0';
}


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
HB_S32 read_eth_config(HB_CHAR *path,IFCFG_ETHX_ITEM_HANDLE ifcfg_eth)
{
    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }
    FILE *file=fopen(path,"r");
    if(file==NULL)
    {
        WRITE_LOG(" open  file[%s]   error\n", path);
        return -2;
    }
    HB_CHAR *result;
    HB_CHAR buf[32];
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
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 write_eth_config(HB_CHAR *path,IFCFG_ETHX_ITEM_HANDLE ifcfg_eth)
{
    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }

    FILE *fp;
    HB_CHAR buf[200];
	IFCFG_ETHX_ITEM_OBJ ifcfg_ethr;
	memset(&ifcfg_ethr,0,sizeof(ifcfg_ethr));
	HB_S32 result=read_eth_config(path,&ifcfg_ethr);
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
    HB_S32 fd=open(path,O_RDWR);
    if(fd<0)
    {
        return -2;
    }
    HB_CHAR buf[200];

	IFCFG_ETHX_ITEM_OBJ ifcfg_ethr;
	memset(&ifcfg_ethr,0,sizeof(ifcfg_ethr));
	HB_S32 result=read_eth_config(path,&ifcfg_ethr);
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
HB_S32 get_mac_dev(HB_CHAR *mac_sn, HB_CHAR *dev)
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



HB_S32 read_conf_value(HB_CHAR *file_path, HB_CHAR *outbuf)
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

HB_S32 update_network_conf(HB_CHAR *str,const HB_CHAR *file)
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

HB_S32 readata_from_dns_conf(HB_CHAR *dns_file_path,NET_CONF_HANDLE net_conf)
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
    	memcpy(net_conf->dns_main_ip, &dns_ip[0][0], 16);
    	memcpy(net_conf->dns_sub_ip, &dns_ip[1][0], 16);
    }
    else
    {
    	memcpy(net_conf->dns_main_ip, &dns_ip[0][0], 16);
    }
    fclose(fp);

    return 0;
}

HB_S32 main()
{
	HB_S32 i_datacode = -1;
    HB_CHAR *buf=NULL;
    NET_CONF_OBJ net_conf;
    IFCFG_ETHX_ITEM_OBJ ifcfg_eth;
    MULTI_VIRTUAL_IP_ITEM_OBJ ip_mask_array[8];

    CgiInitAndConfig(&buf, CGI_SESSION_SAVE_PATH);
    memset(&net_conf, 0, sizeof(NET_CONF_OBJ));
    memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
    memset(&ip_mask_array[0], 0, sizeof(MULTI_VIRTUAL_IP_ITEM_OBJ)*8);

	if (cgi_session_var_exists("USER_NAME") && cgi_session_var_exists("PASSWORD")
			&& cgi_session_var_exists("LAST_ACCESS_TIME")) //服务器端有session文件
	{
		if(CheckUsrPasswd())//session和配置文件匹配
		{
			if (CalcTimeOut())
			{
				printf("Content type: text/xml \n\n");
				printf("{\"Result\":\"-1\",\"ErrMessage\":\"用户登录超时,请重新登录!\"}");
				goto END;
			}
			if( buf != NULL )
			{
				i_datacode = GetDataCode(buf);
				switch(i_datacode)
				{
					case 1031: { GetNetConfInfo(); break;}
					case 1032: { ChangeWanConf(buf); break;}
//					case 1033: { ChangeMainLanConf(buf); break;}
					case 1033: { printf("{\"Result\":\"-2\",\"ErrMessage\":\"未开通此功能[%d]！\"}", i_datacode);break;}
					case 1034: { AddLanSubIp(buf); break;}
					case 1035: { DelLanSubIp(buf); break;}
#ifdef STATIC_ROUTE_VERSION
					case 1036: { AddStaticRoute(buf); break;}
					case 1037: { DelStaticRoute(buf); break;}
#endif
					default:
					{
						//printf("Content type: application/json \n\n");
						printf("{\"Result\":\"-2\",\"ErrMessage\":\"DataCode[%d]错误！\"}", i_datacode);
						WRITE_LOG("DataCode error!\n");
						break;
					}
				}
				return 0;
			}
		}
		else//session和配置文件不匹配， 意味着用户名和密码输入错误
		{
			printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-3\"}");
		}
	}
	else//服务器端无session文件
	{
		printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-4\"}");
		WRITE_LOG("!!!!!!!!!!!!!!!!!!!!!!!!no session!\n");
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
