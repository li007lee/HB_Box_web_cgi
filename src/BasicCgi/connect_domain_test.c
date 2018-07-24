/*
 * connect_domain_test.c
 *
 *  Created on: 2015年4月3日
 *      Author: root
 */
#include "my_include.h"
#include "connect_domain_test.h"

static sigjmp_buf sig_jmp_buf;

static void alarm_func()
{
    siglongjmp(sig_jmp_buf, 1);
}

static struct hostent *gethostbyname_timeout(const char *domain, int timeout)
{
#if 1
//	struct ifaddrs *ifaddr, *ifa;
//	int family, s;
	char host[NI_MAXHOST];
    struct hostent *ipHostent = NULL;
    signal(SIGALRM, alarm_func);
    if(sigsetjmp(sig_jmp_buf, 1) != 0)
    {
        alarm(0);//timout
        signal(SIGALRM, SIG_IGN);
        return NULL;
    }
    alarm(timeout);//setting alarm
    ipHostent = gethostbyname(domain);
//    s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
//	if (s != 0) {
//		WRITE_LOG("getnameinfo() failed: %s\n", gai_strerror(s));
//		return NULL;
//	}
//	WRITE_LOG("\taddress: <%s>\n", host);
    signal(SIGALRM, SIG_IGN);
    return ipHostent;
#endif
}



static HB_S32 get_host_name(HB_CHAR *argv, HB_CHAR (*ip)[IP_HORIZONTAL], HB_S32 ip_vertical, HB_S32 timeout)
{
	HB_S32 i = 0;
	HB_CHAR *ptr,**pptr;
	struct hostent *hptr;
	HB_CHAR str[32];
	/* 取得命令后第一个参数，即要解析的域名或主机名 */
	ptr = argv;

	if(ip == NULL)
	{
		return -1;
	}
	res_init();
	/* 调用gethostbyname()。调用结果都存在hptr中 */
	if((hptr = gethostbyname_timeout(ptr, timeout)) == NULL)
	{
		//printf("\nuuuuuuuuuuuuuuuuuuuPPPPPPPP\n");
		//printf("gethostbyname error for host:%s\n", ptr);
		return -1; /* 如果调用gethostbyname发生错误，返回1 */
	}

	/* 将主机的规范名打出来 */
	//printf("official hostname:%s\n",hptr->h_name);
	/* 主机可能有多个别名，将所有别名分别打出来 */
//	for(pptr = hptr->h_aliases; *pptr != NULL; pptr++)
	//printf(" alias:%s\n",*pptr);
	/* 根据地址类型，将地址打出来 */
	switch(hptr->h_addrtype)
	{
		case AF_INET:
		case AF_INET6:
		pptr=hptr->h_addr_list;
		/* 将刚才得到的所有地址都打出来。其中调用了inet_ntop()函数 */
		for(; *pptr != NULL; pptr++)
		{
			memset(str, 0, sizeof(str));
//			printf("1: address:%s\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)));
			if(inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)) != NULL)
			{
				if(i < ip_vertical)
				{
//					printf("2: address:%s\tsize = %d\n", inet_ntop(hptr->h_addrtype, *pptr, str, sizeof(str)), size);
					memcpy(ip, str, strlen(str));
				//	printf( "ip gethostbyname=[%s]", str);
					ip++;
				}
				i++;
			}
		}
		break;
		default:
			//printf("unknown address type\n");
		break;
	}
	return i;
}

