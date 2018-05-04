#ifndef MULTI_VIRTUAL_IP_H_INCLUDED
#define MULTI_VIRTUAL_IP_H_INCLUDED

#define VITUAL_IP_XML_FILE   "/etc/mnt/vitual_ip.xml"         //配置文件路径
#define  VITUAL_IP_MAX_NUMBER  (8)                       //虚拟ip个数最大值

typedef struct _tagMULTI_VIRTUAL_IP_ITEM
{
    HB_CHAR  ip[16];               //ip
    HB_CHAR mask[16];        //子网掩码
    HB_CHAR gate[16];          //网关   =="";
    HB_CHAR eth0_id[2];   //虚拟ip 编号
}MULTI_VIRTUAL_IP_ITEM_OBJ, *MULTI_VIRTUAL_IP_ITEM_HANDLE;



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
HB_S32 read_from_file(MULTI_VIRTUAL_IP_ITEM_OBJ *virtual_ip_array,HB_CHAR *file_path);



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
HB_S32 add_virtual_ip(MULTI_VIRTUAL_IP_ITEM_OBJ  virtual_ip_mask,HB_CHAR *file_path);


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
HB_S32 auto_config(MULTI_VIRTUAL_IP_ITEM_HANDLE virtual_ip_array,HB_CHAR *file_path);

#endif // MULTI_VIRTUAL_IP_H_INCLUDED
