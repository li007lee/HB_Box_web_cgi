/***********************************************
 * @file dev_operation.c
 * @Func 设备映射页面各个操作模块的实现
 * @author lijian
 * @date 2016-06-29
************************************************/

#include "dev_operation.h"
#include "cJSON.h"
#include "base64_codec.h"

/************************网络通信************************/
HB_S32 NetworkCommunicate(HB_S32 *sockfd, HB_CHAR *ip, HB_S32 port, HB_CHAR *buf, HB_S32 buf_len, HB_S32 buf_size, HB_S32 iTimeOut)
{
	HB_S32 ret;

	ret = create_socket_connect_ipaddr(sockfd, ip, port, 5);
	if (HB_FAILURE == ret)
	{
//		printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
		//printf("{\"Result\":\"0\"}");
		close_sockfd(sockfd);
		return -1;
	}

	if(send_data(sockfd, buf, buf_len, 5) < 0)
	{
		//printf("Content type: application/json \n\n");
//		printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
		close_sockfd(sockfd);

		return -2;
	}
	memset(buf, 0, buf_size);
	if(recv_data(sockfd, buf, buf_size, iTimeOut) < 0)
	{
		//printf("Content type: application/json \n\n");
//		printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
		close_sockfd(sockfd);

		return -3;
	}

	close_sockfd(sockfd);

	return 0;
}
/************************网络通信END************************/

/***************************整体操作***************************/
////基本信息页面获取列表
//HB_VOID GetALLDevList()
//{
//	HB_CHAR list_str[BUF_LEN_OF_JSON] = {0};
//
//	strncpy(list_str, "{", sizeof(list_str));
//
//	GetBoxDevList(list_str, sizeof(list_str)-strlen(list_str));
//	GetTcpDevList(list_str, sizeof(list_str)-strlen(list_str));
//	GetRtspDevList(list_str, sizeof(list_str)-strlen(list_str));
//
//	strncat(list_str, "}", sizeof(list_str)-strlen(list_str));
//
//	printf("%s", list_str);
//	WRITE_LOG("send to web :[%s]\n", list_str);
//}

//设备映射页面获取列表
HB_VOID GetALLDevPortList()
{
	HB_CHAR list_str[BUF_LEN_OF_JSON] = {0};

	strncpy(list_str, "{", sizeof(list_str));

	GetBoxDevList(list_str, sizeof(list_str)-strlen(list_str));
	GetTcpDevList(list_str, sizeof(list_str)-strlen(list_str));
	GetRtspDevPortList(list_str, sizeof(list_str)-strlen(list_str));

	strncat(list_str, "}", sizeof(list_str)-strlen(list_str));

	printf("%s", list_str);
	WRITE_LOG("send to web :[%s]\n", list_str);
}

/***************************整体操作END***************************/



/************************一点通盒子操作************************/
/************************一点通盒子操作************************/
/************************一点通盒子操作************************/
//读取1行数据回调函数
static HB_S32 LoadDeviceInfoOneLine( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR c_mac_sn[32] = {0};
	HB_CHAR buf_var[1024] = {0};

	HB_CHAR c_dev_type[128] = {0};
	HB_CHAR c_dev_name[128] = {0};
	HB_CHAR c_dev_id[64] = {0};
	HB_S32 i_dev_chns = 0;
	HB_CHAR c_dev_ip[20] = {0};
	HB_S32 i_dev_port1 = 0;
	HB_S32 i_dev_port2 = -1;
	HB_CHAR c_dev_usr[128] = {0};
	HB_S32 i_box_port1 = 0;
	HB_S32 i_box_port2 = -1;


	strncpy(c_dev_type, column_value[0], strlen(column_value[0]));
	strncpy(c_dev_name, column_value[1], strlen(column_value[1]));
	strncpy(c_dev_id, column_value[2], strlen(column_value[2]));
	i_dev_chns = atoi(column_value[3]);
	strncpy(c_dev_ip, column_value[4], strlen(column_value[4]));
	i_dev_port1 = atoi(column_value[5]);
	strncpy(c_dev_usr, column_value[6], strlen(column_value[6]));
	i_box_port1 = atoi(column_value[7]);
	i_dev_port2 = atoi(column_value[8]);
	i_box_port2 = atoi(column_value[9]);

	url_decode(c_dev_type, strlen(c_dev_type));
	url_decode(c_dev_name, strlen(c_dev_name));

	get_sys_sn(c_mac_sn, sizeof(c_mac_sn));

	if (i_dev_port2 > 0)
	{
		//获取盒子信息
		snprintf(buf_var, sizeof(buf_var), "\"DevType\":\"%s\",\"DevName\":\"%s\",\"DevID\":\"%s-%s\",\"DevChns\":\"%d\",\"DevIP\":\"%s\",\"DevPort\":\"%d , %d\",\"DevLoginUsr\":\"%s\",\"BoxPort\":\"%d , %d\"",\
				c_dev_type, c_dev_name, c_mac_sn, c_dev_id, i_dev_chns, c_dev_ip, i_dev_port1, i_dev_port2, c_dev_usr, i_box_port1, i_box_port2);
	}
	else
	{
		//获取盒子信息
		snprintf(buf_var, sizeof(buf_var), "\"DevType\":\"%s\",\"DevName\":\"%s\",\"DevID\":\"%s-%s\",\"DevChns\":\"%d\",\"DevIP\":\"%s\",\"DevPort\":\"%d\",\"DevLoginUsr\":\"%s\",\"BoxPort\":\"%d\"",\
				c_dev_type, c_dev_name, c_mac_sn, c_dev_id, i_dev_chns, c_dev_ip, i_dev_port1, c_dev_usr, i_box_port1);
	}
	strncat(dev_info_buf, buf_var, BUF_LEN_OF_JSON-strlen(dev_info_buf));

	return 0;
}

//获取厂商列表回调函数
static HB_S32 LoadDeviceFactory( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR c_factory_code[128] = {0};
	HB_CHAR c_factory_name[128] = {0};

	strncpy(c_factory_code, column_value[0], strlen(column_value[0]));
	strncpy(c_factory_name, column_value[1], strlen(column_value[1]));

	strncpy(dev_info_buf, c_factory_code, strlen(c_factory_code));

	return 0;
}

//获取数据条数回调函数
static HB_S32 LoadDeviceListNum( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_num_buf = (HB_CHAR *)para;

	strncpy(dev_num_buf, column_value[0], strlen(column_value[0]));

	return 0;
}


//获取流媒体服务器列表回调函数
static HB_S32 LoadStreamServerList( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR save_log[512] = {0};
	STREAM_HANDLE stream_info_buf = (STREAM_HANDLE)para;
	STREAM_HANDLE p_stream_info = (STREAM_HANDLE)malloc(sizeof(STREAM_OBJ));

	memset(p_stream_info->c_server_ip, 0, sizeof(p_stream_info->c_server_ip));
	p_stream_info->i_server_port = 0;
	strncpy(p_stream_info->c_server_ip, column_value[0], strlen(column_value[0]));
	p_stream_info->i_server_port = atoi(column_value[1]);


#ifdef WRITE_LOG
	memset(save_log, '\0', sizeof(save_log));
	snprintf(save_log, sizeof(save_log), "%s %d#####stream server ip: [%s]\t\tserver_port:[%d]\n", \
					__FUNCTION__, __LINE__, p_stream_info->c_server_ip, p_stream_info->i_server_port);
	WriteLog(save_log, strlen(save_log));
#endif
	p_stream_info->next = stream_info_buf->next;
	stream_info_buf->next = p_stream_info;

	return 0;
}


