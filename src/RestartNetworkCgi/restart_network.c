#include "my_include.h"
#include "net_api.h"
#include "common.h"
#include "restart_network.h"

//重启网卡重设辅助ip
static HB_S32 SetLanIp( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR cmd[256] = {0};

	//eth0:num ip mask
	snprintf(cmd, sizeof(cmd), "ifconfig %s:%s %s netmask %s", ETH_X, column_value[0], column_value[1], column_value[2]);
	WRITE_LOG("[cmd:%s]\n", cmd);
	system(cmd);

	return 0;
}

//重启网卡重设静态路由
static HB_S32 SetStaticRoute( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR cmd[256] = {0};

	snprintf(cmd, sizeof(cmd), "route add -net %s netmask %s gw %s %s", \
			column_value[0], column_value[1], column_value[2], ETH_LAN);
	WRITE_LOG("[cmd:%s]\n", cmd);
	system(cmd);

	return 0;
}

HB_S32 main()
{
#ifdef SMALL_BOX
	HB_CHAR sql[512] = {0};
//	system(RESTART_NETWORK);
	my_system(RESTART_NETWORK);

	memset(sql, 0, sizeof(sql));
	strncpy(sql, "select net_ethx,net_ipaddr,net_mask from lan_web_ip_data", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, SetLanIp, NULL);

	memset(sql, 0, sizeof(sql));
	strncpy(sql, "select segment,mask,gateway from static_route_data", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, SetStaticRoute, NULL);

#else
	GN_NET_LAYER_OBJ buf_cmd;
	HB_S32 sockfd = 0;
	HB_CHAR recv_buf[128] = {0};

	memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
	buf_cmd.iProtocolVer = 1;
	buf_cmd.iDataType = 9;
	buf_cmd.iBlockHeadFlag = 1;
	buf_cmd.iBlockEndFlag = 1;

	sprintf(buf_cmd.cBuffer, "<TYPE>SystemCmd</TYPE><CmdStr>%s</CmdStr>", RESTART_NETWORK);
	buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);

	create_socket_connect_ipaddr(&sockfd, MAIN_CTRL_IP, MAIN_CTRL_PORT, 2);
	if (sockfd > 0)
	{
		send_data(&sockfd, (HB_VOID *)&buf_cmd, buf_cmd.iActLength, 2);
		WRITE_LOG("send to main:[%s]\n", buf_cmd.cBuffer);
		recv_data(&sockfd, recv_buf, sizeof(recv_buf), 2);
		WRITE_LOG("recv from main:[%s]\n", recv_buf);
		printf("Content type: text/json \n\n");
		printf("{\"Result\":\"0\",\"ErrMessage\":\"修改成功!\"}");

		close_sockfd(&sockfd);
	}
#endif
	return 0;
}
