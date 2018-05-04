#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include"xml_app.h"
#include"get_box_pair_config.h"



// 函数名：get_box_pair_number
// 描述      :  获取盒子配对的配置文件中得盒子个数
// 参数：
//	      [IN]            file_path               配置文件路径
//
// 返回值：成功：返回配置文件中盒子个数   解析配置文件错误：-1   配置文件不存在: -2   参数错误：-3   其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int  get_box_pair_number(char *file_path)
{

    if(file_path==NULL)
        return -3;
     int ret=access(file_path, F_OK);
    if(ret!=0)
    {
        return -2;
    }
   FILE *file=fopen(file_path,"r");
    if(file==NULL)
        return -4;
     xmlDoc* doc=xmlParseFile(file);
     fclose(file);
     if(doc==NULL||doc->root==NULL)
        return -1;
    xmlNode *node=doc->root;
    node=node->first;
    int i=0;
    while(node)
    {
        i++;
        node=node->next;
    }
      xmlFreeDoc(doc);
      return i;
}

// 函数名：check_exist_box_pair
// 描述      : 检查序列号为box_id得盒子是否已经添加到配置文件中
// 参数：
//	      [IN]            file_path               配置文件路径
//           [IN]             box_id                  盒子序列号
// 返回值：不存在 ： 0     存在  ：1         解析配置文件错误：-1   配置文件不存在: -2   参数错误：-3   其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int  check_exist_box_pair(char *file_path,char * box_id)
{
    if(file_path==NULL||box_id==NULL)
        return -3;
     int ret=access(file_path, F_OK);
    if(ret!=0)
    {
        return -2;
    }
   FILE *file=fopen(file_path,"r");
    if(file==NULL)
        return -4;
     xmlDoc* doc=xmlParseFile(file);
     fclose(file);
     if(doc==NULL||doc->root==NULL)
        return -1;
    xmlNode *node=doc->root;
    node=node->first;
    char *str_id;
    while(node)
    {
        str_id=xmlGetTextByAttributeName(node,"BoxId");
        if(strcmp(str_id,box_id)==0)
            return 1;
         node=node->next;

    }
    xmlFreeDoc(doc);
    return 0;
}
xmlAttribute *getAttri(xmlNode *node,char *name)
{
    if(node==NULL||name==NULL)
        return NULL;
     xmlAttribute *attri=xmlGetFirstAttribute(node);
     if(attri==NULL)
        return NULL;
     while(attri)
     {
        if(!strcmp(attri->name,name))
             return attri;
         attri=attri->next;
     }
     return NULL;
}
////////////////////////////////////////////////////////////////////////////////
// 函数名：get_box_pair_info
// 描述：   获取盒子配置文件中某一个盒子信息，并生成链表
// 参数：［IN］             node    xml节点，该节点对应一个盒子
//	           [IN/OUT]        box_info -   生成盒子配对消息结构体
//
// 返回值：成功：0   解析错误：-1   参数错误：-2
// 说明：
////////////////////////////////////////////////////////////////////////////////
int get_box_pair_info(xmlNode *node,GET_BOX_PAIR_HEAD_HANDLE box_info)
{
    if(node==NULL||box_info==NULL)
        return -2;
     char *name[]={"PairType","BoxIp","BoxId"};
    xmlAttribute *attri[3];
     int i;
     //获取盒子得三个属性  并赋值给 box_info
     for(i=0;i<3;i++)
     {
        attri[i]=getAttri(node,name[i]);
        if(attri[i]==NULL)
            return  -1 ;
    }
    if(strcmp("LAN",attri[0]->text)==0)
        {
            box_info->pair_type=1;
        }
        else
        {
            if(strcmp("WAN",attri[0]->text)==0)
                {
                   box_info->pair_type=2;
                }
            else
                    return -1;
        }
     strncpy(box_info->box_ip,attri[1]->text,strlen(attri[1]->text));
     strncpy(box_info->box_id,attri[2]->text,strlen(attri[2]->text));

    //获取盒子得所有item成员

    node=node->first;
    GET_BOX_PAIR_ITEM_LIST_HANDLE  item;
    GET_BOX_PAIR_ITEM_LIST_HANDLE  item_head=(GET_BOX_PAIR_ITEM_LIST_OBJ*)malloc(sizeof(GET_BOX_PAIR_ITEM_LIST_OBJ));
    memset(item_head,0,sizeof(GET_BOX_PAIR_ITEM_LIST_OBJ));
    item_head->next=NULL;
    xmlChar *temp;
    int  item_number=0;
    while(node)
        {    item_number++;
            //获取设备节点得四个元素值，生成链表    app_port  map_port   dev_serial   dev_type
            item=(GET_BOX_PAIR_ITEM_LIST_OBJ*)malloc(sizeof(GET_BOX_PAIR_ITEM_LIST_OBJ));
            memset(item,0,sizeof(GET_BOX_PAIR_ITEM_LIST_OBJ));
            temp=xmlGetChildText(node,"app_port");
            if(temp==NULL)
                return -1;
            strcpy(item->app_port,temp);
            temp=xmlGetChildText(node,"map_port");
            if(temp==NULL)
                return -1;
            strcpy(item->map_port,temp);
            temp=xmlGetChildText(node,"dev_serial");
            if(temp==NULL)
                return -1;
            strcpy(item->dev_serial,temp);
            temp=xmlGetChildText(node,"dev_type");
            if(temp==NULL)
                return -1;
             item->dev_type=atoi(temp);

            item->next=item_head->next;

            item_head->next=item;

            node=node->next;
        }
        box_info->dev_list=item_head->next;
        box_info->dev_num=item_number;
        box_info->next=NULL;
        free(item_head);
     return 0;
}

