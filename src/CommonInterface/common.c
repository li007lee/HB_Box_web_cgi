/***********************************************
 * @file common.c
 * @Func 通用函数接口库
 * @author lijian
 * @date 2016-07-4
************************************************/

#include <stdio.h>
#include <time.h>

#include "session.h"
#include "common.h"
#include "cgi.h"

typedef struct _tagSYSTEM_WEB_DATA
{
	HB_CHAR adm_usr_name[32];//用户名
	HB_CHAR adm_usr_passwd[64];//密码，md5加密过
}SYSTEM_WEB_DATA_OBJ, *SYSTEM_WEB_DATA_HANDLE;

SYSTEM_WEB_DATA_OBJ sys_web_data;

/***********************************
 * Function:错误日志接口
 * args:
 * 		@buf: 报错内容
 * 		@buf_len: 错误日志长度
 * return：no return
 ***********************************/

HB_VOID WriteLog(const HB_CHAR *fmt, ...)
{
	HB_CHAR print_buf[4096] = {0};
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(print_buf, 4096, fmt, ap);
	va_end(ap);

	FILE *fp = fopen(LOG_PATH, "a+");
	if (fp == NULL)
	{
		return ;
	}
	fwrite(print_buf, 1, strlen(print_buf), fp);
	fclose(fp);
}

/****************************************用于计算是否登录超时****************************************/
/****************************************用于计算是否登录超时****************************************/
/****************************************用于计算是否登录超时****************************************/
HB_S32 CalcTimeOut()
{
	time_t cur_time = 0;
	time_t time_difference = 0;
	HB_CHAR lasttime[32] = {0};
	HB_CHAR cur_time_str[32] = {0};

	//计算超时
	cur_time = time(NULL);
	memcpy(lasttime, cgi_session_var("LAST_ACCESS_TIME"), strlen(cgi_session_var("LAST_ACCESS_TIME")));

	time_difference = cur_time - atol(lasttime);

	if ( time_difference > SESSION_TIMEOUT) //超时
	{
		if (time_difference > 157680000) {  //如果时间差大于5年，说明盒子刚刚更新过系统时间，则不算超时（157680000为5年的秒数）
			//没有超时更新最后访问时间
			sprintf(cur_time_str, "%ld", cur_time);
			cgi_session_alter_var("LAST_ACCESS_TIME", cur_time_str);

			return 0;
		}

		cgi_session_destroy();
		WRITE_LOG("cur_time:[%ld]\nlasttime:[%ld]\nLogin Timeout!\n", cur_time, atol(lasttime));

		return 1;
	}
	//没有超时更新最后访问时间
	sprintf(cur_time_str, "%ld", cur_time);
	//注意！！cgi_session_alter_var接口会发送text/html头，所以后面再向浏览器发数据时，不能再发送头信息
	//注意！！cgi_session_alter_var接口会发送text/html头，所以后面再向浏览器发数据时，不能再发送头信息
	//注意！！cgi_session_alter_var接口会发送text/html头，所以后面再向浏览器发数据时，不能再发送头信息
	//注意！！cgi_session_alter_var接口会发送text/html头，所以后面再向浏览器发数据时，不能再发送头信息
	cgi_session_alter_var("LAST_ACCESS_TIME", cur_time_str);

	return 0;
}
/****************************************用于计算是否登录超时END****************************************/
/****************************************用于计算是否登录超时END****************************************/
/****************************************用于计算是否登录超时END****************************************/

/****************************************校验用户名及密码****************************************/
/****************************************校验用户名及密码****************************************/
/****************************************校验用户名及密码****************************************/
static HB_S32 load_sys_web_usr_passwd( void * para, int n_column, char ** column_value, char ** column_name )
{
	memset(&sys_web_data, 0, sizeof(SYSTEM_WEB_DATA_OBJ));
	strncpy(sys_web_data.adm_usr_name, column_value[0], strlen(column_value[0]));
	strncpy(sys_web_data.adm_usr_passwd, column_value[1], strlen(column_value[1]));
	return 0;
}

