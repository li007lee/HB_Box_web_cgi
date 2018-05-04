include ../../Rules.make
#CC = gcc
#CC=/root/work/software/arm-hisiv100nptl-linux/arm-hisiv100-linux/bin/arm-hisiv100-linux-uclibcgnueabi-gcc

INC_PATH =$(GLOB_INC_PATH)  -I ./inc

LIBS = $(GLOB_LIB_PATH) \
			-lcgi -lxml -lsqlite3 -lpthread -lcommon -lm

APPBIN = port.cgi

SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all:
	$(CC) $(SRCS) $(CFLAGS) $(INC_PATH) $(LIBFLAGS) $(LIBS) -o $(APPBIN)
	$(STRIP) $(APPBIN)
	cp $(APPBIN) $(DEST_DIR)
clean:
	rm -rf $(OBJS) $(APPBIN)
	rm -rf $(DEST_DIR)$(APPBIN)
