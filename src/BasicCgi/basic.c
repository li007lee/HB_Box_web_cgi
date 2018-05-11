#include "my_include.h"
#include "cgi.h"
#include "session.h"
#include "basic.h"
#include "sqlite3.h"
#include "connect_domain_test.h"
#include "common.h"
#include "basic_operation.h"

HB_S32 main()
{
	HB_CHAR *buf = NULL;

	HB_CHAR box_list_buf[SEND_JSON_BUF_LEN] = { 0 };
	HB_CHAR tcp_list_buf[TCP_LIST_JSON_BUF_LEN] = { 0 };
	HB_CHAR pRtspListBuf[RTSP_LIST_JSON_BUF_LEN] = { 0 };

	BOX_PAIR_HEAD_OBJ box_dev = { 0 };
	TCP_PAIR_HEAD_OBJ tcp_dev_head;
	RTSP_DEV_LIST_OBJ stRtspDevListHead;

	memset(&tcp_dev_head, 0, sizeof(tcp_dev_head));
	tcp_dev_head.next = NULL;
	if (box_dev.dev_num == 0)
	{
		box_dev.dev_list_head = (BOX_PAIR_DEV_LIST_HANDLE) malloc(sizeof(BOX_PAIR_DEV_LIST_OBJ));
		box_dev.dev_list_head->next = NULL;
	}
#if 1
	CgiInitAndConfig(&buf, CGI_SESSION_SAVE_PATH);
	//判断session是否已经创建，如果没有创建则直接返回登出状态
	if ((cgi_session_var_exists("USER_NAME") && cgi_session_var_exists("PASSWORD") && cgi_session_var_exists("LAST_ACCESS_TIME"))) //服务器端有session文件
#endif
	{
		if (!cgi_session_var_exists("USER_NAME"))
		{
			HB_CHAR *cBufSize = cgi_cookie_value("MY_COOKIE");
			WRITE_LOG("MY_COOKIE :[%s]\n", cBufSize);
			HB_CHAR last_time_str[32] = {0};
			time_t last_time = time(NULL);
			sprintf(last_time_str, "%ld", last_time);
			cgi_session_register_var("USER_NAME", "admin");
			cgi_session_register_var("PASSWORD", "21232f297a57a5a743894a0e4a801fc3");
			cgi_session_register_var("LAST_ACCESS_TIME", last_time_str);
		}
		else if (CalcTimeOut())
		{
			printf("Content type: text/xml \n\n");
			printf("{\"Result\":\"-1\",\"ErrMessage\":\"用户登录超时,请重新登录!\"}");
			goto END;
		}

		HB_CHAR c_data[128] = { 0 };
#if 0
		pthread_t thread_get_net_status;
		HB_S32 ret = 0;
		pthread_attr_t attr;

		ret = pthread_attr_init(&attr);
		ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		ret = pthread_create(&thread_get_net_status, &attr, GetNetStatus2, (void *)c_data);
		pthread_attr_destroy(&attr);
		//pthread_create(&thread_get_net_status, NULL, GetNetStatus2, (void *)c_data);
#endif
		WRITE_LOG("Start GetNetStatus\n");
		GetNetStatus(c_data, sizeof(c_data));
		//snprintf(c_data, sizeof(c_data), "\"Status\":\"%d\",\"GlanIp\":\"%s\",\"MAC\":\"%s\"", i_connect_status, gnlan_ipaddr, mac_sn);
#if 1
		WRITE_LOG("Start GetBasicBoxDevList\n");
		GetBasicBoxDevList(&box_dev);
		WRITE_LOG("Start GetBasicTcpDevList\n");
		GetBasicTcpDevList(&tcp_dev_head);
		WRITE_LOG("Start GetBasicRtspDevList\n");
		memset(&stRtspDevListHead, 0, sizeof(stRtspDevListHead));
		GetBasicRtspDevList(&stRtspDevListHead);

		if ((box_dev.dev_num > 0) || (tcp_dev_head.i_dev_num > 0) || (stRtspDevListHead.iDevNum > 0))
		{
			sleep(3); //等待两秒，进行ip端口连接测试
		}

		MakeJsonBox(box_list_buf, SEND_JSON_BUF_LEN, box_dev.dev_list_head->next);
		MakeJsonTcp(tcp_list_buf, SEND_JSON_BUF_LEN, tcp_dev_head.next);
		MakeJsonRtsp(pRtspListBuf, SEND_JSON_BUF_LEN, stRtspDevListHead.next);

		WRITE_LOG("send to web :[{\"Result\":\"0\",\"DevState\":{%s},\"BoxDevList\":[%s], \"TcpDevList\":[%s], \"RtspDevList\":[%s]}]\n", \
				c_data, box_list_buf, tcp_list_buf, pRtspListBuf);
		//printf("Content type: application/json \n\n");
		printf("{\"Result\":\"0\",\"DevState\":{%s},\"BoxDevList\":[%s], \"TcpDevList\":[%s], \"RtspDevList\":[%s]}", \
				c_data, box_list_buf, tcp_list_buf, pRtspListBuf);
		//释放链表
		BOX_PAIR_DEV_LIST_HANDLE p_box_dev_flag = box_dev.dev_list_head;
		BOX_PAIR_DEV_LIST_HANDLE q_box_dev_flag = box_dev.dev_list_head;
		while (p_box_dev_flag)
		{
			q_box_dev_flag = p_box_dev_flag->next;
			free(p_box_dev_flag);
			p_box_dev_flag = q_box_dev_flag;
		}

		//释放链表
		TCP_PAIR_HEAD_HANDLE p_tcp_dev_flag = tcp_dev_head.next;
		TCP_PAIR_HEAD_HANDLE q_tcp_dev_flag = tcp_dev_head.next;
		while (p_tcp_dev_flag)
		{
			q_tcp_dev_flag = p_tcp_dev_flag->next;
			free(p_tcp_dev_flag);
			p_tcp_dev_flag = q_tcp_dev_flag;
		}

		//释放链表
		RTSP_DEV_LIST_HANDLE pRtspDev = stRtspDevListHead.next;
		RTSP_DEV_LIST_HANDLE qRtspDev = stRtspDevListHead.next;
		while (pRtspDev)
		{
			qRtspDev = pRtspDev->next;
			free(pRtspDev);
			pRtspDev = qRtspDev;
		}
#endif
	}
	else
	{
		printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!\"}");
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

