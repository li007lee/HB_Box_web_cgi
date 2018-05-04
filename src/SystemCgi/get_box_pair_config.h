#ifndef GET_BOX_PAIR_CONFIG_H_INCLUDED
#define GET_BOX_PAIR_CONFIG_H_INCLUDED
typedef struct _tagGetBOX_PAIR_ITEM
  {
      char app_port[8];
      char map_port[8];
      int    dev_type;
      char dev_serial[32];
      struct _tagGetBOX_PAIR_ITEM *next;
  }GET_BOX_PAIR_ITEM_LIST_OBJ, *GET_BOX_PAIR_ITEM_LIST_HANDLE;
typedef struct _tagGet_BOX_PAIR_HEAD
{
	int dev_num; //盒子下挂设备个数
	int pair_type; //1-LAN，2-WAN
    char box_id[32];//盒子序列号
    char box_ip[16];//盒子ip
    GET_BOX_PAIR_ITEM_LIST_OBJ *dev_list;
    struct _tagGet_BOX_PAIR_HEAD*next;  //下一个盒子
}GET_BOX_PAIR_HEAD_OBJ, *GET_BOX_PAIR_HEAD_HANDLE;
typedef struct _tagGet_BOX_PAIR_CONFIG
{
     int box_num;  //配置文件中得盒子个数
     GET_BOX_PAIR_HEAD_HANDLE box_list;
}GET_BOX_PAIR_CONFIG_OBJ , *GET_BOX_PAIR_CONFIG_HANDLE;

//盒子配置信息
typedef struct _tagBOX_CONFIG_INFO
{
	char lan_pair_switch[4];//局域网配对开关 ON-开，OFF-关
	char wan_pair_switch[4];//广域网配对开关 ON-开，OFF-关
	char lan_pair_code[64];//局域网配对码
	char wan_pair_code[64];//广域网配对码
    char dev_list_time[16];//基本设备列表配置文件的最后更新时间戳
    char pair_list_time[16];//配对设备列表配置文件的最后更新时间戳
}BOX_CONFIG_INFO_OBJ, *BOX_CONFIG_INFO_HANDLE;



// 函数名：get_box_pair_number
// 描述      :  获取盒子配对的配置文件中得盒子个数
// 参数：
//	      [IN]            file_path               配置文件路径
//
// 返回值：成功：返回配置文件中盒子个数   解析配置文件错误：-1   配置文件不存在: -2   参数错误：-3   其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int  get_box_pair_number(char *file_path);

// 函数名：check_exist_box_pair
// 描述      : 检查序列号为box_id得盒子是否已经添加到配置文件中
// 参数：
//	      [IN]            file_path               配置文件路径
//           [IN]             box_id                  盒子序列号
// 返回值：不存在 ： 0     存在  ：1         解析配置文件错误：-1   配置文件不存在: -2   参数错误：-3   其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int  check_exist_box_pair(char *file_path,char * box_id);

////////////////////////////////////////////////////////////////////////////////
// 函数名   get_box_pair_config
// 描述：   获取整个配置文件信息 ，并生成链表
// 参数：［IN］             file_path                     配置文件路径
//	           [IN/OUT]        box_pair_config -    保存配置文件信息得结构体 ，调用函数前先分配结构内存
//
// 返回值：成功：0   解析配置文件错误：-1   配置文件不存在: -2  参数错误：-3  其它错误 ： -4
// 说明：
////////////////////////////////////////////////////////////////////////////////
int get_box_pair_config(char *file_path,GET_BOX_PAIR_CONFIG_HANDLE box_pair_config);
//释放空间
void delete_box_pair_config(GET_BOX_PAIR_CONFIG_HANDLE box_pair_config);
//打印配置文件生成得结构体
void print_box_pair_config(GET_BOX_PAIR_CONFIG_HANDLE box_pair_config);

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
int  get_box_config_info(char*file_path,BOX_CONFIG_INFO_HANDLE box_info);

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
int update_box_config_info(char*file_path,BOX_CONFIG_INFO_HANDLE box_info);
#endif // GET_BOX_PAIR_CONFIG_H_INCLUDED