//测试连接状态线程
static HB_S32 connect_ip_test(HB_CHAR *c_dev_ip, HB_S32 i_dev_port)
{
	HB_S32 ret = 0;
	HB_S32 psockfd;
	HB_CHAR save_log[512] = {0};

	if((psockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return -1;
	}

#ifdef WRITE_LOG
			memset(save_log, '\0', sizeof(save_log));
			snprintf(save_log, sizeof(save_log), "\n%s_%s_%d#####:dev_ip:[%s]\tdev_port:[%d]\n", \
							__FILE__, __FUNCTION__, __LINE__, c_dev_ip, i_dev_port);
			WriteLog(save_log, strlen(save_log));
#endif

	ret = create_socket_connect_ipaddr(&psockfd, c_dev_ip, i_dev_port, 2);
	if(ret < 0)
	{
		if(psockfd > 0)
		{
			close(psockfd);
			psockfd = -1;
		}
		return -2;
	}
	close(psockfd);
	psockfd = -1;

	return 0;
}


//测试天联是否正常，正常返回0,不正常返回-1
static HB_S32 test_gnLan_alive()
{
	HB_S32 rand1 = -1, rand2 = -1;
	HB_S32 rand_times = 0;
	FILE *p_fd = NULL;
	HB_CHAR out_msg[256] = {0};

	HB_CHAR *arr_Ping[] = {	"ping -I gnLan 10.6.171.2 -c 1 -w 5",
							"ping -I gnLan 10.7.8.48 -c 1 -w 5",
							"ping -I gnLan 10.7.9.164 -c 1 -w 5",
							"ping -I gnLan 10.7.9.231 -c 1 -w 5",
							"ping -I gnLan 10.7.10.197  -c 1 -w 5",
							"ping -I gnLan 10.4.120.242 -c 1 -w 5",
							"ping -I gnLan 10.7.9.119 -c 1 -w 5",
							"ping -I gnLan 10.7.9.228 -c 1 -w 5",
							"ping -I gnLan 10.7.10.175 -c 1 -w 5",
							"ping -I gnLan 10.7.10.72 -c 1 -w 5"};

	while(rand_times < 3)
	{
		srand(time(NULL));
		rand1 = rand()%10;

		if (rand1 == rand2)
		{
			continue;
		}
		rand2 = rand1;

		p_fd = popen(arr_Ping[rand1], "r");
		if(NULL == p_fd)
		{
			rand_times++;
			continue;
		}
		else
		{
			while(fgets(out_msg, 256, p_fd) != NULL)
			{
				if(strstr(out_msg, "ttl=") != NULL)//gnLan正常
				{
//					printf("\n#############  gnLan alive!\n");
					pclose(p_fd);
					return 0;
				}
				memset(out_msg, 0, sizeof(out_msg));
			}
			rand_times++;
			pclose(p_fd);
		}

	}

//	printf("\n#############  gnLan died!\n");
	return -1;
}

//添加一点通盒子设备
HB_S32 AddBoxDev(HB_S32 i_data_code, HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 ret = -1;
	HB_S32 sockfd = -1;
	HB_S32 error_code = 0;
	HB_CHAR c_dev_num[8] = {0}; //用于记录数据库中数据的条数
	HB_CHAR c_tcp_dev_num[8] = {0}; //用于记录数据库中数据的条数

	HB_S32 dev_type_flag = 0;
	HB_CHAR c_dev_type_code[8] = {0};
    HB_CHAR c_dev_type_name[128] = {0};
	HB_CHAR c_dev_name[128] = {0};
	HB_CHAR c_dev_ip[16] = {0};
	HB_CHAR c_dev_port1[8] = {0};
	HB_CHAR c_dev_port2[8] = {0};
	HB_CHAR c_dev_chns[8] = {0};
	HB_CHAR c_dev_user[64] = {0};
	HB_CHAR c_dev_pwd[128] = {0};
	HB_CHAR c_box_port1[8] = {0};
	HB_CHAR c_box_port2[8] = {0};
	HB_CHAR c_dev_id[128] = {0};

	HB_CHAR sql[512] = {0};
	HB_CHAR c_device_info_buf[1024] = {0};
	GN_NET_LAYER_OBJ buf_cmd;

	if((pos = strstr(buf, "DevType"))) //添加设备
	{
		memset(sql, 0, sizeof(sql));
		snprintf(sql, sizeof(sql), "select count (*) as num from dev_add_web_data");
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceListNum, (void *)c_dev_num);
		WRITE_LOG("[%d] devices in database!\n", atoi(c_dev_num));

		memset(sql, 0, sizeof(sql));
//		snprintf(sql, sizeof(sql), "select count (*) as num from tcp_dev_data");
		snprintf(sql, sizeof(sql), "SELECT COUNT(DISTINCT tcp_dev_ip) FROM tcp_dev_data");
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceListNum, (void *)c_tcp_dev_num);
		WRITE_LOG("[%d] devices in database!\n", atoi(c_tcp_dev_num));

		if ((atoi(c_dev_num)+atoi(c_tcp_dev_num)) >= MAX_DEV_NUM)
		{
			printf("{\"Result\":\"-207\",\"ErrMessage\":\"挂载的设备数量达到上限！-207\"}");
			error_code = -8;
			goto ERR_RETURN;
		}

		if (0 == get_sys_gnLan())
		{
			printf("{\"Result\":\"-210\",\"ErrMessage\":\"虚拟ip未启动！-210\"}");
			error_code = -1;
			goto ERR_RETURN;
		}

		if (test_gnLan_alive() < 0)
		{
			printf("{\"Result\":\"-224\",\"ErrMessage\":\"虚拟ip异常！-224\"}");
			error_code = -1;
			goto ERR_RETURN;
		}

		if (i_data_code == ADD_BOX_DEV_HAND) //双端口,手动添加模式
		{
			if (strstr(buf, "=&")) {
				sscanf(pos, "DevType=%[^&]&DevName=%[^&]&DevIP=%[^&]&DevPort1=%[^&]&DevPort2=&DevChns=%[^&]&DevLoginUsr=%[^&]&DevLoginPwd=%[^&]&BoxPort1=%[^&]",
							c_dev_type_name, c_dev_name, c_dev_ip, c_dev_port1, c_dev_chns, c_dev_user, c_dev_pwd, c_box_port1);
			}
			else
			{
				sscanf(pos, "DevType=%[^&]&DevName=%[^&]&DevIP=%[^&]&DevPort1=%[^&]&DevPort2=%[^&]&DevChns=%[^&]&DevLoginUsr=%[^&]&DevLoginPwd=%[^&]&BoxPort1=%[^&]&BoxPort2=%s",
								c_dev_type_name, c_dev_name, c_dev_ip, c_dev_port1, c_dev_port2, c_dev_chns, c_dev_user, c_dev_pwd, c_box_port1, c_box_port2);
			}
		}
		else
		{
			sscanf(pos, "DevType=%[^&]&DevName=%[^&]&DevIP=%[^&]&DevPort=%[^&]&DevLoginUsr=%[^&]&DevLoginPwd=%s",
						c_dev_type_name, c_dev_name, c_dev_ip, c_dev_port1, c_dev_user, c_dev_pwd);
		}
		/**************************************************************************************/
		/*************************************测试设备连通性*************************************/
		/**************************************************************************************/
		{
			ret = connect_ip_test(c_dev_ip, atoi(c_dev_port1));
			switch(ret)
			{
				case -1:	//创建socket失败
					printf("{\"Result\":\"-211\",\"ErrMessage\":\"创建socket失败！-211\"}");
					error_code = -2;
					goto ERR_RETURN;
				case -2:	//设备不在线，ip或端口设置有错误
					printf("{\"Result\":\"-212\",\"ErrMessage\":\"此设备不通！-212\"}");
					error_code = -3;
					goto ERR_RETURN;
				default: //on_line
					break;
			}

			if (atoi(c_dev_port2) > 0) {
				ret = connect_ip_test(c_dev_ip, atoi(c_dev_port2));
				switch(ret)
				{
					case -1:	//创建socket失败
						printf("{\"Result\":\"-211\",\"ErrMessage\":\"创建socket失败！-211\"}");
						error_code = -2;
						goto ERR_RETURN;
					case -2:	//设备不在线，ip或端口设置有错误
						printf("{\"Result\":\"-212\",\"ErrMessage\":\"设备端口２不通！-212\"}");
						error_code = -3;
						goto ERR_RETURN;
					default: //on_line
						break;
				}
			}
		}
		/**************************************************************************************/
		/************************************测试设备连通性END***********************************/
		/**************************************************************************************/

		//获取厂家id
		url_decode(c_dev_type_name, strlen(c_dev_type_name));
		memset(sql, 0, sizeof(sql));
		snprintf(sql, sizeof(sql), "select * from factory_list_data where factory_name = '%s' ", c_dev_type_name);
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceFactory, (void *)c_dev_type_code);
		dev_type_flag = atoi(c_dev_type_code);

		/**************************************************************************************/
		/**************************************发送到流媒体**************************************/
		/**************************************************************************************/
		if (i_data_code == ADD_BOX_DEV_AUTO)
		{
			HB_CHAR *p_pos_recv_buf = NULL;
			HB_S32 m_hSocket = -1;
			NET_LAYER m_package;
			HB_CHAR i_err_code[8] = {0};
			HB_CHAR sQuery[256]= {0};
			HB_CHAR gnlan_ip[128] = {0};
			STREAM_OBJ stream_info;
			STREAM_HANDLE p_stream_info = NULL;

			get_dev_ip("gnLan", gnlan_ip);

			snprintf(sQuery, sizeof(sQuery), "<TYPE>VerifyDeviceEx</TYPE><YDTBoxIP>%s</YDTBoxIP><YDTBoxPort>8101</YDTBoxPort><DeviceType>%d</DeviceType><LocalIP>%s</LocalIP><LocalPort>%s</LocalPort><User>%s</User><Pwd>%s</Pwd>", \
							gnlan_ip, dev_type_flag, c_dev_ip, c_dev_port1, c_dev_user, c_dev_pwd);

			stream_info.next = NULL;

			memset(sql, 0, sizeof(sql));
			snprintf(sql, sizeof(sql), "select stream_server_ip,stream_server_port from stream_server_list_data");
			SqlOperation(sql, BOX_DATA_BASE_NAME, LoadStreamServerList, (void *)&stream_info);

			p_stream_info = stream_info.next;

			if (p_stream_info == NULL)
			{
				printf("{\"Result\":\"-7\",\"ErrMessage\":\"获取流媒体地址失败！-7\"}");
				error_code = -7;
				goto ERR_RETURN;
			}

#if 1
			//根据流媒体服务器链表连接流媒体，如果都连接不上则返回失败
			while (p_stream_info != NULL)
			{
				WRITE_LOG("stream server ip: [%s]\tserver_port:[%d]\n", p_stream_info->c_server_ip, p_stream_info->i_server_port);
				ret = create_socket_connect_ipaddr(&m_hSocket, p_stream_info->c_server_ip, p_stream_info->i_server_port, 5);
				//ret = create_socket_connect_ipaddr(&m_hSocket, STREAM_SERVER_IP, STREAM_SERVER_PORT, 5);
				if (ret < 0)
				{
					p_stream_info = p_stream_info->next;
				}
				else
				{
					break;
				}
			}
#endif
			//ret = create_socket_connect_ipaddr(&m_hSocket, STREAM_SERVER_IP, STREAM_SERVER_PORT, 5);
			if (ret < 0) {
				printf("{\"Result\":\"-6\",\"ErrMessage\":\"连接流媒体失败！-213\"}");
				error_code = -4;
				goto ERR_RETURN;
			}
			WRITE_LOG("@@@@@@@@@@@@@@@@@send_data to stream: %s\n", sQuery);
			SendData(m_hSocket, sQuery, strlen(sQuery), 9);
			RecvData(m_hSocket, &m_package, sizeof(m_package), 25);
			close(m_hSocket);
			m_hSocket = -1;

			p_stream_info = stream_info.next;
			while (p_stream_info != NULL)
			{
				stream_info.next = p_stream_info->next;
				free(p_stream_info);
				p_stream_info = stream_info.next;
			}

			p_pos_recv_buf = m_package.cBuffer;
			if(strstr(p_pos_recv_buf, "<Return>Success</Return>"))
			{
				//通讯成功，解析串并写数据库
				//<Return>Success</Return><IP>10.6.47.199</IP><MapPort1>9000</MapPort1><MapPort2>0</MapPort2><Serial>5a09cc94</Serial><Chn>1</Chn>
				p_pos_recv_buf = strstr(p_pos_recv_buf, "<MapPort1>");
				sscanf(p_pos_recv_buf, "<MapPort1>%[^<]</MapPort1><MapPort2>%[^<]</MapPort2><Serial>%[^<]</Serial><Chn>%[^<]</Chn>",
								c_box_port1, c_box_port2, c_dev_id, c_dev_chns);

				//写数据库
				//insert into dev_add_web_data (dev_type,dev_name,dev_id,dev_ip,dev_chns,dev_login_usr,dev_login_pwd,dev_port,box_port,dev_port2,box_port2,dev_state) values ()
				memset(sql, 0, sizeof(sql));
				snprintf(sql, sizeof(sql), "insert into dev_add_web_data (dev_type,dev_name,dev_id,dev_ip,dev_chns,dev_login_usr,dev_login_pwd,dev_port,box_port,dev_port2,box_port2,dev_state) values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','0')", \
								c_dev_type_code, c_dev_name, c_dev_id, c_dev_ip, c_dev_chns, c_dev_user, c_dev_pwd, c_dev_port1, c_box_port1, c_box_port2, c_box_port2); //dev_port2与box_port2的值都取自c_box_port2，此处并没有错误，第二个设备端口和映射端口值相同，且数值来自c_box_port2
				ret = SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
				if (ret < 0)
				{
					printf("{\"Result\":\"-7\",\"ErrMessage\":\"数据插入数据库失败，设备重复！-7\"}");
					error_code = -7;
					goto ERR_RETURN;
				}

//				WRITE_LOG("insert to sql:[%s]\n", sql);
			}
			else
			{
				p_pos_recv_buf = strstr(m_package.cBuffer, "<Result>");
				if (p_pos_recv_buf != NULL)
				{
					sscanf(p_pos_recv_buf, "<Result>%s</Result>", i_err_code);
#if 0
					-1：流媒体与盒子不通(流媒体与盒子通信)
					-2：流媒体发送命令给盒子失败(流媒体与盒子通信)
					-3：流媒体接受盒子数据失败(流媒体与盒子通信)
					-4：盒子返回给流媒体数据非法(流媒体与盒子通信)
					-5：盒子虚拟IP不通
					-6：盒子映射失败
					-7：盒子与前端设备不通
					-8：用户名或密码错误
#endif
					switch(atoi(i_err_code))
					{
						case -1:
							printf("{\"Result\":\"-214\",\"ErrMessage\":\"流媒体与盒子不通！-214\"}");
							break;
						case -2:
							printf("{\"Result\":\"-215\",\"ErrMessage\":\"流媒体发送命令给盒子失败！-215\"}");
							break;
						case -3:
							printf("{\"Result\":\"-216\",\"ErrMessage\":\"流媒体接受盒子数据失败！-216\"}");
							break;
						case -4:
							printf("{\"Result\":\"-217\",\"ErrMessage\":\"盒子返回给流媒体数据非法！-217\"}");
							break;
						case -5:
							printf("{\"Result\":\"-218\",\"ErrMessage\":\"盒子虚拟IP不通！-218\"}");
							break;
						case -6:
							printf("{\"Result\":\"-219\",\"ErrMessage\":\"盒子映射失败！-219\"}");
							break;
						case -7:
							printf("{\"Result\":\"-220\",\"ErrMessage\":\"盒子与前端设备不通！-220\"}");
							break;
						case -8:
							printf("{\"Result\":\"-221\",\"ErrMessage\":\"用户名或密码错误！-221\"}");
							break;
						default:
							printf("{\"Result\":\"-222\",\"ErrMessage\":\"与流媒体通信错误[%d]！-222\"}", atoi(i_err_code));
							break;
					}
					//return atoi(i_err_code);
					error_code = -5;
					goto ERR_RETURN;
				}
				else
				{
					printf("{\"Result\":\"-6\",\"ErrMessage\":\"流媒体返回数据格式有误！-223\"}");
					error_code = -6;
					goto ERR_RETURN;
				}
			}

		}
		/*****************************************************************************************/
		/**************************************发送到流媒体End**************************************/
		/*****************************************************************************************/

