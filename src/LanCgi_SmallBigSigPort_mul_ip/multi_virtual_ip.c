#include "my_include.h"
#include"xml_app.h"
#include"multi_virtual_ip.h"


#if 0
//向filename文件中追加写日志
static HB_S32 write_logs_file(HB_CHAR *filename, HB_CHAR *str)
{
	HB_S32 fd = -1;
	fd = open(filename, O_CREAT|O_WRONLY|O_APPEND, 0664);
	if (fd < 0)
	{
		return HB_FAILURE;
	}
	//fcntl(fd, F_SETLKW, file_lock(F_WRLCK, SEEK_SET));
	write(fd, str, strlen(str));
	//fcntl(fd, F_SETLK, file_lock(F_UNLCK, SEEK_SET));
	close(fd);
	return HB_SUCCESS;
}
#endif

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
// 函数名：read_from_file
// 描述：    读取配置文件中的路由信息 与虚拟ip配置信息  ，将其保存到 结构体数组中 ，
// 参数：
//  ［IN ]file_path - 配置文件路径
//    [ IN | OUT] virtual_ip_array   配置信息结构体数组
//
// 返回代码。0  成功
//                       -1  参数错误
//                      -2    只读方式打开配置文件失败
//                      -3    解析配置文件失败
//
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 read_from_file(MULTI_VIRTUAL_IP_ITEM_OBJ *virtual_ip_array,HB_CHAR *file_path)
{
     if(file_path==NULL||virtual_ip_array==NULL)
    {
        return -1;
    }
   FILE *file=fopen(file_path,"r");
   if(file==NULL)
   {
       return -2;
   }
    xmlDoc * doc=xmlParseFile(file);
    fclose(file);
      if(doc==NULL||doc->root==NULL)
      {
        return  -3;
      }
      xmlNode *ip_item=doc->root->first;
       xmlNode *ip;
       xmlNode *mask;
       xmlNode *gate;
        xmlNode *eth0_id;
      while(ip_item)
      {
        ip=xmlGetFirstChildByName(ip_item, "ip");
        mask=xmlGetFirstChildByName(ip_item, "mask");
        gate=xmlGetFirstChildByName(ip_item, "gate");
        eth0_id=xmlGetFirstChildByName(ip_item, "eth0_id");
        if(ip&&mask&&gate&&eth0_id)
        {

            HB_S32 pos=atoi(eth0_id->text);
            if(eth0_id->text)
            {
                strcpy(virtual_ip_array[pos-1].eth0_id,eth0_id->text);
            }
            if(gate->text)
            {
                strcpy(virtual_ip_array[pos-1].gate,gate->text);
            }
            if(ip->text)
            {
                 strcpy(virtual_ip_array[pos-1].ip,ip->text);
            }
            if(mask->text)
            {
                strcpy(virtual_ip_array[pos-1].mask,mask->text);
            }

         }
        else
        {
            return  -3;
        }
         ip_item=ip_item->next;
      }
       xmlFreeDoc(doc);
      return 0;
}


