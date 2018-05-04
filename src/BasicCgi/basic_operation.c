#include "my_include.h"
#include "cgi.h"
#include "session.h"
#include "basic.h"
#include "sqlite3.h"
#include "connect_domain_test.h"
#include "common.h"
#include "basic_operation.h"

//获取网卡序列号
//mac_sn 网卡序列号, dev 网卡名
static HB_S32 get_mac_dev(HB_CHAR *mac_sn, HB_CHAR *dev)
{
	struct ifreq tmp;
	HB_S32 sock_mac;
	// HB_CHAR *tmpflag;
	//HB_CHAR mac_addr[30];
	sock_mac = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_mac == -1)
	{
		return -1;
	}
	memset(&tmp, 0, sizeof(tmp));
	strncpy(tmp.ifr_name, dev, sizeof(tmp.ifr_name) - 1);
	if ((ioctl(sock_mac, SIOCGIFHWADDR, &tmp)) < 0)
	{
		close(sock_mac);
		return -1;
	}

	close(sock_mac);
	memcpy(mac_sn, tmp.ifr_hwaddr.sa_data, 6);
	return 0;
}

//获取网卡序列号
static HB_S32 get_sys_sn(HB_CHAR *sn, HB_S32 sn_size)
{
	HB_U64 sn_num = 0;
	HB_CHAR sn_mac[32] = { 0 };
	HB_CHAR mac[32] = { 0 };
	get_mac_dev(mac, ETH_X);
	sprintf(sn_mac, "0x%02x%02x%02x%02x%02x%02x", (HB_U8) mac[0],
					(HB_U8) mac[1], (HB_U8) mac[2], (HB_U8) mac[3],
					(HB_U8) mac[4], (HB_U8) mac[5]);
	sn_num = strtoull(sn_mac, 0, 16);
	snprintf(sn, sn_size, "%llu", sn_num);

	return 0;
#if 0
	HB_CHAR mac[32] =
	{	0};
	get_mac_dev(mac, ETH_X);

	sprintf(sn, "%02x%02x%02x%02x%02x%02x",
					(HB_U8)mac[0],
					(HB_U8)mac[1],
					(HB_U8)mac[2],
					(HB_U8)mac[3],
					(HB_U8)mac[4],
					(HB_U8)mac[5]
	);
	return 0;
#endif
}

