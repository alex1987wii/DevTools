#ifndef _DEV_TOOLS_UI_H_
#define _DEV_TOOLS_UI_H_

#define WINVER 0x0501

/* _WIN32_IE defines the minimum Internet Explorer version required by the program.
   Unication Dev Tools need Internet Explorer 5.01(0x0501) and the followings  */
#define _WIN32_IE 0x0501
#include <stdint.h>
#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <dbt.h>
#include <devguid.h>
#include <Winsock2.h>
#include <time.h>

#include "utils.h"
#include "error_code.h"
#include "UpgradeLib.h"
#include "win.h"

#define UNI_APP_MUTEX      "Unication Dev Tools"

#define IS_LISTENING_ON_NOTHING   (0)
#define IS_LISTENING_ON_MFG       (1)
#define IS_LISTENING_ON_SPL       (2)
#define IS_LISTENING_ON_LINUX	  (3)

#define dump_time()

typedef int (*background_func)(void );
typedef void (*complete_func)(int retval,void *private_data);

#endif