#if 1
		/*****************************************************************************************/
		/****************************************发送到主控****************************************/
		/*****************************************************************************************/
		{
			HB_S32 i;
			//HB_CHAR c_rand_buf[5] = {0};
			if (i_data_code == ADD_BOX_DEV_HAND) //手动填写时，说明不走验证服务器，需要自动生成设备ID
			{
				memset(c_dev_id, 0, sizeof(c_dev_id));
				//get_sys_sn(c_dev_mac, sizeof(c_dev_mac));
				srand((unsigned int) time(NULL));
				for (i=0; i<4; i++)
				{
					c_dev_id[i] = 'a' + ( 0+ (int)(26.0 *rand()/(RAND_MAX + 1.0)));
				}
				WRITE_LOG("c_rand_dev_id:[%s]\n", c_dev_id);

				//写数据库
				memset(sql, 0, sizeof(sql));
				//insert into dev_add_web_data (dev_type,dev_name,dev_id,dev_ip,dev_chns,dev_login_usr,dev_login_pwd,dev_port,box_port,dev_port2,box_port2,dev_state) values ()
				if (atoi(c_dev_port2) > 0)
				{
					if (check_port(atoi(c_dev_port1)) < 0)
					{
						printf("{\"Result\":\"-222\",\"ErrMessage\":\"映射端口1已占用\"}");
						return -1;
					}
					if (check_port(atoi(c_dev_port2)) < 0)
					{
						printf("{\"Result\":\"-222\",\"ErrMessage\":\"映射端口2已占用\"}");
						return -1;
					}
					snprintf(sql, sizeof(sql), "insert into dev_add_web_data (dev_type,dev_name,dev_id,dev_ip,dev_chns,dev_login_usr,dev_login_pwd,dev_port,box_port,dev_port2,box_port2,dev_state) values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','0')", \
									c_dev_type_code, c_dev_name, c_dev_id, c_dev_ip, c_dev_chns, c_dev_user, c_dev_pwd, c_dev_port1, c_box_port1, c_dev_port2, c_box_port2);
				}
				else
				{
					if (check_port(atoi(c_dev_port1)) < 0)
					{
						printf("{\"Result\":\"-222\",\"ErrMessage\":\"映射端口1已占用\"}");
						return -1;
					}
					snprintf(sql, sizeof(sql), "insert into dev_add_web_data (dev_type,dev_name,dev_id,dev_ip,dev_chns,dev_login_usr,dev_login_pwd,dev_port,box_port,dev_port2,box_port2,dev_state) values ('%s','%s','%s','%s','%s','%s','%s','%s','%s','0','0','0')", \
									c_dev_type_code, c_dev_name, c_dev_id, c_dev_ip, c_dev_chns, c_dev_user, c_dev_pwd, c_dev_port1, c_box_port1);
				}
				SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
			}

			//如果是海康设备，查询设备是不是已经在rtsp_list表中添加过，若添加过需要更新rtsp_list表中的rtsp端口，因为流媒体会修改海康设备的rtsp端口号
			if (dev_type_flag == 2)
			{
				HB_CHAR cNum[4] = {0};//查询到的数据条数
				memset(sql, 0, sizeof(sql));
				snprintf(sql, sizeof(sql), "select count(*) from onvif_dev_data_list where dev_id='%s'", c_dev_id);
				SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceListNum, (HB_VOID *)cNum);
				if (atoi(cNum) > 0)
				{
					//先前已经在onvif设备中添加过此设备
					memset(sql, 0, sizeof(sql));
					snprintf(sql, sizeof(sql), "update onvif_dev_data_list set rtsp_port='%s' where dev_id='%s';", c_box_port2, c_dev_id);
					SqlOperation(sql, BOX_DATA_BASE_NAME, NULL, NULL);
				}
			}

			dev_type_flag = atoi(c_device_info_buf);
			memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
			buf_cmd.iProtocolVer = 1;
			buf_cmd.iDataType = 9;
			buf_cmd.iBlockHeadFlag = 1;
			buf_cmd.iBlockEndFlag = 1;
			if (i_data_code == ADD_BOX_DEV_HAND)
			{
				sprintf(buf_cmd.cBuffer, "<TYPE>AddPortInfoOnWeb</TYPE><DevId>%s</DevId>", c_dev_id);
			}
			else
			{
				sprintf(buf_cmd.cBuffer, "<TYPE>RegistDevInfoOnWeb</TYPE><DevId>%s</DevId>", c_dev_id);
			}
			WRITE_LOG("!!!!!!!!!!!!!!!!!sendto box_main_ctrl:[%s]\n", buf_cmd.cBuffer);
			buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);
			ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&buf_cmd, buf_cmd.iActLength, sizeof(buf_cmd), 10);
			if (ret < 0)
			{
				switch(ret)
				{
					case -1:
						printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
						break;
					case -2:
						printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
						break;
					case -3:
						printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
						break;
					default:
						break;
				}
				return -2;
			}

			char ret_code[8] = {0};
			sscanf(buf_cmd.cBuffer,"<ReturnCode>%[^<]</ReturnCode>", ret_code);
			switch(atoi(ret_code))
			{
				case 0:
					memset(c_device_info_buf, 0, sizeof(c_device_info_buf));
					//添加成功发送表单
					memset(sql, 0, sizeof(sql));
					snprintf(sql, sizeof(sql), "select factory_list_data.factory_name,dev_name,dev_id,dev_chns,dev_ip,dev_port,dev_login_usr,box_port,dev_port2,box_port2 from dev_add_web_data left join factory_list_data on dev_add_web_data.dev_type=factory_list_data.factory_code where dev_id = '%s'", c_dev_id);
					SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceInfoOneLine, (void *)c_device_info_buf);
					WRITE_LOG("send to web :[{\"Result\":\"0\",%s}]\n", c_device_info_buf);
//					printf("Content type: application/json \n\n");
					printf("{\"Result\":\"0\",%s}", c_device_info_buf);
					break;
#if 0
				case -1:
					//printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-203\",\"ErrMessage\":\"设置的映射端口值非法！-203\"}");
					break;
				case -2:
					printf("{\"Result\":\"-204\",\"ErrMessage\":\"添加的设备达到上限！-204\"}");
					break;
				case -3:
					printf("{\"Result\":\"-205\",\"ErrMessage\":\"映射端口已被占用!-205\"}");
					break;
				case -4:
					printf("{\"Result\":\"-206\",\"ErrMessage\":\"数据库错误！-206\"}");
					break;
#endif
				default:
					printf("{\"Result\":\"-207\",\"ErrMessage\":\"主控返回数据格式错误！-207\"}");
					error_code = -7;
					goto ERR_RETURN;
			}
		}
		/*******************************************************************************************/
		/****************************************发送到主控End****************************************/
		/*******************************************************************************************/
#endif
	}

ERR_RETURN:
	return error_code;
}


//删除一条一点通盒子设备
HB_S32 DelOneBoxDev(HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 ret;
	HB_S32 sockfd = -1;
	HB_S32 offset = 0;
	HB_CHAR index[8] = {0};
	HB_CHAR c_mac_sn[32] = {0};
	HB_CHAR c_dev_id[128] = {0};
	GN_NET_LAYER_OBJ buf_cmd;
	if((pos = strstr(buf, "DevID"))) //删除设备
	{
		sscanf(pos, "DevID=%[^&]&DataIndex=%s", c_dev_id, index);

		get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
		offset = strlen(c_mac_sn)+1;

		memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
		buf_cmd.iProtocolVer = 1;
		buf_cmd.iDataType = 9;
		buf_cmd.iBlockHeadFlag = 1;
		buf_cmd.iBlockEndFlag = 1;
		sprintf(buf_cmd.cBuffer, "<TYPE>DelMapPortOnWeb</TYPE><DevId>%s</DevId>", c_dev_id+offset);
		buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);
		ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&buf_cmd, buf_cmd.iActLength, sizeof(buf_cmd), 10);
		if (ret < 0)
		{
			switch(ret)
			{
				case -1:
					printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
					break;
				case -2:
					printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
					break;
				case -3:
					printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
					break;
				default:
					break;
			}
			return -2;
		}
		sscanf(buf_cmd.cBuffer,"<ReturnCode>%d</ReturnCode>",&ret);
		if (ret == 0)
		{
			printf("{\"Result\":\"0\",\"DevID\":\"%s\",\"DataIndex\":\"%s\"}", c_dev_id, index);
		}
		else
		{
			printf("{\"Result\":\"-208\",\"ErrMessage\":\"删除映射失败！-208\"}");
		}
	}

	return 0;
}



