#include "my_include.h"


/*
 * my_arping.c
 *
 *  Created on: 2016-8-24
 *      Author: li007lee
 */

HB_S32 my_arping_check_ip(HB_CHAR *ethx, HB_CHAR *addr, HB_S32 timeout)
{
    FILE   *stream;
    HB_CHAR *pos = NULL;
    HB_CHAR   buf[1024];

    memset( buf, 0, sizeof(buf) );//初始化buf,以免后面写如乱码到文件中
    snprintf(buf, sizeof(buf), "arping -I %s -D %s -w %d", ethx, addr, timeout);
    stream = popen( buf, "r" ); //将“ls －l”命令的输出 通过管道读取（“r”参数）到FILE* stream

    memset( buf, 0, sizeof(buf) );
    fread( buf, sizeof(HB_CHAR), sizeof(buf),  stream);  //将刚刚FILE* stream的数据流读取到buf中
    pclose( stream );

    if (strstr(buf, "unknown host") != NULL)//ip格式有误
    {
    	return -1;
    } else if (strstr(buf, "reply from") != NULL) //ip冲突
    {
    	return 0;
    }

	//printf("###### [%s]\n", buf);



	return 1;
}

