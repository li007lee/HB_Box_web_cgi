/***********************************************
 * @file common.h
 * @Func 通用函数接口库
 * @author lijian
 * @date 2016-07-4
************************************************/

#ifndef COMMON_H
#define COMMON_H

#include "my_include.h"
#include "sqlite3.h"

/*
 * Function:错误日志接口
 * args:
 * 		@buf: 报错内容
 * 		@buf_len: 错误日志长度
 * return：no return
 */
HB_VOID WriteLog(const HB_CHAR *fmt, ...);

//用于计算是否登录超时
HB_S32 CalcTimeOut();
//判断用户名密码是否正确
HB_BOOL CheckUsrPasswd();

//字符串编码
HB_CHAR *url_encode(HB_CHAR const *s, HB_S32 len, HB_S32 *new_length);
//字符串解码
HB_S32 url_decode(HB_CHAR *str, HB_S32 len);

//获取指令代码
HB_S32 GetDataCode(HB_CHAR *buf);

/***********************************
 * Function:数据库操作接口
 * args:
 * 		@sql: 需要执行的sql语句
 * 		@db_path_name: 需要操作的数据库
 * 		@callback:执行sql时的回调函数
 * 		@arg:回调函数的参数
 * return：成功返回0,失败返回负数
 ***********************************/
HB_S32 SqlOperation(HB_CHAR *sql, HB_CHAR *db_path_name, HB_S32 (*callback)(HB_VOID *, HB_S32, HB_CHAR **,HB_CHAR **), HB_VOID *arg);

//Cgi初始化以及配置操作
HB_S32 CgiInitAndConfig(HB_CHAR **buf, HB_CHAR *p_session_save_path);

HB_S32 my_system(HB_CHAR *cmd_str);
#endif