//删除全部一点通盒子设备
HB_S32 DelAllBoxDev()
{
	HB_S32 ret = 0;
	HB_S32 sockfd = -1;
	GN_NET_LAYER_OBJ buf_cmd;

	memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
	buf_cmd.iProtocolVer = 1;
	buf_cmd.iDataType = 9;
	buf_cmd.iBlockHeadFlag = 1;
	buf_cmd.iBlockEndFlag = 1;
	sprintf(buf_cmd.cBuffer, "<TYPE>DelMapPortOnWeb</TYPE>");
	buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);

	ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&buf_cmd, buf_cmd.iActLength, sizeof(buf_cmd), 10);
	if (ret < 0)
	{
		switch(ret)
		{
			case -1:
				printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
				break;
			case -2:
				printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
				break;
			case -3:
				printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
				break;
			default:
				break;
		}
		return -2;
	}
	sscanf(buf_cmd.cBuffer,"<ReturnCode>%d</ReturnCode>",&ret);
	if (ret == 0)
	{
		printf("{\"Result\":\"0\"}");
	}
	else
	{
		printf("{\"Result\":\"-209\",\"ErrMessage\":\"删除全部映射失败！-209\"}");
	}

	return 0;
}


//获取一点通盒子列表回调函数
static HB_S32 LoadDeviceInfo( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR c_mac_sn[32] = {0};
	HB_CHAR buf_var[1024] = {0};

	HB_CHAR c_dev_type_name[128] = {0};
	HB_CHAR c_dev_name[128] = {0};
	HB_CHAR c_dev_id[64] = {0};
	HB_S32 i_dev_chns = 0;
	HB_CHAR c_dev_ip[20] = {0};
	HB_S32 i_dev_port = 0;
	HB_CHAR c_dev_usr[128] = {0};
	HB_S32 i_box_port = 0;
	HB_S32 i_dev_status = 0;
	HB_S32 i_dev_port2 = -1;
	HB_S32 i_box_port2 = -1;


	strncpy(c_dev_type_name, column_value[0], strlen(column_value[0]));
	strncpy(c_dev_name, column_value[1], strlen(column_value[1]));
	strncpy(c_dev_id, column_value[2], strlen(column_value[2]));
	i_dev_chns = atoi(column_value[3]);
	strncpy(c_dev_ip, column_value[4], strlen(column_value[4]));
	i_dev_port = atoi(column_value[5]);
	strncpy(c_dev_usr, column_value[6], strlen(column_value[6]));
	i_box_port = atoi(column_value[7]);
	i_dev_status = atoi(column_value[8]);
	i_dev_port2 = atoi(column_value[9]);
	i_box_port2 = atoi(column_value[10]);

	if ((i_dev_status == 2) || (i_dev_status == 3)) {
		return 1;
	}

	url_decode(c_dev_type_name, strlen(c_dev_type_name));
	url_decode(c_dev_name, strlen(c_dev_name));

	get_sys_sn(c_mac_sn, sizeof(c_mac_sn));

	if (i_dev_port2 > 0) {
		//获取盒子信息
		snprintf(buf_var, sizeof(buf_var), "{\"DevPortNum\":\"2\",\"DevType\":\"%s\",\"DevName\":\"%s\",\"DevID\":\"%s-%s\",\"DevChns\":\"%d\",\"DevIP\":\"%s\",\"DevPort1\":\"%d\",\"DevPort2\":\"%d\",\"DevLoginUsr\":\"%s\",\"BoxPort1\":\"%d\",\"BoxPort2\":\"%d\"},",\
						c_dev_type_name, c_dev_name, c_mac_sn, c_dev_id, i_dev_chns, c_dev_ip, i_dev_port, i_dev_port2, c_dev_usr, i_box_port, i_box_port2);
	}
	else
	{
		//获取盒子信息
		snprintf(buf_var, sizeof(buf_var), "{\"DevPortNum\":\"1\",\"DevType\":\"%s\",\"DevName\":\"%s\",\"DevID\":\"%s-%s\",\"DevChns\":\"%d\",\"DevIP\":\"%s\",\"DevPort\":\"%d\",\"DevLoginUsr\":\"%s\",\"BoxPort\":\"%d\"},",\
						c_dev_type_name, c_dev_name, c_mac_sn, c_dev_id, i_dev_chns, c_dev_ip, i_dev_port, c_dev_usr, i_box_port);
	}
	strncat(dev_info_buf, buf_var, BUF_LEN_OF_JSON-strlen(dev_info_buf));

	return 0;
}

//获取一点通厂商列表回调函数
static HB_S32 LoadDeviceTypeInfo( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR buf_var[1024] = {0};

	HB_CHAR c_factory_code[8] = {0};
	HB_CHAR c_factory_name[128] = {0};

	strncpy(c_factory_code, column_value[0], strlen(column_value[0]));
	strncpy(c_factory_name, column_value[1], strlen(column_value[1]));

	url_decode(c_factory_name, strlen(c_factory_name));

    //获取盒子信息
	snprintf(buf_var, sizeof(buf_var), "\"%s\",", c_factory_name);
	strncat(dev_info_buf, buf_var, BUF_LEN_OF_JSON-strlen(dev_info_buf));

	return 0;
}


//用于获取是否开启了云端验证
static HB_S32 GetCloudVerifyStatus( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_cloud_Verify = (HB_CHAR *)para;

	strncpy(p_cloud_Verify, column_value[0], strlen(column_value[0]));

	return 0;
}

//获取一点通盒子列表
HB_S32 GetBoxDevList(HB_CHAR *packeting_str, HB_S32 size_packeting_str)
{
	HB_CHAR c_cloud_verify[8] = {0};
	HB_CHAR c_factory_info_buf[BUF_LEN_OF_JSON] = {0};
	HB_CHAR c_device_info_buf[BUF_LEN_OF_JSON] = {0};
	HB_CHAR c_make_str[BUF_LEN_OF_JSON] = {0};
	HB_CHAR sql[512] = {0};


	//获取云端验证状态
	memset(sql, 0, sizeof(sql));
	snprintf(sql, sizeof(sql), "select cloud_verify from system_web_data");
	SqlOperation(sql, BOX_DATA_BASE_NAME, GetCloudVerifyStatus, (void *)c_cloud_verify);
	memset(sql, 0, sizeof(sql));
	strncpy(sql, "select factory_code,factory_name from factory_list_data", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceTypeInfo, (void *)c_factory_info_buf);
	memset(sql, 0, sizeof(sql));
	strncpy(sql, "select factory_list_data.factory_name,dev_name,dev_id,dev_chns,dev_ip,dev_port,dev_login_usr,box_port,dev_state,dev_port2,box_port2 from dev_add_web_data left join factory_list_data on dev_add_web_data.dev_type=factory_list_data.factory_code", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceInfo, (void *)c_device_info_buf);

	if (c_factory_info_buf[strlen(c_factory_info_buf)-1] == ',')
	{
		c_factory_info_buf[strlen(c_factory_info_buf)-1] = '\0';
	}

	if (c_device_info_buf[strlen(c_device_info_buf)-1] == ',')
	{
		c_device_info_buf[strlen(c_device_info_buf)-1] = '\0';
	}
	snprintf(c_make_str, sizeof(c_make_str), "\"Result\":\"0\",\"CloudVerify\":\"%s\",\"DeviceType\":[%s],\"BoxDeviceInfo\":[%s],", c_cloud_verify, c_factory_info_buf, c_device_info_buf);
	strncat(packeting_str, c_make_str, size_packeting_str);
	//printf("{\"Result\":\"0\",\"CloudVerify\":\"%s\",\"DeviceType\":[%s],\"BoxDeviceInfo\":[%s]}", c_cloud_verify, c_factory_info_buf, c_device_info_buf);

	return 0;
}

/************************一点通盒子操作END************************/
/************************一点通盒子操作END************************/
/************************一点通盒子操作END************************/


/************************RTSP设备操作************************/
/************************RTSP设备操作************************/
/************************RTSP设备操作************************/