HB_BOOL CheckUsrPasswd()
{
	HB_CHAR *sql = "SELECT adm_usr_name, adm_usr_passwd FROM system_web_data";
	HB_CHAR user[128]={0};
	HB_CHAR passwd[128]={0};
	HB_CHAR outbuf[1024] = {0};
	HB_CHAR check_buf[1024] = {0};

	memcpy(user,cgi_session_var("USER_NAME"),strlen(cgi_session_var("USER_NAME")));
	memcpy(passwd,cgi_session_var("PASSWORD"),strlen(cgi_session_var("PASSWORD")));

	//strncpy(sql, "SELECT adm_usr_name, adm_usr_passwd FROM system_web_data", sizeof(sql));
	SqlOperation(sql, BOX_DATA_BASE_NAME, load_sys_web_usr_passwd, NULL);

	sprintf(outbuf, "%s:%s", sys_web_data.adm_usr_name, sys_web_data.adm_usr_passwd);
	sprintf(check_buf, "%s:%s", user, passwd);//读出session里到用户名和密码

	if( !strncmp(check_buf, outbuf, strlen(check_buf)))//session和配置文件匹配
	{
		return HB_TRUE ;
	}

	WRITE_LOG("sql  :[%s]\nsesion:[%s]\nsesion compare failed!\n");

	return HB_FALSE;
}
/****************************************校验用户名及密码END****************************************/
/****************************************校验用户名及密码END****************************************/
/****************************************校验用户名及密码END****************************************/


/****************************************编解码****************************************/
/****************************************编解码****************************************/
/****************************************编解码****************************************/
static HB_S32 url_htoi(HB_CHAR *s)
{
	HB_S32 value;
	HB_S32 c;

    c = ((HB_U8 *)s)[0];
    if (isupper(c))
    {
    	c = tolower(c);
    }
    value = (c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10) * 16;
    c = ((HB_U8 *)s)[1];
    if (isupper(c))
    {
    	c = tolower(c);
    }
    value += c >= '0' && c <= '9' ? c - '0' : c - 'a' + 10;
    return (value);
}

HB_CHAR *url_encode(char const *s, int len, int *new_length)
{
    register HB_CHAR c;
    HB_CHAR *to, *start;
    HB_CHAR const *from, *end;
    static HB_CHAR hexchars[] = "0123456789ABCDEF";

    from = (HB_CHAR *)s;
    end  = (HB_CHAR *)s + len;
    start = to = (HB_CHAR *) calloc(1, 3*len+1);

    while (from < end)
    {
        c = *from++;
        if (c == ' ')
        {
            *to++ = '+';
        }
        else if ((c < '0' && c != '-' && c != '.') ||
                 (c < 'A' && c > '9') ||
                 (c > 'Z' && c < 'a' && c != '_') ||
                 (c > 'z'))
        {
            to[0] = '%';
            to[1] = hexchars[c >> 4];
            to[2] = hexchars[c & 15];
            to += 3;
        }
        else
        {
            *to++ = c;
        }
    }
    *to = 0;
    if (new_length)
    {
        *new_length = to - start;
    }
    return (HB_CHAR *) start;
}


HB_S32 url_decode(HB_CHAR *str, HB_S32 len)
{
	HB_CHAR *dest = str;
	HB_CHAR *data = str;

    while (len--)
    {
        if (*data == '+')
        {
            *dest = ' ';
        }
        else if (*data == '%' && len >= 2 && isxdigit((HB_S32) *(data + 1)) && isxdigit((HB_S32) *(data + 2)))
        {
            *dest = (HB_CHAR) url_htoi(data + 1);
            data += 2;
            len -= 2;
        }
        else
        {
            *dest = *data;
        }
        data++;
        dest++;
    }
    *dest = '\0';
    return dest - str;
}
/****************************************编解码End****************************************/
/****************************************编解码End****************************************/
/****************************************编解码End****************************************/

/****************************************获取DataCode****************************************/
/****************************************获取DataCode****************************************/
/****************************************获取DataCode****************************************/
HB_S32 GetDataCode(HB_CHAR *buf)
{
	HB_CHAR *pos = NULL;
	HB_S32 i_data_code = 0;

	if (NULL == buf) {
		return -1;
	}

	pos = strstr(buf, "DataCode=");
	if (NULL == pos) {
		return -2;
	}
	pos += strlen("DataCode=");
	i_data_code = atoi(pos);

//	WRITE_LOG("DataCode [%d]!\n", i_data_code);

	return i_data_code;
}
/****************************************获取DataCode_End****************************************/
/****************************************获取DataCode_End****************************************/
/****************************************获取DataCode_End****************************************/


