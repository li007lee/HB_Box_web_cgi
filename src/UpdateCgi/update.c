/*
 * upload.c
 *
 *  Created on: 2016年1月4日
 *      Author: root
 */

#include "my_include.h"
#include "net_api.h"
#include "cgi.h"
#include "session.h"
#include "sqlite3.h"
#include "common.h"

#define FILE_SAVE_DIR "/tmp/"
#define NET_BUFFER_LEN			(8*1024)

typedef struct _tagGN_NET_LAYER
{
	HB_S32 		iActLength;    // 一个TCP包实际长度
	HB_S32		iProtocolVer;  //网络通信协议版本,防止以后升级能识别,目前定为1
	HB_S32 	iDataType;     //传输的数据类型，目前固定为9
	HB_S32 	iBlockHeadFlag;//头标识
	HB_S32 		iBlockEndFlag;//尾标识
	HB_CHAR 	cBuffer[NET_BUFFER_LEN];
}GN_NET_LAYER_OBJ, *GN_NET_LAYER_HANDLE;


static HB_S32 atoii (HB_CHAR *zzzz)
{
	HB_S32 i = 0;
	HB_S32 num=0;
	for(i=0;i<20;i++)
	{
		if(zzzz[i] >= '0' && zzzz[i] <= '9')
		{
			num = num * 10 + (zzzz[i] - '0');
		}
		else
		{
			break;
		}
	}
	return num;
}


#if 0
char* getCgiData(FILE* fp, char* requestmethod)
{

       char* input;
       int len;
       char *pppp;
       int size = 1024;
      // int i = 0;
     if(!strcmp(requestmethod, "GET"))
       {

              input = getenv("QUERY_STRING");
              return input;

       }
       else if (!strcmp(requestmethod, "POST"))
       {
            pppp=getenv("CONTENT_LENGTH");
             len = atoii(pppp);
              input = (char*)malloc(sizeof(char)*(size + 1));

              if (len == 0)
              {
                     input[0] = '\0';

                     return input;
              }

        fgets(input, len+1, stdin);
            input[len]='\0';
         return input;
       }

       return NULL;
}
#endif

//static unsigned int tmppp=0;

static HB_CHAR *getFileName(HB_CHAR *req)
{
	//HB_U32 tmppp=0;
	HB_CHAR *psz1;
	HB_CHAR *psz2;
    //unsigned char *cur_post,*buf;

	// get filename keyword
	if ((psz1=strstr(req, "filename=")) == NULL)
	{
		//return (char *)&tmppp;
		return NULL;
	}

    // get pointer to actual filename (it's in quotes)
    psz1+=strlen("filename=");
    if ((psz1 = strtok(psz1, "\"")) == NULL)
    {
    	//return (char *)&tmppp;
    	return NULL;
    }
    // remove leading path for both PC and UNIX systems
    if ((psz2 = strrchr(psz1,'\\')) != NULL)
    {
    	psz1 = psz2+1;
    }
    if ((psz2 = strrchr(psz1,'/')) != NULL)
    {
    	psz1 = psz2+1;
    }
    return psz1;
}


HB_S32 main()
{
	HB_S32 ret = 0;
	HB_CHAR *reqMethod;
    HB_S32 len;
    HB_S64 total,i,count;
    HB_CHAR *fileName,*ps1;
    HB_CHAR Boundary[256];
    HB_CHAR errorBuf[200]="";
    HB_CHAR tmpBuf[512];
    HB_CHAR filePath[256]=FILE_SAVE_DIR;//directory of uploaded file
    FILE *fileBuf=NULL;
    reqMethod=getenv("REQUEST_METHOD");
    len=atoii(getenv("CONTENT_LENGTH"));
#if 0
	printf("Content type: text/html\n\n");
	printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
	"<script>alert(\"[%d]\");"
	"</script><meta http-equiv=\"Refresh\" content=\"0;URL=system.cgi\" />"
	"</head><body></body></html>",len);
	return 0;