//添加RTSP设备
HB_S32 add_rtsp_dev(HB_CHAR *buf)
{
	sqlite3 *db;
	HB_CHAR *errmsg = NULL;
	HB_CHAR *pPos = NULL;
	HB_S32 i = 0;
	HB_S32 iRet = 0;
	HB_S32 iSockFd = -1;
	HB_CHAR cRetCode[8] = {0};
	HB_CHAR cDataIndex[8] = {0};
	HB_CHAR cOnvifServicePort[8] = {0}; //Onvif服务端口
	HB_CHAR cCalcBasicAuth[128] = {0}; //用于计算基本认证
	HB_CHAR cSql[512] = {0};
	ONVIF_DEV_INFO_OBJ stOnvifDevInfo;
	GN_NET_LAYER_OBJ stBufCmd;

	memset(&stOnvifDevInfo, 0, sizeof(ONVIF_DEV_INFO_OBJ));
	memset(&stBufCmd, 0, sizeof(GN_NET_LAYER_OBJ));
	stBufCmd.iProtocolVer = 1;
	stBufCmd.iDataType = 9;
	stBufCmd.iBlockHeadFlag = 1;
	stBufCmd.iBlockEndFlag = 1;

	if((pPos = strstr(buf, "DataIndex"))) //添加设备
	{
		sscanf(pPos, "DataIndex=%[^&]&DevName=%[^&]&DevLoginUser=%[^&]&DevLoginPasswd=%[^&]&DevIp=%[^&]&DevPort=%[^&]&DevServiceUrl=%s", \
			cDataIndex, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd, \
			stOnvifDevInfo.cDevIp, cOnvifServicePort, stOnvifDevInfo.cDevServiceUrl);

		url_decode(stOnvifDevInfo.cDevServiceUrl, strlen(stOnvifDevInfo.cDevServiceUrl));

		snprintf(stBufCmd.cBuffer, sizeof(stBufCmd.cBuffer), \
			"{\"TYPE\":\"AddOnvifDev\",\"LoginUser\":\"%s\",\"LoginPasswd\":\"%s\",\"DevIp\":\"%s\",\"DevPort\":\"%s\",\"ServiceUrl\":\"%s\"}", \
			stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd, stOnvifDevInfo.cDevIp, cOnvifServicePort, stOnvifDevInfo.cDevServiceUrl);
		stBufCmd.iActLength = 5*sizeof(HB_S32) + strlen(stBufCmd.cBuffer);

		WRITE_LOG("!!!!!!!!!!!!!!!!!sendto box_main_ctrl:[%s]\n", stBufCmd.cBuffer);
		iRet = NetworkCommunicate(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, sizeof(stBufCmd.cBuffer), 10);
		if (iRet < 0)
		{
			switch(iRet)
			{
				case -1:
					printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
					break;
				case -2:
					printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
					break;
				case -3:
					printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
					break;
				default:
					break;
			}
			return -2;
		}

		WRITE_LOG("!!!!!!!!!!!!!!!!!recv from box_main_ctrl:[%s]\n", stBufCmd.cBuffer);
		if(strstr(stBufCmd.cBuffer, "<ReturnCode>") != NULL)
		{
			//错误信息处理
			sscanf(stBufCmd.cBuffer,"<ReturnCode>%[^<]</ReturnCode>", cRetCode);
			switch(atoi(cRetCode))
			{
				case -1:
					//printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-228\",\"ErrMessage\":\"与设备通信异常-228\"}");
					break;
				case -2:
					printf("{\"Result\":\"-229\",\"ErrMessage\":\"用户名或密码错误！-229\"}");
					break;
				default:
					break;
			}
		}
		else
		{
//			HB_CHAR c_mac_sn[32] = {0};
//			get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
//
//			cJSON *pItem = NULL;
//			cJSON *pJsonRoot = cJSON_Parse(stBufCmd.cBuffer); //json根
//			pItem = cJSON_GetObjectItem(pJsonRoot,"OnvifDevId");
//			strncpy(stOnvifDevInfo.cDevId, pItem->valuestring, strlen(pItem->valuestring));
//			pItem = cJSON_GetObjectItem(pJsonRoot,"ChlNums");
//			strncpy(stOnvifDevInfo.cDevChnls, pItem->valuestring, strlen(pItem->valuestring));
//			cJSON_Delete(pJsonRoot);

			printf("{\"Result\":0}");


#if 0
			HB_CHAR c_mac_sn[32] = {0};
			//设备搜索成功，进行json串的解析以及插入数据库
			snprintf(cCalcBasicAuth, sizeof(cCalcBasicAuth), "%s:%s", stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd);
			base64_encode(stOnvifDevInfo.cBasicAuth, (HB_U8 *)cCalcBasicAuth, strlen(cCalcBasicAuth), sizeof(stOnvifDevInfo.cBasicAuth));

			cJSON *pItem = NULL;
			cJSON *pJsonRoot = cJSON_Parse(stBufCmd.cBuffer); //json根
			pItem = cJSON_GetObjectItem(pJsonRoot,"OnvifDevId");
			strncpy(stOnvifDevInfo.cDevId, pItem->valuestring, strlen(pItem->valuestring));
			pItem = cJSON_GetObjectItem(pJsonRoot,"ChlNums");
			strncpy(stOnvifDevInfo.cDevChnls, pItem->valuestring, strlen(pItem->valuestring));
			cJSON *pChlListArry=cJSON_GetObjectItem(pJsonRoot,"ChlList");//取数组
			HB_S32 iArrySize=cJSON_GetArraySize(pChlListArry);//数组大小
			cJSON *pChlListArryChild=pChlListArry->child;//子对象

			sqlite3_open(BOX_DATA_BASE_NAME, &db);
			for (i=0; i<iArrySize; i++) //如果添加的设备时NVR或DVR iArrySize会大于1
			{
				HB_CHAR *pPos = NULL;
				HB_CHAR cTmpBuf[512] = {0};
				memset(cSql, 0, sizeof(cSql));
				memset(stOnvifDevInfo.cRtspMainUrl, 0, sizeof(stOnvifDevInfo.cRtspMainUrl));
				memset(stOnvifDevInfo.cRtspSubUrl, 0, sizeof(stOnvifDevInfo.cRtspSubUrl));
				pItem = cJSON_GetObjectItem(pChlListArryChild,"RtspStreamMain");
//				strncpy(stOnvifDevInfo.cRtspMainUrl, pItem->valuestring, strlen(pItem->valuestring));
				strncpy(cTmpBuf, pItem->valuestring, strlen(pItem->valuestring));
				if ((cTmpBuf != NULL) && strlen(cTmpBuf)>0)
				{
					//rtsp://[ip]:[port]/url
					//rtsp://[ip]/url
					pPos = strstr(&cTmpBuf[7], ":"); //实际为strstr(cTmpBuf+strlen("rtsp://"), ":");
					if (pPos != NULL)
					{
						//设置rtsp端口
						pPos = pPos + 1;
						stOnvifDevInfo.iDevPort = atoi(pPos);
						pPos = strstr(pPos, "/");
					}
					else
					{
						//设置默认的rtsp端口
						stOnvifDevInfo.iDevPort = 554;
						pPos = strstr(&cTmpBuf[7], "/");//实际为strstr(cTmpBuf+strlen("rtsp://"), "/");
					}
					strncpy(stOnvifDevInfo.cRtspMainUrl, pPos, strlen(pPos));
				}


				memset(cTmpBuf, 0, sizeof(cTmpBuf));
				pItem = cJSON_GetObjectItem(pChlListArryChild,"RtspStreamSub");
				strncpy(cTmpBuf, pItem->valuestring, strlen(pItem->valuestring));
				if ((cTmpBuf != NULL) && strlen(cTmpBuf)>0)
				{
					//rtsp://[ip]:[port]/url
					//rtsp://[ip]/url
					pPos = strstr(&cTmpBuf[7], ":");//实际为strstr(cTmpBuf+strlen("rtsp://"), ":");
					if (pPos != NULL)
					{
						pPos = strstr(pPos, "/");
					}
					else
					{
						pPos = strstr(&cTmpBuf[7], "/");//实际为strstr(cTmpBuf+strlen("rtsp://"), "/");
					}
					strncpy(stOnvifDevInfo.cRtspSubUrl, pPos, strlen(pPos));
				}

				//插入数据库
				snprintf(cSql, sizeof(cSql), \
					"insert into rtsp_list (dev_id,dev_chnl_num,rtsp_main,rtsp_sub) values ('%s','%d','%s','%s')", \
					stOnvifDevInfo.cDevId, i, stOnvifDevInfo.cRtspMainUrl, stOnvifDevInfo.cRtspSubUrl);
				sqlite3_exec(db, cSql, NULL, NULL, &errmsg);
				pChlListArryChild = pChlListArryChild->next;
			}
			memset(cSql, 0, sizeof(cSql));
			snprintf(cSql, sizeof(cSql), \
				"insert into onvif_dev_data (dev_name,dev_ip,rtsp_port,onvif_service_port,dev_id,dev_chnl_num,dev_login_usr,dev_login_passwd,basic_authenticate,dev_state) "
				"values ('%s','%s','%d','%s','%s','%s','%s','%s','%s','0')", \
				stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.iDevPort, \
				cOnvifServicePort, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevChnls, \
				stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd, stOnvifDevInfo.cBasicAuth);
			sqlite3_exec(db, cSql, NULL, NULL, &errmsg);
			sqlite3_free(errmsg);
			sqlite3_close(db);

			get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
			url_decode(stOnvifDevInfo.cDevName, strlen(stOnvifDevInfo.cDevName));
			printf("{\"Result\":\"0\",\"DataIndex\":\"%s\",\"DevId\":\"%s-%s\",\"DevName\":\"%s\",\"DevIp\":\"%s\",\"DevChnls\":\"%s\"}", \
				cDataIndex, c_mac_sn, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.cDevChnls);
			WRITE_LOG("send to web:[{\"Result\":\"0\",\"DataIndex\":\"%s\",\"DevId\":\"%s-%s\",\"DevName\":\"%s\",\"DevIp\":\"%s\",\"DevChnls\":\"%s\"}]\n", \
				cDataIndex, c_mac_sn, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.cDevChnls);

			memset(stBufCmd.cBuffer, 0, sizeof(stBufCmd.cBuffer));
			snprintf(stBufCmd.cBuffer, sizeof(stBufCmd.cBuffer), "{\"TYPE\":\"RegisterOnvifDev\",\"DevId\":\"%s\"}", stOnvifDevInfo.cDevId);
			stBufCmd.iActLength = 5*sizeof(HB_S32) + strlen(stBufCmd.cBuffer);

			WRITE_LOG("!!!!!!!!!!!!!!!!!sendto box_main_ctrl:[%s]\n", stBufCmd.cBuffer);
			create_socket_connect_ipaddr(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
			send_data(&iSockFd, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, 2);
//			NetworkCommunicate(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, sizeof(stBufCmd.cBuffer), 10);
			close_sockfd(&iSockFd);
//			NetworkCommunicate(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, sizeof(stBufCmd.cBuffer), 0);
#endif
		}
	}

	return 0;
}

//删除一条RTSP设备
HB_S32 DelOneRtspDev(HB_CHAR *buf)
{
	HB_CHAR *pPos = NULL;
	HB_S32 ret = 0;
	HB_S32 sockfd = -1;
	HB_S32 iOffset = 0;
	HB_CHAR index[8] = {0};
	HB_CHAR c_mac_sn[32] = {0};
	HB_CHAR cDevId[128] = {0};
	GN_NET_LAYER_OBJ stBufCmd;

	memset(&stBufCmd, 0, sizeof(GN_NET_LAYER_OBJ));
	stBufCmd.iProtocolVer = 1;
	stBufCmd.iDataType = 9;
	stBufCmd.iBlockHeadFlag = 1;
	stBufCmd.iBlockEndFlag = 1;

	get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
	iOffset = strlen(c_mac_sn)+1;


	if((pPos = strstr(buf, "DataIndex"))) //删除设备
	{
		sscanf(pPos, "DataIndex=%[^&]&DevId=%s", index, cDevId);

		url_decode(cDevId, strlen(cDevId));
		sprintf(stBufCmd.cBuffer, "{\"TYPE\":\"DelOnvifDev\", \"DevId\":\"%s\"}", cDevId);

		snprintf(stBufCmd.cBuffer, sizeof(stBufCmd.cBuffer), "{\"TYPE\":\"DelOnvifDev\", \"DevId\":\"%s\"}", cDevId+iOffset);
		stBufCmd.iActLength = 5*sizeof(HB_S32) + strlen(stBufCmd.cBuffer);
		ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, sizeof(stBufCmd.cBuffer), 10);
		if (ret < 0)
		{
			switch(ret)
			{
				case -1:
					printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
					break;
				case -2:
					printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
					break;
				case -3:
					printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
					break;
				default:
					break;
			}
			return -2;
		}

		sscanf(stBufCmd.cBuffer,"<ReturnCode>%d</ReturnCode>",&ret);
		if (ret == 0)
		{
			printf("{\"Result\":\"0\",\"DevId\":\"%s\",\"DataIndex\":\"%s\"}", cDevId, index);
			WRITE_LOG("send to web:[{\"Result\":\"0\",\"DevId\":\"%s\",\"DataIndex\":\"%s\"}]\n", cDevId, index);
		}
		else
		{
			printf("{\"Result\":\"-230\",\"ErrMessage\":\"删除RTSP设备失败！-230\"}");
			WRITE_LOG("send to web:[{\"Result\":\"-230\",\"ErrMessage\":\"删除RTSP设备失败！-230\"}]\n");
		}
	}

	return 0;
}