static HB_S32 read_server_ip_list(HB_CHAR *domain,HB_CHAR *srv_ip, HB_S32 timeout)//读取服务器列表，预留
{
	//printf("\n#################  read_server_ip_list()\n");
	HB_S32 i = 0;
    HB_S32 iprtn_count = 0;
    HB_CHAR ip_str[IP_VERTICAL][IP_HORIZONTAL];

    memset(ip_str, 0, sizeof(ip_str));

	iprtn_count = get_host_name(domain, ip_str, IP_VERTICAL, timeout);
	if(iprtn_count <= 0)
	{
		return -1;
	}
	for(i = 0; i< IP_VERTICAL; i++)
	if(strlen(ip_str[i]) > 0)
	{
		//printf("\nNNNNNNNNNN   ip = %s\n",ip_str[i]);
	}
	//printf("ip get success!ip:[%s],port:[%d]================>\n",ip_str[0], TEST_PORT);

	//printf("\n######## LLLL  srv_ip len= %d , ipstr_len = %d   ipstr=%s\n",sizeof(srv_ip), strlen(ip_str[0]), ip_str[0]);
	strcpy(srv_ip, ip_str[0]);

    return iprtn_count;
}


static HB_S32 connect_noblock(HB_S32 sockfd, struct sockaddr *addr, socklen_t salen, HB_S32 nsec)
{
    HB_S32 flags;
    HB_S32 n;
    HB_S32 error = 0;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;
    flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
    if ((n = connect(sockfd, addr, salen)) < 0)
    {
        if (errno != EINPROGRESS)
        {
            return HB_CONNECT_ERR;
        }
    }

    if (0 == n)
    {
        goto done;					// connect completed immediately
    }

    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);
    wset = rset;
    tval.tv_sec = nsec;
    tval.tv_usec = 0;
    if (0 == (n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)))	// timeout
    {
        return HB_CONNECT_TIME_OUT;
    }

    if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
    {
        len = sizeof(error);
        if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            return HB_CONNECT_ERR;
        }
    }
    else
    {
        //printf("select error:sockfd no set!\n");
    }

done:
//    fcntl(sockfd, F_SETFL, flags);	// restrore file status flags
    if (error)
    {
        return HB_CONNECT_ERR;
    }

    return HB_SUCCESS;
}

//返回0表示连接成功，其他值为连接错误
static HB_S32 pt_connect(HB_S32 *psockfd, HB_CHAR *addr, HB_S32 port, HB_S32 waitsec)
{
	HB_S32 ret = -1;
    HB_S32 trueflag = 1;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    setsockopt(*psockfd, SOL_SOCKET, SO_REUSEADDR, (HB_VOID *)&trueflag, sizeof(trueflag));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_port = htons(port);
    memset(server_addr.sin_zero, 0, 8);

    ret = connect_noblock(*psockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in), waitsec);

    return ret;
}

//通过域名和端口测试联通性
HB_S32 connect_test(HB_CHAR *domain, HB_S32 port, HB_S32 timeout)
{
	HB_S32 psockfd;
    HB_CHAR ipaddr[32] = {0};
    HB_S32 ret = -1;

	if((psockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	    return -3;
	}

	memset(ipaddr, 0, sizeof(ipaddr));
	if(read_server_ip_list(domain, ipaddr, timeout) < 0)
	{
		if(psockfd > 0)
		{
			close(psockfd);
			psockfd = -1;
		}
		return -2;
	}
	//printf("\nMMMMMMMMMMMMMMM\n");

	ret = pt_connect(&psockfd, ipaddr, port, timeout);
	if(ret != 0)
	{
		if(psockfd > 0)
		{
			close(psockfd);
			psockfd = -1;
		}
		return -2;
	}
	close(psockfd);
	psockfd = -1;
	return 1;
}


//通过IP和端口测试联通性
HB_S32 connect_ip_port_test(HB_CHAR *ipaddr, HB_S32 port, HB_S32 timeout)
{
	HB_S32 psockfd;

    HB_S32 ret = -1;

	if((psockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	    return -3;
	}

	ret = pt_connect(&psockfd, ipaddr, port, timeout);
	if(ret != 0)
	{
		if(psockfd > 0)
		{
			close(psockfd);
			psockfd = -1;
		}
		return -2;
	}
	close(psockfd);
	psockfd = -1;
	return 1;
}