#endif

    Boundary[0] = '\r';
    Boundary[1] = '\n';
    Boundary[2] = '\0';

    if (fgets(&Boundary[2], sizeof(Boundary)-2, stdin) == NULL)
    {
        sprintf(errorBuf,"%s","Get boundary failed !");
        goto error;
    }

    //strip terminating CR / LF
    if ((ps1=strchr(&Boundary[2],'\r')) != NULL)
    {
        *ps1 = '\0';
    }

    if ((ps1=strchr(&Boundary[2],'\n')) != NULL)
    {
        *ps1 = '\0';
    }

    fgets(tmpBuf,512,stdin);


    fileName=getFileName(tmpBuf);

    if(fileName)
    {
        //printf("fileName=%s<br>",fileName);
    }

	ret = access("/tmp/box.bin", F_OK);
	if (0 == ret)
	{
		system("rm /tmp/box.bin");
	}

    //strcat(filePath,fileName);
    strcat(filePath,"box.bin");
    //printf("filepath===%s<br>",filePath);

    memset(tmpBuf, 0, 512);
    fgets(tmpBuf,512,stdin);
    //printf("%s<br>",tmpBuf);//content-type
    memset(tmpBuf, 0, 512);
    fgets(tmpBuf,512,stdin);
    //printf("%s<br>",tmpBuf);// \r\n

    if(fopen(filePath,"rb"))
    {
        sprintf(errorBuf,"%s","File already exist.");
        remove(filePath);
        goto error;
    }

    if ((fileBuf = fopen(filePath, "wb+")) == NULL)
    {
        sprintf(errorBuf,"%s","File open error.Make sure you have the permission.");
        goto error;
    }
    // copy the file
    while ((count=fread(tmpBuf, 1, 512, stdin)) != 0)
    {
        if ((fwrite(tmpBuf, 1, count, fileBuf)) != count)
        {
            sprintf(errorBuf,"%s","Write file error.");
            goto error;
        }
    }
    // re read last 128 bytes of file, handling files < 128 bytes
    if ((count = ftell(fileBuf)) == -1)
    {
        goto error;
    }

    if (count > 128)
    {
        count = 128;
    }

    if (fseek(fileBuf, 0-count, SEEK_END) != 0)
    {
        goto error;
    }
    // get the new position
    if ((total = ftell(fileBuf)) == -1)
    {
        goto error;
    }

    // and read the data from fileBuf
    count = fread(tmpBuf, 1, sizeof(tmpBuf), fileBuf);
    tmpBuf[count] = '\0';
    //printf("count=%ld<br>",count);
    // determine offset of terminating boundary line
    for (i=0; i<=(count); i++)//-(long)strlen(Boundary)
    {
        //printf("%c",tmpBuf[i]);
        if (tmpBuf[i] == Boundary[0])
        {
            //printf("found /r<br>");
            if(strncmp(Boundary, &tmpBuf[i], strlen(Boundary)) == 0)
            {
            total+=i;
        //  printf("find boudary.<br>");
            break;
            }
        }
    }
    //printf("<br>i=%ld<br>",i);
    //printf("total=%ld<br>",total);
    if (fseek(fileBuf,total, SEEK_SET) != 0)
    {
        goto error;
    }

    if ((total = ftell(fileBuf)) == -1)
    {
    goto error;
    }
    //printf("total=%ld<br>",total);
    // truncate the terminating boundary line .
    int fd=fileno(fileBuf);
    ftruncate(fd,total);

    fflush(fileBuf);
error:
    if (fileBuf != NULL)
    {
        fclose(fileBuf);
    }
    if(errorBuf[0]!='\0')
    {
        //打印信息到网页的隐藏的iframe中
    	printf("<script type='text/javascript' language='javascript'>alert('%s');parent.location.replace('system.cgi');</script>",errorBuf);
    }
    else
    {
#if 0
		printf("Content type: text/html\n\n");
		printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
		"<script>alert(\"升级文件上传成功！\");"
		"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
		"</head><body></body></html>");
		//return 0;
#endif

#if 1
		HB_S32 sockfd = -1;
		HB_S32 ret = 0;
		GN_NET_LAYER_OBJ buf_cmd;
		ret = create_socket_connect_ipaddr(&sockfd, "127.0.0.1", MAIN_CTRL_PORT, 5);
		if (HB_FAILURE == ret)
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"盒子内部通信错误(-200)！\");"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}

		memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
		buf_cmd.iProtocolVer = 1;
		buf_cmd.iDataType = 9;
		buf_cmd.iBlockHeadFlag = 1;
		buf_cmd.iBlockEndFlag = 1;

		sprintf(buf_cmd.cBuffer, "<TYPE>ManualUpgrade</TYPE>");
		char tmpbuf[512]={0};
		memcpy(tmpbuf, buf_cmd.cBuffer, strlen(buf_cmd.cBuffer));

		buf_cmd.iActLength = 5*sizeof(HB_S32) + strlen(buf_cmd.cBuffer);
		WRITE_LOG("Send to ipcam_main_bak : [%s], Send size=[%d]\n", buf_cmd.cBuffer, buf_cmd.iActLength);
//		send(sockfd, (char*)&send_cmd, send_cmd.iActLength, 0);
    	ret = send_data(&sockfd, (char*)&buf_cmd, buf_cmd.iActLength, 5);
    	if(ret != 0)
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级设备失败(-201)！\");"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}

		memset(&buf_cmd, 0, sizeof(GN_NET_LAYER_OBJ));
		ret = recv_data(&sockfd, (char*)&buf_cmd, sizeof(GN_NET_LAYER_OBJ), 60);
		WRITE_LOG("Recv from ipcam_main_bak : [%s], Recv size=[%d]\n", buf_cmd.cBuffer, ret);
		if(ret <= 0)
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级设备失败(-202)！\");"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}

		if(!strcmp(buf_cmd.cBuffer,"<Return>0</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级设备成功！\\n点击确认，设备重启后重新登陆！\");"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-1</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"打开文件失败！\");"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-2</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"此升级文件与设备类型不匹配！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-3</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"此升级文件版本号小于等于当前设备版本号！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-4</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"创建临时文件夹失败！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-5</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级文件头标识不匹配！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-6</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级文件大小不匹配！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-7</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级文件校验失败！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-8</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"打开升级文件失败！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		else if(!strcmp(buf_cmd.cBuffer,"<Return>-9</Return>"))
		{
			printf("Content type: text/html\n\n");
			printf("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
			"<script>alert(\"升级设备失败！\");</script>"
			"</script><meta http-equiv=\"Refresh\" content=\"0;URL=../system.html\" />"
			"</head><body></body></html>");
			close(sockfd);
			return 0;
		}
		close(sockfd);
#endif

    }
    return 0;
}
