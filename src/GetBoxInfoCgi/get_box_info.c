/*
 * main.c
 *
 *  Created on: 2015年12月2日
 *      Author: root
 */

#include "my_include.h"
#include "fifo.h"
#include "cgi.h"
#include "session.h"
#include "sqlite3.h"
#include "common.h"

int GetCPUMEM(int *cpu_c, float *mem_c);

HB_S32 main()
{
	FILE *fp_t = NULL;
    HB_S32 cpu_c;
    HB_FLOAT mem_c;
	FIFO_HANDLE fifo_main = NULL;
	HB_CHAR box_name_type[64]={0};
#if 0  //校验
	cgi_init();
	cgi_session_cookie_name("MY_COOKIE");
	cgi_process_form();

	cgi_session_save_path(CGI_SESSION_SAVE_PATH);
	cgi_session_start();
	if (cgi_session_var_exists("USER_NAME") && cgi_session_var_exists("PASSWORD")
			&& cgi_session_var_exists("LAST_ACCESS_TIME")) //服务器端有session文件
	{
		if( CheckUsrPasswd() )//session和配置文件匹配
#endif
		{
			//HB_CHAR box_version[32] = {0};
			fp_t = fopen(BOX_VERSION_FILE, "r");
			if (NULL != fp_t)
			{
				HB_CHAR *ps1 = NULL;
				fgets(box_name_type, 64, fp_t);
				if ((ps1=strchr(box_name_type,'\r')) != NULL)
				{
					*ps1 = '\0';
				}
				else if ((ps1=strchr(box_name_type,'\n')) != NULL)
				{
					*ps1 = '\0';
				}
#if 0
				fgets(box_version, 32, fp_t);
				if ((ps1=strchr(box_version,'\r')) != NULL)
				{
					*ps1 = '\0';
				}
				else if ((ps1=strchr(box_version,'\n')) != NULL)
				{
					*ps1 = '\0';
				}
#endif
				fclose(fp_t);
			}

			//获取CPU和MEM使用信息
			GetCPUMEM(&cpu_c, &mem_c);

			fifo_main = fifo_create("/tmp/fifo");
			if(NULL == fifo_main)
			{
				printf("Content type: text/html\n\n");
				printf("{\"Result\":\"0\",\"BoxType\":\"%s\",\"BoxSpeed\":\"00kb/s\",\"ActiveLink\":\"00\",\"BoxCPU\":\"%d%%\",\"BoxMEM\":\"%.0f%%\"}", \
						box_name_type, cpu_c, mem_c);
				WRITE_LOG("send to web :[{\"Result\":\"0\",\"BoxType\":\"%s\",\"BoxSpeed\":\"00kb/s\",\"ActiveLink\":\"00\",\"BoxCPU\":\"%d%%\",\"BoxMEM\":\"%.0f%%\"}]\n", \
						box_name_type, cpu_c, mem_c);
				//printf("Content type: text/html\n\n");
				//printf("00kb/s    活跃链路:00");
			}
			else
			{
				printf("Content type: application/json \n\n");
				printf("{\"Result\":\"0\",\"BoxType\":\"%s\",\"BoxSpeed\":\"%dkb/s\",\"ActiveLink\":\"%d\",\"BoxCPU\":\"%d%%\",\"BoxMEM\":\"%.0f%%\"}", \
						box_name_type, fifo_main->kb_num, fifo_main->act_link, cpu_c, mem_c);
//				WRITE_LOG("send to web :[{\"Result\":\"0\",\"BoxType\":\"%s\",\"BoxSpeed\":\"%dkb/s\",\"ActiveLink\":\"%d\",\"BoxCPU\":\"%d%%\",\"BoxMEM\":\"%.0f%%\"}]\n", \
										box_name_type, fifo_main->kb_num, fifo_main->act_link, cpu_c, mem_c);
				//printf("Content type: text/html\n\n");
				//printf("%dkb/s    活跃链路:%d", fifo_main->kb_num, fifo_main->act_link);
			}
		}
#if 0
	}
#endif
	return 0;
}

