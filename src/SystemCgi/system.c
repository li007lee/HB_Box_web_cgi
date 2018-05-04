#include "system_opt.h"

HB_S32 main()
{
	HB_CHAR *buf = NULL;

	CgiInitAndConfig(&buf, CGI_SESSION_SAVE_PATH);
#if 0
	cgi_init();
	cgi_session_cookie_name("MY_COOKIE");

	str_len = getenv("CONTENT_LENGTH"); //获取输入字符长度

	if (str_len != NULL)
	{
		len = atoi(str_len);
		buf = (HB_CHAR *) malloc(sizeof(HB_CHAR) * (len + 2));
		fgets(buf, len + 1, stdin);    //从标准输入中读取len个字符
		WRITE_LOG("recv data from web:[%s]\n", buf);
	}
	cgi_process_form();
	cgi_session_save_path(CGI_SESSION_SAVE_PATH);
	cgi_session_start();
#endif
	if (cgi_session_var_exists("USER_NAME")
					&& cgi_session_var_exists("PASSWORD")
					&& cgi_session_var_exists("LAST_ACCESS_TIME"))
	{
		if (CheckUsrPasswd())
		{
			if (CalcTimeOut())
			{
				printf("Content type: text/xml \n\n");
				printf("{\"Result\":\"-1\",\"ErrMessage\":\"用户登录超时,请重新登录!\"}");
				goto END;
			}

			if (buf != NULL) //有表单提交过来的数据
			{
				switch (GetDataCode(buf))
				{
					case 1101: FactroyReset(); break;	//恢复出厂设置
					case 1102: TcpdumpCapture(buf); break;	//数据分析
					case 1103: //系统升级
						//参见update.c
						break;
					case 1104: ChangeUserPwd(buf); break;	//修改一点通帐户信息
					case 1105: GetBoxVersion(); break;	//获取当前版本号
					case 1106: SetCloudVerifyStatus(buf); break;//设置是否开启云端验证
					case 1107: SetWanConnection(buf);break; //设置是否关闭广域网
					default:
					{
						printf("{\"Result\":\"-2\",\"ErrMessage\":\"DataCode错误！\"}");
						break;
					}
				}
				/*
				 if ((pos = strstr(buf, "LanBoxPairFlag"))) //开启或关闭局域网配对模式
				 {
				 HB_CHAR tmp_lan_pair_mod[6] = {0};
				 sscanf(pos, "LanBoxPairFlag=%s", tmp_lan_pair_mod);
				 memset(sql, 0, 1024);
				 sprintf(sql, "UPDATE system_web_data SET lan_pair_mod = '%s'", tmp_lan_pair_mod);
				 ret = sqlite3_open(BOX_DATA_BASE_NAME, &db);
				 ret = sqlite3_exec(db, sql, 0, NULL, &errmsg);
				 sqlite3_close(db);
				 if(ret != SQLITE_OK)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置局域网配对模式失败！\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 }

				 else if ((pos = strstr(buf, "WanBoxPairFlag"))) //开启或关闭广域网配对模式
				 {
				 #if 0
				 HB_CHAR tmp_wan_pair_mod[6] = {0};
				 sscanf(pos, "WanBoxPairFlag=%s", tmp_wan_pair_mod);

				 BOX_CONFIG_INFO_OBJ tmp_box_info;
				 memset(&tmp_box_info, 0, sizeof(BOX_CONFIG_INFO_OBJ));
				 get_box_config_info(BOX_INFO_XML_FILE, &tmp_box_info);
				 ret = on_off_wan_pair_flag(SERVER_IP, SERVER_PORT, box_info.wan_pair_switch, tmp_box_info.wan_pair_code);
				 if (ret != 0)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置广域网配对模式失败！(-1)\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }

				 ret = update_box_config_info(BOX_INFO_XML_FILE, &box_info);
				 if(ret != 0)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置广域网配对模式失败！(-2)\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 #endif
				 }

				 #if 0
				 else if ((pos = strstr(buf, "lan_new_pair_code"))) //设置局域网配对码
				 {
				 char tmp_code[64] = {0};
				 memset(&box_info, 0, sizeof(BOX_CONFIG_INFO_OBJ));
				 sscanf(pos, "lan_new_pair_code=%[^&]&re_lan_new_pair_code=%s",
				 box_info.lan_pair_code, tmp_code);
				 if(strcmp(box_info.lan_pair_code, tmp_code))
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"两次输入的局域网配对码不一致！\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 else
				 {
				 ret = update_box_config_info(BOX_INFO_XML_FILE, &box_info);
				 if(ret != 0)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置局域网盒子配对码失败！\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 }
				 }

				 else if ((pos = strstr(buf, "wan_new_pair_code"))) //设置广域网配对码
				 {
				 char tmp_code[64] = {0};
				 memset(&box_info, 0, sizeof(BOX_CONFIG_INFO_OBJ));
				 sscanf(pos, "wan_new_pair_code=%[^&]&re_wan_new_pair_code=%s",
				 box_info.wan_pair_code, tmp_code);
				 if(strcmp(box_info.wan_pair_code, tmp_code))
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"两次输入的广域网配对码不一致！\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 else
				 {
				 ret = set_new_code_tell_server(SERVER_IP, SERVER_PORT, box_info.wan_pair_code);
				 if (HB_FAILURE == ret)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置广域网盒子配对码失败！(-1) \");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }

				 ret = update_box_config_info(BOX_INFO_XML_FILE, &box_info);
				 if(ret != 0)
				 {
				 cgi_init_headers();
				 printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				 "<script>alert(\"设置广域网盒子配对码失败！(-2)\");"
				 "</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
				 "</head><body></body></html>");
				 free(buf);
				 cgi_end();
				 return 0;
				 }
				 }
				 }
				 #endif
				 */
			}
		}
		else
		{
			printf("Content type: text/xml \n\n");
			printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录错误！-3\"}");
		}
	}
	else
	{
		cgi_session_destroy();
		printf("Content type: text/xml \n\n");
		printf("{\"Result\":\"-1\",\"ErrMessage\":\"登录有误!-4\"}");
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
