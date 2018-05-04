/*
 * lan_ip_conf.c
 *
 *  Created on: 2016年8月17日
 *      Author: root
 */
#include "my_include.h"
#include "lan_ip_conf.h"
#include "common.h"

static HB_VOID mystrcpy(HB_CHAR *dest, HB_CHAR *src)
{
    HB_S32 i = 0;
    HB_S32 j = 0;
    while(src[j++]!='=');
    while(src[j]!='\n')
    {
        dest[i++]=src[j++];
    }
    dest[i]='\0';
}

////////////////////////////////////////////////////////////////////////////////
// 函数名：netmask_str2len
// 描述：获取子网掩码长度
// 参数：
//  ［IN ]mask - 子网掩码字符串
//
// 返回值： 子网掩码长度
// 错误代码。
// 说明：
////////////////////////////////////////////////////////////////////////////////
static HB_S32 netmask_str2len(HB_CHAR* mask)
{
    HB_S32 netmask = 0;
    HB_U32 mask_tmp;

    mask_tmp = ntohl((HB_S32)inet_addr(mask));
    while (mask_tmp & 0x80000000)
    {
        netmask++;
        mask_tmp = (mask_tmp << 1);
    }

    return netmask;
}

////////////////////////////////////////////////////////////////////////////////
// 函数名：read_eth_conf
// 描述：  读取网卡配置文件， 生成结构体
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
static HB_S32 read_eth_conf(HB_CHAR *path, IFCFG_ETHX_ITEM_HANDLE ifcfg_eth)
{
    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }
    HB_CHAR *result;
    HB_CHAR buf[32];

    FILE *file=fopen(path,"r");
    if(file==NULL)
    {
        //printf(" open  file   error\n");
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
// 函数名：write_eth_conf
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
static HB_S32 write_eth_conf(HB_CHAR *path,IFCFG_ETHX_ITEM_HANDLE ifcfg_eth)
{
    if(path==NULL||ifcfg_eth==NULL)
    {
        return -1;
    }

    FILE *fp;
    HB_CHAR buf[1024] = {0};
#if 0
	IFCFG_ETH1_ITEM_OBJ ifcfg_ethr;
	memset(&ifcfg_ethr,0,sizeof(ifcfg_ethr));
	HB_S32 result=read_ifcfgeth1(path,&ifcfg_ethr);
	if(result!=0)
	{
		return -2;
	}
	strcpy(ifcfg_eth->device,ifcfg_ethr.device);
	strcpy(ifcfg_eth->bootproto,ifcfg_ethr.bootproto);
	strcpy(ifcfg_eth->hwaddr,ifcfg_ethr.hwaddr);
#endif
    sprintf(buf,"DEVICE=%s\nBOOTPROTO=%s\nHWADDR=%s\nIPADDR=%s\nNETMASK=%s\nGATEWAY=%s\n",
    		ifcfg_eth->device,ifcfg_eth->bootproto,ifcfg_eth->hwaddr,ifcfg_eth->ipaddr,ifcfg_eth->netmask,ifcfg_eth->gateway);

    fp = fopen(path, "wb");
    if(fp == NULL)
    {
    	return -1;
    }
    fprintf(fp,"%s",buf);
    fflush(fp);
    fclose(fp);
    return 0;
}



HB_S32 del_lan_sub_ip(HB_CHAR *eth_name, HB_CHAR *ip, HB_CHAR *mask,HB_CHAR *gate)
{
	HB_S32 status;
	HB_CHAR buf[512]={0};

#ifdef WRITE_LOG
	HB_CHAR save_log[128] = {0};
	memset(save_log, '\0', sizeof(save_log));
	snprintf(save_log, sizeof(save_log), "@@@@@@@@@@@@@@@@@@@@@@2Del Lan ip:[%s\t%s]!\n", eth_name, ip);
	WriteLog(save_log, strlen(save_log));
#endif

	if(strlen(gate) > 0)//存在网关（存在路由）
	{
		struct in_addr net;
		net.s_addr=(inet_addr(gate)&inet_addr(mask));
		sprintf(buf,"route del -net %s netmask %s",inet_ntoa(net),mask);
		status= system(buf);
		//   if (!(-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status)))
		if (-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
		{
			//printf("route del  success\n");
		}
		else
		{
			//printf("route del error\n");
		}
	}

	memset(buf,0,512);
	sprintf(buf,"ip addr del  %s/%d dev %s", ip, netmask_str2len(mask), eth_name);
	status= system(buf);
	if (!(-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status)))
	{
		//printf("\n#####ip del error\n");
		return -1;
	}
	//printf("ip del success\n");

	return 0;
}




////////////////////////////////////////////////////////////////////////////////
// 函数名：add_lan_sub_ip
// 描述： 添加一个结构体成员  并调用 ifconfig   或 route add命令
// 参数：
//  ［IN ]virtual_ip_mask - 成员结构体
//   [IN  ] file_path               配置文件路径
// 返回值：
//          0    成功
//          -1  //参数错误    ip  和 子网掩码 均不能为空
//          -2  只读方式打开配置文件失败
//          -3  解析配置文件错误
//          -4 结构体的eth0_id成员在配置文件中不存在
//          -5  只写方式打开配置文件失败
//          -6  调用 system  执行  ifconfig 或 route  add 命令时失败
//          -7  读取配置文件所有信息失败
//          -8  和原来配置信息一样   无需添加
//          -9 此配置信息 已在别的地方添加
//
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 add_lan_sub_ip(HB_CHAR *eth_name, HB_CHAR *ip, HB_CHAR *mask, HB_CHAR *gate)
{
	//开始配置新的虚拟ip 和 路由
	HB_S32 status;
	HB_CHAR cmd_buf[512]={0};

	sprintf(cmd_buf,"ifconfig %s %s netmask %s", eth_name, ip, mask);
#ifdef WRITE_LOG
	HB_CHAR save_log[128] = {0};
	memset(save_log, '\0', sizeof(save_log));
	snprintf(save_log, sizeof(save_log), "Config Lan ip:[%s]!\n", cmd_buf);
	WriteLog(save_log, strlen(save_log));
#endif
	status= system(cmd_buf);
#if 1
	if (-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
	{
        if(strlen(gate)!=0)  //有网关
        {
            struct in_addr net;
            net.s_addr=(inet_addr(ip)&inet_addr(mask));
            memset(cmd_buf,0,512);
			sprintf(cmd_buf,"route add -net %s netmask %s gw  %s %s", inet_ntoa(net), mask, gate, eth_name);

			WRITE_LOG("exec %s\n", cmd_buf);
            status= system(cmd_buf);
            if (!(-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status)))
            {
                //printf("\n########### run shell script < route add> error\n");
            	WRITE_LOG("exec %s falied\n", cmd_buf);
                memset(cmd_buf,0,512);
				sprintf(cmd_buf,"ip addr del  %s/%d dev %s", ip, netmask_str2len(mask), eth_name);
                system(cmd_buf);
                return -2;
               // errflag=1;  //出错
            }
         }
	}
	else
	{
		//printf("run shell script  < ifconfig  eth0 > error.\n");
		return -2;
		// errflag=1;  //出错
	}
#endif
	//printf("ifconfig eth0 success\n");
	return 0;
}



//修改lan口主ip
HB_S32 alter_lan_main_ip(HB_CHAR *eth_name, HB_CHAR *lan_ip, HB_CHAR *lan_mask, HB_CHAR *lan_gateway)
{
	HB_S32 ret = 0;
	IFCFG_ETHX_ITEM_OBJ ifcfg_eth;
    memset(&ifcfg_eth, 0, sizeof(IFCFG_ETHX_ITEM_OBJ));
    read_eth_conf(BOX_IFCFG_LAN, &ifcfg_eth);//读网卡配置文件，给结构体赋值。
    ret = del_lan_sub_ip(eth_name, ifcfg_eth.ipaddr, ifcfg_eth.netmask,ifcfg_eth.gateway);
    if (ret != 0)
    {
    	ret = -1;
    }

    memset(ifcfg_eth.ipaddr, 0, sizeof(ifcfg_eth.ipaddr));
	strcpy(ifcfg_eth.ipaddr, lan_ip);
	memset(ifcfg_eth.netmask, 0, sizeof(ifcfg_eth.netmask));
	strcpy(ifcfg_eth.netmask, lan_mask);
	memset(ifcfg_eth.gateway, 0, sizeof(ifcfg_eth.gateway));
	strcpy(ifcfg_eth.gateway, lan_gateway);

    ret = add_lan_sub_ip(eth_name, ifcfg_eth.ipaddr, ifcfg_eth.netmask,ifcfg_eth.gateway);
    if (ret != 0)
    {
    	ret = -1;
    }

    write_eth_conf(BOX_IFCFG_LAN, &ifcfg_eth);
    return 0;
}




