#// �ļ����� Rules.make
#// ���ߣ�   �ż���
#// �汾��   1.0
#// ���ڣ�   2015-03-10
#// �������������̱���Ļ�������
#// ��ʷ��¼��
#////////////////////////////////////////////////////////////////////////////////

#////////////////////////////////////////////////////////////////////////////////
#���ñ��빤�߻���������
#////////////////////////////////////////////////////////////////////////////////


#小盒子
BOX_TYPE?=small_box
#BOX_TYPE?=big_box_single_port
#BOX_TYPE?=big_box_multiple_port
#BOX_TYPE?=big_box_encrypt
#BOX_TYPE?=big_box_elevator

#BUILD?=DEBUG
BUILD?=RELEASE

BOX_CGI_BASE_DIR := /mnt/hgfs/share_dir/my_project/Box_web_cgi
GLOB_INC_PATH := -I $(BOX_CGI_BASE_DIR)/inc


ifeq ($(BOX_TYPE), small_box)

BOX_TYPE_SUB?=big_small_box_single_mulip_port
#BOX_TYPE_SUB?=big_small_box_single_port

CC = arm-hisiv100nptl-linux-gcc
STRIP = arm-hisiv100nptl-linux-strip
GLOB_LIB_PATH := -L$(BOX_CGI_BASE_DIR)/lib/hisi100
COMMON_LIB_PATH := $(BOX_CGI_BASE_DIR)/lib/hisi100
COMMON_INC_PATH	:= $(BOX_CGI_BASE_DIR)/inc
LIBFLAGS = -Wl,-rpath=/mnt/lib

ifeq ($(BOX_TYPE_SUB), big_small_box_single_mulip_port)
DEST_DIR:= /mnt/hgfs/share_dir/nfs_dir/boa_small_box/boa_small_mul_ip/www/cgi_bin/
else
DEST_DIR:= /mnt/hgfs/share_dir/nfs_dir/boa_small_box/boa_server/www/cgi_bin/
endif
endif


ifeq ($(BOX_TYPE), big_box_single_port)

BOX_TYPE_SUB?=big_small_box_single_mulip_port
#BOX_TYPE_SUB?=big_small_box_single_port

CC=arm-hisiv200-linux-gcc
STRIP=arm-hisiv200-linux-strip
GLOB_LIB_PATH := -L$(BOX_CGI_BASE_DIR)/lib/hisi200
COMMON_LIB_PATH := $(BOX_CGI_BASE_DIR)/lib/hisi200
COMMON_INC_PATH	:= $(BOX_CGI_BASE_DIR)/inc
LIBFLAGS = -Wl,-rpath,/mnt/ydt_box/lib:/mnt/mtd/nfs_dir/lib200

ifeq ($(BOX_TYPE_SUB), big_small_box_single_mulip_port)
DEST_DIR:= /mnt/hgfs/share_dir/nfs_dir/boa_big_box/boa_single_mul/www/cgi_bin/
else
DEST_DIR:= /mnt/hgfs/share_dir/nfs_dir/boa_big_box/boa_single/www/cgi_bin/
endif
endif


ifeq ($(BOX_TYPE), big_box_multiple_port)
CC=arm-hisiv200-linux-gcc
STRIP=arm-hisiv200-linux-strip
GLOB_LIB_PATH := -L$(BOX_CGI_BASE_DIR)/lib/hisi200
COMMON_LIB_PATH := $(BOX_CGI_BASE_DIR)/lib/hisi200
COMMON_INC_PATH	:= $(BOX_CGI_BASE_DIR)/inc
LIBFLAGS = -Wl,-rpath /mnt/ydt_box/lib
DEST_DIR:= /mnt/hgfs/share_dir/nfs_dir/boa_big_box/boa_mul/www/cgi_bin/
endif


ifeq ($(BOX_TYPE), big_box_encrypt)
CC=arm-hisiv200-linux-gnueabi-gcc
STRIP=arm-hisiv200-linux-gnueabi-strip
GLOB_LIB_PATH := -L$(BOX_CGI_BASE_DIR)/lib/hisi200
LIBFLAGS = -Wl,-rpath /mnt/ydt_box/lib
endif


ifeq ($(BOX_TYPE), big_box_elevator)
CC=arm-hisiv200-linux-gnueabi-gcc
STRIP=arm-hisiv200-linux-gnueabi-strip
GLOB_LIB_PATH := -L$(BOX_CGI_BASE_DIR)/lib/hisi200
LIBFLAGS = -Wl,-rpath /mnt/ydt_box/lib
endif



ifeq ($(BUILD), DEBUG)
CFLAGS   = -Wall  -D$(BUILD)
else
CFLAGS   = -Wall -O2 -D$(BUILD)
endif