////////////////////////////////////////////////////////////////////////////////
// 函数名   get_box_pair_config
// 描述：   获取整个配置文件信息 ，并生成链表
// 参数：［IN］             file_path                     配置文件路径
//	           [IN/OUT]        box_pair_config -    保存配置文件信息得结构体 ，调用函数前先分配结构内存
//
// 返回值：成功：0   解析配置文件错误：-1   配置文件不存在: -2  参数错误：-3  其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int get_box_pair_config(char *file_path,GET_BOX_PAIR_CONFIG_HANDLE box_pair_config)

{
    if(file_path==NULL||box_pair_config==NULL)
        return -3;
    int ret=access(file_path, F_OK);
    if(ret!=0)
    {
        return -2;
    }
   FILE *file=fopen(file_path,"r");
    if(file==NULL)
        return -4;
     xmlDoc* doc=xmlParseFile(file);
     fclose(file);
     if(doc==NULL||doc->root==NULL)
        return -1;
    xmlNode *node=doc->root;
    node=node->first;
    int number=0;
    GET_BOX_PAIR_HEAD_HANDLE box_info;
    while(node)
    {
        number++;
        box_info=(GET_BOX_PAIR_HEAD_HANDLE )malloc(sizeof(GET_BOX_PAIR_HEAD_OBJ));
        memset(box_info, 0, sizeof(GET_BOX_PAIR_HEAD_OBJ));
        if(box_info==NULL)
            return -4;
        ret=get_box_pair_info(node,box_info);
        if(ret==-1)
            return -1;
        box_info->next=box_pair_config->box_list;
        box_pair_config->box_list=box_info;
        node=node->next;
    }
    box_pair_config->box_num=number;
    xmlFreeDoc(doc);
    return 0;
}