//删除全部RTSP设备
HB_S32 DelAllRtspDev()
{
	HB_S32 ret = 0;
	HB_S32 sockfd = -1;
	HB_CHAR buffer[64] = {0};

	sprintf(buffer, "deleteall");
	ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, buffer, strlen(buffer), sizeof(buffer), 10);
	if (ret < 0)
	{
		switch(ret)
		{
			case -1:
				printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
				break;
			case -2:
				printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
				break;
			case -3:
				printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
				break;
			default:
				break;
		}
		return -2;
	}
	sscanf(buffer,"<ErrCode>%d</ErrCode>",&ret);
	switch(ret)
	{
		case 0:
			printf("{\"Result\":\"0\"}");
			break;
		case -1:
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-220\",\"ErrMessage\":\"删除UDP设备失败！-220\"}");
			break;
		case -2:
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-221\",\"ErrMessage\":\"删除UDP设备失败！-221\"}");
			break;
		default:
			//printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-222\",\"ErrMessage\":\"其他错误！-222\"}");
			break;
	}

	return 0;
}


//获取RTSP设备列表回调函数
static HB_S32 load_rtsp_list_port_page( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name)
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR cDevName[512] = {0};
	HB_CHAR cMacSn[32] = {0};
	HB_CHAR buf_var[1024] = {0};

	strncpy(cDevName, column_value[2], sizeof(cDevName));
	url_decode(cDevName, strlen(cDevName));

	get_sys_sn(cMacSn, sizeof(cMacSn));

    //获取盒子信息
	snprintf(buf_var, sizeof(buf_var), "{\"DevIp\":\"%s\",\"DevId\":\"%s-%s\",\"DevName\":\"%s\",\"DevChnls\":\"%s\"},",\
			column_value[0], cMacSn, column_value[1], cDevName, column_value[3]);
	strncat(dev_info_buf, buf_var, BUF_LEN_OF_JSON-strlen(dev_info_buf));
	return 0;
}

//获取RTSP设备列表(设备映射页面)
HB_S32 GetRtspDevPortList(HB_CHAR *packeting_str, HB_S32 size_packeting_str)
{
	HB_CHAR *sql = "select dev_ip,dev_id,dev_name,dev_chnl_num from onvif_dev_data";
	HB_CHAR c_device_info_buf[BUF_LEN_OF_JSON] = {0};
	HB_CHAR c_make_str[BUF_LEN_OF_JSON] = {0};

	SqlOperation(sql, BOX_DATA_BASE_NAME, load_rtsp_list_port_page, (void *)c_device_info_buf);
	if (c_device_info_buf[strlen(c_device_info_buf)-1] == ',')
	{
		c_device_info_buf[strlen(c_device_info_buf)-1] = '\0';
	}

	snprintf(c_make_str, sizeof(c_make_str), "\"OnvifDevInfo\":[%s]", c_device_info_buf);
	strncat(packeting_str, c_make_str, size_packeting_str);

	return 0;
}


//获取RTSP设备列表回调函数
static HB_S32 search_rtsp_dev( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;

	strncpy(dev_info_buf, column_value[0], 256);

	return 0;
}

//搜索rtsp设备（搜索onvif设备）
HB_S32 DiscoverRtspDev()
{
	HB_S32 iRet = 0;
	HB_S32 iSockFd = -1;
	GN_NET_LAYER_OBJ stBufCmd;

	memset(&stBufCmd, 0, sizeof(GN_NET_LAYER_OBJ));
	stBufCmd.iProtocolVer = 1;
	stBufCmd.iDataType = 9;
	stBufCmd.iBlockHeadFlag = 1;
	stBufCmd.iBlockEndFlag = 1;

	strncpy(stBufCmd.cBuffer, "<TYPE>DiscoverOnvifDev</TYPE>", sizeof(stBufCmd.cBuffer));
	stBufCmd.iActLength = 5*sizeof(HB_S32) + strlen(stBufCmd.cBuffer);
	iRet = NetworkCommunicate(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, sizeof(stBufCmd), 10);
	if (iRet < 0)
	{
		switch(iRet)
		{
			case -1:
				printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
				break;
			case -2:
				printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
				break;
			case -3:
				printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
				break;
			default:
				break;
		}
		return -2;
	}

	WRITE_LOG("recv from mainctrl: [%s]\n", stBufCmd.cBuffer);

	if(strstr(stBufCmd.cBuffer, "<ReturnCode>") != NULL)
	{
		printf("{\"Result\":\"-225\",\"ErrMessage\":\"未搜索到设备！-225\"}");
	}
	else
	{
		//找到了数据，解析json串, json串例子如下
		//{"OnvifServerAddr":["http://192.168.8.21:8888/onvif/device_service","http://192.168.8.64/onvif/device_service"]}
		HB_S32 i;
		HB_S32 iFindDevFlag = 0;
		cJSON *pJsonRoot = cJSON_Parse(stBufCmd.cBuffer); //json根
		cJSON *pTaskArry=cJSON_GetObjectItem(pJsonRoot,"OnvifServerAddr");//取数组
		HB_S32 iArrySize=cJSON_GetArraySize(pTaskArry);//数组大小
		cJSON *pTaskList=pTaskArry->child;//子对象
		HB_CHAR cOnvifServiceUrl[256] = {0};
		HB_CHAR cSendToWeb[BUF_LEN_OF_JSON] = {0};

		strncpy(cSendToWeb, "{\"Result\":\"0\",\"OnvifServerAddr\":[", sizeof(cSendToWeb));
		for (i=0;i<iArrySize;i++)
		{
			HB_CHAR cDevIp[16] = {0};
			HB_CHAR cSql[512] = {0};
			HB_CHAR cDevName[256] = {0};
			memset(cOnvifServiceUrl, 0, sizeof(cOnvifServiceUrl));
			snprintf(cOnvifServiceUrl, sizeof(cOnvifServiceUrl), "%s", cJSON_Print(pTaskList));
			HB_CHAR *pStart = strstr(cOnvifServiceUrl, "//") + 2;
			HB_CHAR *pEnd = strstr(pStart, ":");
			if (pEnd == NULL)
			{
				pEnd = strstr(pStart, "/");
			}
			WRITE_LOG("cDevIp:[%s]\n", cDevIp);
			strncpy(cDevIp, pStart, pEnd-pStart);
			snprintf(cSql, sizeof(cSql), "select dev_name from onvif_dev_data where dev_ip='%s'", cDevIp);
			WRITE_LOG("cSql:[%s]\n", cSql);
			SqlOperation(cSql, BOX_DATA_BASE_NAME, search_rtsp_dev, (void *)cDevName);
			if (strlen(cDevName) > 0)
			{
				pTaskList=pTaskList->next;
				//说明数据库中已经有此设备，不再需要在界面显示
				continue;
			}
			else
			{
				strncat(cSendToWeb, cOnvifServiceUrl, sizeof(cSendToWeb)-strlen(cSendToWeb));
				strncat(cSendToWeb, ",", sizeof(cSendToWeb)-strlen(cSendToWeb));
				iFindDevFlag = 1;
			}
			pTaskList=pTaskList->next;
		}

		if (iFindDevFlag)
		{
			//如果有未添加过的数据，需要去掉字符串中结尾的','字符
			cSendToWeb[strlen(cSendToWeb)-1] = '\0';
			strncat(cSendToWeb, "]}", sizeof(cSendToWeb)-strlen(cSendToWeb));
			printf("%s", cSendToWeb);
			WRITE_LOG("send to web:[%s]\n", cSendToWeb);
		}
		else
		{
			printf("{\"Result\":\"-227\",\"ErrMessage\":\"未搜索到新设备！-227\"}");
		}
	}

	return 0;
}