////////////////////////////////////////////////////////////////////////////////
// 函数名：add_virtual_ip
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
HB_S32 add_virtual_ip(MULTI_VIRTUAL_IP_ITEM_OBJ  virtual_ip_mask,HB_CHAR *file_path)
{
    if(file_path==NULL||strcmp(virtual_ip_mask.ip,"")==0||strcmp(virtual_ip_mask.mask,"")==0)
    {
        return  -1;  //参数错误    ip  和 子网掩码 均不能为空
    }

   //读取配置文件  ，看 该信息是否已被添加
    MULTI_VIRTUAL_IP_ITEM_OBJ ip_mask[8];
    memset(ip_mask,0,sizeof(MULTI_VIRTUAL_IP_ITEM_OBJ)*8);
    HB_S32 ret=read_from_file(ip_mask,file_path);
    if(ret)
    {
        return -7;  //读取配置文件失败
    }
    HB_S32 i;
    for(i=0;i<8;i++)
    {
        if(strcmp(ip_mask[i].ip,virtual_ip_mask.ip)==0&&strcmp(ip_mask[i].mask,virtual_ip_mask.mask)==0)
        {
            if(strcmp(ip_mask[i].eth0_id,virtual_ip_mask.eth0_id)==0)
            {
                //编号相同时 分两种情况  ，网关相同时 则直接退出，网关不同时 ，则继续执行
                if(strcmp(ip_mask[i].gate,virtual_ip_mask.gate)==0)
                {
                    return  -8; //和原来配置信息一样   无需添加
                }
                else
                {
                    break;         //跳出循环  继续执行
                }
            }
            else
            {
                //编号不同时  直接返回错误
                return    -9;  //此配置信息 已在别的地方添加
            }
        }
    }

		FILE *file=fopen(file_path,"r");
	   if(file==NULL)
	   {
			return -2;   //只读方式打开配置文件失败
	   }
	   xmlDoc * doc=xmlParseFile(file);
	   fclose(file);
	  if(NULL==doc||doc->root==NULL||doc->root->first==NULL)
	  {
		  return  -3;     //解析配置文件错误
	  }
	  xmlNode *ip_item=doc->root->first;
	  xmlNode *eth0_id;
	  xmlNode *gate;
	  xmlNode *mask;
	  xmlNode *ip;
	  while(ip_item)
	   {
		 eth0_id=xmlGetFirstChildByName(ip_item, "eth0_id");
		 if(strcmp(eth0_id->text,virtual_ip_mask.eth0_id)==0)
		 {
			   break;
		 }
		 ip_item=ip_item->next;
	  }
	  if(ip_item==NULL)
	  {
		  return  -4  ;     //结构体的eth0_id成员在配置文件中不存在
	  }
	  //获取配置文件中的信息  目的是 再填入新的信息时  调用 system 清除原来的配置信息
	  gate=xmlGetFirstChildByName(ip_item, "gate");
	  mask=xmlGetFirstChildByName(ip_item, "mask");
	  ip=xmlGetFirstChildByName(ip_item, "ip");

		if(gate==NULL||mask==NULL||ip==NULL)
		{
			return -3;   //解析配置文件失败
		}
	  HB_CHAR buf[100];
	  //清除路由       先清除路由 再清除 虚拟ip
	  if(gate->text!=NULL&&mask->text)
	  {
			struct in_addr net;
			net.s_addr=(inet_addr(gate->text)&inet_addr(mask->text));
			memset(buf,0,100);
			sprintf(buf,"route del -net %s netmask %s gw  %s eth0:%s",inet_ntoa(net),mask->text,gate->text,eth0_id->text);
			system(buf);
	  }
	  //清除虚拟ip
		if(ip->text!=NULL&&mask->text&&eth0_id->text)
		{
			memset(buf,0,100);
			sprintf(buf,"ip addr del  %s/%d dev eth0:%s",ip->text,netmask_str2len(mask->text),eth0_id->text);
			system(buf);
		}
	//清除之后 ，将各节点赋值为 空
	 if(ip->text)
	 {
		 strcpy(ip->text,"");
	 }
	 if(gate->text)
	 {
		 strcpy(gate->text,"");
	 }
	  if(mask->text)
	 {
		 strcpy(mask->text,"");
	 }

	//开始配置新的虚拟ip 和 路由
	HB_S32 status;
	HB_S32 errflag=0;  //为0 则没有出错
	memset(buf,0,100);
	sprintf(buf,"ifconfig eth0:%s %s netmask %s",virtual_ip_mask.eth0_id,virtual_ip_mask.ip,virtual_ip_mask.mask);
	status= system(buf);
	if (-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
	{
		if(strcmp(virtual_ip_mask.gate,"")!=0)  //开着网关
		{
			struct in_addr net;
			net.s_addr=(inet_addr(virtual_ip_mask.gate)&inet_addr(virtual_ip_mask.mask));
			memset(buf,0,100);
			sprintf(buf,"route add -net %s netmask %s gw  %s eth0:%s",inet_ntoa(net),virtual_ip_mask.mask,virtual_ip_mask.gate,virtual_ip_mask.eth0_id);
			status= system(buf);
			if (!(-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status)))
			{
				 printf("run shell script < route add> error\n");
				memset(buf,0,100);
				 sprintf(buf,"ip addr del  %s/%d dev eth0:%s",virtual_ip_mask.ip,netmask_str2len(virtual_ip_mask.mask),virtual_ip_mask.eth0_id);
				 system(buf);
				 errflag=1;  //出错
			}
			else
			{
				 printf("run shell script < route add> success\n");
			}
		 }
	}
	else
	{
		printf("run shell script  < ifconfig  eth0 > error.\n");
		errflag=1;  //出错
	}
	if(errflag==0)
	{
		 printf("run shell script  < ifconfig  eth0 > successfully.\n");
		 ip->text=virtual_ip_mask.ip;
		 gate->text=virtual_ip_mask.gate;
		 mask->text=virtual_ip_mask.mask;

	}

	file=fopen(file_path,"w");
	if(file==NULL)
	{
		return  -5; //  只写方式打开配置文件失败
	}
	xmlOutputToFile(doc, file, 1);
	fclose(file);
	xmlFreeDoc(doc);
	if(errflag)
	{
		return -6;//    调用 system  执行  ifconfig 或 route  add 命令时失败
	}
	return 0;
}



