#include "my_include.h"
#include "common.h"
#include "stream_communicate.h"

int	SendData(int m_hSocket, char *pSrc, int iLength,int iDataType)
{
	//m_sRecvXmlData="";//一次请求前，清空字符串XML，避免累加.
	NET_LAYER	_NetLayer;
	_NetLayer.byProtocolType=0;			//协议类型
	_NetLayer.byProtocolVer=9;			//协议版本
	_NetLayer.byDataType=iDataType;		//数据类型,手机通讯时,DATA_TYPE_REAL_XML:9:交互命令,DATA_TYPE_SMS_CMD:12:云台控制命令,DATA_TYPE_SMS_MEDIA:13:流媒体数据
	_NetLayer.byFrameType=0;			//帧类型
	_NetLayer.iTimeStampHigh=0;			//时间戳
	_NetLayer.iTimeStampLow=0;			//时间戳
	_NetLayer.iVodFilePercent=0;		//VOD进度,默认值
	_NetLayer.iVodCurFrameNo=0;			//VOD当前帧,默认值

	int iSendBytes=0;
	int i;
	char *pSrcOffset;
	int iSplit;				//如果大于8K，拆分的包数
	int iLastBlockLength;	//拆分后，前面包有效数据长度为8K，最后一包的长度
	if (iLength%NET_BUFFER_LEN==0)
	{
		iSplit=iLength/NET_BUFFER_LEN;
		iLastBlockLength=NET_BUFFER_LEN;
	}
	else
	{
		iSplit=(iLength+NET_BUFFER_LEN)/NET_BUFFER_LEN;
		iLastBlockLength=iLength%NET_BUFFER_LEN;
	}
	for(i=0;i<iSplit;i++)
	{
		pSrcOffset=pSrc+i*NET_BUFFER_LEN;
		if (i==iSplit-1)//最后一包
		{
			_NetLayer.iActLength=PACKAGE_EXTRA_LEN+iLastBlockLength;
			memcpy(&_NetLayer.cBuffer,pSrcOffset,iLastBlockLength);
			if(iSplit==1)
			{
				_NetLayer.byBlockHeadFlag=TRUE;
				_NetLayer.byBlockEndFlag=TRUE;
			}
			else
			{
				_NetLayer.byBlockHeadFlag=FALSE;
				_NetLayer.byBlockEndFlag=TRUE;
			}
		}
		else//前面的包
		{
			_NetLayer.iActLength=Net_LAYER_STRUCT_LEN;
			memcpy(_NetLayer.cBuffer,pSrcOffset,NET_BUFFER_LEN);
			if(i==0)
			{
				_NetLayer.byBlockHeadFlag=TRUE;
				_NetLayer.byBlockEndFlag=FALSE;
			}
			else
			{
				_NetLayer.byBlockHeadFlag=FALSE;
				_NetLayer.byBlockEndFlag=FALSE;
			}
		}
		iSendBytes=send(m_hSocket,(char *)&_NetLayer,_NetLayer.iActLength,0);
		if(iSendBytes<=0)//2011-12-26更改，ARP冲突，有可能数据发送为0
		{
			return -1;
		}
	}
	return iSendBytes;
}


int	RecvData(int m_hSocket, NET_LAYER *m_package, int size, int i_timeout)
{
	int i_recv_bytes = 0;
	int iSelectReturn;
	fd_set fdRead;
	struct timeval seltime;

	seltime.tv_sec = i_timeout;
	seltime.tv_usec = 0;//10毫秒
	FD_ZERO(&fdRead);
	FD_SET(m_hSocket, &fdRead);
	iSelectReturn=select(m_hSocket+1,&fdRead,NULL, NULL,&seltime);
	if(iSelectReturn==-1)
	{
		//perror("select");
		return SELECT_ERROR;
	}
	else if (iSelectReturn)
	{
		i_recv_bytes=recv(m_hSocket,(char *)m_package, size,0);
		if(i_recv_bytes < 0)
		{
			i_recv_bytes=-1;
	        return RECV_ERROR;
		}
		else if (i_recv_bytes ==0)
		{
			//connection closed
			WRITE_LOG("######connection closed\n");
			return SOCKET_CLOSED;
		}
		else
		{
			//printf("recv_data : %s\n", m_package->cBuffer);
			WRITE_LOG("recv from stream:[%s]\n", m_package->cBuffer);
		}
		return i_recv_bytes;
	}
	else //timeout
	{
		return TIME_OUT;
	}

	return i_recv_bytes;
}

//获取gnLan登录信息
//返回 1 表示gnLan登录成功 , 返回 0 表示gnLan登录失败
HB_S32 get_sys_gnLan(HB_VOID)
{
	HB_CHAR tmp[128] = {0};
	FILE *fp;
	//fp = popen("ifconfig eth0 | awk '/Link encap:/ {print $5}'", "r");
	fp = popen("/sbin/ifconfig gnLan | grep Link", "r");
	if (fp == NULL) {
		WRITE_LOG("popen gnLan error!\n");
		return 0;
	}
	fgets(tmp, sizeof(tmp), fp);
	pclose(fp);

	WRITE_LOG("######gnlan string : [%s]\n", tmp);
	//TRACE_LOG("\n######  mac: %s\n", tmp);
	if (strlen(tmp)>0 && strstr(tmp, "Link encap:"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

