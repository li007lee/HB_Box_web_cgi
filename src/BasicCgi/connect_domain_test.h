/*
 * connect_domain_test.h
 *
 *  Created on: 2015��4��3��
 *      Author: root
 */

#ifndef BASICCGI_CONNECT_DOMAIN_TEST_H_
#define BASICCGI_CONNECT_DOMAIN_TEST_H_



#define TEST_PORT 		80

#define IP_VERTICAL 		6
#define IP_HORIZONTAL 		40


typedef enum _tagERROR_CODE
{
	HB_INTERNET_ERR = 1,
	HB_SOCK_CREAT_ERR,
	HB_CONNECT_ERR,
	HB_CONNECT_TIME_OUT,
	HB_OTHERS_ERR
}ERROR_CODE;

//通过域名和端口测试联通性
HB_S32 connect_test(HB_CHAR *domain, HB_S32 port, HB_S32 timeout);

//通过IP和端口测试联通性
HB_S32 connect_ip_port_test(HB_CHAR *ipaddr, HB_S32 port, HB_S32 timeout);

#endif /* BASICCGI_CONNECT_DOMAIN_TEST_H_ */
