//
//  ipcheck.c
//  Mapport
//
//  Created by MoK on 15-2-14.
//  Copyright (c) 2015年 MoK. All rights reserved.
//
#include "ipcheck.h"
/*该函数能够验证合法的ip地址，ip地址中可以有前导0，也可以有空格*/

//#define DEBUG_PRINT printf
#define NO_DEBUG
#ifdef DEBUG
#define COLOR_STR_NONE          "\033[0m"
#define COLOR_STR_YELLOW      "\033[1;33m"


#define DEBUG_PRINT(str, args...)  printf(COLOR_STR_YELLOW  str COLOR_STR_NONE, ## args);
#else
#define DEBUG_PRINT(str, args...)   do{} while(0)
#endif /* ERR_DEBUG */



int ip_rightful_check(char *inip)
{
    int part1, part2, part3, part4;
    char tail = 0;
    int field;
    
    if(inip == NULL)
    {
        return 0;
    }
    field=sscanf(inip, "%d . %d . %d . %d %c", &part1, &part2, &part3, &part4, &tail);
    if(field < 4 || field > 5)
    {
        DEBUG_PRINT("expect 4 field,get %d\n", field);
        return 0;
    }
    if(tail != 0)
    {
        DEBUG_PRINT("ip address mixed with non number\n");
        return 0;
    }
    if( (part1 > 0 && part1 < 255 && part1!=127) && (part2>=0 && part2<=255) &&  (part3>=0 && part3<=255) &&  (part4>0 && part4<255) )
    {
        //inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
        return 1;
    }
    else
    {
        DEBUG_PRINT("not good ip %d:%d:%d:%d\n",part1,part2,part3,part4);
    }
    return 0;
}

int mask_rightful_check(char *inip)
{
    int part1, part2, part3, part4;
    char tail = 0;
    int field;

    if(inip == NULL)
    {
        return 0;
    }
    field=sscanf(inip, "%d . %d . %d . %d %c", &part1, &part2, &part3, &part4, &tail);
    if(field < 4 || field > 5)
    {
        DEBUG_PRINT("expect 4 field,get %d\n", field);
        return 0;
    }
    if(tail != 0)
    {
        DEBUG_PRINT("mask address mixed with non number\n");
        return 0;
    }
    if( (part1 > 0 && part1 <= 255) && (part2>=0 && part2<=255) &&  (part3>=0 && part3<=255) &&  (part4>=0 && part4<=255) )
    {
    	if((part1>0 && part1<255) && (part2==0 && part3==0 && part4==0))
    	{
            //inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
            return 1;
    	}
    	else if (part1==255 && part2>=0 && part3==0 && part4==0)
    	{
            //inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
            return 1;
    	}
    	else if (part1==255 && part2==255 && part3>=0 && part4==0)
    	{
            //inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
            return 1;
    	}
    	else if (part1==255 && part2==255 && part3==255 && part4>=0 && part4<255)
    	{
            //inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
            return 1;
    	}
    	else
    	{
    		return 0;
    	}
    }
    else
    {
        DEBUG_PRINT("not good mask %d:%d:%d:%d\n",part1,part2,part3,part4);
    }
    return 0;
}

int gateway_rightful_check(char *inip)
{
    int part1, part2, part3, part4;
    char tail = 0;
    int field;

    if(inip == NULL)
    {
        return 0;
    }
    field=sscanf(inip, "%d . %d . %d . %d %c", &part1, &part2, &part3, &part4, &tail);
    if(field < 4 || field > 5)
    {
        DEBUG_PRINT("expect 4 field,get %d\n", field);
        return 0;
    }
    if(tail != 0)
    {
        DEBUG_PRINT("gateway address mixed with non number\n");
        return 0;
    }
    if( (part1 >0 && part1 <= 255) && (part2>=0 && part2<=255) &&  (part3>=0 && part3<=255) &&  (part4>0 && part4<255) )
    {
       // inaddr = part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
        return 1;
    }
    else
    {
        DEBUG_PRINT("not good gateway %d:%d:%d:%d\n",part1,part2,part3,part4);
    }
    return 0;
}

