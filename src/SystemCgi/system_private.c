/*
 * system_private.c
 *
 *  Created on: 2016年4月21日
 *      Author: root
 */

#include "my_include.h"
#include "system_private.h"


static HB_VOID mystrcpy(HB_CHAR *dest, HB_CHAR *src)
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
static HB_S32 read_eth_config(HB_CHAR *path,IFCFG_ETH1_ITEM_HANDLE ifcfg_eth)
{
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
    HB_CHAR buf[1024] = {0};
	IFCFG_ETH1_ITEM_OBJ tmp_ethr;
	memset(&tmp_ethr,0,sizeof(tmp_ethr));
	HB_S32 result=read_eth_config(path,&tmp_ethr);
	if(result!=0)
	{
		return -2;
	}
	strcpy(ifcfg_eth->device,tmp_ethr.device);
	strcpy(ifcfg_eth->bootproto,tmp_ethr.bootproto);
	strcpy(ifcfg_eth->hwaddr,tmp_ethr.hwaddr);
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
}


HB_S32 update_file(HB_CHAR *str, HB_CHAR *file)
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


////执行cmd_str命令
////返回值:0成功，-1失败
//HB_S32 my_system(HB_CHAR *cmd_str)
//{
//	HB_S32 status = 0;
//	status= system(cmd_str);
//	if (-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
//	{
//		return 0;
//	}
//	else
//	{
//		return -1;
//	}
//}
