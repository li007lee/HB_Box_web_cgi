#ifndef BASIC_OPERATION_H
#define BASIC_OPERATION_H

#define SEND_JSON_BUF_LEN (15360)//(1024*15)
#define TCP_LIST_JSON_BUF_LEN (7680)//(15360/2)
#define RTSP_LIST_JSON_BUF_LEN (7680)//(15360/2)

//获取网络状态，盒子序列号，天联ip
//HB_VOID * GetNetStatus(HB_VOID *arg);
HB_VOID * GetNetStatus(HB_VOID *arg, HB_S32 i_data_size);
HB_VOID * GetNetStatus2(HB_VOID *arg);

/*****************一点通盒子相关*****************/
//获取一点通盒子列表
HB_S32 GetBasicBoxDevList(BOX_PAIR_HEAD_HANDLE box_dev);
//组织一点通盒子列表json串
HB_S32 MakeJsonBox(HB_CHAR *box_list_buf, HB_S32 box_list_buf_len, BOX_PAIR_DEV_LIST_HANDLE box_dev_flag);
/*****************一点通盒子相关END*****************/


/*****************tcp设备相关*****************/
//获取tcp设备列表
HB_S32 GetBasicTcpDevList(TCP_PAIR_HEAD_HANDLE tcp_dev);
//组织tcp列表json
HB_S32 MakeJsonTcp(HB_CHAR *tcp_list_buf, HB_S32 tcp_list_buf_len, TCP_PAIR_HEAD_HANDLE tcp_dev_flag);
/*****************tcp设备相关END*****************/

/*****************rtsp设备相关*****************/
//获取RTSP设备列表
HB_VOID GetBasicRtspDevList(RTSP_DEV_LIST_HANDLE pRtspDevList);
//组织rtsp列表json
HB_S32 MakeJsonRtsp(HB_CHAR *pRtspListBuf, HB_S32 iRtspListBufLen, RTSP_DEV_LIST_HANDLE pRtspDev);
/*****************rtsp设备相关END*****************/

#endif
