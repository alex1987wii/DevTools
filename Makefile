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
PROJECT=$(strip REPEATER_BBA)
valid_proj=AD6900_BBA U3 U3_2ND U4 U4_BBA G4_BBA G4_BBA_V2 REPEATER_BBA M2 BR01 BR01_2ND
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

CFLAGS  = -DCONFIG_PROJECT_$(PROJECT) -DFT_WINDOW -DNT_WINDOW -O2 -I. -I./include -I./$(LIB)/include -DSF_VERSION=\"$(WIN_UTIL_VERSION)\" 
LDFLAGS = -lcomctl32 -mwindows -lsetupapi -lWs2_32 -L./$(LIB)/lib -lupgrade -L./lib -liniparser

# build target and dependency definitions
TARGETS= $(TARGET_MAINTAIN) $(TARGET_DEV) $(TARGET_PRODUCTION)
TARGET_DEV = UniDevTools.exe
TARGET_MAINTAIN = UniMaintainTools.exe
TARGET_PRODUCTION = UniNandFlashProgramming.exe
WIN_UTIL_PATH=./output/$(PROJECT)
UTILS=./$(LIB)/lib/libupgrade.dll

DEPS = devToolsRes.h

all: clean ./lib/libiniparser.a $(TARGETS) install

./lib/libiniparser.a:./src/dictionary.o ./src/iniparser.o
	$(AR) rcs $@ $^
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
devToolsUI_maintain.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DMAINTAINMENT $(CFLAGS) $<

devToolsUI_dev.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DDEVELOPMENT $(CFLAGS) $<

devToolsUI_product.o: devToolsUI.c ./$(LIB)/include/devToolsUI_private.h
	$(CC) -c -o $@ -DPRODUCTION $(CFLAGS) $<

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prepare_dir:
	[ -d "$(WIN_UTIL_PATH)" ] || mkdir $(WIN_UTIL_PATH) -p
	[ -d "$(WIN_UTIL_PATH)/$(basename $(TARGET_DEV))" ] || mkdir $(WIN_UTIL_PATH)/$(basename $(TARGET_DEV))
	[ -d "$(WIN_UTIL_PATH)/$(basename $(TARGET_MAINTAIN))" ] || mkdir $(WIN_UTIL_PATH)/$(basename $(TARGET_MAINTAIN))
	[ -d "$(WIN_UTIL_PATH)/$(basename $(TARGET_PRODUCTION))" ] || mkdir $(WIN_UTIL_PATH)/$(basename $(TARGET_PRODUCTION))

install:prepare_dir
	cp -rf for_user_file.ini for_mfg_file.ini readme.txt $(WIN_UTIL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf $(UTILS) $(WIN_UTIL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf $(TARGET_DEV) $(WIN_UTIL_PATH)/$(basename $(TARGET_DEV))/
	cp -rf for_user_file.ini $(WIN_UTIL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf $(UTILS) $(WIN_UTIL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf $(TARGET_MAINTAIN)  $(WIN_UTIL_PATH)/$(basename $(TARGET_MAINTAIN))/
	cp -rf for_mfg_file.ini $(WIN_UTIL_PATH)/$(basename $(TARGET_PRODUCTION))/
	cp -rf $(UTILS) $(WIN_UTIL_PATH)/$(basename $(TARGET_PRODUCTION))/
	cp -rf $(TARGET_PRODUCTION)  $(WIN_UTIL_PATH)/$(basename $(TARGET_PRODUCTION))/
clean:
	rm -f *.o src/*.o $(TARGETS)
