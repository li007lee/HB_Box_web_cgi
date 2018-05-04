#include "my_include.h"
#include "cgi.h"
#include "sqlite3.h"
#include "common.h"


typedef struct _tagSYSTEM_WEB_DATA
{
	HB_CHAR adm_usr_name[32];//用户名
	HB_CHAR adm_usr_passwd[64];//密码，md5加密过
}SYSTEM_WEB_DATA_OBJ, *SYSTEM_WEB_DATA_HANDLE;

SYSTEM_WEB_DATA_OBJ sys_web_data;


int load_sys_web_usr_passwd( void * para, int n_column, char ** column_value, char ** column_name )
{
	memset(&sys_web_data, 0, sizeof(SYSTEM_WEB_DATA_OBJ));
	strncpy(sys_web_data.adm_usr_name, column_value[0], strlen(column_value[0]));
	strncpy(sys_web_data.adm_usr_passwd, column_value[1], strlen(column_value[1]));
	return 0;
}


HB_S32 main()
{
	HB_CHAR *str_len=NULL;//定义字符型指针
	HB_CHAR *buf = NULL;
	HB_S32 len = 0;
    HB_S32 DataCode;
    HB_CHAR user[20] = {0};
    HB_CHAR passwd[128] = {0};
    HB_CHAR sql[128] = {0};
    HB_CHAR outbuf[1024] = {0};
    HB_CHAR check_buf[1024] = {0};
    HB_CHAR last_time_str[32] = {0};
    time_t last_time = 0;

	cgi_init();
    str_len = getenv("CONTENT_LENGTH");//获取输入字符长度
	if( str_len != NULL )
	{
		//len=atoi(str_len);
		sscanf(str_len,"%d",&len);
		buf=(char *)malloc(sizeof(char)*(len+2));
		fgets(buf, len+1, stdin);    //从标准输入中读取len个字符
		WRITE_LOG("recv from web:[%s]\n", buf);
	}

	cgi_process_form();

	system(RM_LOG_FILE);
	system(RM_SESSION_FILE);
	strncpy(sql, "SELECT adm_usr_name, adm_usr_passwd FROM system_web_data", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, load_sys_web_usr_passwd, NULL);

	if (buf != NULL) //有用户名和密码登录
	{
		DataCode = GetDataCode(buf);
		switch(DataCode)
		{
			case 100: //用户登录
			{
				HB_CHAR *pos = NULL;
				if((pos = strstr(buf, "name")))
				{
					sscanf(pos, "name=%[^&]&passwd=%s", user,passwd);//格式化输入获得登陆名和密码
					//read_conf_value(SHADOW_SAVE_PATH, outbuf);//读取用户名和密码的配置文件
					bzero(check_buf,1024);
					sprintf(check_buf, "%s:%s", user, passwd);
					sprintf(outbuf, "%s:%s", sys_web_data.adm_usr_name, sys_web_data.adm_usr_passwd);

					if (!strncmp(check_buf, outbuf, strlen(check_buf)))//登录界面输入的用户名密码和配置文件中的比较
					{
						int conndfd = -1;
						struct sockaddr_in serverAddr;
						char buf[8] = {0};
						conndfd=socket(AF_INET,SOCK_STREAM,0);

						memset(&serverAddr,0,sizeof(serverAddr));
						serverAddr.sin_family=AF_INET;
						serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
						serverAddr.sin_port=htons(7879);
						if(-1==connect(conndfd,(struct sockaddr*)&serverAddr,sizeof(serverAddr)))
						{
							WRITE_LOG("Connetc iptable server failed\n");
						}
						else
						{
							WRITE_LOG("send GetStreamInfo\n");
							send(conndfd, "GetStreamInfo", strlen("GetStreamInfo"), 0);
							close(conndfd);
							conndfd = -1;
						}
//						usleep(500000);
//						sleep(2);
//						recv(conndfd, buf, 8, 0);
//						WRITE_LOG("recv from iptable server: [%s]\n", buf);

						cgi_session_cookie_name("MY_COOKIE");
						cgi_session_save_path(CGI_SESSION_SAVE_PATH);
						cgi_session_start();
						last_time = time(NULL);
						sprintf(last_time_str, "%ld", last_time);
						cgi_session_register_var("USER_NAME", user);
						cgi_session_register_var("PASSWORD", passwd);
						cgi_session_register_var("LAST_ACCESS_TIME", last_time_str);

						printf("Content type: application/json \n\n");
						printf("{\"Result\":0}");

					}
					else //用户名和密码不匹配
					{
						printf("Content type: application/json \n\n");
						printf("{\"Result\":\"-5\",\"ErrMessage\":\"用户名和密码不匹配！-5\"}");
					}
				}
				else
				{
					printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-10\",\"ErrMessage\":\"Json解析错误！-10\"}");
				}
				break;
			}
			default: //用户登出
			{
				//cgi_session_destroy();
				printf("Content type: application/json \n\n");
				printf("{\"Result\":\"0\"}");
				break;
			}
		}
	}
	else
	{
		WRITE_LOG("recv from web error!\n");
	}


	free(buf);
	cgi_end();

	return 0;
}
