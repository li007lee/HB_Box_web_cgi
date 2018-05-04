////////////////////////////////////////////////////////////////////////////////
// 版权所有，2009-2012，北京汉邦高科数字技术有限公司
// 本文件是未公开的，包含了汉邦高科的机密和专利内容
////////////////////////////////////////////////////////////////////////////////
// 文件名： fifo.c
// 作者：   乔勇
// 版本：   1.0
// 日期：   2013-09-02
// 描述：   信号量封装头文件
// 历史记录：
///////////////////////////////////////////////////////////////////////////////

#include "my_include.h"
#include "fifo.h"


////////////////////////////////////////////////////////////////////////////////
// 函数名：fifo_reset
// 描述：fifo重置
// 参数：
//    [in]FIFO_HANDLE handle - fifo结构体指针;
// 返回值：信号量id。
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_VOID fifo_reset(FIFO_HANDLE handle)
{
#ifdef DEBUG
	assert(NULL != handle);
#endif
	if (NULL == handle)
	{
		return;
	}
	handle->kb_num = 0;
	handle->act_link = 0;
	return;
}

////////////////////////////////////////////////////////////////////////////////
// 函数名：fifo_create
// 描述：fifo创建
// 参数：
//    [in]FIFO_HANDLE handle - fifo结构体指针;
// 返回值：信号量id。
// 说明：
////////////////////////////////////////////////////////////////////////////////
FIFO_HANDLE	fifo_create(HB_CHAR* pname)
{
#ifdef DEBUG
	assert(NULL != pname);
	//assert(bufsize > 0);
#endif
	if (NULL == pname)
	{
		return NULL;
	}

	HB_S32 	fd     = -1;
	HB_S32	maplen = 0;
	//HB_BOOL	binit  = HB_FALSE;

	FIFO_HANDLE handle = NULL;

	//binit  = access(pname, F_OK) == -1 ? HB_TRUE : HB_FALSE;
	maplen = sizeof(FIFO_OBJ);
	fd = open(pname, O_CREAT | O_RDWR, 0666);
	ftruncate(fd, maplen);

	lseek(fd, maplen - 1, SEEK_SET);
	write(fd, "\0", 1);
	lseek(fd, 0, SEEK_SET);

	handle = (FIFO_HANDLE)mmap(NULL, maplen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (NULL == handle)
	{
		//printf("ERROR: (%s|%s|%d) mmap failed\n", __FILE__, __func__, __LINE__);
		return NULL;
	}

	close(fd);

	//如果是第一次初始化

	//handle->kb_num = 0;
	//handle->act_link = 0;

	return handle;
}

////////////////////////////////////////////////////////////////////////////////
// 函数名：fifo_delete
// 描述：fifo删除
// 参数：[in]FIFO_HANDLE handle - fifo结构体指针;
//    [in]FIFO_HANDLE handle - fifo结构体指针;
// 返回值：信号量id。
// 说明：
////////////////////////////////////////////////////////////////////////////////
HB_S32 fifo_delete(FIFO_HANDLE handle)
{
#ifdef DEBUG
	assert(NULL != handle);
#endif
	if (NULL == handle)
	{
		return HB_FAILURE;
	}

	munmap(handle, sizeof(FIFO_OBJ));
	return HB_SUCCESS;
}