static HB_S32 get_onvif_list(HB_CHAR *buf, FIFO_HANDLE iRtspListInfo)
{
	sqlite3 *db;
	HB_CHAR *errmsg = NULL;
	HB_CHAR *pPos = NULL;
	HB_S32 i = 0;
	HB_S32 iRet = 0;
	HB_S32 iSockFd = -1;
	HB_CHAR cRetCode[8] = {0};
	HB_CHAR cDataIndex[8] = {0};
	HB_CHAR cOnvifServicePort[8] = {0}; //Onvif服务端口
	HB_CHAR cCalcBasicAuth[128] = {0}; //用于计算基本认证
	HB_CHAR cSql[512] = {0};
	ONVIF_DEV_INFO_OBJ stOnvifDevInfo;
	GN_NET_LAYER_OBJ stBufCmd;

	if (strlen(iRtspListInfo->cMsg) == 0)
	{
		return -1;
	}

	memset(&stOnvifDevInfo, 0, sizeof(ONVIF_DEV_INFO_OBJ));
	memset(&stBufCmd, 0, sizeof(GN_NET_LAYER_OBJ));
	stBufCmd.iProtocolVer = 1;
	stBufCmd.iDataType = 9;
	stBufCmd.iBlockHeadFlag = 1;
	stBufCmd.iBlockEndFlag = 1;

	if((pPos = strstr(buf, "DataIndex"))) //添加设备
	{
		sscanf(pPos, "DataIndex=%[^&]&DevName=%[^&]&DevLoginUser=%[^&]&DevLoginPasswd=%[^&]&DevIp=%[^&]&DevPort=%[^&]&DevServiceUrl=%s", \
			cDataIndex, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd, \
			stOnvifDevInfo.cDevIp, cOnvifServicePort, stOnvifDevInfo.cDevServiceUrl);

		url_decode(stOnvifDevInfo.cDevServiceUrl, strlen(stOnvifDevInfo.cDevServiceUrl));

		HB_CHAR c_mac_sn[32] = {0};
		//设备搜索成功，进行json串的解析以及插入数据库
		snprintf(cCalcBasicAuth, sizeof(cCalcBasicAuth), "%s:%s", stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd);
		base64_encode(stOnvifDevInfo.cBasicAuth, (HB_U8 *)cCalcBasicAuth, strlen(cCalcBasicAuth), sizeof(stOnvifDevInfo.cBasicAuth));

		cJSON *pItem = NULL;
		cJSON *pJsonRoot = cJSON_Parse(iRtspListInfo->cMsg); //json根
		pItem = cJSON_GetObjectItem(pJsonRoot,"OnvifDevId");
		strncpy(stOnvifDevInfo.cDevId, pItem->valuestring, strlen(pItem->valuestring));
		pItem = cJSON_GetObjectItem(pJsonRoot,"ChlNums");
		strncpy(stOnvifDevInfo.cDevChnls, pItem->valuestring, strlen(pItem->valuestring));
		cJSON *pChlListArry=cJSON_GetObjectItem(pJsonRoot,"ChlList");//取数组
		HB_S32 iArrySize=cJSON_GetArraySize(pChlListArry);//数组大小
		cJSON *pChlListArryChild=pChlListArry->child;//子对象

		sqlite3_open(BOX_DATA_BASE_NAME, &db);
		for (i=0; i<iArrySize; i++) //如果添加的设备时NVR或DVR iArrySize会大于1
		{
			HB_CHAR *pPos = NULL;
			HB_CHAR cTmpBuf[512] = {0};
			memset(cSql, 0, sizeof(cSql));
			memset(stOnvifDevInfo.cRtspMainUrl, 0, sizeof(stOnvifDevInfo.cRtspMainUrl));
			memset(stOnvifDevInfo.cRtspSubUrl, 0, sizeof(stOnvifDevInfo.cRtspSubUrl));
			pItem = cJSON_GetObjectItem(pChlListArryChild,"RtspStreamMain");
//				strncpy(stOnvifDevInfo.cRtspMainUrl, pItem->valuestring, strlen(pItem->valuestring));
			strncpy(cTmpBuf, pItem->valuestring, strlen(pItem->valuestring));
			if ((cTmpBuf != NULL) && strlen(cTmpBuf)>0)
			{
				//rtsp://[ip]:[port]/url
				//rtsp://[ip]/url
				pPos = strstr(&cTmpBuf[7], ":"); //实际为strstr(cTmpBuf+strlen("rtsp://"), ":");
				if (pPos != NULL)
				{
					//设置rtsp端口
					pPos = pPos + 1;
					stOnvifDevInfo.iDevPort = atoi(pPos);
					pPos = strstr(pPos, "/");
				}
				else
				{
					//设置默认的rtsp端口
					stOnvifDevInfo.iDevPort = 554;
					pPos = strstr(&cTmpBuf[7], "/");//实际为strstr(cTmpBuf+strlen("rtsp://"), "/");
				}
				strncpy(stOnvifDevInfo.cRtspMainUrl, pPos, strlen(pPos));
			}

			memset(cTmpBuf, 0, sizeof(cTmpBuf));
			pItem = cJSON_GetObjectItem(pChlListArryChild,"RtspStreamSub");
			strncpy(cTmpBuf, pItem->valuestring, strlen(pItem->valuestring));
			if ((cTmpBuf != NULL) && strlen(cTmpBuf)>0)
			{
				//rtsp://[ip]:[port]/url
				//rtsp://[ip]/url
				pPos = strstr(&cTmpBuf[7], ":");//实际为strstr(cTmpBuf+strlen("rtsp://"), ":");
				if (pPos != NULL)
				{
					pPos = strstr(pPos, "/");
				}
				else
				{
					pPos = strstr(&cTmpBuf[7], "/");//实际为strstr(cTmpBuf+strlen("rtsp://"), "/");
				}
				strncpy(stOnvifDevInfo.cRtspSubUrl, pPos, strlen(pPos));
			}

			//插入数据库
			snprintf(cSql, sizeof(cSql), \
				"insert into rtsp_list (dev_id,dev_chnl_num,rtsp_main,rtsp_sub) values ('%s','%d','%s','%s')", \
				stOnvifDevInfo.cDevId, i, stOnvifDevInfo.cRtspMainUrl, stOnvifDevInfo.cRtspSubUrl);
			sqlite3_exec(db, cSql, NULL, NULL, &errmsg);
			pChlListArryChild = pChlListArryChild->next;
		}
		cJSON_Delete(pJsonRoot);
		memset(cSql, 0, sizeof(cSql));
		snprintf(cSql, sizeof(cSql), \
			"insert into onvif_dev_data (dev_name,dev_ip,rtsp_port,onvif_service_port,dev_id,dev_chnl_num,dev_login_usr,dev_login_passwd,basic_authenticate,dev_state) "
			"values ('%s','%s','%d','%s','%s','%s','%s','%s','%s','0')", \
			stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.iDevPort, \
			cOnvifServicePort, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevChnls, \
			stOnvifDevInfo.cDevLoginUsr, stOnvifDevInfo.cDevLoginPasswd, stOnvifDevInfo.cBasicAuth);
		sqlite3_exec(db, cSql, NULL, NULL, &errmsg);
		sqlite3_free(errmsg);
		sqlite3_close(db);

		get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
		url_decode(stOnvifDevInfo.cDevName, strlen(stOnvifDevInfo.cDevName));
		printf("{\"Result\":\"0\",\"DataIndex\":\"%s\",\"DevId\":\"%s-%s\",\"DevName\":\"%s\",\"DevIp\":\"%s\",\"DevChnls\":\"%s\"}", \
			cDataIndex, c_mac_sn, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.cDevChnls);
		WRITE_LOG("send to web:[{\"Result\":\"0\",\"DataIndex\":\"%s\",\"DevId\":\"%s-%s\",\"DevName\":\"%s\",\"DevIp\":\"%s\",\"DevChnls\":\"%s\"}]\n", \
			cDataIndex, c_mac_sn, stOnvifDevInfo.cDevId, stOnvifDevInfo.cDevName, stOnvifDevInfo.cDevIp, stOnvifDevInfo.cDevChnls);

		memset(stBufCmd.cBuffer, 0, sizeof(stBufCmd.cBuffer));
		snprintf(stBufCmd.cBuffer, sizeof(stBufCmd.cBuffer), "{\"TYPE\":\"RegisterOnvifDev\",\"DevId\":\"%s\"}", stOnvifDevInfo.cDevId);
		stBufCmd.iActLength = 5*sizeof(HB_S32) + strlen(stBufCmd.cBuffer);

		WRITE_LOG("!!!!!!!!!!!!!!!!!sendto box_main_ctrl:[%s]\n", stBufCmd.cBuffer);
		create_socket_connect_ipaddr(&iSockFd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
		send_data(&iSockFd, (HB_CHAR *)&stBufCmd, stBufCmd.iActLength, 2);
		close_sockfd(&iSockFd);
	}

	return 0;
}

//获取当前添加onvif的进度
HB_S32 get_add_rtsp_present(HB_CHAR *buf)
{
	HB_S32	iPercent = 0;
	HB_S32	iPercent2 = 0;
	static HB_S32 iCount = 0; //由于采集端有采集延时，此值用于记录获取的次数，若总是获取不到，则报错返回
	static HB_S32 iSamePercent = 0; //用于记录多次采集到的百分比的值是否一样，若总是一样说明采集端出了问题，需要报错返回
	HB_S32 shmid;//共享内存标识符
	HB_VOID *shm = NULL;
	FIFO_HANDLE fifo_main = NULL;

	sleep(1);

	//创建共享内存
	shmid = shmget((key_t)1234, sizeof(FIFO_OBJ), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		printf("{\"Result\":\"-234\",\"ErrMessage\":\"百分比获取失败！-234\"}\n");
		WRITE_LOG("{\"Result\":\"-234\",\"ErrMessage\":\"百分比获取失败！-234\"}\n");
		return -1;
	}
    //将共享内存连接到当前进程的地址空间
	shm = (FIFO_HANDLE)shmat(shmid, 0, 0);
    if(shm == (HB_VOID*)-1)
    {
		printf("{\"Result\":\"-23411\",\"ErrMessage\":\"百分比获取失败！-23411\"}\n");
		WRITE_LOG("{\"Result\":\"-23411\",\"ErrMessage\":\"百分比获取失败！-23411\"}\n");
		return -1;
    }
    fifo_main = (FIFO_HANDLE)shm;
	if (fifo_main->iTotalChnlCount > 0)
	{
		if (fifo_main->iCurChnlNum < 0)
		{
			printf("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
			WRITE_LOG("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
		    shmdt(shm);//把共享内存从当前进程中分离
		    shmctl(shmid, IPC_RMID, 0);//删除共享内存
			return -2;
		}
		iPercent = fifo_main->iCurChnlNum * 100 / fifo_main->iTotalChnlCount;
		if (iPercent == 100)
		{
			if (strlen(fifo_main->cMsg) == 0)
			{
				printf("{\"Result\":\"-232\",\"ErrMessage\":\"rtsp列表为空！-232\"}");
				WRITE_LOG("{\"Result\":\"-232\",\"ErrMessage\":\"rtsp列表为空！-232\"}");
			    shmdt(shm);//把共享内存从当前进程中分离
			    shmctl(shmid, IPC_RMID, 0);//删除共享内存
				return -2;
			}
			get_onvif_list(buf, fifo_main);
			shmdt(shm);//把共享内存从当前进程中分离
			shmctl(shmid, IPC_RMID, 0);//删除共享内存
			return 0;
		}
	}


	while(1)
	{
		sleep(1);
		if (fifo_main->iTotalChnlCount > 0)
		{
			if (fifo_main->iCurChnlNum < 0)
			{
				printf("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
				WRITE_LOG("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
			    shmdt(shm);//把共享内存从当前进程中分离
			    shmctl(shmid, IPC_RMID, 0);//删除共享内存
				return -2;
			}
			iPercent2 = fifo_main->iCurChnlNum * 100 / fifo_main->iTotalChnlCount;
			if (iPercent2 == iPercent)
			{
				iSamePercent++;
				if(iSamePercent > 5)
				{
					printf("{\"Result\":\"-235\",\"ErrMessage\":\"百分比获取失败！-235\"}\n");
					WRITE_LOG("{\"Result\":\"-235\",\"ErrMessage\":\"百分比获取失败！-235\"}\n");
					shmdt(shm);//把共享内存从当前进程中分离
					shmctl(shmid, IPC_RMID, 0);//删除共享内存
					return -1;
				}
				WRITE_LOG("iSamePercent=%d, iVideoSourcesNums=%d, iTotalChnlCount=%d, iCurChnlNum=%d\n", \
						iSamePercent, fifo_main->iVideoSourcesNums, fifo_main->iTotalChnlCount, fifo_main->iCurChnlNum);
				continue;
			}
		}
		else
		{
			if (fifo_main->iCurChnlNum < 0)
			{
				printf("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
				WRITE_LOG("{\"Result\":\"-237\",\"ErrMessage\":\"%s！-237\"}", fifo_main->cMsg);
			    shmdt(shm);//把共享内存从当前进程中分离
			    shmctl(shmid, IPC_RMID, 0);//删除共享内存
				return -2;
			}

			if (++iCount > 5)
			{
				printf("{\"Result\":\"-236\",\"ErrMessage\":\"百分比获取失败！-236\"}\n");
				WRITE_LOG("{\"Result\":\"-236\",\"ErrMessage\":\"百分比获取失败！-236\"}\n");
				shmdt(shm);//把共享内存从当前进程中分离
				shmctl(shmid, IPC_RMID, 0);//删除共享内存
				return -1;
			}
			WRITE_LOG("iCount=%d, iVideoSourcesNums=%d, iTotalChnlCount=%d, iCurChnlNum=%d\n", \
					iCount, fifo_main->iVideoSourcesNums, fifo_main->iTotalChnlCount, fifo_main->iCurChnlNum);
			continue;
		}

		if (iPercent2 == 100)
		{
			if (strlen(fifo_main->cMsg) == 0)
			{
				printf("{\"Result\":\"-232\",\"ErrMessage\":\"rtsp列表为空！-232\"}");
				WRITE_LOG("{\"Result\":\"-232\",\"ErrMessage\":\"rtsp列表为空！-232\"}");
				shmdt(shm);//把共享内存从当前进程中分离
				shmctl(shmid, IPC_RMID, 0);//删除共享内存
				return -2;
			}
			get_onvif_list(buf, fifo_main);
			shmdt(shm);//把共享内存从当前进程中分离
			shmctl(shmid, IPC_RMID, 0);//删除共享内存
			break;
		}
		else
		{
			printf("{\"Result\":\"1\",\"Percent\":\"%d\"}", iPercent);
			WRITE_LOG("{\"Result\":\"1\",\"Percent\":\"%d\"}", iPercent);
			shmdt(shm);//把共享内存从当前进程中分离
			break;
		}
	}

	return 0;
}
/************************RTSP设备操作END************************/
/************************RTSP设备操作END************************/
/************************RTSP设备操作END************************/

/************************TCP设备操作************************/
//读取1行Tcp数据回调函数
static HB_S32 LoadDeviceInfoOneLineTCP( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR buf_var[1024] = {0};
	HB_CHAR c_tcp_dev_name[128] = {0};

	strncpy(c_tcp_dev_name, column_value[2], sizeof(c_tcp_dev_name));

	url_decode(c_tcp_dev_name, strlen(c_tcp_dev_name));
    //获取盒子信息
	snprintf(buf_var, sizeof(buf_var), "\"TcpDevIp\":\"%s\",\"TcpDevPort\":\"%s\",\"TcpDevName\":\"%s\",\"TcpBoxPort\":\"%s\"",\
			column_value[0], column_value[1], c_tcp_dev_name, column_value[3]);
	strncat(dev_info_buf, buf_var, 1024);

	return 0;
}

//添加一条Tcp设备
HB_S32 AddTcpDev(HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 ret = -111;
	HB_S32 sockfd = -1;
	HB_CHAR c_dev_num[8] = {0}; //用于记录数据库中数据的条数
	HB_CHAR c_tcp_dev_num[8] = {0};

	HB_CHAR	c_tcp_dev_ip[16] = {0};
	HB_CHAR	c_tcp_dev_port[8] = {0};
	HB_CHAR	c_tcp_dev_name[128] = {0};
	HB_CHAR	c_tcp_box_port[8] = {0};

	HB_CHAR sql[512] = {0};
	HB_CHAR c_device_info_buf[1024] = {0};
	GN_NET_LAYER_OBJ buf_cmd;

	if((pos = strstr(buf, "TcpDevIp"))) //添加设备
	{
		memset(sql, 0, sizeof(sql));
//		snprintf(sql, sizeof(sql), "select count (*) as num from tcp_dev_data");
		snprintf(sql, sizeof(sql), "SELECT COUNT(DISTINCT tcp_dev_ip) FROM tcp_dev_data");
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceListNum, (void *)c_tcp_dev_num);
		WRITE_LOG("[%d] devices in database!\n", atoi(c_tcp_dev_num));

		memset(sql, 0, sizeof(sql));
		snprintf(sql, sizeof(sql), "select count (*) as num from dev_add_web_data");
		SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceListNum, (void *)c_dev_num);
		WRITE_LOG("[%d] devices in database!\n", atoi(c_dev_num));

		if ((atoi(c_dev_num)+atoi(c_tcp_dev_num)) >= MAX_DEV_NUM)
		{
			printf("{\"Result\":\"-207\",\"ErrMessage\":\"挂载的设备数量达到上限！-207\"}");
			return -1;
		}

		sscanf(pos, "TcpDevIp=%[^&]&TcpDevPort=%[^&]&TcpDevName=%[^&]&TcpBoxPort=%s", \
				c_tcp_dev_ip, c_tcp_dev_port, c_tcp_dev_name, c_tcp_box_port);

		memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
		buf_cmd.iProtocolVer = 1;
		buf_cmd.iDataType = 9;
		buf_cmd.iBlockHeadFlag = 1;
		buf_cmd.iBlockEndFlag = 1;

		sprintf(buf_cmd.cBuffer, "<TYPE>AddTcpDevOnWeb</TYPE><TcpDevIp>%s</TcpDevIp><TcpDevPort>%s</TcpDevPort><TcpDevName>%s</TcpDevName><TcpBoxPort>%s</TcpBoxPort>",
				c_tcp_dev_ip, c_tcp_dev_port, c_tcp_dev_name, c_tcp_box_port);
		buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);
		ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&buf_cmd, buf_cmd.iActLength, sizeof(buf_cmd), 10);
		if (ret < 0)
		{
			switch(ret)
			{
				case -1:
					printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
					break;
				case -2:
					printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
					break;
				case -3:
					printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
					break;
				default:
					break;
			}
			return -2;
		}

		if (strlen(buf_cmd.cBuffer) <= 0)
		{
			printf("{\"Result\":\"-199\",\"ErrMessage\":\"从主控接收数据有误！-199\"}");
			return -1;
		}


		sscanf(buf_cmd.cBuffer,"<ReturnCode>%d</ReturnCode>", &ret);
		WRITE_LOG("recv from main_ctrl [%s] ret:[%d]\n", buf_cmd.cBuffer, ret);
		switch(ret)
		{
			case 0://成功
				//添加成功发送表单
				snprintf(sql, sizeof(sql), "select tcp_dev_ip,tcp_dev_port,tcp_dev_name,tcp_box_port from tcp_dev_data where tcp_box_port = \"%s\"", \
										c_tcp_box_port);
				SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceInfoOneLineTCP, (void *)c_device_info_buf);
				printf("{\"Result\":\"0\",%s}", c_device_info_buf);
				break;
			case -1:
				//printf("Content type: application/json \n\n");
				printf("{\"Result\":\"-203\",\"ErrMessage\":\"设置的映射端口值非法！-203\"}");
				break;
			case -2:
				printf("{\"Result\":\"-204\",\"ErrMessage\":\"添加的设备达到上限！-204\"}");
				break;
			case -3:
				printf("{\"Result\":\"-205\",\"ErrMessage\":\"映射端口已被占用!-205\"}");
				break;
			case -4:
				printf("{\"Result\":\"-206\",\"ErrMessage\":\"数据库错误！-206\"}");
				break;
			default :
				printf("{\"Result\":\"-199\",\"ErrMessage\":\"从主控接收数据有误！-199\"}");
				break;
#if 0
				//打开数据库
				ret = sqlite3_open(BOX_DATA_BASE_NAME, &db);
				snprintf(sql, sizeof(sql), "select tcp_dev_ip,tcp_dev_port,tcp_dev_name,tcp_box_port from tcp_dev_data where tcp_box_port = \"%s\"", \
						c_tcp_box_port);
				ret = sqlite3_exec(db, sql, LoadDeviceInfoOneLineTCP, (void *)c_device_info_buf, &errmsg);
				//printf("err= %s\n", errmsg);
				sqlite3_free(errmsg);
				sqlite3_close(db);
#endif
		}
	}

	return 0;
}


//删除一条Tcp设备
HB_S32 DelOneTcpDev(HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 ret;
	HB_S32 sockfd = -1;
	HB_CHAR index[8] = {0};
	HB_CHAR c_box_port[8] = {0};
	GN_NET_LAYER_OBJ buf_cmd;
	if((pos = strstr(buf, "DataIndex"))) //删除设备
	{
		sscanf(pos, "DataIndex=%[^&]&TcpBoxPort=%s", index, c_box_port);

		memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
		buf_cmd.iProtocolVer = 1;
		buf_cmd.iDataType = 9;
		buf_cmd.iBlockHeadFlag = 1;
		buf_cmd.iBlockEndFlag = 1;
		sprintf(buf_cmd.cBuffer, "<TYPE>DelTcpDevOnWeb</TYPE><TcpBoxPort>%s</TcpBoxPort>", c_box_port);
		buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);

		ret = NetworkCommunicate(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, (HB_CHAR *)&buf_cmd, buf_cmd.iActLength, sizeof(buf_cmd), 10);
		if (ret < 0)
		{
			switch(ret)
			{
				case -1:
					printf("{\"Result\":\"-200\",\"ErrMessage\":\"连接主控失败！-200\"}");
					break;
				case -2:
					printf("{\"Result\":\"-201\",\"ErrMessage\":\"发送数据到主控超时！-201\"}");
					break;
				case -3:
					printf("{\"Result\":\"-202\",\"ErrMessage\":\"等待主程序回应超时！-202\"}");
					break;
				default:
					break;
			}
			return -2;
		}
		sscanf(buf_cmd.cBuffer,"<ReturnCode>%d</ReturnCode>",&ret);
		if (ret == 0)
		{
			printf("{\"Result\":\"0\",\"TcpBoxPort\":\"%s\",\"DataIndex\":\"%s\"}", c_box_port, index);
		}
		else
		{
			printf("{\"Result\":\"-208\",\"ErrMessage\":\"删除映射失败！-208\"}");
		}
	}
	return 0;
}


