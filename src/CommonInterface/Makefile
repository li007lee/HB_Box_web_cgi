include ../../Rules.make
#CC = /root/work/software/arm-hisiv200-linux/arm-hisiv200-linux/bin/arm-hisiv200-linux-gnueabi-gcc
#AR = /root/work/software/arm-hisiv200-linux/arm-hisiv200-linux/bin/arm-hisiv200-linux-gnueabi-ar
#CC = arm-hisiv100nptl-linux-gcc
#AR = arm-hisiv100nptl-linux-ar
LIBA = libcommon.a

INC_PATH =$(GLOB_INC_PATH) -I ./inc
LIBS =$(GLOB_LIB_PATH) \
			-lcgi -lxml -lsqlite3 -lpthread

CFLAGS = -Wall -O2
COMPIL= $(CC) $(CFLAGS) $(INC_PATH) $(LIBS) -c
INSTALL	 = install -c


SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))


all:$(LIBA)
$(LIBA):$(OBJS)
	$(AR) -crs $(LIBA) $(OBJS)
	cp $(LIBA) $(COMMON_LIB_PATH)
	cp *.h $(COMMON_INC_PATH)

%.o:%.c
	$(COMPIL) $< -o $@

clean:
	rm -f $(OBJS) $(LIBA)