void delete_box_pair_item_list(GET_BOX_PAIR_ITEM_LIST_HANDLE  *list)
{
    GET_BOX_PAIR_ITEM_LIST_HANDLE next;
    if(*list==NULL)
        return;
    while(*list)
    {
      next=(*list)->next;
      free(*list);
      *list=next;
    }
    *list=NULL;
}
void delete_box_pair(GET_BOX_PAIR_HEAD_HANDLE *list)
{
    if(*list==NULL)
        return ;
     GET_BOX_PAIR_HEAD_HANDLE next;
     while(*list)
     {
        delete_box_pair_item_list(&((*list)->dev_list));
        next=(*list)->next;
        free(*list);
       *list=next;
     }
    *list=NULL;
 }
////////////////////////////////////////////////////////////////////////////////
// 函数名   delete_box_pair_config
// 描述：   释放空间
// 参数：［IN］             box_pair_config                     配置文件生成得结构体
//
//
// 返回值：成功：0
// 说明：
////////////////////////////////////////////////////////////////////////////////
void delete_box_pair_config(GET_BOX_PAIR_CONFIG_HANDLE list)

{

    if(list==NULL)
        return ;
     delete_box_pair(&(list->box_list));
}


void print_box_pair_item_list(GET_BOX_PAIR_ITEM_LIST_HANDLE  list)
{
    if(list==NULL)
        return;
     int i=0;
    GET_BOX_PAIR_ITEM_LIST_HANDLE  temp=list;
    while(temp)
    {

        printf("\n\nitem::%d\n\n",i);
        printf("app_port::%s\n",temp->app_port);
        printf("map_port::%s\n",temp->map_port);
        printf("dev_type::%d\n",temp->dev_type);
        printf("dev_serial::%s\n",temp->dev_serial);
        i++;
        temp=temp->next;
    }
}

#if 1
void print_box_pair(GET_BOX_PAIR_HEAD_HANDLE list)
{
    if(list==NULL)
        return;
    int i=0;
    GET_BOX_PAIR_HEAD_HANDLE temp=list;
    while(temp)
    {
        printf("\n\nbox_%d\n\n",i);
        printf("dev_num::  %d\n",temp->dev_num);
        printf("pair_type::  %d\n",temp->pair_type);
        printf("box_id   :: %s\n",temp->box_id);
        printf("box_ip   ::%s\n",temp->box_ip);
        print_box_pair_item_list(temp->dev_list);
        i++;
        temp=temp->next;
    }
}
#endif

void print_box_pair_config(GET_BOX_PAIR_CONFIG_HANDLE list)
{
    if(list==NULL)
        return;
     printf("box_num::%d\n\n",list->box_num);

      print_box_pair(list->box_list);

}


////////////////////////////////////////////////////////////////////////////////
// 函数名：get_box_config_info
// 描述      : 根据盒子配置文件获取相应结构体
// 参数：
//	      [IN]            file_path     配置文件路径
//           [IN/OUT]  box_info     盒子信息结构体
//
// 返回值：成功：0   解析文件错误：-1   指定文件不存：-2   参数错误: -3  其它错误 -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int  get_box_config_info(char*file_path,BOX_CONFIG_INFO_HANDLE box_info)
{
    if(file_path==NULL||box_info==NULL)
            return -3;
    int ret=access(file_path, F_OK);
    if(ret!=0)
            return -2;
   FILE *file=fopen(file_path,"r");
    if(file==NULL)
            return -4;
     xmlDoc* doc=xmlParseFile(file);
     fclose(file);
    if(doc==NULL||doc->root==NULL||strcmp(doc->root->name,"box_info_config"))
            return -1;
    xmlNode *node=doc->root->first;
    xmlNode *child;
    for(node=doc->root->first; node!=NULL; node=node->next)
	{
		if(strcmp(node->name,"hb_box_pair")==0)
		{
			for(child=node->first;child;child=child->next)
			{
				if(strcmp(child->name,"lan_pair_switch")==0&&child->text!=NULL)
					 {
						strcpy(box_info->lan_pair_switch,child->text);
						continue;
					}
				 if(strcmp(child->name,"lan_pair_code")==0&&child->text!=NULL)
					 {
						strcpy(box_info->lan_pair_code,child->text);
						continue;
					}
				 if(strcmp(child->name,"wan_pair_switch")==0&&child->text!=NULL)
					 {
						strcpy(box_info->wan_pair_switch,child->text);
						continue;
					}
				 if(strcmp(child->name,"wan_pair_code")==0&&child->text!=NULL)
					 {
						strcpy(box_info->wan_pair_code,child->text);
						continue;
					}
			}
			continue;
		}
		if(strcmp(node->name,"hb_dev_xml_file_timestamp")==0)
		{
			for(child=node->first;child;child=child->next)
			{
				if(strcmp(child->name,"dev_list_file_time")==0&&child->text!=NULL)
				{
					strcpy(box_info->dev_list_time,child->text);
					continue;
				}
				if(strcmp(child->name,"pair_list_file_time")==0&&child->text!=NULL)
				{
					 strcpy(box_info->pair_list_time,child->text);
					continue;
				}
			}
		}
	}
      xmlFreeDoc(doc);
        return 0;
    }

