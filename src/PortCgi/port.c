#include "dev_operation.h"

HB_S32 main()
{
	HB_CHAR *buf=NULL;
	HB_S32 i_datacode = -1;

	CgiInitAndConfig(&buf, CGI_SESSION_SAVE_PATH);
	if (cgi_session_var_exists("USER_NAME") && cgi_session_var_exists("PASSWORD")
			&& cgi_session_var_exists("LAST_ACCESS_TIME")) //服务器端有session文件
	{
		if( CheckUsrPasswd() )//session和配置文件匹配
		{
			if (CalcTimeOut())
			{
				printf("Content type: text/xml \n\n");
				printf("{\"Result\":\"-1\",\"ErrMessage\":\"用户登录超时,请重新登录!\"}");
				goto END;
			}

			if (buf != NULL)
			{
				i_datacode = GetDataCode(buf);
				switch(i_datacode)
				{
					case GET_ALL_DEV_LIST_PORT: GetALLDevPortList(); break;
//					case GET_ALL_DEV_LIST: GetALLDevList(); break;

					case ADD_BOX_DEV_AUTO:
					case ADD_BOX_DEV_HAND: AddBoxDev(i_datacode, buf); break;	//添加一点通盒子设备
					case DEL_ONE_BOX_DEV: DelOneBoxDev(buf); break;//删除1条一点通盒子数据
					case DEL_ALL_BOX_DEV: DelAllBoxDev(); break;	//删除全部一点通盒子设备
					//case GET_BOX_DEV_LIST: GetBoxDevList(); break;	//获取一点通设备列表

					case ADD_RTSP_DEV: add_rtsp_dev(buf); break;	//添加onvif设备
					case DEL_ONE_RTSP_DEV: DelOneRtspDev(buf); break;//删除一条onvif设备
					case DEL_ALL_RTSP_DEV: DelAllRtspDev(); break;	//删除所有onvif设备
//					case GET_RTSP_DEV_LIST: GetRtspDevList(); break;	//获取onvif设备列表
					case DISCOVER_RTSP_DEV:	DiscoverRtspDev(); break; //搜索onvif设备（搜索onvif设备）
					case REFRESH_RTSP_DEV: break;//更新数据库中onvif设备列表，当已添加的设备修改了端口，可以用词操作同步更新数据库中的数据
					case GET_ADD_RTSP_PERSENT: get_add_rtsp_present(buf);break;//获取添加onvife设备时的进度

					case ADD_TCP_DEV: AddTcpDev(buf); break;	//添加TCP设备
					case DEL_ONE_TCP_DEV: DelOneTcpDev(buf); break;	//删除一条TCP设备
					case DEL_ALL_TCP_DEV: DelAllTcpDev(); break;	//删除所有TCP设备
					//case GET_TCP_DEV_LIST: GetTcpDevList(); break;	//获取TCP设备列表

					default:
					{
						//printf("Content type: application/json \n\n");
						printf("{\"Result\":\"-2\",\"ErrMessage\":\"内部通信错误！-2\"}");
						WRITE_LOG("DataCode error!");
						break;
					}
				}
			}
		}
		else//session和配置文件不匹配， 意味着用户名和密码输入错误
		{
			printf("Content type: application/json \n\n");
			printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-3\"}");
		}
	}
	else//服务器端无session文件
	{
		printf("Content type: application/json \n\n");
		printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-4\"}");
		WRITE_LOG("!!!!!!!!!!!!!!!!!!!!!!!!no session!\n");
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

