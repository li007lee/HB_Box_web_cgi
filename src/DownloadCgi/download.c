/*
 * download.c
 *
 *  Created on: 2015年12月16日
 *      Author: root
 */

#include "my_include.h"

#define MAX_FILE_LEN  (1024*30)

HB_S32 main()
{
	FILE *fp;
	struct stat sb;
	HB_CHAR filebuf[MAX_FILE_LEN] = {0};
	HB_CHAR cmd[512] = {0};

	//buf = getenv("QUERY_STRING");//获取输入字符长度
#if 0
	char *str_len=NULL;//定义字符型指针
    int len=0;
    char *buf=NULL;

	if( str_len != NULL )
	{
		len=atoi(str_len);
		buf=(char *)malloc(sizeof(char)*(len+2));
		fgets(buf, len+1, stdin);    //从标准输入中读取len个字符
	}
#endif
	//if( buf != NULL )
	{
		//if(!strcmp("param=DownloadPcap", buf)) //下载tcpdump抓的数据包
		{
			sprintf(cmd, "%s%s", DOWNLOAD_FILE_PCAP_PATH, DOWNLOAD_FILE_PCAP_NAME);
			if(-1 == stat(cmd, &sb))
			{
				printf("Content type: text/html\n\n");
				//printf("{\"Result\":\"-420\",\"ErrMessage\":\"没有数据!-420\"}");
				printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
				"<script>alert(\"没有任何数据！\");window.close();</script>"
				"</head><body></body></html>");
				return 0;
			}
			printf("Content-Disposition:attachment;filename=%s", DOWNLOAD_FILE_PCAP_NAME);
			printf("\r\n");
			printf("Content-Length:%d", (int)(sb.st_size));
			printf("\r\n");
			printf("Content-Type:application/octet-stream\r\n");
			printf("\r\n");
			//sprintf(cmd, "%s%s", DOWNLOAD_FILE_PCAP_PATH, DOWNLOAD_FILE_PCAP_NAME);
			if((fp=fopen(cmd, "r+b")))
			{
				do
				{
					int rs = fread(filebuf, 1, sizeof(filebuf), fp);
					fwrite(filebuf, rs, 1, stdout);
				}
				while(!feof(fp));
				fclose(fp);

				memset(cmd, 0, sizeof(cmd));
				sprintf(cmd, "rm %s%s", DOWNLOAD_FILE_PCAP_PATH, DOWNLOAD_FILE_PCAP_NAME);
				system(cmd);
			}
		}
	}

	return 1;
}