//删除所有Tcp设备
HB_S32 DelAllTcpDev()
{

	return 0;
}



//获取tcp设备列表回调函数
static HB_S32 LoadDeviceInfoTcp( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *dev_info_buf = (HB_CHAR *)para;
	HB_CHAR buf_var[1024] = {0};
	HB_CHAR c_tcp_dev_name[128] = {0};

	strncpy(c_tcp_dev_name, column_value[2], sizeof(c_tcp_dev_name));
	url_decode(c_tcp_dev_name, strlen(c_tcp_dev_name));

    //获取盒子信息
	snprintf(buf_var, sizeof(buf_var), "{\"TcpDevIp\":\"%s\",\"TcpDevPort\":\"%s\",\"TcpDevName\":\"%s\",\"TcpBoxPort\":\"%s\"},",\
			column_value[0], column_value[1], c_tcp_dev_name, column_value[3]);
	strncat(dev_info_buf, buf_var, BUF_LEN_OF_JSON-strlen(dev_info_buf));
	return 0;
}

//获取Tcp设备列表
HB_S32 GetTcpDevList(HB_CHAR *packeting_str, HB_S32 size_packeting_str)
{
	HB_CHAR *sql = "select tcp_dev_ip,tcp_dev_port,tcp_dev_name,tcp_box_port from tcp_dev_data";
	HB_CHAR c_device_info_buf[BUF_LEN_OF_JSON] = {0};
	HB_CHAR c_make_str[BUF_LEN_OF_JSON] = {0};

	SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceInfoTcp, (void *)c_device_info_buf);
	if (c_device_info_buf[strlen(c_device_info_buf)-1] == ',')
	{
		c_device_info_buf[strlen(c_device_info_buf)-1] = '\0';
	}
	//printf("Content type: application/json \n\n");
	//printf("{\"Result\":\"0\",\"TCPDeviceInfo\":[%s]}", c_device_info_buf);
	snprintf(c_make_str, sizeof(c_make_str), "\"TCPDeviceInfo\":[%s],", c_device_info_buf);
	strncat(packeting_str, c_make_str, size_packeting_str);

	return 0;
}
/************************TCP设备操作END************************/