static HB_S32 get_ip_dev(HB_CHAR *eth, HB_CHAR *ipaddr)
{
#if 0
	struct sockaddr_in servAddr;
	/* open socket */
	int handle;
	if((handle = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	return -1;
	//gethostbyname by getaddrinfo replacement
	struct addrinfo hints;
	memset(hints, 0, sizeof(hints));
	hints.ai_flags = AI_ALL;
	hints.ai_family = PF_INET;
	hints.ai_protocol = AF_UNSPEC;
	struct addrinfo* pResult = NULL;
	int errcode = getaddrinfo(ipaddr, NULL, &hints, &pResult);
	if(errcode != 0)
	return -2;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_addr = ((struct sockaddr_in*)pResult->ai_addr)->sin_addr;
	inet_ntoa(((struct sockaddr_in*)pResult->ai_addr)->sin_addr);
	return 0;
#endif
#if 1
	struct ifreq req;
	HB_S32 sock;
	HB_CHAR *temp_ip = NULL;
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
	{
		return -1;
	}
	strncpy(req.ifr_name, eth, IFNAMSIZ);
	if (ioctl(sock, SIOCGIFADDR, &req) < 0)
	{
		close(sock);
		fprintf(stderr, "ioctl error: %s\n", strerror(errno));
		return -1;
	}
	//temp_ip = inet_ntoa(*(struct in_addr *)&((struct sockaddr_in *)&req.ifr_addr)->sin_addr);
	temp_ip = inet_ntoa( (struct in_addr) ((struct sockaddr_in *)&req.ifr_addr)->sin_addr);
	//temp_ip = inet_ntoa(((struct sockaddr_in*)&(req.ifr_addr))->sin_addr);
	strcpy(ipaddr, temp_ip);
	close(sock);
	return HB_SUCCESS;
#endif
}

static HB_S32 SQLGetMachineCode(HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name)
{
	HB_CHAR *p_machine_code = (HB_CHAR *) para;

	if ((column_value[0] == NULL) || strlen(column_value[0]) < 10)
	{
		strncpy(p_machine_code, "0000000000", sizeof("0000000000"));
		WRITE_LOG("use default machine_code:[%s]\n", p_machine_code);
	}
	else
	{
		strncpy(p_machine_code, column_value[0], strlen(column_value[0]));
		WRITE_LOG("machine_code:[%s]\n", p_machine_code);
	}
	return 0;
}

HB_VOID *GetNetStatus(HB_VOID *arg, HB_S32 i_data_size)
{
	HB_CHAR *c_data = (HB_CHAR *) arg;
	HB_CHAR *sql = NULL;
	HB_S32 i_connect_status = 0;
	HB_CHAR c_gnlan_ipaddr[24] = { 0 };
	HB_CHAR c_mac_sn[32] = { 0 };
	HB_CHAR c_machine_code[32] = { 0 };

	//测试网络连接
	if (connect_test("www.baidu.com", 80, 2) == 1) //网络连接正常
	{
		i_connect_status = 1;
	}
	else //异常
	{
		if (connect_test("www.taobao.com", 80, 2) == 1) //网络连接正常
		{
			i_connect_status = 1;
		}
	}

	//测试天联连接状况
	if (0 != get_ip_dev("gnLan", c_gnlan_ipaddr))
	{
		//连接失败,将天联ip置为0.0.0.0
		strncpy(c_gnlan_ipaddr, "0.0.0.0", sizeof(c_gnlan_ipaddr));
	}
	get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
	sql = "select machine_code from system_web_data";
	SqlOperation(sql, BOX_DATA_BASE_NAME, SQLGetMachineCode, (HB_VOID *) c_machine_code);
	if ((strlen(c_machine_code) < 1) || (strlen(c_machine_code) > 32))
	{
		strncpy(c_machine_code, "0000000000", sizeof("0000000000"));
	}

	sprintf(c_data,"\"Status\":\"%d\",\"GlanIp\":\"%s\",\"MAC\":\"%s\",\"MachineCode\":\"%s\"",
					i_connect_status, c_gnlan_ipaddr, c_mac_sn, c_machine_code);
	return 0;
}

/*******************************************************************************
 *********************************一点通盒子设备列表********************************
 *******************************************************************************/

//获取一点通盒子列表回调函数
static HB_S32 LoadDeviceInfo(HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name)
{
	BOX_PAIR_HEAD_HANDLE box_info = (BOX_PAIR_HEAD_HANDLE) para;

	box_info->dev_num += 1;

	//获取盒子信息
	BOX_PAIR_DEV_LIST_HANDLE item;
	item = (BOX_PAIR_DEV_LIST_HANDLE) malloc(sizeof(BOX_PAIR_DEV_LIST_OBJ));
	memset(item, 0, sizeof(BOX_PAIR_DEV_LIST_OBJ));

	item->if_online = 0;
	strncpy(item->c_dev_id, column_value[0], strlen(column_value[0]));
	strncpy(item->c_dev_type, column_value[1], strlen(column_value[1]));
	strncpy(item->c_dev_name, column_value[2], strlen(column_value[2]));
	strncpy(item->c_dev_login_name, column_value[3], strlen(column_value[3]));
	strncpy(item->c_dev_login_passwd, column_value[4], strlen(column_value[4]));
	strncpy(item->c_dev_ip, column_value[5], strlen(column_value[5]));
	item->i_dev_port1 = atoi(column_value[6]);
	item->i_box_port1 = atoi(column_value[7]);
	item->i_dev_channel = atoi(column_value[8]);
	item->i_dev_status = atoi(column_value[9]);
	item->i_dev_port2 = atoi(column_value[10]);
	item->i_box_port2 = atoi(column_value[11]);

	if (item->i_dev_port2 > 0)
	{
		item->i_port_num = 2;
	}
	else
	{
		item->i_port_num = 1;
	}
	item->next = box_info->dev_list_head->next;

	box_info->dev_list_head->next = item;
	return 0;
}

//测试连接状态线程
static HB_VOID *
connect_ip_test(void *param)
{
	HB_S32 ret = 0;
	BOX_PAIR_DEV_LIST_HANDLE map_dev = (BOX_PAIR_DEV_LIST_HANDLE) param;
	ret = connect_ip_port_test(map_dev->c_dev_ip, map_dev->i_dev_port1, 2);
	if (1 == ret)
	{
		map_dev->if_online = 1; //能联通，表示在线
	}
	else
	{
		map_dev->if_online = 0; //不能联通，表示不在线
	}

	if (map_dev->i_dev_port2 > 0)
	{
		ret = connect_ip_port_test(map_dev->c_dev_ip, map_dev->i_dev_port2, 1);
		if (1 == ret)
		{
			map_dev->if_online2 = 1; //能联通，表示在线
		}
		else
		{
			map_dev->if_online2 = 0; //不能联通，表示不在线
		}
	}

	return NULL;
}

HB_S32 GetBasicBoxDevList(BOX_PAIR_HEAD_HANDLE box_dev)
{
	HB_CHAR *sql = "select dev_id,factory_list_data.factory_name,dev_name,dev_login_usr,dev_login_pwd,dev_ip,dev_port,box_port,dev_chns,dev_state,dev_port2,box_port2 from dev_add_web_data left join factory_list_data on dev_add_web_data.dev_type=factory_list_data.factory_code";
	BOX_PAIR_DEV_LIST_HANDLE box_dev_flag = NULL;
	HB_S32 ret = 0;

	SqlOperation(sql, BOX_DATA_BASE_NAME, LoadDeviceInfo, (void *) box_dev);
	box_dev_flag = box_dev->dev_list_head->next;
	while (box_dev_flag)
	{
		pthread_attr_t attr;
		pthread_t connect_test_pthread_id;
		ret = pthread_attr_init(&attr);
		ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		ret = pthread_create(&connect_test_pthread_id, &attr, connect_ip_test, box_dev_flag);
		pthread_attr_destroy(&attr);
		box_dev_flag = box_dev_flag->next;
	}

	return 0;
}

HB_S32 MakeJsonBox(HB_CHAR *box_list_buf, HB_S32 box_list_buf_len, BOX_PAIR_DEV_LIST_HANDLE box_dev_flag)
{
	HB_CHAR buf_var[1024] = { '\0' };
	HB_CHAR c_mac_sn[32] = { 0 };

	get_sys_sn(c_mac_sn, sizeof(c_mac_sn));
	while (box_dev_flag)
	{
		if ((2 == box_dev_flag->i_dev_status) || (3 == box_dev_flag->i_dev_status)) //待删除状态不在前端显示
		{
			box_dev_flag = box_dev_flag->next;
			continue;
		}

		url_decode(box_dev_flag->c_dev_name, strlen(box_dev_flag->c_dev_name));
		url_decode(box_dev_flag->c_dev_type, strlen(box_dev_flag->c_dev_type));

		if (box_dev_flag->i_port_num == 1)
		{
			snprintf(buf_var, sizeof(buf_var),
							"{ \"DevPortNum\": \"%d\", \"DevType\": \"%s\", \"DevName\": \"%s\", \"DevID\": \"%s-%s\" ,\"DevIP\":\"%s\", \"DevTnum\":\"%d\", \"DevLname\":\"%s\", \"DevPort1\":\"%d\", \"DevStatus1\":\"%d\", \"DevPort2\":\"%d\", \"DevStatus2\":\"0\", \"DevMapPort1\":\"%d\" ,\"DevSign\":\"%d\"},",
							box_dev_flag->i_port_num, box_dev_flag->c_dev_type,
							box_dev_flag->c_dev_name, c_mac_sn,
							box_dev_flag->c_dev_id, box_dev_flag->c_dev_ip,
							box_dev_flag->i_dev_channel,
							box_dev_flag->c_dev_login_name,
							box_dev_flag->i_dev_port1, box_dev_flag->if_online,
							box_dev_flag->i_dev_port2,
							box_dev_flag->i_box_port1,
							box_dev_flag->i_dev_status);

		}
		else
		{
			snprintf(buf_var, sizeof(buf_var),
							"{ \"DevPortNum\": \"%d\", \"DevType\": \"%s\", \"DevName\": \"%s\", \"DevID\": \"%s-%s\" ,\"DevIP\":\"%s\", \"DevTnum\":\"%d\", \"DevLname\":\"%s\", \"DevPort1\":\"%d\", \"DevStatus1\":\"%d\", \"DevPort2\":\"%d\", \"DevStatus2\":\"%d\", \"DevMapPort1\":\"%d\", \"DevMapPort2\":\"%d\", \"DevSign\":\"%d\"},",
							box_dev_flag->i_port_num, box_dev_flag->c_dev_type,
							box_dev_flag->c_dev_name, c_mac_sn,
							box_dev_flag->c_dev_id, box_dev_flag->c_dev_ip,
							box_dev_flag->i_dev_channel,
							box_dev_flag->c_dev_login_name,
							box_dev_flag->i_dev_port1, box_dev_flag->if_online,
							box_dev_flag->i_dev_port2, box_dev_flag->if_online2,
							box_dev_flag->i_box_port1,
							box_dev_flag->i_box_port2,
							box_dev_flag->i_dev_status);
		}
		strncat(box_list_buf, buf_var, box_list_buf_len - strlen(box_list_buf));

		box_dev_flag = box_dev_flag->next;
	}

	if (box_list_buf[strlen(box_list_buf) - 1] == ',')
	{
		box_list_buf[strlen(box_list_buf) - 1] = '\0';
	}

	return 0;
}

/*******************************************************************************
 *******************************一点通盒子设备列表END*******************************
 *******************************************************************************/

/*******************************************************************************
 ***********************************Tcp设备列表***********************************
 *******************************************************************************/
//加载tcp设备列表回调函数
static HB_S32 LoadTcpDevInfo(HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name)
{
	TCP_PAIR_HEAD_HANDLE tcp_dev_info = (TCP_PAIR_HEAD_HANDLE) para;

	tcp_dev_info->i_dev_num += 1;

	//获取盒子信息
	TCP_PAIR_HEAD_HANDLE item;
	item = (TCP_PAIR_HEAD_HANDLE) malloc(sizeof(TCP_PAIR_HEAD_OBJ));
	memset(item, 0, sizeof(TCP_PAIR_HEAD_OBJ));

	strncpy(item->c_tcp_dev_ip, column_value[0], strlen(column_value[0]));
	item->i_tcp_dev_port = atoi(column_value[1]);
	strncpy(item->c_tcp_dev_name, column_value[2], strlen(column_value[2]));
	item->i_box_port = atoi(column_value[3]);
	item->i_if_online = 0;

	item->next = tcp_dev_info->next;
	tcp_dev_info->next = item;

	return 0;
}

HB_VOID *ConnectIpTest(HB_VOID *param)
{
	HB_S32 ret = 0;
	HB_CHAR save_log[512] = { 0 };
	TCP_PAIR_HEAD_HANDLE map_dev = (TCP_PAIR_HEAD_HANDLE) param;

	snprintf(save_log, sizeof(save_log), "test_connect:%s:%d\n", map_dev->c_tcp_dev_ip, map_dev->i_tcp_dev_port);
	WriteLog(save_log, strlen(save_log));
	ret = connect_ip_port_test(map_dev->c_tcp_dev_ip, map_dev->i_tcp_dev_port, 2);
	if (1 == ret)
	{
		map_dev->i_if_online = 1; //能联通，表示在线
	}
	else
	{
		map_dev->i_if_online = 0; //不能联通，表示不在线
	}

	return NULL;
}

HB_S32 GetBasicTcpDevList(TCP_PAIR_HEAD_HANDLE tcp_dev)
{
	HB_CHAR *sql = "select tcp_dev_ip,tcp_dev_port,tcp_dev_name,tcp_box_port from tcp_dev_data";
	TCP_PAIR_HEAD_HANDLE tcp_dev_flag = NULL;
	HB_S32 ret = 0;

	SqlOperation(sql, BOX_DATA_BASE_NAME, LoadTcpDevInfo, (void *) tcp_dev);

	tcp_dev_flag = tcp_dev->next;
	while (tcp_dev_flag != NULL)
	{
		pthread_attr_t attr;
		pthread_t connect_test_pthread_id;
		ret = pthread_attr_init(&attr);
		ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		ret = pthread_create(&connect_test_pthread_id, &attr, ConnectIpTest, tcp_dev_flag);
		pthread_attr_destroy(&attr);
		tcp_dev_flag = tcp_dev_flag->next;
	}

	return 0;
}

//组织json
HB_S32 MakeJsonTcp(HB_CHAR *tcp_list_buf, HB_S32 tcp_list_buf_len, TCP_PAIR_HEAD_HANDLE tcp_dev_flag)
{
	HB_CHAR buf_var[1024] = {0};

	while (tcp_dev_flag)
	{
		url_decode(tcp_dev_flag->c_tcp_dev_name, strlen(tcp_dev_flag->c_tcp_dev_name));
		memset(buf_var, 0, sizeof(buf_var));
		snprintf(buf_var, sizeof(buf_var),
			"{\"TcpDevIp\": \"%s\", \"TcpDevPort\": \"%d\", \"TcpDevName\": \"%s\" ,\"TcpBoxPort\":\"%d\", \"TcpDevStatus\":\"%d\"},",
			tcp_dev_flag->c_tcp_dev_ip,
			tcp_dev_flag->i_tcp_dev_port,
			tcp_dev_flag->c_tcp_dev_name,
			tcp_dev_flag->i_box_port,
			tcp_dev_flag->i_if_online);

		strncat(tcp_list_buf, buf_var, tcp_list_buf_len - strlen(tcp_list_buf));
		tcp_dev_flag = tcp_dev_flag->next;
	}

	if (tcp_list_buf[strlen(tcp_list_buf) - 1] == ',')
	{
		tcp_list_buf[strlen(tcp_list_buf) - 1] = '\0';
	}

	return 0;
}
/*******************************************************************************
 **********************************Tcp设备列表END**********************************
 *******************************************************************************/


/*******************************************************************************
 ********************************Rtsp设备列表END********************************
 *******************************************************************************/

//rtsp设备测试联通性
static HB_VOID *rtsp_dev_connect_test(HB_VOID *param)
{
	HB_S32 ret = 0;
	HB_CHAR save_log[512] = { 0 };
	RTSP_DEV_LIST_HANDLE pRtspDevTest = (RTSP_DEV_LIST_HANDLE) param;

	snprintf(save_log, sizeof(save_log), "test_connect:%s:%d\n", pRtspDevTest->cDevIp, pRtspDevTest->iDevPort);
	WriteLog(save_log, strlen(save_log));
	ret = connect_ip_port_test(pRtspDevTest->cDevIp, pRtspDevTest->iDevPort, 2);
	if (1 == ret)
	{
		pRtspDevTest->iIfOnline = 1; //能联通，表示在线
	}
	else
	{
		pRtspDevTest->iIfOnline = 0; //不能联通，表示不在线
	}

	return NULL;
}


//获取RTSP设备列表回调函数
static HB_S32 load_rtsp_list_basic_page(HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name)
{
//	dev_name,dev_id,dev_ip,dev_port,dev_chnl_num,dev_login_usr,dev_state
	RTSP_DEV_LIST_HANDLE pRtspDevListHead = (RTSP_DEV_LIST_HANDLE) para;

	pRtspDevListHead->iDevNum += 1;

	//获取盒子信息
	RTSP_DEV_LIST_HANDLE pRtspDevInfo = (RTSP_DEV_LIST_HANDLE) malloc(sizeof(RTSP_DEV_LIST_OBJ));
	memset(pRtspDevInfo, 0, sizeof(RTSP_DEV_LIST_OBJ));

	pRtspDevInfo->iIfOnline = 0;
	strncpy(pRtspDevInfo->cDevName, column_value[0], strlen(column_value[0]));
	strncpy(pRtspDevInfo->cDevId, column_value[1], strlen(column_value[1]));
	strncpy(pRtspDevInfo->cDevIp, column_value[2], strlen(column_value[2]));
	pRtspDevInfo->iDevPort = atoi(column_value[3]);
	pRtspDevInfo->iDevChannel = atoi(column_value[4]);
	strncpy(pRtspDevInfo->cDevLoginName, column_value[5], strlen(column_value[5]));
	pRtspDevInfo->iDevStatus = atoi(column_value[6]);

	pRtspDevInfo->next = pRtspDevListHead->next;
	pRtspDevListHead->next = pRtspDevInfo;
	return 0;
}

//获取RTSP设备列表
HB_VOID GetBasicRtspDevList(RTSP_DEV_LIST_HANDLE pRtspDevList)
{
	HB_CHAR *sql = "select dev_name,dev_id,dev_ip,rtsp_port,dev_chnl_num,dev_login_usr,dev_state from onvif_dev_data";
	RTSP_DEV_LIST_HANDLE pRtspDev = NULL;

	SqlOperation(sql, BOX_DATA_BASE_NAME, load_rtsp_list_basic_page, (void *)pRtspDevList);

	pRtspDev = pRtspDevList->next;
	while (pRtspDev != NULL)
	{
		pthread_attr_t attr;
		pthread_t connect_test_pthread_id;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&connect_test_pthread_id, &attr, rtsp_dev_connect_test, pRtspDev);
		pthread_attr_destroy(&attr);
		pRtspDev = pRtspDev->next;
	}
}


HB_S32 MakeJsonRtsp(HB_CHAR *pRtspListBuf, HB_S32 iRtspListBufLen, RTSP_DEV_LIST_HANDLE pRtspDev)
{
	HB_CHAR cMacSn[32] = {0};
	HB_CHAR buf_var[1024] = { 0 };

	get_sys_sn(cMacSn, sizeof(cMacSn));

	while (pRtspDev)
	{
		if ((2 == pRtspDev->iDelFlag) || (3 == pRtspDev->iDelFlag)) //待删除状态不在前端显示
		{
			pRtspDev = pRtspDev->next;
			continue;
		}

		url_decode(pRtspDev->cDevName, strlen(pRtspDev->cDevName));

		snprintf(buf_var, sizeof(buf_var),
			"{\"DevName\": \"%s\", \"DevIP\":\"%s\", \"DevPort\":\"%d\", "
			"\"DevChnl\":\"%d\", \"DevConnectStatus\":\"%d\", \"DevId\":\"%s-%s\", "
			"\"DevLoginName\":\"%s\", \"DevStatus\":\"%d\"},",
			pRtspDev->cDevName, pRtspDev->cDevIp, pRtspDev->iDevPort,
			pRtspDev->iDevChannel, pRtspDev->iIfOnline, cMacSn, pRtspDev->cDevId,
			pRtspDev->cDevLoginName, pRtspDev->iDevStatus);

		strncat(pRtspListBuf, buf_var, iRtspListBufLen - strlen(pRtspListBuf));
		pRtspDev = pRtspDev->next;
	}

	if (pRtspListBuf[strlen(pRtspListBuf) - 1] == ',')
	{
		pRtspListBuf[strlen(pRtspListBuf) - 1] = '\0';
	}

	return 0;
}
/*******************************************************************************
 ********************************Rtsp设备列表END********************************
 *******************************************************************************/