////////////////////////////////////////////////////////////////////////////////
// 函数名：update_box_config_info
// 描述      : 更新盒子配置文件
// 参数：
//	      [IN]            file_path     配置文件路径
//           [IN/OUT]  box_info     盒子信息结构体
//
// 返回值：成功：0   解析文件错误：-1   指定文件不存：-2   参数错误: -3  其它错误 -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int update_box_config_info(char*file_path,BOX_CONFIG_INFO_HANDLE box_info)
{
    if(file_path==NULL||box_info==NULL)
            return -3;
    int ret=access(file_path, F_OK);
    if(ret!=0)
            return -2;
   FILE *file=fopen(file_path,"r");
    if(file==NULL)
            return -4;
     xmlDoc* doc=xmlParseFile(file);
     fclose(file);
    if(doc==NULL||doc->root==NULL||strcmp(doc->root->name,"box_info_config"))
            return -1;
    xmlNode *node=doc->root->first;
    xmlNode *child;
    for(node=doc->root->first; node!=NULL; node=node->next)
	{
		if(strcmp(node->name,"hb_box_pair")==0)
		{
			for(child=node->first;child;child=child->next)
			{
				if(strcmp(child->name,"lan_pair_switch")==0)
				{
					if(strlen(box_info->lan_pair_switch))
							strcpy(child->text,box_info->lan_pair_switch);
					continue;
				}
				if(strcmp(child->name,"lan_pair_code")==0)
				{
					  if(strlen(box_info->lan_pair_code))
							strcpy(child->text,box_info->lan_pair_code);
					continue;
				}
				if(strcmp(child->name,"wan_pair_switch")==0)
				{
					 if(strlen(box_info->wan_pair_switch))
							strcpy(child->text,box_info->wan_pair_switch);
					continue;
				}
				if(strcmp(child->name,"wan_pair_code")==0)
				{
					 if(strlen(box_info->wan_pair_code))
							strcpy(child->text,box_info->wan_pair_code);
					continue;
				}
			}
			continue;
		}
		if(strcmp(node->name,"hb_dev_xml_file_timestamp")==0)
		{
			for(child=node->first;child;child=child->next)
			{
				if(strcmp(child->name,"dev_list_file_time")==0)
				{
					 if(strlen(box_info->dev_list_time))
							strcpy(child->text,box_info->dev_list_time);
					continue;
				}
				if(strcmp(child->name,"pair_list_file_time")==0)
				{
					 if(strlen(box_info->pair_list_time))
							strcpy(child->text,box_info->pair_list_time);
					continue;
				}
			}
		}
	}
     file=fopen(file_path,"w");
            if(file==NULL)
            {
                return -4;
            }
     xmlOutputToFile(doc, file, 1);
     fclose(file);
    xmlFreeDoc(doc);
    return 0;
}