/****************************************数据库操作****************************************/
/****************************************数据库操作****************************************/
/****************************************数据库操作****************************************/
/***********************************
 * Function:数据库操作接口
 * args:
 * 		@sql: 需要执行的sql语句
 * 		@db_path_name: 需要操作的数据库
 * 		@callback:执行sql时的回调函数
 * 		@arg:回调函数的参数
 * return：成功返回0,失败返回负数
 ***********************************/
HB_S32 SqlOperation(HB_CHAR *sql, HB_CHAR *db_path_name, HB_S32 (*callback)(HB_VOID *, HB_S32, HB_CHAR **,HB_CHAR **), HB_VOID *arg)
{
	sqlite3 *db;
	HB_CHAR *errmsg = NULL;
	HB_S32 ret = 0;
	HB_S32 errcode = 0;

	//打开数据库
	ret = sqlite3_open(db_path_name, &db);
	if (ret != SQLITE_OK) {
		WRITE_LOG("open db [%s] failed(%d)!\n", db_path_name, ret);
		errcode = -1;
		goto Err;
	}

	ret = sqlite3_exec(db, sql, callback, arg, &errmsg);
	if (ret != SQLITE_OK) {
		WRITE_LOG("exec sql failed:[%s]\nerr message(%d):[%s]\n", sql, ret, errmsg);
		//设备id重复
		if(!strcmp(errmsg, "UNIQUE constraint failed: dev_add_web_data.dev_id"))
		{
			errcode = -2;
			goto Err;
		}
		errcode = -3;
		goto Err;
	}
Err:
	sqlite3_free(errmsg);
	sqlite3_close(db);

	return errcode;
}
/****************************************数据库操作END****************************************/
/****************************************数据库操作END****************************************/
/****************************************数据库操作END****************************************/


/****************************************CGI操作****************************************/
/****************************************CGI操作****************************************/
/****************************************CGI操作****************************************/

HB_S32 CgiInitAndConfig(HB_CHAR **buf, HB_CHAR *p_session_save_path)
{
	//HB_CHAR *buf = NULL;
	HB_CHAR *buf_len = NULL; //定义字符型指针
	HB_S32 len = 0;

	if (p_session_save_path == NULL)
	{
		WRITE_LOG("error: session file path name can't be NULL\n", buf);
		return -1;
	}

	cgi_init();
	cgi_session_cookie_name("MY_COOKIE");
	buf_len = getenv("CONTENT_LENGTH"); //获取输入字符长度
	if (buf_len != NULL)
	{
		sscanf(buf_len, "%d", &len);
		*buf = (HB_CHAR *) malloc(sizeof(HB_CHAR) * (len + 2));
		//dst_buf = (HB_CHAR *) malloc(sizeof(HB_CHAR) * (len + 2));
		fgets(*buf, len + 1, stdin);    //从标准输入中读取len个字符
		WRITE_LOG("recv from web :[%s]\n", *buf);
		//strncpy(dst_buf, buf, sizeof(dst_buf));
		//free(buf);
	}
	cgi_process_form();
	cgi_session_save_path(p_session_save_path);
	cgi_session_start();
	return 0;
}

/****************************************CGI操作END****************************************/
/****************************************CGI操作END****************************************/
/****************************************CGI操作作END****************************************/


//返回值：0成功 -1-2-3失败
HB_S32 my_system(HB_CHAR *cmd_str)
{
    pid_t status;
    status = system(cmd_str);
    if (-1 == status)
    {
        WRITE_LOG("system error!");
        return -1;
    }
    else
    {
        //printf("exit status value = [0x%x]\n", status);
        if (WIFEXITED(status))
        {
            if (0 == WEXITSTATUS(status))
            {
            	WRITE_LOG("run system cmd or shell script successfully.\n");
            }
            else
            {
            	WRITE_LOG("01 run system cmd or shell script fail, script exit code: %d\n", WEXITSTATUS(status));
                return -2;
            }
        }
        else
        {
        	WRITE_LOG("02 run system cmd or shell script fail, exit status = [%d]\n", WEXITSTATUS(status));
            return -3;
        }
    }

    return 0;
}