int ip_gate_mask_check(char *ipaddr, char *gateway, char *ipmask)
{
	int ip_part1=0, ip_part2=0, ip_part3=0, ip_part4=0;
	int gate_part1=0, gate_part2=0, gate_part3=0, gate_part4=0;
	int mask_part1=0, mask_part2=0, mask_part3=0, mask_part4=0;
	char tail = 0;

    sscanf(ipaddr, "%d . %d . %d . %d %c", &ip_part1, &ip_part2, &ip_part3, &ip_part4, &tail);
    sscanf(gateway, "%d . %d . %d . %d %c", &gate_part1, &gate_part2, &gate_part3, &gate_part4, &tail);
    sscanf(ipmask, "%d . %d . %d . %d %c", &mask_part1, &mask_part2, &mask_part3, &mask_part4, &tail);

    if((ip_part1 & mask_part1) == (gate_part1 & mask_part1) && (ip_part2 & mask_part2) == (gate_part2 & mask_part2)
    		&& (ip_part3 & mask_part3) == (gate_part3 & mask_part3) && (ip_part4 & mask_part4) == (gate_part4 & mask_part4))
    {
    	return 1;
    }

    return 0;
}


int isvalidstr(char *buf,int *pAddr)
{
    int part1,part2,part3,part4;
    char tail=0;
    int field;
    if(buf==NULL)
    {
        return 0;
    }
    field=sscanf(buf,"%d . %d . %d . %d %c",&part1,&part2,&part3,&part4,&tail);
    if(field<4|| field>5)
    {
        DEBUG_PRINT("expect 4 field,get %d\n",field);
        return 0;
    }
    if(tail!=0)
    {
        DEBUG_PRINT("ip address mixed with non number\n");
        return 0;
    }
    if( (part1>=0 && part1<=255) &&  (part2>=0 && part2<=255) &&  (part3>=0 && part3<=255) &&  (part4>=0 && part4<=255) )
    {
        if(pAddr)
            *pAddr= part4<<24 | part3<<16 | part2<<8 | part1;/*转换成网络序*/
        return 1;
    }
    else
    {
        DEBUG_PRINT("not good ip %d:%d:%d:%d\n",part1,part2,part3,part4);
    }
    return 0;
}

int testip(char *buf,int expect)
{
    int result=0;
    int addr=0;
    result=isvalidstr(buf,&addr);
    if(result==expect)
    {
        DEBUG_PRINT("OK:valid ip %s,expect %d,get %d\n",buf,expect,result);
        if(expect==1)
        {
            DEBUG_PRINT("\twe convert %s to 0x%x\n",buf,addr);
        }
    }
    else
    {
        DEBUG_PRINT("ERROR:valid ip %s,expect %d,get %d\n",buf,expect,result);
    }
    return 0;
}

void checkip_abcd(char *addr)
{
    int ip_addr;
    char *a = addr;
    
    ip_addr = atoi(a);
    
    DEBUG_PRINT("%d\n", ip_addr);
    
    if((ip_addr >> 7) == 0)
    {
        DEBUG_PRINT("A\n");
    }
    else if((ip_addr >> 6) == 2)
    {
        DEBUG_PRINT("B\n");
    }
    else if((ip_addr >> 5) == 6)
    {
        DEBUG_PRINT("C\n");
    }
    else if((ip_addr >> 4) == 14)
    {
        DEBUG_PRINT("D\n");
    }
    else
    {
        DEBUG_PRINT("E\n");
    }
    
}
//int main (int argc, char const* argv[])
//{
//    /*空ip*/
//    testip(NULL,0);
//    /*正常ip*/
//    testip("10.129.43.244",1);
//    /*带空格的ip*/
//    testip(" 10.129.43.244",1);
//    testip("10 .129 .43.244 ",1);
//    /*带前导0的ip*/
//    testip("010.129.043.244",1);
//    testip("010.0129.043.0244",1);
//    /*在前面带非法字符的ip*/
//    testip("x10.129.43.244",0);
//    /*最后带非法字符的ip*/
//    testip("10.129.43.24y",0);
//    testip("10.129.43.y",0);
//    return 0;
//}