////////////////////////////////////////////////////////////////////////////////
//      函数名：auto_config
//      描述：    系统开机后 ，调用该函数，自动调用system ，生成虚拟ip和路由信息
//      参数：
//      [IN|OUT]    ip_mask_array    保存 已经调用 system 并且成功的虚拟ip结构相关信息  的数组
//      [IN]            file_path                  配置文件路径
//      返回代码。
//      0   成功  ，
//      -1  参数错误
//      -2  只读方式打开文件失败
//      -3  解析配置文件错误
//      -4  只写方式打开配置文件失败
//      说明：
//            机器启动时 ，应自动调用该函数  以便将配置文件保存的信息生效
////////////////////////////////////////////////////////////////////////////////
HB_S32 auto_config(MULTI_VIRTUAL_IP_ITEM_HANDLE virtual_ip_array,HB_CHAR *file_path)
{
 if(file_path==NULL||virtual_ip_array==NULL)
    {
        return -1;  //参数错误
    }
   FILE *file=fopen(file_path,"r");
   if(file==NULL)
   {
       return -2;    //只读方式打开文件失败
   }
    xmlDoc * doc=xmlParseFile(file);
    fclose(file);
      if(doc==NULL||doc->root==NULL)
      {
        return  -3;  //解析配置文件错误
      }
      xmlNode *ip_item=doc->root->first;
       xmlNode *ip;
       xmlNode *mask;
       xmlNode *gate;
       xmlNode *eth0_id;
       HB_CHAR buf[100];
        HB_S32  status;
        HB_S32 errflag=0;
    while(ip_item)
{
        ip=xmlGetFirstChildByName(ip_item, "ip");
        mask=xmlGetFirstChildByName(ip_item, "mask");
        gate=xmlGetFirstChildByName(ip_item, "gate");
        eth0_id=xmlGetFirstChildByName(ip_item, "eth0_id");
        if(ip&&mask&&gate&&eth0_id)
        {

            if(ip->text==NULL||mask->text==NULL||eth0_id->text==NULL)
                {
                ip_item=ip_item->next;
                continue;
                }
            memset(buf,0,100);
            sprintf(buf,"ifconfig eth0:%s %s netmask %s",eth0_id->text,ip->text,mask->text);
            status=system(buf);
            if (-1!= status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
            {
               if(gate->text)  //开网关
                {
                    struct in_addr net;
                    net.s_addr=(inet_addr(gate->text)&inet_addr(mask->text));
                    memset(buf,0,100);
                    sprintf(buf,"route add -net %s netmask %s gw  %s eth0:%s",inet_ntoa(net),mask->text,gate->text,eth0_id->text);
                    status= system(buf);
                    if (!(-1 != status&&WIFEXITED(status)&&0 == WEXITSTATUS(status)))
                    {
                        memset(buf,0,100);
                        sprintf(buf,"ip addr del  %s/%d dev eth0:%s",ip->text,netmask_str2len(mask->text),eth0_id->text);
                        system(buf);
                        printf("ip_addr:%s   mask:%s  auto_config  route   error\n",ip->text,mask->text);
                        errflag=1;  //出错
                        strcpy(ip->text,"");
                        strcpy(gate->text,"");
                        strcpy(mask->text,"");
                        ip_item=ip_item->next;
                        continue;
                     }
                } //  if(gate->text)  //开网关

                HB_S32 pos=atoi(eth0_id->text);
                strcpy(virtual_ip_array[pos-1].eth0_id,eth0_id->text);
                if(gate->text)
                    {
                         strcpy(virtual_ip_array[pos-1].gate,gate->text);
                    }

                strcpy(virtual_ip_array[pos-1].ip,ip->text);
                strcpy(virtual_ip_array[pos-1].mask,mask->text);
            }//if (-1!= status&&WIFEXITED(status)&&0 == WEXITSTATUS(status))
            else
                {
                        errflag=1;  //出错
                        strcpy(ip->text,"");
                        if(gate->text)
                        {
                            strcpy(gate->text,"");
                        }

                        strcpy(mask->text,"");
                        ip_item=ip_item->next;
                        continue;
                }


        } //if(ip&&mask&&gate&&eth0_id)
    else
        {
        return  -3;     //获取 ip   mask   gate   eth0_id 这四个节点失败  ，属于解析配置文件错误
        }
      ip_item=ip_item->next;
}//while
       if(errflag==1)    //说明有的配置信息没有配置成功   所以要重写一次文件
       {
           file=fopen(file_path,"w");
           if(file==NULL)
           {
               return  -4; //只写方式打开配置文件失败
           }
           xmlOutputToFile(doc, file, 1);
          fclose(file);
          xmlFreeDoc(doc);

        }
         return  0;
    }
