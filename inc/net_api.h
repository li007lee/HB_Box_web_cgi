//
//  thread_depend.h
//  MapPort
//
//  Created by MoK on 15/1/21.
//  Copyright (c) 2015ๅนด MoK. All rights reserved.
//

#ifndef __MapPort__thread_depend__
#define __MapPort__thread_depend__



////////////////////////////////////////////////////////////////////////////////
// 函数名：create_socket_connect_domain
// 描述：tcp网络连接
// 参数：
//  ［IN］psockfd - 套接字指针。
//  ［IN］domain – 要连接的域名
//    [IN] domain_port - 要连接域名的端口
//   [IN] timeout - 超时时间
// 返回值：-1失败,0成功
// 说明：
//通过域名和端口创建socket并且连接到对端，timeout秒后仍然没有连接上，则返回失败-1，成功返回0
////////////////////////////////////////////////////////////////////////////////
HB_S32 create_socket_connect_domain(HB_S32 *psockfd, HB_CHAR *domain, HB_S32 domain_port, HB_S32 timeout);


////////////////////////////////////////////////////////////////////////////////
// 函数名：create_socket_connect_ipaddr
// 描述：tcp网络连接
// 参数：
//  ［IN］psockfd - 套接字指针。
//  ［IN］ipaddr – 要连接的IP地址
//    [IN] port - 要连接的端口
//   [IN] timeout - 超时时间
// 返回值：-1失败,0成功
// 说明：
//通过IP和端口创建socket并且连接到对端，timeout秒后仍然没有连接上，则返回失败
////////////////////////////////////////////////////////////////////////////////
HB_S32 create_socket_connect_ipaddr(HB_S32 *psockfd, HB_CHAR *ipaddr, HB_S32 port, HB_S32 timeout);


////////////////////////////////////////////////////////////////////////////////
// 函数名：send_data
// 描述：tcp网络数据发送
// 参数：
//  ［IN］psockfd - 套接字指针。
//  ［IN］send_buf -发送缓冲区首地址
//    [IN] data_len - 要发送的数据长度
//   [IN] timeout - 超时时间
// 返回值：-1超时，-2失败，0成功
// 说明：
//发送指定长度datalen的数据，如果timeout秒后还为发送，则返回超时失败-1，
//否则返回发送剩余的字节数，成功表示全部发送完成则返回0，否则失败返回非0
////////////////////////////////////////////////////////////////////////////////
HB_S32 send_data(HB_S32 *psockfd, HB_VOID *send_buf, HB_S32 data_len, HB_S32 timeout);


////////////////////////////////////////////////////////////////////////////////
// 函数名：recv_data
// 描述：tcp网络数据接收
// 参数：
//  ［IN］psockfd - 套接字指针。
//  ［IN］send_buf -接收缓冲区首地址
//    [IN] data_len - 接收缓冲区长度
//   [IN] timeout - 超时时间
// 返回值：-1超时，-2失败，0对端close，大于0成功
// 说明：
//接收数据，接收缓冲区为recv_buf,缓冲区长度为recv_buf_len,超过timeout秒仍没接收
//到数据，则返回失败(0-超时，小于0-其他错误，大于0-成功接收)
////////////////////////////////////////////////////////////////////////////////
HB_S32 recv_data(HB_S32 *psockfd, HB_VOID *recv_buf, HB_S32 recv_buf_len, HB_S32 timeout);


////////////////////////////////////////////////////////////////////////////////
// 函数名：close_sockfd
// 描述：关闭tcp网络套接字
// 参数：
//  ［IN］psockfd - 套接字指针。
// 返回值：1
// 说明：
//关闭tcp网络套接字，并初始化为-1
////////////////////////////////////////////////////////////////////////////////
HB_S32 close_sockfd(HB_S32 *sockfd);


HB_S32 setup_listen_socket(HB_U16 port);

HB_S32 check_port(HB_S32 port);
HB_S32 get_dev_ip(HB_CHAR *eth, HB_CHAR *ipaddr);
HB_S32 get_dev_mac(HB_CHAR *eth, HB_CHAR *mac);
//获取盒子到序列号
HB_S32 get_sys_sn(HB_CHAR *sn, HB_S32 sn_size);


#endif /* defined(__MapPort__thread_depend__) */
