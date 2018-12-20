#  Name:    Makefile
#
#  Purpose: Makefile to build Unication Dev Tools
#
#  UNICATION CO. LTD. PROPRIETARY INFORMATION
#
#  SECURITY LEVEL - HIGHLY CONFIDENTIAL
#
#  DO NOT COPY
#
#  This document and the information contained in it is confidential and
#  proprietary to Unication Co., Ltd. The reproduction or disclosure in
#  whole or in part, to anyone outside of Unication Co., Ltd. without the
#  written approval of the President of Unication Co., Ltd., under a
#  Non-Disclosure Agreement, or to any employee of Unication Co. Ltd. who
#  has not previously obtained written authorization for access from the
#  individual responsible for the document, will have a significant
#  detrimental effect on Unication Co., Ltd. and is expressly prohibited.
#
#  Copyright (c) $Date: 2015/02/06 08:03:07 $ Unication Co., Ltd.
#
#  All rights reserved

# config varibles
PROJECT=g4
#valid_proj=u3 u4 g4 m2 br01
# compiler and bin utilities definitions
CC=gcc
AR=ar
RC=windres

# compile flag and link flag definitions

CFLAGS  = -g -DCONFIG_PROJECT_G4_BBA -DFT_WINDOW -DNT_WINDOW -O2 -I. -I./include -I./$(PROJECT)/include -DSF_VERSION=\"$(WIN_UTIL_VERSION)\" 
LDFLAGS = -lcomctl32 -mwindows -lsetupapi -lWs2_32 -L./$(PROJECT)/lib -lupgrade -L./lib -liniparser

# build target and dependency definitions
TARGETS= $(TARGET_MAINTAIN) $(TARGET_DEV) $(TARGET_PRODUCTION)
TARGET_DEV = UniDevTools.exe
TARGET_MAINTAIN = UniMaintainTools.exe
TARGET_PRODUCTION = UniNandFlashProgramming.exe

 
DEPS = devToolsRes.h

all: clean ./lib/libiniparser.a $(TARGETS)

./lib/libiniparser.a:./src/dictionary.o ./src/iniparser.o
	$(AR) rcs -o $@ $^
./src/iniparser.o:./src/iniparser.c
	$(CC) -c -o $@ $< $(CFLAGS) -ansi
./src/dictionary.o:./src/dictionary.c
	$(CC) -c -o $@ $< $(CFLAGS) -ansi
$(TARGET_MAINTAIN):resource_maintain.o devToolsUI_maintain.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

$(TARGET_DEV):resource_dev.o devToolsUI_dev.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)   

$(TARGET_PRODUCTION):resource_product.o devToolsUI_product.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)   

# resourece object                          
resource_maintain.o: devTools.rc devToolsRes.h
	$(RC) $< -DMAINTAINMENT $@

resource_dev.o: devTools.rc devToolsRes.h
	$(RC) $< -DDEVELOPMENT $@

resource_product.o: devTools.rc devToolsRes.h
	$(RC) $< -DPRODUCTION $@

# UI object
devToolsUI_maintain.o: devToolsUI.c ./$(PROJECT)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DMAINTAINMENT $(CFLAGS) $<

devToolsUI_dev.o: devToolsUI.c ./$(PROJECT)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DDEVELOPMENT $(CFLAGS) $<

devToolsUI_product.o: devToolsUI.c ./$(PROJECT)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DPRODUCTION $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

install:
	cp -rf $(TARGETS) $(WIN_UTIL_PATH)/

clean:
	rm -f *.o $(TARGETS) $(SYMBLE_LINKS)
