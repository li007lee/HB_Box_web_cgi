#ifndef SYSTEM_OPT_H
#define SYSTEM_OPT_H

#include "my_include.h"
#include "net_api.h"
#include "get_box_pair_config.h"
#include "cgi.h"
#include "session.h"
#include "system_private.h"
#include "sqlite3.h"
#include "common.h"


typedef struct _tagSYSTEM_WEB_DATA
{
	//char net_mod[4];//网络穿透模式 GN-天联，VV-威威，默认是开启天联
	HB_CHAR adm_usr_name[32];//用户名
	HB_CHAR adm_usr_passwd[64];//密码，md5加密过
	HB_CHAR lan_pair_mod[6];//局域网配对开关 ON-开，OFF-关
	HB_CHAR wan_pair_mod[6];//广域网配对开关 ON-开，OFF-关
	HB_CHAR lan_pair_code[64];//局域网配对码
	HB_CHAR wan_pair_code[64];//广域网配对码
}SYSTEM_WEB_DATA_OBJ, *SYSTEM_WEB_DATA_HANDLE;

//恢复出厂设置
HB_S32 FactroyReset();

//数据分析（抓包）
HB_S32 TcpdumpCapture(HB_CHAR *buf);

//修改账户密码
HB_S32 ChangeUserPwd(HB_CHAR *src_buf);

//获取盒子版本号
HB_S32 GetBoxVersion();

//设置云端验证是否开启
HB_S32 SetCloudVerifyStatus(HB_CHAR *src_buf);

//设置是否关闭广域网
HB_S32 SetWanConnection(HB_CHAR *src_buf);

#endif
