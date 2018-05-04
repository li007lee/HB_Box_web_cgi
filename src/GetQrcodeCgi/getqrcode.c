/*
 * download.c
 *
 *  Created on: 2015年12月16日
 *      Author: root
 */

#include "my_include.h"
#include "cgi.h"
#include "session.h"
#include "common.h"
#include "sqlite3.h"
#include "net_api.h"

#define MAX_FILE_LEN  (1024*30)

static HB_S32 SQLGetMachineCode( HB_VOID * para, HB_S32 n_column, HB_CHAR ** column_value, HB_CHAR ** column_name )
{
	HB_CHAR *p_machine_code = (HB_CHAR *)para;

	strncpy(p_machine_code, column_value[0], strlen(column_value[0]));

	return 0;
}


HB_S32 main()
{
    HB_S32 len=0;
    HB_CHAR *buf=NULL;
	HB_CHAR *buf_len=NULL;//定义字符型指针
	HB_CHAR *sql = NULL;
	struct stat sb;
	HB_CHAR cMac[32] = {0};
	HB_CHAR cMacSn[32] = {0};
	HB_CHAR cCmd[512] = {0};

	cgi_init();
	cgi_session_cookie_name("MY_COOKIE");

	buf_len = getenv("CONTENT_LENGTH");//获取输入字符长度
	if( buf_len != NULL )
	{
		len=atoi(buf_len);
		buf=(HB_CHAR *)malloc(sizeof(HB_CHAR)*(len+2));
		fgets(buf, len+1, stdin);    //从标准输入中读取len个字符
	}

	cgi_process_form();
	cgi_session_save_path(CGI_SESSION_SAVE_PATH);
	cgi_session_start();

	sql = "select machine_code from system_web_data";
	SqlOperation(sql, BOX_DATA_BASE_NAME, SQLGetMachineCode, (HB_VOID *)cMac);

	if( buf != NULL )
	{
		switch(GetDataCode(buf))
		{
			case 1201:
			{
				get_sys_sn(cMacSn, sizeof(cMacSn));
				memset(cCmd, 0, sizeof(cCmd));
				snprintf(cCmd, sizeof(cCmd), "rm -rf " BOX_QRCODE_FILE);
				system(cCmd);
				//生成二维码图片
				memset(cCmd, 0, sizeof(cCmd));

#ifdef SMALL_BOX
				snprintf(cCmd, sizeof(cCmd), "qrencode -v 1 -s 6 -m 0 -o " BOX_QRCODE_FILE " \"qrClass=mediaGateway;sn=%s;hw=%s;\"", \
								cMacSn, cMac);
#endif
#ifdef BIG_BOX
				snprintf(cmd, sizeof(cmd), BIN_PATH"qrencode -v 1 -s 6 -m 0 -o " BOX_QRCODE_FILE " \"qrClass=mediaGateway;sn=%s;hw=%s;\"", \
								c_mac_sn, c_mac);
#endif
				system(cCmd);
				WRITE_LOG("create qrencode :%s\n", cCmd);

				if(-1 == stat(BOX_QRCODE_FILE, &sb))
				{
					printf("Content type: text/xml \n\n");
					printf("{\"Result\":\"-500\",\"ErrMessage\":\"二维码生成失败，请刷新重试！-500\"}");
					return -1;
				}
			}
			break;
			case 1202:
			{
				HB_CHAR *pPos = NULL;
				HB_CHAR cQrcodePath[32] = {0};
				HB_CHAR cDevId[64] = {0};
				HB_CHAR cDevType[8] = {0};

				pPos = strstr(buf, "DevId");
				if(pPos == NULL) //添加设备
				{
					printf("Content type: application/json \n\n");
					printf("{\"Result\":\"-501\",\"ErrMessage\":\"json串有误!-11\"}");
					return -2;
				}

				sscanf(pPos, "DevId=%[^&]&DevType=%s", cDevId, cDevType);

				memset(cCmd, 0, sizeof(cCmd));
				snprintf(cCmd, sizeof(cCmd), "rm -rf /tmp/qrcode_dev*");
				system(cCmd);

				snprintf(cQrcodePath, sizeof(cQrcodePath), "/tmp/qrcode_dev%ld.png", time(NULL));

				//生成二维码图片
				memset(cCmd, 0, sizeof(cCmd));

#ifdef SMALL_BOX
				if (strncmp(cDevType, "ydt", strlen("ydt")) == 0)
				{
					snprintf(cCmd, sizeof(cCmd), "qrencode -v 1 -s 5 -m 0 -o %s \"qrClass=mgDevice;sn=%s;hw=%s;\"", \
									cQrcodePath, cDevId, cMac);
				}
				else if (strncmp(cDevType, "onvif", strlen("onvif")) == 0)
				{
					snprintf(cCmd, sizeof(cCmd), "qrencode -v 1 -s 5 -m 0 -o %s \"qrClass=mgDevice;sn=%s;hw=%s;\"", \
									cQrcodePath, cDevId, cMac);
				}
#endif
#ifdef BIG_BOX
				snprintf(cmd, sizeof(cmd), BIN_PATH"qrencode -v 1 -s 5 -m 0 -o %s \"qrClass=mgDevice;sn=%s;hw=%s;\"", \
								qrcode_path, c_dev_id, c_mac);
#endif
				system(cCmd);
				WRITE_LOG("create qrencode :%s\n", cCmd);
				if(-1 == stat(cQrcodePath, &sb))
				{
					printf("Content type: text/xml \n\n");
					printf("{\"Result\":\"-500\",\"ErrMessage\":\"二维码生成失败，请刷新重试！-500\"}");
					return -3;
				}
				else
				{
					printf("Content type: text/xml \n\n");
					printf("{\"Result\":\"0\",\"qrcode_path\":\"%s\"}", cQrcodePath);
					return 0;
				}
			}
			break;
		}
	}
	printf("Content type: text/xml \n\n");
	printf("{\"Result\":\"0\",\"ErrMessage\":\"succeed!\"}");
	return 1;
}
