//
//  ipcheck.h
//  Mapport
//
//  Created by MoK on 15-2-14.
//  Copyright (c) 2015骞� MoK. All rights reserved.
//

#ifndef __Mapport__ipcheck__
#define __Mapport__ipcheck__

#include <stdio.h>

#endif /* defined(__Mapport__ipcheck__) */
#include <stdio.h>
#include <stdlib.h>
int isvalidstr(char *buf,int *pAddr);
void checkip_abcd(char *addr);
int ip_rightful_check(char *inip);
int ip_gate_mask_check(char *ipaddr, char *gateway, char *ipmask);
int mask_rightful_check(char *inip);
int gateway_rightful_check(char *inip);
