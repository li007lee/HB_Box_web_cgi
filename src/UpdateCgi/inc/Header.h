//
//  Header.h
//  Mapport
//
//  Created by MoK on 15-3-4.
//  Copyright (c) 2015骞� MoK. All rights reserved.
//

#ifndef Mapport_Header_h
#define Mapport_Header_h
#define LOG_FILE    "/tmp/tmp_root/error.log"

#define MAX_CONN_NUM            128
#define MAPPORT_ARRAY           256
#define LISTTEN_SRV_PORT        8888

#define HBGK_DEV_MAPPORT        10000
#define HKVIS_DEV_MAPPORT       20000
#define DH_DEV_MAPPORT          30000
#define CXHZ_MAPPORT            40000

#define MSECOND                 1000
#define USECOND                 (1000*1000)
#define NSECOND                 (1000*1000*1000)

#include <stdio.h>
#include <pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<fcntl.h>
#include<netinet/in.h>
#include<signal.h>
#include<arpa/inet.h>
#include<sys/select.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include<signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

typedef struct _tagNET_CONF
{
	char ip_addr[16];
	char net_mask[16];
	char gate_way[16];
	char dns_main_ip[16];
	char dns_sub_ip[16];
}NET_CONF_OBJ, *NET_CONF_HANDLE;




#endif
