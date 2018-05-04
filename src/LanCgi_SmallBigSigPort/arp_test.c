/*
 * arp_test.c
 *
 *  Created on: 2017年7月28日
 *      Author: root
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <linux/if_arp.h>
#include <arpa/inet.h>



//根据网卡eth获取相应的地址
//返回值：-1获取失败，0获取成功
int get_src_ip(char *eth, struct in_addr *src)
{
	int sock;
    struct ifreq req;
    struct  sockaddr_in my_addr;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        return -1;
    }
    strncpy(req.ifr_name, eth, IFNAMSIZ);
    if ( ioctl(sock, SIOCGIFADDR, &req) < 0 )
    {
        close(sock);
        return -1;
    }
    memcpy(&my_addr, &req.ifr_addr, sizeof(my_addr));
    memcpy(src, &(my_addr.sin_addr), sizeof(struct in_addr));
    close(sock);
    return 0;
}



int check_device(char* if_dev, int ss)
{
	int ifindex;
	struct ifreq ifr;
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, if_dev, IFNAMSIZ-1);
	if (ioctl(ss, SIOCGIFINDEX, &ifr) < 0)
	{
		//printf("\n##### arping: unknown iface %s\n", if_dev);
		return -1;
	}
	ifindex = ifr.ifr_ifindex;
	if (ioctl(ss, SIOCGIFFLAGS, (char*)&ifr))
	{
		//printf("\n##### ioctl(SIOCGIFFLAGS)\n");
		return -1;
	}
	if (!(ifr.ifr_flags&IFF_UP))
	{
		//printf("\n##### Interface \"%s\" is down\n", if_dev);
		return -1;
	}
	if (ifr.ifr_flags&(IFF_NOARP|IFF_LOOPBACK))
	{
		//printf("\n##### Interface \"%s\" is not ARPable\n", if_dev);
		return -1;
	}
	return ifindex;
}



int socket_init(char *ethx, struct sockaddr_ll *me, struct sockaddr_ll *he)
{
	int sock_fd,  s_errno;
	sock_fd = socket(PF_PACKET,  SOCK_DGRAM,  0);
	s_errno = errno;
	me->sll_family = AF_PACKET;
	me->sll_ifindex = check_device(ethx,  sock_fd);
	if(me->sll_ifindex < 0)
	{
		return -4;
	}
	me->sll_protocol = htons(ETH_P_ARP);
	if (bind(sock_fd,  (struct sockaddr*)me,  sizeof(struct sockaddr_ll)) == -1)
	{
		//printf("\n#### bind() err\n");
		return -1;
	}
	int alen = sizeof(struct sockaddr_ll);
	if (getsockname(sock_fd, (struct sockaddr*)me, &alen) == -1)
	{
		//printf("\n##### getsockname() err");
		return -2;
	}
	if (me->sll_halen == 0)
	{
		//printf("\n#### Interface \"%s\" is not ARPable (no ll address)\n", ethx);
		return -3;
	}
	//he = me;
	memcpy(he, me, sizeof(struct sockaddr_ll));
	memset(he->sll_addr, -1, he->sll_halen);  // set dmac addr FF:FF:FF:FF:FF:FF
	return sock_fd;
}



int create_pkt(unsigned char * buf, struct in_addr src, struct in_addr dst, struct sockaddr_ll * FROM, struct sockaddr_ll * TO)
{
	struct arphdr *ah = (struct arphdr*) buf;
	unsigned char *p = (unsigned char *)(ah+1);
	ah->ar_hrd = htons(FROM->sll_hatype);
	if (ah->ar_hrd == htons(ARPHRD_FDDI))
	{
		ah->ar_hrd = htons(ARPHRD_ETHER);
	}

	ah->ar_pro = htons(ETH_P_IP);
	ah->ar_hln = FROM->sll_halen;
	ah->ar_pln = 4;
	ah->ar_op  =  htons(ARPOP_REQUEST);
	memcpy(p, &FROM->sll_addr, ah->ar_hln);
	p+=FROM->sll_halen;
	memcpy(p, &src, 4);
	p+=4;
	memcpy(p, &TO->sll_addr, ah->ar_hln);
	p+=ah->ar_hln;
	memcpy(p, &dst, 4);
	p+=4;
	memcpy(p, "aaaaaaaaaaaa", strlen("aaaaaaaaaaaa") );
	p+=12;
	return  (p-buf);
}


int send_pkt(int socket_id, struct in_addr *dst, struct in_addr *src, struct sockaddr_ll *me, struct sockaddr_ll *he, struct timeval *send_time)
{
	unsigned char send_buf[256];
	int pkt_size = create_pkt(send_buf,  *src,  *dst,  me,  he);
	gettimeofday(send_time, NULL);
	int cc = sendto(socket_id, send_buf, pkt_size, 0, (struct sockaddr*)he, sizeof(struct sockaddr_ll));
	if( cc == pkt_size )
	{
		return 0;
	}
	return -1;
}


int chk_recv_pkt(struct in_addr *dst, struct in_addr *src,unsigned char * buf, struct sockaddr_ll * FROM, struct sockaddr_ll *me)
{
	struct arphdr *ah = (struct arphdr*)buf;
	unsigned char *p = (unsigned char *)(ah+1);
	struct in_addr src_ip, dst_ip;
	if (ah->ar_op != htons(ARPOP_REQUEST) &&  ah->ar_op != htons(ARPOP_REPLY))
	{
		return 0;
	}
	if (ah->ar_pro != htons(ETH_P_IP) || ah->ar_pln != 4 || ah->ar_hln != me->sll_halen )
	{
		return 0;
	}

	memcpy(&src_ip, p+ah->ar_hln, 4);
	memcpy(&dst_ip, p+ah->ar_hln+4+ah->ar_hln, 4);
	if (src_ip.s_addr != dst->s_addr || src->s_addr != dst_ip.s_addr )
	{
		return 0;
	}
	return (p-buf);
}


void disp_info(struct in_addr dst, int msecs, int usecs, struct sockaddr_ll from)
{
	////printf("%03d ", received);
	//printf("%s ", from.sll_pkttype==PACKET_HOST ? "Unicast" : "Broadcast");
	//printf("%s from %s",  "reply",   inet_ntoa(dst) );
	//printf(" [%02X:%02X:%02X:%02X:%02X:%02X] ", from.sll_addr[0], from.sll_addr[1], \
	  from.sll_addr[2], from.sll_addr[3], from.sll_addr[4], from.sll_addr[5]);
	//printf(" %ld.%ld ms\n", (long int)msecs, (long int)usecs);
	fflush(stdout);
}



////////////////////////////////////////////////////////////////////////////////
// 函数名：arp_test_ip
// 描述：ip冲突检测
// 参数：
//  ［IN］ethx - 本地所用的网卡名
//  ［IN］ip_addr -要探测的ip地址
//	[IN]timeout  探测超时，单位秒
// 返回值：0-探测的ip可用;  1-探测的ip局域网内存在，请使用其他ip;   小于0-探测失败。
////////////////////////////////////////////////////////////////////////////////
int arp_test_ip(char *ethx, char *ip_addr, int timeout)
{
	int ret = 0;
	int socket_fd = -1;
	struct sockaddr_ll   me,  he;
	struct in_addr  src, dst;
	struct timeval   send_time, recv_time;
	uid_t uid = getuid();
	setuid(uid);

	if (inet_aton(ip_addr, &dst) != 1)
	{
		struct hostent *hp;
		hp = gethostbyname2(ip_addr, AF_INET);
//		//printf("\ntarget = %s \n", ip_addr );
		if (!hp)
		{
//			//printf("\n###### arping: unknown host %s\n", ip_addr);
			return -3;
		}
		memcpy(&dst, hp->h_addr, 4);
	}
	ret =  get_src_ip(ethx, &src);
	if (!src.s_addr )
	{
//		//printf("\n###### arping: no source address in not-DAD mode\n");
		return -4;
	}
	socket_fd = socket_init(ethx, &me, &he);
	if(socket_fd <= 0)
	{
		return -5;
	}
//	//printf("\nARPING %s ", inet_ntoa(dst));
//	//printf("from %s %s\n\n",  inet_ntoa(src), ethx ? : "");
	ret = send_pkt(socket_fd, &dst, &src, &me, &he, &send_time);
	if(ret < 0)
	{
		return -6;
	}
	struct timeval wait_time = {0, 0};
	int recv_count = 0;
	fd_set fdread;
	wait_time.tv_sec  = timeout;
	wait_time.tv_usec = 0;
	while(1)
	{
		if(recv_count >= 2)
		{
//			//printf("\n##### The IP can use!\n");
			return 0;
		}
	    FD_ZERO(&fdread);
	    FD_SET(socket_fd, &fdread);
		ret = select(socket_fd + 1, &fdread, NULL, NULL, &wait_time);
	    if (0 == ret)		// 超时
	    {
//	    	//printf("\n##### select timeout!\n");
	        return 0;
	    }
	    else if (ret < 0)	// 出错
	    {
//	    	//printf("\n##### select error!\n");
	        return -2;
	    }
		struct sockaddr_ll from;
		int alen = sizeof(from);
		char recv_buf[1024]={0};

		int recv_size = recvfrom(socket_fd, recv_buf,  sizeof(recv_buf), 0,  (struct sockaddr *)&from, &alen );
//		//printf("\n############# recvfrom() recv_size=%d\n", recv_size);

		gettimeofday(&recv_time, NULL);
		if( recv_size < 0 )
		{
			perror("arping: recvfrom");
			continue;
		}
		if( chk_recv_pkt(&dst, &src, recv_buf, &from, &me) > 0 )
		{
			memcpy(he.sll_addr, from.sll_addr, he.sll_halen);
			long usecs, msecs;
			if (recv_time.tv_sec)
			{
				usecs = (recv_time.tv_sec - send_time.tv_sec) * 1000000 + recv_time.tv_usec - send_time.tv_usec;
				msecs = (usecs+500)/1000;
				usecs -= msecs*1000 - 500;
			}
			disp_info(dst,  msecs,  usecs,  from);
			////printf("\n##### The ip has been used, please use other ip!\n");
			return 1;
		}
		recv_count++;
	}
	return 0;
}


//int main()
//{
//	int ret = 0;
//	ret = arp_test_ip("eth0", "192.168.8.1");
//	//printf("\n>>>>>>  ret=%d\n", ret);
//	return 0;
//}
