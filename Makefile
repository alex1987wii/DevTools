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
PROJECT=$(strip BR01_2ND)
VERSION=v02p01c
valid_proj=BR01 BR01_2ND
ifeq ($(strip $(foreach pro,$(valid_proj),$(shell [ "$(PROJECT)" = "$(pro)" ] && echo "$(PROJECT)" ))),)
    $(warning we only support: )
    $(foreach pro,$(valid_proj),$(warning $(pro)))
    $(warning)
    $(error you specified $(PROJECT) which is not a valid project)
endif

U3LIB=U3 U3_2ND

ifeq ($(strip $(foreach pro,$(U3LIB),$(shell [ "$(pro)" = "$(PROJECT)" ] && echo "$(PROJECT)"))),)
    LIB=Gx
else
    LIB=U3
endif

# compiler and bin utilities definitions
CC=gcc
AR=ar
RC=windres

# compile flag and link flag definitions

CFLAGS  = -DCONFIG_PROJECT_$(PROJECT) -DVERSION=\"$(VERSION)\" -DFT_WINDOW -DNT_WINDOW -O2 -I. -I./include -I./$(LIB)/include 
LDFLAGS = -lcomctl32 -mwindows -lsetupapi -lWs2_32 -L./$(LIB)/lib -lupgrade -L./lib -liniparser

# build target and dependency definitions
TARGETS= $(TARGET_MAINTAIN) $(TARGET_DEV) $(TARGET_PRODUCTION) $(TARGET_LIMITED)
TARGET_DEV = UniDevTools.exe
TARGET_LIMITED = UniDevLimitedTools.exe
TARGET_MAINTAIN = UniMaintainTools.exe
TARGET_PRODUCTION = UniNandFlashProgramming.exe
WIN_TOOL_PATH=./output/$(PROJECT)
LIBS=./$(LIB)/lib/libupgrade.dll
UTILS=./utils #only for linux
OBJS=./src/winlog.o

DEPS = devToolsRes.h

all: clean ./lib/libiniparser.a $(TARGETS) install

./lib/libiniparser.a:./src/dictionary.o ./src/iniparser.o
	[ -d "lib" ] || mkdir lib
	$(AR) rcs $@ $^
./src/iniparser.o:./src/iniparser.c
	$(CC) -c -o $@ $< $(CFLAGS) -ansi
./src/dictionary.o:./src/dictionary.c
	$(CC) -c -o $@ $< $(CFLAGS) -ansi

%.o:%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET_MAINTAIN):resource_maintain.o devToolsUI_maintain.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)

$(TARGET_DEV):resource_dev.o devToolsUI_dev.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)   

$(TARGET_PRODUCTION):resource_product.o devToolsUI_product.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)   

$(TARGET_LIMITED):resource_limited.o devToolsUI_limited.o $(OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS)   

# resourece object                          
resource_maintain.o: devTools.rc devToolsRes.h
	$(RC) $< -DMAINTAINMENT $@

resource_dev.o: devTools.rc devToolsRes.h
	$(RC) $< -DDEVELOPMENT $@

resource_product.o: devTools.rc devToolsRes.h
	$(RC) $< -DPRODUCTION $@

resource_limited.o: devTools.rc devToolsRes.h
	$(RC) $< -DDEVELOPMENT -DLIMITED $@

# UI object
devToolsUI_maintain.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DMAINTAINMENT $(CFLAGS) $<

devToolsUI_dev.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DDEVELOPMENT $(CFLAGS) $<

devToolsUI_product.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DPRODUCTION $(CFLAGS) $<

devToolsUI_limited.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DDEVELOPMENT -DLIMITED $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prepare_dir:
	[ -d "$(WIN_TOOL_PATH)" ] || mkdir $(WIN_TOOL_PATH) -p
	[ -d "$(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))" ] || mkdir $(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))
	[ -d "$(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))" ] || mkdir $(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))
	[ -d "$(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))" ] || mkdir $(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))
	[ -d "$(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))" ] || mkdir $(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))

install:prepare_dir
	cp -rf readme_for_development.txt $(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))/readme.txt
	cp -rf for_user_file.ini for_mfg_file.ini $(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf $(LIBS) $(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf $(TARGET_DEV) $(WIN_TOOL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf readme_for_maintainment.txt $(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))/readme.txt
	cp -rf for_user_file.ini $(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf $(LIBS) $(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf $(TARGET_MAINTAIN)  $(WIN_TOOL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf readme_for_production.txt $(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))/readme.txt
	cp -rf for_mfg_file.ini $(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))/
	cp -rf $(LIBS) $(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))/
	cp -rf $(TARGET_PRODUCTION)  $(WIN_TOOL_PATH)/$(basename $(TARGET_PRODUCTION))/
	cp -rf readme_for_development.txt $(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))/readme.txt
	cp -rf for_user_file.ini $(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))/
	cp -rf $(LIBS) $(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))/
	cp -rf $(TARGET_LIMITED) $(WIN_TOOL_PATH)/$(basename $(TARGET_LIMITED))/
clean:
	rm -f *.o src/*.o $(TARGETS)
