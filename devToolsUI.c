/*
 *  Name:    mfg_tools_ui_20151022.c
 *
 *  Purpose: Unication Development Tools User Interface application
 *
 *  UNICATION CO. LTD. PROPRIETARY INFORMATION
 *
 *  SECURITY LEVEL - HIGHLY CONFIDENTIAL
 *
 *  DO NOT COPY
 *
 *  This document and the information contained in it is confidential and
 *  proprietary to Unication Co., Ltd. The reproduction or disclosure in
 *  whole or in part, to anyone outside of Unication Co., Ltd. without the
 *  written approval of the President of Unication Co., Ltd., under a
 *  Non-Disclosure Agreement, or to any employee of Unication Co. Ltd. who
 *  has not previously obtained written authorization for access from the
 *  individual responsible for the document, will have a significant
 *  detrimental effect on Unication Co., Ltd. and is expressly prohibited.
 *
 *  Copyright (c) $Date: 2017/09/11 03:45:11 $ Unication Co., Ltd.
 *
 *  All rights reserved
 */

/*
 *  flows
 *  NandProgram:
 *  1 user trigger click "Programming" button, invoke WM_COMMAND message;
 *  2 DevToolsWindowProcedure invoke ProcessLinuxCommand
 *  3 waiting MFG insert
 *  4 WM_DEVICECHANGE detect MFG, trigger SPL download
 *  5 SPL download finished
 *  6 waiting SPL running
 *  7 WM_DEVICECHANGE detect SPL, trigger partition download
 *  8 one partition download finished
 *  9 goto 7, until all partitions download finished
 *
 */


/* WINVER defines the minimum Windows system the program can run on.
   Unication Dev Tools need run on the Windows XP(0x0501) and the following system  */
#define WINVER 0x0501

/* _WIN32_IE defines the minimum Internet Explorer version required by the program.
   Unication Dev Tools need Internet Explorer 5.01(0x0501) and the followings  */
#define _WIN32_IE 0x0501

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
#include "UpgradeLib.h"
#include "uni_image_header.h"
#include "error_code.h"
#include "devToolsUI_private.h"
#include "devToolsRes.h"
#include "iniparser.h"
/*  ===========================================================================
 *  Macro definitions
 *  ===========================================================================
 */
#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define UI_DEBUG(fmt,args...)			do{\
TCHAR buff[1024];\
snprintf(buff,1024,TEXT("%s:%s:%d:")fmt,__FILE__,__func__,__LINE__,##args);\
MessageBox(hwndMain,buff,TEXT("UI_DEBUG"),MB_ICONWARNING);}while(0)

#define debug_positions(args1,args2)

#else
	
#define UI_DEBUG(fmt,args...)
#define debug_positions(args1,args2)
#endif
#define MAX_STRING	1024
#define ERROR_MESSAGE(message,args...)	do{TCHAR MessageBoxBuff[MAX_STRING];\
snprintf(MessageBoxBuff,MAX_STRING,TEXT(message),##args);\
MessageBox(hwndMain,MessageBoxBuff,szAppName,MB_ICONERROR);\
}while(0) 
	
/*user message definition which is not conflict with "devToolsUI_private.h"*/
#define WM_CREATE_PROGRESS_BAR		(WM_USER+110)
#define WM_DESTROY_PROGRESS_BAR		(WM_USER+111)
#define WM_CREATE_PARTITION_LIST	(WM_USER+112)
#define WM_DESTROY_PARTITION_LIST	(WM_USER+113)
#define WM_ERROR					(WM_USER+114)

#define DISABLE						0
#define ENABLE						1

/*stage for progress_reply_status_get  */
#define PROGRESS_STATUS_PREPARING     (0)
#define PROGRESS_STATUS_FLASHING      (1)
#define PROGRESS_STATUS_VERIFYING     (2)
#define PROGRESS_STATUS_EXECUTING     (3)
#define PROGRESS_STATUS_FINISHED      (4)
#define PROGRESS_STATUS_TRANSFER	  (5)
#define MAX_STATUS					(6)
	
#if defined DEVELOPMENT
#define DEV_TOOLS_NUMBER        4           /* Unication dev tools have three utilities */
#elif defined MAINTAINMENT
#define DEV_TOOLS_NUMBER        1           /* Unication dev tools have only one utilities */
#elif defined PRODUCTION
#define DEV_TOOLS_NUMBER        1           /* Unication dev tools have only one utilities */
#endif

#define MAGIC_VALUE             (0xdeaddead)
#define IS_IDEL                 (0)
#define IS_DOWNLOADING          (1)
#define IS_UPLOADING            (2)

/* layout */
#define DEV_TOOLS_WIDTH         700         /* Unication dev tools main window width */
#define DEV_TOOLS_HEIGHT        580         /* Unication dev tools main window height */

#define X_MARGIN                10
#define Y_MARGIN                10
#define HEIGHT_CONTROL          30
#define WIDTH_EDIT              420

#define WIDTH_CHECKBOX          95
#define WIDTH_BUTTON            90
#define WIDTH_TEXT              80
#define WIDTH_COMBO             200


#define TAB_HEAD_HEIGHT         30

#define TAB_CLIENT_WIDTH        (DEV_TOOLS_WIDTH - X_MARGIN*4)
#define TAB_CLIENT_HEIGHT       (DEV_TOOLS_HEIGHT - Y_MARGIN*4 - TAB_HEAD_HEIGHT - Y_MARGIN)

#define POP_WIN_HEIGHT          110

#define TAB_CLIENT_START_X      X_MARGIN
#define TAB_CLIENT_START_Y      (TAB_HEAD_HEIGHT + Y_MARGIN*2)
 
#define WIDTH_EDIT_NAND         360
#define WIDTH_EDIT1             135



#define WIDTH_BUTTON_NANDFLASH_PROGRAM 110


#define TAB_NANDFLASH_START_X   5*X_MARGIN
#define TAB_NANDFLASH_WIDTH     (DEV_TOOLS_WIDTH - X_MARGIN*4) - 60
#define TAB_NANDFLASH_HEIGHT    (DEV_TOOLS_HEIGHT - Y_MARGIN*4 - TAB_HEAD_HEIGHT)/2 - 50

#define FILE_DL_START_X         10
#define FILE_DL_START_Y         0
#define FILE_DL_WIDTH           TAB_CLIENT_WIDTH-X_MARGIN*2
#define FILE_DL_HEIGHT          (TAB_CLIENT_HEIGHT - 2*Y_MARGIN)/2 - 30
#define WIN_STATIC_OFFSET       (3)
#define WIN_COMBO_OFFSET        (10)




/********************************************************************************/
#if (defined(CONFIG_PROJECT_U3) || defined(CONFIG_PROJECT_U3_2ND))
#define PROJECT			"U3"
#define U3_LIB
#elif defined CONFIG_PROJECT_U4
#define PROJECT			"U4"
#elif defined CONFIG_PROJECT_U5
#define PROJECT			"U5"
#elif defined CONFIG_PROJECT_G4_BBA
#define PROJECT			"G4_BBA"
#elif defined CONFIG_PROJECT_BR01_2ND
#define PROJECT			"BR01_2nd"
#elif defined CONFIG_PROJECT_M2
#define PROJECT			"M2"

#else
	#error "must pass the macro CONFIG_PROJECT_XXX to indicate which project it is"
#endif


static char partition_name[UNI_MAX_PARTITION][UNI_MAX_PARTITION_NAME_LEN];
static int total_partition = 0;
static int cbs_per_line = 0;
static BOOL get_image_info(const char*image)
{
	FILE *fp = fopen(image,"rb");
	if(fp == NULL)
		return FALSE;
	uni_image_header_t image_header;
		
	if(1 != fread(&image_header,sizeof(uni_image_header_t),1,fp))
	{
		fclose(fp);
		return FALSE;
	}
	fclose(fp);
	total_partition = image_header.total_partitions;
	int i;
	for(i = 0; i < total_partition; ++i)
	{		
		memcpy(partition_name[i],image_header.partition[i].partition_name,UNI_MAX_PARTITION_NAME_LEN);
	}
	cbs_per_line = total_partition/2;
	return TRUE;
}

static HWND hwndPartitionCheckBox[UNI_MAX_PARTITION] = {NULL,};
int partition_x_pos = 0;
int partition_y_pos = 0;


/*********************Tab Select ID****************************/
#define SELECT_LINUX_PROGRAMMING		0
#define SELECT_SPL_PROGRAMMING			1
#define SELECT_MFG_PROGRAMMING			2
#define SELECT_FILE_TRANSFER			3


/* Unication UniDevTools lines buffer definitions */
#if defined DEVELOPMENT
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"For LINUX ", " For SPL ", " For MFG ","File Transfer",};
#elif defined MAINTAINMENT
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"Partition Transfer"};
#elif defined PRODUCTION
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"Nand Flash programming"};
#endif


/**********************************************************************/
#if defined(DEVELOPMENT)
#define APP_TITLE          "Unication Dev Tools"
static const char *ini_file = "for_user_file.ini";/*select partition by check checkbox*/
static int burn_mode = SELECT_LINUX_PROGRAMMING;
#elif defined(MAINTAINMENT)
#define APP_TITLE          "Unication Maintenance Tools"
static const char *ini_file = "for_user_file.ini";
static const int burn_mode = SELECT_LINUX_PROGRAMMING;
#elif defined(PRODUCTION)
#define APP_TITLE          "Unication NandFlash Programmer"
static const char *ini_file = "for_mfg_file.ini";
static const int burn_mode = SELECT_MFG_PROGRAMMING;
#endif

#define UNI_APP_MUTEX      "Unication Dev Tools"
typedef void (*background_func)(void);
typedef void (*complete_func)(int retval,void *private_data);
static background_func g_background_func = NULL;
static BOOL g_transfer_complete = FALSE;
static complete_func g_complete_func = NULL;
static void *g_complete_private_data = NULL;
static int g_locking = 0;
static int g_retval = 0;
static char work_path[MAX_PATH];

static HANDLE g_event = NULL;    // event
static HANDLE g_event_wait = NULL;    // event
static HANDLE g_hTransfer = NULL;
static HANDLE g_h_wait = NULL;

TCHAR szAppName[] = TEXT(APP_TITLE);

static int screenWidth, screenHeight;  /* User screen size */

static HINSTANCE           hInst;      /* Handle to the instance of uni UniDevTools */
static HWND hwndMain;   /* Handle to the main window of uni UniDevTools */

static HANDLE              hMutex;     /* Handle to mutex that permit only one UniDevTools instance */

/* NandFlash programmer window handle definitions */
static HWND    hwndTab;                /* Handle to the tab control window of uni dev tools */

const char target_type[3][64] = {
#ifdef MAINTAINMENT
#if ( defined(CONFIG_PROJECT_G4_BBA) )
    "         Target:  Pager",
#elif ( defined(CONFIG_PROJECT_BR01_2ND) )
    "         Target:  Repeater",
#else
    "         Target:  Unication Device",
#endif

#else
    "         Target:  Linux device",
#endif
    "         Target:  SPL   device",
    "         Target:  MFG   device",
};
static char project_target[64];

static RECT rcClient; // rcClient.right, rcClient.bottom

typedef struct _ini_file_info
{
    char ip[16]; // xxx.xxx.xxx.xxx
    char name_of_image[MAX_PATH];
} ini_file_info_t;

static ini_file_info_t ini_file_info;
static void *image_buffer = NULL;
/**********************Linux Handler Declaration*************************/
static HWND    hwndLinPage;
static HWND    hwndLinGroupReadme;
static HWND    hwndLinTextTarget,hwndLinStaticInfo,hwndLinStaticNotice;

static HWND    hwndLinIpAddr,hwndLinDevList,hwndLinBtnRefresh;/*for Br01*/
static HWND    hwndLinGroupOptions;
/*not used now
static HWND    hwndEditBrowser;
*/
static HWND    hwndLinBtnCheckImg,hwndLinBtnDown,hwndBtnEnableTelnet;
static HWND    hwndCheckBoxDelete,hwndCheckBoxSkipBatCheck;


/**********************SPL Handler Declaration*************************/
static HWND    hwndSPLPage;
static HWND    hwndSPLGroupReadme;
static HWND    hwndSPLTextTarget,hwndSPLStaticInfo,hwndSPLStaticNotice;

static HWND    hwndSPLIpAddr,hwndSPLDevList,hwndSPLBtnRefresh;/*for Br01*/
static HWND    hwndSPLGroupOptions;

static HWND    hwndSPLBtnCheckImg,hwndSPLBtnDown,hwndSPLBtnStop;
//static HWND    hwndCheckBoxSkipBatCheck;

/**********************MFG Handler Declaration*************************/

static HWND    hwndMFGPage;
static HWND    hwndMFGGroupReadme;
static HWND    hwndMFGTextTarget,hwndMFGStaticInfo,hwndMFGStaticNotice;

static HWND    hwndMFGIpAddr,hwndMFGDevList,hwndMFGBtnRefresh;/*for Br01*/
static HWND    hwndMFGGroupOptions;


static HWND    hwndMFGBtnCheckImg,hwndMFGBtnDown,hwndMFGBtnStop;
static HWND    hwndCheckBoxBatch;

/**********************File Transfer Handler Declaration*************************/
#define MAX_BUFF_LEN	0x01400000 /*20M*/
static HWND	   hwndFileTransferPage,hwndFtNotice;
static HWND    hwndFdGroupBox,hwndFdSrcFile,hwndFdDestFile,hwndFdStaticSrcFile,hwndFdEditDestFile,hwndFdButtonBrowse,hwndFdButtonDownload,hwndFdNotify;
static HWND    hwndFuGroupBox,hwndFuSrcFile,hwndFuDestFile,hwndFuEditSrcFile,hwndFuStaticDestFile,hwndFuButtonBrowse,hwndFuButtonUpload,hwndFuNotify;

/* Popup window handle definitions */
static HWND hwndPop, hwndPopStatic, hwndPopPercent, hwndPopProgress;


/*  ===========================================================================
 *  Function declaration
 *  ===========================================================================
 */

LRESULT CALLBACK DevToolsWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


#ifdef ENABLE_DEBUG
/*
 *  Name:     InitDebugConsole
 *
 *  Purpose:  Init debug console
 *
 *  Params:    none
 *
 *  Return:    If the function succeeds, the return value is TRUE
 *               If the function fails, the return value is FALSE
 *
 */
BOOL InitDebugConsole( void )
{
	/* Allocates a new console for the calling process */
	if( !AllocConsole() )
    {      
        return FALSE;
    }
    
    /* redirect stdout and stdin to console out and console in */
    freopen(TEXT("CONOUT$"),TEXT("w"), stdout);
    freopen(TEXT("CONIN$"), TEXT("r"), stdin);
    
    return TRUE;

}

/*
 *  Name:     ExitDebugConsole
 *
 *  Purpose:  Exit debug console
 *
 *  Params:    none
 *
 *  Return:    none
 *
 */
void ExitDebugConsole( void )
{
	FreeConsole();
	return;
}

#else

BOOL InitDebugConsole( void )
{   
    return TRUE;

}
void ExitDebugConsole( void )
{
	return;
}

#endif


static inline void CreatePartitionList(void)
{
	//create partition box
	int i;
	for(i = 0; i < total_partition; ++i)
	{
		hwndPartitionCheckBox[i] = CreateWindow(TEXT("button"),
		partition_name[i],
		WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE,
		partition_x_pos + (i%cbs_per_line)*(WIDTH_CHECKBOX) + X_MARGIN,
		partition_y_pos + (i/cbs_per_line)*HEIGHT_CONTROL + Y_MARGIN,
		WIDTH_CHECKBOX, HEIGHT_CONTROL,
		hwndLinPage,NULL,
		hInst,NULL);		
	}
}
static inline void DestoryPartitionList(void)
{
	int i;
	for(i = 0; i < total_partition; ++i)
	{
		DestroyWindow(hwndPartitionCheckBox[i]);
		hwndPartitionCheckBox[i] = NULL;		
	}
}
static inline void ShowPartitionList(BOOL enable)
{	
	int i;
	for(i = 0; i < total_partition; ++i)
	{
		ShowWindow(hwndPartitionCheckBox[i],enable);
	}
}
static inline void EnablePartitionList(BOOL enable)
{
	int i;
	for(i = 0; i < total_partition; ++i)
	{
		EnableWindow(hwndPartitionCheckBox[i],enable);
	}
}



/*get src 's filename concatenate to dest
* dest is linux filename，
* src is windows filename
*/
static inline int get_abs_file_name(char *dest,const char *src)
{
	if(dest == NULL || src == NULL)
		return FALSE;
	char tmp_buf[MAX_PATH];
	int i = 0;
	dest += strlen(dest);
	src += strlen(src);
	while(*src != '\\')
		tmp_buf[i++] = *src--;
	--i;
	if(*(dest-1) != '/')
		*dest++ = '/';
	
	while(i >= 0)
		*dest++ = tmp_buf[i--];
	
	return TRUE;
}
static inline int get_file_len(const char *file)
{
	int retval = -1;
	FILE *fp = fopen(file,"rb");
	if(fp == NULL)
	{
		retval = ERROR_CODE_OPEN_FILE_ERROR;
		return retval;
	}
	fseek(fp,0,SEEK_END);
	retval = ftell(fp);
	fclose(fp);
	return retval;
}
static inline int read_file(void *buff,int len,const char *filename)
{
	int retval = -1;
	FILE *fp = fopen(filename,"rb");
	if(fp == NULL)
	{
		retval = ERROR_CODE_OPEN_FILE_ERROR;
		return retval;
	}
	retval = fread(buff,1,len,fp);	
	fclose(fp);
	return retval;
}
static inline int write_file(void *buff,int len,const char *filename)
{
	int retval = -1;
	FILE *fp = fopen(filename,"wb+");
	if(fp == NULL)
		return retval;
	retval = fwrite(buff,1,len,fp);
	fclose(fp);
	return retval;
}

static inline int exec_in_tg(const char *script)
{	
	int retval = -1;
#ifdef U3_LIB
	/*write script into tmpfile then pass tmpfile name to file_download*/
	char *tmp_file = tempname(NULL,"tmp");
	if(tmp_file == NULL)
		return retval;
	if(strlen(script) != write_file(script,strlen(script),tmp_file))
		return retval;
	retval = file_download(tmp_file,"/tmp/tmp_script.sh");
	remove(tmp_file);
#else
	retval = download_file(script,strlen(script),"/tmp/tmp_script.sh");		
#endif
	if(retval != 0)
		return retval;
	retval = exec_file_in_tg("/tmp/tmp_script.sh");
	return retval;
}
static inline int RebootTarget()
{	
	return exec_in_tg("#!/bin/sh\nreboot\n");
}
static inline int EnableTelnet()
{
	return exec_in_tg("#!/bin/sh\ntelent &\n");
}

static inline BOOL parse_ini_file(const char *file)
{	
	char *ip,*image;
	/*read the ini file and parse it into struct*/
	dictionary *ini_config = iniparser_load(file);
	if(ini_config == NULL)
	{		
		return FALSE;
	}
	if(iniparser_getnsec(ini_config) != 1)
	{
		goto EXIT;
	}
	ip = iniparser_getstring(ini_config,"Options:ip",NULL);
	image = iniparser_getstring(ini_config,"Options:image",NULL);
	if(ip != NULL && image != NULL && strlen(ip) < 16 && strlen(image) < MAX_PATH)
	{		
		strcpy(ini_file_info.ip,ip);
		strcpy(ini_file_info.name_of_image,image);
		iniparser_freedict(ini_config);
		return TRUE;
	}
EXIT:
	iniparser_freedict(ini_config);
	return FALSE;	
}

static void ChangeProgressBar(int percent)
{
    
    char temp[16];   
    sprintf(temp, " %02d%% ", percent);
    SetWindowText(hwndPopPercent, TEXT(temp));
    
    /* update the progress bar */
    SendMessage(hwndPopProgress, PBM_SETPOS, (WPARAM)percent, 0L); 
    
    return;
}
static inline void ChangeProgressBarText(const char *string)
{
	SetWindowText(hwndPopStatic,TEXT(string));
}
static inline void DestoryProgressBar(void)
{	
    if (hwndPop)
    {		
        DestroyWindow(hwndPop);
		hwndPop = NULL;
	}	
    return;
}

/*  ===========================================================================
 *  Static Variable Definitions
 *  ===========================================================================
 */

static char *get_error_info(unsigned short ec)
{
    int i;

    for (i=0; i<(sizeof(error_code_info)/sizeof(error_code_info[0])); i++)
    {
        unsigned short k = error_code_info[i].ec;

        if (k == ec)
        {
            return (char *)error_code_info[i].string;
        }
    }

    return "error code not found";
}

/*{*/
static const GUID GUID_DEVCLASS_AD6900_SPL = {0xb35924d6UL, 0x3e16, 0x4a9e, {0x97, 0x82, 0x55, 0x24, 0xa4, 0xb7, 0x9b, 0xac}};
static const GUID GUID_DEVCLASS_AD6900_MFG = {0xb35924d6UL, 0x3e16, 0x4a9e, {0x97, 0x82, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66}};
static const GUID GUID_DEVCLASS_AD6900_LAN = {0xad498944UL, 0x762f, 0x11d0, {0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c}};

HDEVNOTIFY ad6900_lan = NULL;
HDEVNOTIFY ad6900_spl = NULL;
HDEVNOTIFY ad6900_mfg = NULL;


static inline void waiting_for_reboot(void)
{
    SetTimer(hwndMain, TIMER_WAITING_FOR_REBOOT, 2000, NULL);
}

static inline void counting_interfaces(void)
{
    SetTimer(hwndMain, TIMER_COUNTING_INTERFACES, 8000, NULL);
}

static inline void scanning_progress_timer(void)
{
    SetTimer(hwndMain, TIMER_SCANNING_DEVS, 1000, NULL);
}
/********************************helper******************************/


/*
 *  USB hot plug notify register
 */
static BOOL usbChangeNotificationRegister(GUID InterfaceClassGuid, HWND hWnd, HDEVNOTIFY *phDeviceNotify)
{
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

    *phDeviceNotify = NULL;

    /* Initialize notification filter */
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = InterfaceClassGuid;

    /* Sent WM_DEVICECHANGE message to the specified windows when U3 setup class device changed */
    *phDeviceNotify = RegisterDeviceNotification(
            hWnd,                       // events recipient
            &NotificationFilter,        // type of device
            DEVICE_NOTIFY_WINDOW_HANDLE/*|DEVICE_NOTIFY_ALL_INTERFACE_CLASSES*/ // type of recipient handle
            );

    if ( NULL == *phDeviceNotify )
    {
        return FALSE;
    }

    return TRUE;
}

static void usbChangeNotificationUnRegister(HDEVNOTIFY *phDeviceNotify)
{
    if ((phDeviceNotify) && (*phDeviceNotify))
    {
        UnregisterDeviceNotification(*phDeviceNotify);
        *phDeviceNotify = NULL;
    }
}

/*
 *  helper, get GUID of notify device
 */
#define AD6900_SPL (1)
#define AD6900_MFG (2)
#define AD6900_LAN (3)
static int32_t get_notify_device_type(LPARAM lParam)
{
    PDEV_BROADCAST_DEVICEINTERFACE pnotify = NULL;

    pnotify = (PDEV_BROADCAST_DEVICEINTERFACE)lParam;
    if (!(memcmp(&pnotify->dbcc_classguid, &GUID_DEVCLASS_AD6900_SPL, sizeof(GUID_DEVCLASS_AD6900_SPL))))
    {
        return AD6900_SPL;
    }
    else if (!(memcmp(&pnotify->dbcc_classguid, &GUID_DEVCLASS_AD6900_MFG, sizeof(GUID_DEVCLASS_AD6900_MFG))))
    {
        return AD6900_MFG;
    }
    else if (!(memcmp(&pnotify->dbcc_classguid, &GUID_DEVCLASS_AD6900_LAN, sizeof(GUID_DEVCLASS_AD6900_LAN))))
    {
        return AD6900_LAN;
    }
    else
    {
        return -1;
    }
}

static void register_notifyer(void)
{
    /* register notifyer */
    usbChangeNotificationRegister(GUID_DEVCLASS_AD6900_SPL, hwndMain, &ad6900_spl);
    usbChangeNotificationRegister(GUID_DEVCLASS_AD6900_MFG, hwndMain, &ad6900_mfg);
    usbChangeNotificationRegister(GUID_DEVCLASS_AD6900_LAN, hwndMain, &ad6900_lan);
}

static void unregister_notifyer(void)
{
    usbChangeNotificationUnRegister(&ad6900_lan);
    usbChangeNotificationUnRegister(&ad6900_spl);
    usbChangeNotificationUnRegister(&ad6900_mfg);
}

FILE *open_file_for_readonly(const char *filename)
{
    FILE *fptr = fopen(filename, "rb");
    return fptr;
}


#define START_WAITING() do { g_mi.is_waiting = 1; SetEvent(g_event_wait); } while (0)
#define STOP_WAITING()  do { g_mi.is_waiting = 0; } while (0)


static inline void start_timer(int reason, int set_time_us)
{
   
}

static inline void stop_timer(int reason)
{
    
}

void reset_all_timers (void)
{
   
}

static void show_waiting()
{
    SetWindowText(hwndLinStaticInfo, "Processing               ");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing...            ");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing... ...        ");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing    ... ...    ");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing        ... ...");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing            ...");
    Sleep(500);
    SetWindowText(hwndLinStaticInfo, "Processing               ");
    Sleep(500);
}


static DWORD WINAPI waiting_thread()
{	
    return 0;
}

#define WAKE_THREAD_UP() do {SetEvent(g_event); } while (0)

char* current_step[] = {
    "Download successfully !",
    "Waiting for mfg mode device...",
    "Waiting for spl mode device...",
    "Waiting for upgrade mode device...",
    "Error code: ",
};

static DWORD WINAPI TransferThread(LPVOID lpParam)
{
	int retval = 0;
	unsigned short percent = 0;
	unsigned short status = 0;
	unsigned char index = 0;	
	TCHAR text[256];
	HWND hwndInfo = NULL;
	char *stage[MAX_STATUS] = {"Preparing","Flashing","Verifying","Executing","Finished","Transfer"};
#ifdef DEVELOPMENT
	switch(burn_mode)
		{
			case SELECT_FILE_TRANSFER:
			/*file transfer don't get progress yet*/
			break;
			case SELECT_LINUX_PROGRAMMING:
			hwndInfo = hwndLinStaticInfo;
			break;
			case SELECT_SPL_PROGRAMMING:
			hwndInfo = hwndSPLStaticInfo;
			break;
			case SELECT_MFG_PROGRAMMING:
			hwndInfo = hwndMFGStaticInfo;
			break;
#ifdef ENABLE_DEBUG
			default:
			ERROR_MESSAGE("Unhandled burn_mode");
			break;
#endif
		}
#elif defined(PRODUCTION)
		hwndInfo = hwndMFGStaticInfo;
#elif defined(MAINTAINMENT)
		hwndInfo = hwndLinStaticInfo;
#endif

	while(1)
	{
		Sleep(250);
		if(g_transfer_complete)
		{
			if(hwndPop)
				SendMessage(hwndMain,WM_DESTROY_PROGRESS_BAR,0,0);
			SetWindowText(hwndInfo,"Transfer complete.");
			break;
		}
		retval = progress_reply_status_get(&index,&percent,&status);
#ifdef ENABLE_DEBUG
		printf("retval = %d, status = %d, index = %02x, percent = %d\n",
		retval,status,index,percent);
#endif
		if(retval != 0)
		{
			break;
		}
		if(hwndPop == NULL && status != 0)		
			SendMessage(hwndMain,WM_CREATE_PROGRESS_BAR,0,0);		

		switch(status)
		{
			case PROGRESS_STATUS_PREPARING:
			break;
			case PROGRESS_STATUS_FINISHED:
			snprintf(text,256,"%s %s",stage[status],partition_name[index]);
			break;
			case PROGRESS_STATUS_FLASHING:
			case PROGRESS_STATUS_VERIFYING:
			case PROGRESS_STATUS_EXECUTING:
			case PROGRESS_STATUS_TRANSFER:
			snprintf(text,256,"%s %s\nPercetage:[%d%%]",stage[status],partition_name[index],percent);
			break;
		}
		SetWindowText(hwndInfo,text);
		
		if(hwndPop)
		{
			snprintf(text,256,"%s %s",stage[status],partition_name[index]);
			ChangeProgressBarText(text);
			ChangeProgressBar(percent);	
		}
		
	}
	return retval;
}

static inline BOOL transfer_start(void)
{
	g_transfer_complete = FALSE;
	return NULL != CreateThread(NULL, 0, TransferThread, NULL, 0, NULL);
}
static inline void transfer_complete(void)
{
	g_transfer_complete = TRUE;
}

/*
 * the static global g_mi in devToolsUI.c is used for
 * information collection only, is a backup for g_mi array
 * in mfg_ctrl.
 *
 * the important information in g_mi is not changed during updating
 */
static DWORD WINAPI BackGroundThread(LPVOID lpParam)
{
	
	while(1)
	{
		WaitForSingleObject(g_event,INFINITE);
		ResetEvent(g_event);		
        
        if(g_locking == FALSE)
        {
			g_locking = TRUE;
            if(g_background_func)
			{
				g_background_func();
				g_background_func = NULL;
			}
			g_locking = FALSE;			
        }
		       
	}	
    return 0;
}



static BOOL StartThread(void)
{
    g_hTransfer = CreateThread(NULL, 0, BackGroundThread, NULL, 0, NULL);
    if (g_hTransfer == NULL)
    {
        ERROR_MESSAGE("create BackGroundThread error\n");
        return FALSE;
    }	
    return TRUE;
}

static BOOL start_wait_thread(void)
{
    g_h_wait = CreateThread(NULL, 0, waiting_thread, NULL, 0, NULL);
    if (g_h_wait == NULL)
    {
        ERROR_MESSAGE("create waiting_thread error\n");
        return FALSE;
    }
    return TRUE;
}

/*
 * return 1 check 0 not check
 */
#define CHECKBOX_IS_CLICK(x)  (Button_GetCheck(x) == BST_CHECKED)

/*******************************************************/
/*                 helpers                             */
/*******************************************************/
/*{*/


/*
 *  Name:     GetFileName
 *
 *  Purpose:  Get the selected file name by the Open File Common Dialog
 *
 *  Params:
 *      file name: buffer to hold the selected file name
 *      size:      buffer size
 *
 *  Return:  If the function succeeds, the return value is TRUE
 *               If the function fails, the return value is FALSE
 *
 */
static BOOL GetFileName(char *fileName, int size)
{
    OPENFILENAME ofn;

    /* Initialize OPENFILENAME */
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = fileName;

    /* Set lpstrFile[0] to '\0' so that GetOpenFileName does not
       use the contents of szFile to initialize itself */
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = size;
    ofn.Flags = OFN_PATHMUSTEXIST;

    BOOL ret = GetOpenFileName(&ofn);
	SetCurrentDirectory(work_path);
	return ret;
}

/*******************************************************/
/*                  Initialization                     */
/*******************************************************/
/*{*/
/*
 *  Name:     InitApplication
 *
 *  Purpose:  Initializes Unication Console application
 *
 *  Params:    none
 *
 *  Return:    If the function succeeds, the return value is TRUE
 *               If the function fails, the return value is FALSE
 *
 */
static BOOL InitApplication( void )
{
    WNDCLASSEX     wndclass;

    /* only one instance can run on the system */
    hMutex = CreateMutex(NULL, FALSE, UNI_APP_MUTEX);
    if(hMutex == NULL)
    {
        ERROR_MESSAGE("Create mutex error!\n");
        return FALSE;
    }

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hMutex);
        ERROR_MESSAGE(APP_TITLE" is running\n");
        return FALSE;
    }

    /* Register the Unication UniDevTools main window class */
    memset(&wndclass, 0, sizeof(WNDCLASSEX));

    wndclass.cbSize        = sizeof(WNDCLASSEX);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc   = DevToolsWindowProcedure;
    wndclass.hInstance     = hInst;
    wndclass.hIcon         = LoadIcon(hInst, MAKEINTRESOURCE (IDI_ICON));
    wndclass.hCursor       = LoadCursor(NULL, IDC_HAND);
    wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wndclass.lpszMenuName  = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClassEx(&wndclass))
    {
        CloseHandle(hMutex);
        ERROR_MESSAGE("RegisterClass error\n");
        return FALSE;
    }

	/*get current work path*/
	int ret = GetCurrentDirectory(MAX_PATH,work_path);
	if(ret <= 0 || ret > MAX_PATH)
	{
		CloseHandle(hMutex);
		ERROR_MESSAGE("Path too long\n");
		return FALSE;
	}
    return TRUE;
}


/*
 *  Name:     InitLinuxWindow
 *
 *  Purpose:  Create nand program child window
 *
 *  Params:    none
 *
 *  Return:    none
 *
 */
static void InitLinuxWindow(void)
{
#define HEIGHT_TAG        (2*Y_MARGIN)
#define PAGE_START_X      (X_MARGIN)
#define PAGE_START_Y      (HEIGHT_TAG+Y_MARGIN)
#define GBOX1_START_X     (X_MARGIN)
#define GBOX1_START_Y     (Y_MARGIN)
#define H_GAPS            (X_MARGIN/2)
#define V_GAPS            (Y_MARGIN/2)
#define COMPENSATION      (2*Y_MARGIN)
#define STATIC_WIDTH      (max_Groupbox1_width-2*X_MARGIN-4*H_GAPS)

    int max_page_width = rcClient.right-2*X_MARGIN;
    int max_page_height = rcClient.bottom-HEIGHT_TAG-2*Y_MARGIN;

    int max_Groupbox1_width = max_page_width-2*X_MARGIN;
    int max_Groupbox1_height = (max_page_height - 3*Y_MARGIN) * 3 / 5 - Y_MARGIN - COMPENSATION;

    int max_Groupbox2_width = max_Groupbox1_width;
    int max_Groupbox2_height = (max_page_height - 3*Y_MARGIN) * 2 / 5 - Y_MARGIN + COMPENSATION;
    int start_y_of_Groupbox2 = Y_MARGIN + max_Groupbox1_height + 3*Y_MARGIN;
    int relative_x;
    int relative_y;
	const TCHAR *readme;

    printf("hwndLinPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndLinPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndLinPage, "hwndLinPage");

    printf("hwndLinGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, GBOX1_START_Y, max_Groupbox1_width, max_Groupbox1_height);

    hwndLinGroupReadme = CreateWindow( TEXT ("button"), "Readme",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, GBOX1_START_Y,
            max_Groupbox1_width, max_Groupbox1_height,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinGroupReadme, "hwndLinGroupReadme");

    relative_x = GBOX1_START_X + V_GAPS;
    relative_y = GBOX1_START_Y;

    sprintf(project_target, "%s%s%s", "Project:  ",PROJECT , target_type[0]);

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
#ifdef MAINTAINMENT
	readme = TEXT("1. Connect Device with PC via USB cable.\n2. Press CheckImg Button.\n3. Press Upgrade/Download Button and wait until it finished.\n4. If any error occurs during updating/downloading,please reconnect device and reopen this tool to try it again.");
#else
	readme = project_target;
#endif
    printf("hwndLinTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndLinPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndLinTextTarget, "hwndLinTextTarget");

      
    relative_y += 2 * (HEIGHT_CONTROL + V_GAPS) + HEIGHT_CONTROL+35;

    printf("hwndLinStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndLinStaticInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL+10,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticInfo, "hwndLinStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    printf("hwndLinGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndLinGroupOptions = CreateWindow( TEXT ("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinGroupOptions, "hwndLinGroupOptions");



    relative_x = GBOX1_START_X + X_MARGIN;
	relative_y = start_y_of_Groupbox2 + Y_MARGIN*3;
#ifdef MAINTAINMENT
	hwndCheckBoxDelete = CreateWindow( TEXT("button"), "Delete User Data",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            relative_x, relative_y,
            WIDTH_BUTTON*2, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL;
	hwndLinStaticNotice = CreateWindow( TEXT("static"), "**CAUTION**\nAll user data will be delete if this option is selected.",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL+2*Y_MARGIN,
            hwndLinPage, NULL,
            hInst, NULL);   
    relative_y += HEIGHT_CONTROL + 2*Y_MARGIN;
#endif

#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
	hwndLinIpAddr = CreateWindow(TEXT("static"), "DevIpAddr:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndDevLists, "hwndDevLists");
	relative_x += WIDTH_BUTTON + X_MARGIN;
	
	hwndLinDevList = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("ComboBox"),"",
			WS_CHILD | WS_VISIBLE |CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_UPPERCASE | CBS_AUTOHSCROLL,
			relative_x,relative_y,
			WIDTH_COMBO, HEIGHT_CONTROL*5,
            hwndLinPage, NULL,
            hInst, NULL);
	relative_x += WIDTH_COMBO + X_MARGIN;		
	hwndLinBtnRefresh = CreateWindow(TEXT("button"), "Refresh",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinBtnRefresh, "hwndLinBtnRefresh");
#endif
#ifdef DEVELOPMENT
	partition_x_pos = GBOX1_START_X + V_GAPS;;
	partition_y_pos = relative_y + HEIGHT_CONTROL + Y_MARGIN;

#endif
    relative_x = max_Groupbox2_width - WIDTH_BUTTON - X_MARGIN;
    relative_y = start_y_of_Groupbox2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	printf("hwndLinBtnCheckImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndLinBtnCheckImg = CreateWindow(TEXT ("button"), "CheckImg",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinBtnCheckImg, "hwndLinBtnCheckImg");
    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;

    printf("hwndLinBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);
#ifdef MAINTAINMENT
	hwndLinBtnDown = CreateWindow( TEXT ("button"), "Upgrade",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);	
#else
    hwndLinBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
#endif
    debug_positions(hwndLinBtnDown, "hwndLinBtnDown");
	
	 printf("hwndLinBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);


    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#ifdef DEVELOPMENT
    printf("hwndBtnEnableTelnet's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndBtnEnableTelnet = CreateWindow( TEXT("button"), "EnableTelnet",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndBtnEnableTelnet, "hwndBtnEnableTelnet");
	relative_y += HEIGHT_CONTROL + V_GAPS*2;
#endif
#if (defined(CONFIG_PROJECT_G4_BBA) )/*G4 support battery check*/	
    printf("hwndCheckBoxSkipBatCheck's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x-4*X_MARGIN, relative_y, WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5);

    hwndCheckBoxSkipBatCheck = CreateWindow(TEXT("button"), TEXT("Skip battery check"),
            WS_CHILD | WS_VISIBLE  | BS_AUTOCHECKBOX,
            relative_x-4*X_MARGIN, relative_y,
            WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndCheckBoxSkipBatCheck, "hwndCheckBoxSkipBatCheck");
        
    relative_y += HEIGHT_CONTROL-5 + V_GAPS*2;
#endif

#if (defined(DEVELOPMENT) || defined(MAINTAINMENT))
    ShowWindow(hwndLinPage, TRUE);
#endif
    return;
}
/*
 *  Name:     InitSPLWindow
 *
 *  Purpose:  Create SPL window
 *
 *  Params:    none
 *
 *  Return:    none
 *
 */
static void InitSPLWindow(void)
{
	int max_page_width = rcClient.right-2*X_MARGIN;
    int max_page_height = rcClient.bottom-HEIGHT_TAG-2*Y_MARGIN;

    int max_Groupbox1_width = max_page_width-2*X_MARGIN;
    int max_Groupbox1_height = (max_page_height - 3*Y_MARGIN) * 3 / 5 - Y_MARGIN - COMPENSATION;

    int max_Groupbox2_width = max_Groupbox1_width;
    int max_Groupbox2_height = (max_page_height - 3*Y_MARGIN) * 2 / 5 - Y_MARGIN + COMPENSATION;
    int start_y_of_Groupbox2 = Y_MARGIN + max_Groupbox1_height + 3*Y_MARGIN;
    int relative_x;
    int relative_y;
	const TCHAR *readme;

    printf("hwndSPLPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndSPLPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndSPLPage, "hwndSPLPage");

    printf("hwndSPLGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, GBOX1_START_Y, max_Groupbox1_width, max_Groupbox1_height);

    hwndSPLGroupReadme = CreateWindow( TEXT ("button"), "Readme",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, GBOX1_START_Y,
            max_Groupbox1_width, max_Groupbox1_height,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLGroupReadme, "hwndSPLGroupReadme");

    relative_x = GBOX1_START_X + V_GAPS;
    relative_y = GBOX1_START_Y;

    sprintf(project_target, "%s%s%s", "Project:  ",PROJECT , target_type[1]);

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	readme = project_target;
	printf("hwndSPLTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndSPLPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndSPLTextTarget, "hwndSPLTextTarget");

      
    relative_y += 2 * (HEIGHT_CONTROL + V_GAPS) + HEIGHT_CONTROL+35;

    printf("hwndSPLStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndSPLStaticInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL+10,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLStaticInfo, "hwndSPLStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    printf("hwndSPLGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndSPLGroupOptions = CreateWindow( TEXT ("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLGroupOptions, "hwndSPLGroupOptions");



    relative_x = GBOX1_START_X + X_MARGIN;
	relative_y = start_y_of_Groupbox2 + Y_MARGIN*3;


#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
	hwndSPLIpAddr = CreateWindow(TEXT("static"), "DevIpAddr:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndDevLists, "hwndDevLists");
	relative_x += WIDTH_BUTTON + X_MARGIN;
	
	hwndSPLDevList = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("ComboBox"),"",
			WS_CHILD | WS_VISIBLE |CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_UPPERCASE | CBS_AUTOHSCROLL,
			relative_x,relative_y,
			WIDTH_COMBO, HEIGHT_CONTROL*5,
            hwndSPLPage, NULL,
            hInst, NULL);
	relative_x += WIDTH_COMBO + X_MARGIN;		
	hwndSPLBtnRefresh = CreateWindow(TEXT("button"), "Refresh",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnRefresh, "hwndSPLBtnRefresh");
#endif

    relative_x = max_Groupbox2_width - WIDTH_BUTTON - X_MARGIN;
    relative_y = start_y_of_Groupbox2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	printf("hwndSPLBtnCheckImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndSPLBtnCheckImg = CreateWindow(TEXT ("button"), "CheckImg",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnCheckImg, "hwndSPLBtnCheckImg");
    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;

    printf("hwndSPLBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndSPLBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnDown, "hwndSPLBtnDown");
	
	 printf("hwndSPLBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

#ifdef PRODUCTION
    hwndSPLBtnStop = CreateWindow( TEXT ("button"), "Stop",
            WS_CHILD /*| WS_VISIBLE */| BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnStop, "hwndSPLBtnStop");

    relative_y += HEIGHT_CONTROL + V_GAPS*2;



    printf("hwndCheckBoxBatch's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x-4*X_MARGIN, relative_y, WIDTH_BUTTON*2, HEIGHT_CONTROL);

    hwndCheckBoxBatch = CreateWindow(TEXT("button"), TEXT("Batch Programing"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            relative_x-4*X_MARGIN, relative_y,
            WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndCheckBoxBatch, "hwndCheckBoxBatch");
	
#endif
}
/*
 *  Name:     InitMFGWindow
 *
 *  Purpose:  Create MFG window
 *
 *  Params:    none
 *
 *  Return:    none
 *
 */
static void InitMFGWindow(void)
{
	int max_page_width = rcClient.right-2*X_MARGIN;
    int max_page_height = rcClient.bottom-HEIGHT_TAG-2*Y_MARGIN;

    int max_Groupbox1_width = max_page_width-2*X_MARGIN;
    int max_Groupbox1_height = (max_page_height - 3*Y_MARGIN) * 3 / 5 - Y_MARGIN - COMPENSATION;

    int max_Groupbox2_width = max_Groupbox1_width;
    int max_Groupbox2_height = (max_page_height - 3*Y_MARGIN) * 2 / 5 - Y_MARGIN + COMPENSATION;
    int start_y_of_Groupbox2 = Y_MARGIN + max_Groupbox1_height + 3*Y_MARGIN;
    int relative_x;
    int relative_y;
	const TCHAR *readme;

    printf("hwndMFGPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndMFGPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndMFGPage, "hwndMFGPage");

    printf("hwndMFGGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, GBOX1_START_Y, max_Groupbox1_width, max_Groupbox1_height);

    hwndMFGGroupReadme = CreateWindow( TEXT ("button"), "Readme",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, GBOX1_START_Y,
            max_Groupbox1_width, max_Groupbox1_height,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGGroupReadme, "hwndMFGGroupReadme");

    relative_x = GBOX1_START_X + V_GAPS;
    relative_y = GBOX1_START_Y;

    sprintf(project_target, "%s%s%s", "Project:  ",PROJECT , target_type[2]);

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	readme = project_target;
	printf("hwndMFGTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndMFGPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndMFGTextTarget, "hwndMFGTextTarget");

      
    relative_y += 2 * (HEIGHT_CONTROL + V_GAPS) + HEIGHT_CONTROL+35;

    printf("hwndMFGStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndMFGStaticInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL+10,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGStaticInfo, "hwndMFGStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    printf("hwndMFGGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndMFGGroupOptions = CreateWindow( TEXT ("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGGroupOptions, "hwndMFGGroupOptions");



    relative_x = GBOX1_START_X + X_MARGIN;
	relative_y = start_y_of_Groupbox2 + Y_MARGIN*3;


#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
	hwndMFGIpAddr = CreateWindow(TEXT("static"), "DevIpAddr:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndDevLists, "hwndDevLists");
	relative_x += WIDTH_BUTTON + X_MARGIN;
	
	hwndMFGDevList = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("ComboBox"),"",
			WS_CHILD | WS_VISIBLE |CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_UPPERCASE | CBS_AUTOHSCROLL,
			relative_x,relative_y,
			WIDTH_COMBO, HEIGHT_CONTROL*5,
            hwndMFGPage, NULL,
            hInst, NULL);
	relative_x += WIDTH_COMBO + X_MARGIN;		
	hwndMFGBtnRefresh = CreateWindow(TEXT("button"), "Refresh",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnRefresh, "hwndMFGBtnRefresh");
#endif

    relative_x = max_Groupbox2_width - WIDTH_BUTTON - X_MARGIN;
    relative_y = start_y_of_Groupbox2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	printf("hwndMFGBtnCheckImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndMFGBtnCheckImg = CreateWindow(TEXT ("button"), "CheckImg",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnCheckImg, "hwndMFGBtnCheckImg");
    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;

    printf("hwndMFGBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndMFGBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnDown, "hwndMFGBtnDown");
	
	 printf("hwndMFGBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

#ifdef PRODUCTION
    hwndMFGBtnStop = CreateWindow( TEXT ("button"), "Stop",
            WS_CHILD /*| WS_VISIBLE */| BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnStop, "hwndMFGBtnStop");

    relative_y += HEIGHT_CONTROL + V_GAPS*2;



    printf("hwndCheckBoxBatch's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x-4*X_MARGIN, relative_y, WIDTH_BUTTON*2, HEIGHT_CONTROL);

    hwndCheckBoxBatch = CreateWindow(TEXT("button"), TEXT("Batch Programing"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            relative_x-4*X_MARGIN, relative_y,
            WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndCheckBoxBatch, "hwndCheckBoxBatch");
	ShowWindow(hwndMFGPage, TRUE);
#endif
}
/*
 *  Name:     InitMainWindow
 *
 *  Purpose:  Create Main window and Tab control of Unication Dev Tools
 *
 *  Params:    none
 *
 *  Return:    If the function succeeds, the return value is TRUE
 *               If the function fails, the return value is FALSE
 *
 */
static BOOL InitMainWindow(void)
{
    TCITEM      tie;
    int         i;
    INITCOMMONCONTROLSEX icex;
    TCHAR mainTitle[128];

    /* Get user screen information */
    screenWidth = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    /* Show the main window in the screen center */
    sprintf(mainTitle, TEXT("%s For %s"), APP_TITLE, PROJECT);
    hwndMain = CreateWindowEx(0, szAppName, mainTitle,
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPSIBLINGS,
            (screenWidth-DEV_TOOLS_WIDTH)/2, (screenHeight-DEV_TOOLS_HEIGHT)/2,
            DEV_TOOLS_WIDTH, DEV_TOOLS_HEIGHT,
            NULL, NULL, hInst, NULL);
    if(hwndMain == NULL)
    {
        ERROR_MESSAGE("Create Main Window error\n");
        return FALSE;
    }

    /* Initialize tab controls */
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    /* Create a tab control child window */
    GetClientRect(hwndMain, &rcClient);
    hwndTab = CreateWindowEx(0, WC_TABCONTROL, TEXT(""),
            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, rcClient.right, rcClient.bottom,
            hwndMain, NULL, hInst, NULL);

    if (hwndTab == NULL)
    {
        ERROR_MESSAGE("Create Tab Window error\n");
        DestroyWindow(hwndMain);
        return FALSE;
    }

    /* Add tabs string */
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;

    for(i = 0; i < DEV_TOOLS_NUMBER; i++)
    {
        tie.pszText = tabString[i];
        if(TabCtrl_InsertItem(hwndTab, i, &tie) == -1)
        {
            ERROR_MESSAGE("Add Tabs error\n");
            DestroyWindow(hwndTab);
            DestroyWindow(hwndMain);
            return FALSE;
        }
    }

    return TRUE;
}



void update_ui_resources(int enable)
{
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:
		EnableWindow(hwndLinBtnCheckImg,enable);
		EnableWindow(hwndLinBtnDown,enable);
		EnableWindow(hwndLinBtnRefresh,enable);
		EnableWindow(hwndCheckBoxDelete,enable);
		EnableWindow(hwndBtnEnableTelnet,enable);
		EnableWindow(hwndCheckBoxSkipBatCheck,enable);
		
#ifdef DEVELOPMENT
		EnablePartitionList(enable);
#endif
		break;
		case SELECT_SPL_PROGRAMMING:
		//EnableWindow(hwndLinBtnCheckImg,enable);
		EnableWindow(hwndSPLBtnDown,enable);		
		break;
		case SELECT_MFG_PROGRAMMING:
		EnableWindow(hwndMFGBtnDown,enable);
		EnableWindow(hwndCheckBoxBatch,enable);
		EnableWindow(hwndMFGBtnStop,enable);
		break;
		case SELECT_FILE_TRANSFER:		
		break;
	}
	EnableWindow(hwndMain,enable);
}


static void PopProgressBar( void )
{   

    /* Create and show the Progress bar window */
    hwndPop = CreateWindowEx(0, szAppName, TEXT ("Transfer"),
                         WS_POPUP | WS_CAPTION | WS_CLIPSIBLINGS|WS_VISIBLE,
                         (screenWidth-TAB_CLIENT_WIDTH)/2, (screenHeight-POP_WIN_HEIGHT)/2,
                         TAB_CLIENT_WIDTH, POP_WIN_HEIGHT,
                         hwndTab, NULL, hInst, NULL);
    
    hwndPopStatic = CreateWindow ( TEXT ("static"), "Start Transfer....",
                         WS_CHILD | WS_VISIBLE  | SS_LEFT,
                         X_MARGIN, Y_MARGIN,
                         300, HEIGHT_CONTROL,
                         hwndPop, NULL,
                         hInst, NULL);

    hwndPopPercent = CreateWindow ( TEXT ("static"), "",
                         WS_CHILD | WS_VISIBLE  | SS_LEFT,
                         X_MARGIN + 300, Y_MARGIN,
                         60, HEIGHT_CONTROL,
                         hwndPop, NULL,
                         hInst, NULL);

    hwndPopProgress = CreateWindow( PROGRESS_CLASS, "",
                        WS_CHILD | WS_VISIBLE | PBS_SMOOTH,
                        X_MARGIN, Y_MARGIN+HEIGHT_CONTROL,
                        TAB_CLIENT_WIDTH-2*X_MARGIN-X_MARGIN/2, HEIGHT_CONTROL,
                        hwndPop, NULL,
                        hInst, NULL);
      
}

static BOOL network_init(void)
{
	int retval = -1;	
	int image_len = 0;
	unsigned char *buff = NULL;
	
	image_len = get_file_len(ini_file_info.name_of_image);
	if(image_len <= 0)
	{
		ERROR_MESSAGE("%s not exsit.",ini_file_info.name_of_image);
		return FALSE;
	}
	/*alloc memory for image_buffer,it will be released when tool terminated*/
	void *ptmp = realloc(image_buffer,image_len);
	if(ptmp == NULL)
	{
		free(image_buffer);
		image_buffer = NULL;
		ERROR_MESSAGE("Out of memory.");
		return FALSE;
	}
	image_buffer = ptmp;
	/*read image into iamge_buffer*/
	retval = read_file(image_buffer,image_len,ini_file_info.name_of_image);
	if(image_len != retval)
	{
		printf("ini_file_info.name_of_image = %s\n",ini_file_info.name_of_image);
		printf("retval = %d, image_len = %d\n",retval,image_len);
		ERROR_MESSAGE("%s read error.",ini_file_info.name_of_image);
		return FALSE;
	}
	printf("before init\n");
	/*make WinUpgradeLibInit run in backgroud_func*/
#ifdef DEVELOPMENT
	switch(burn_mode)
	{
		case SELECT_FILE_TRANSFER:
		case SELECT_LINUX_PROGRAMMING:
		#ifdef U3_LIB
		retval = WinUpgradeLibInit(ini_file_info.name_of_image,image_len);
		#else		
		retval = WinUpgradeLibInit(image_buffer,image_len,ini_file_info.ip,Button_GetCheck(hwndCheckBoxSkipBatCheck) == BST_CHECKED ? 0 : 1);
		printf("name_of_image = %s\nimage_len = %d\nip = %s\n",ini_file_info.name_of_image,image_len,ini_file_info.ip);
		#endif
		break;
		case SELECT_SPL_PROGRAMMING:
		/*SPL Programing not support yet*/
		break;
		case SELECT_MFG_PROGRAMMING:
		retval = burnMFGinit(ini_file_info.name_of_image);
		break;
	}
#elif defined(MAINTAINMENT)
#ifdef U3_LIB
	retval = WinUpgradeLibInit(ini_file_info.name_of_image,image_len);
#else
	retval = WinUpgradeLibInit(image_buffer,image_len,ini_file_info.ip,Button_GetCheck(hwndCheckBoxSkipBatCheck) == BST_CHECKED ? 0 : 1);
#endif
#elif defined(PRODUCTION)
	retval = burnMFGinit(ini_file_info.name_of_image);
#endif		
	if(retval != 0)
	{
		/*just print the errcode string*/
		ERROR_MESSAGE("%s",get_error_info(retval));
		return FALSE;
	}
	
#if 0	
/*let invoker to process according the return value*/
#ifdef DEVELOPMENT
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:
		printf("Linux PROGRAMMING for developer.\n");		
		//enable Download button
		EnableWindow(hwndLinBtnDown,TRUE);
		break;
		case SELECT_SPL_PROGRAMMING:
		printf("SPL PROGRAMMING for developer.\n");
		/*SPL Programing not support yet*/
		EnableWindow(hwndSPLBtnDown,TRUE);
		break;
		case SELECT_MFG_PROGRAMMING:
		printf("MFG PROGRAMMING for developer.\n");
		EnableWindow(hwndMFGBtnDown,TRUE);
		break;
	}
#elif defined(MAINTAINMENT)
	EnableWindow(hwndLinBtnDown,TRUE);
#elif defined(PRODUCTION)
	EnableWindow(hwndMFGBtnDown,TRUE);		
#endif

#endif
	
	return TRUE;
}
static inline BOOL check_ini(void)
{
	/*read the ini file and parse it into struct*/
	if(get_file_len(ini_file) <= 0)
	{
		ERROR_MESSAGE("%s lost.",ini_file);
		return FALSE;
	}
	if(parse_ini_file(ini_file) == FALSE)
	{
		ERROR_MESSAGE("%s syntax error.",ini_file);
		return FALSE;
	}
	printf("ini_file = %s\n",ini_file);
	printf("ip = %s, image = %s\n",ini_file_info.ip,ini_file_info.name_of_image);
	if(get_image_info(ini_file_info.name_of_image) == FALSE)
	{
		ERROR_MESSAGE("get image partition failed.check if image file exsit.");
		return FALSE;
	}
	return TRUE;
}
/*********************Nand Programing Button Process***********************/
static void OnBtnCheckImgClick(void)
{
	/*only invoke by developer tools for linux*/
	EnableWindow(hwndMain,FALSE);	
	if(check_ini() == TRUE)
	{
		SendMessage(hwndMain,WM_CREATE_PARTITION_LIST,0,0);
		EnableWindow(hwndLinBtnDown,TRUE);
	}		
	EnableWindow(hwndMain,TRUE);
}

static void OnBtnDownClick(void)
{
	/*begin to download partition.
	if it's for consumer,then it's according the ini file,
	if it's for developer,then it's according the autocheckbox,
	if it's for factory,then it's MFG burning,and if Batch box is checked,
	then Down button will hiden,and Stop button show
	*/
	int retval = -1;
	int partition_selected = 0;
	HWND hwndInfo = NULL;
	int i = 0;	
#ifdef DEVELOPMENT
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:
		hwndInfo = hwndLinStaticInfo;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}
		if(partition_selected == 0)
		{
			ERROR_MESSAGE("No partition select.");
			return ;
		}
		break;
		case SELECT_SPL_PROGRAMMING:
		hwndInfo = hwndSPLStaticInfo;
		break;
		case SELECT_MFG_PROGRAMMING:
		hwndInfo = hwndMFGStaticInfo;
		break;
	}
#elif defined(MAINTAINMENT)
	hwndInfo = hwndLinStaticInfo;
#elif defined(PRODUCTION)
	hwndInfo = hwndMFGStaticInfo;
#endif
	SetWindowText(hwndInfo,"Waiting for target reboot into upgrade mode..");
	if(FALSE == network_init())
	{
		SetWindowText(hwndInfo,"Error occurs,please try it again.");
		return ;
	}
	SetWindowText(hwndInfo,"Init success.");
	transfer_start();/*start a transfer thread*/
	EnableWindow(hwndLinPage,FALSE);	
	
#ifdef DEVELOPMENT
	
	EnablePartitionList(FALSE);
	
	printf("partition_selected = 0x%04x\n",partition_selected);
	retval = burnpartition(partition_selected);
	
#elif defined(MAINTAINMENT)
	if(Button_GetCheck(hwndCheckBoxDelete) == BST_CHECKED)
		retval = burnpartition(0x30);/*is it right*/
	else
		retval = burnImage();
#elif defined(PRODUCTION)
	partition_selected = 0x3F;/*all partition*/
#endif
	transfer_complete();	
	if(retval != 0)
	{		
		SetWindowText(hwndInfo,"Operation failed!");
		ERROR_MESSAGE("Operation failed!Please try it again.");		
	}
	else
	{		
		/*reboot target*/
		SetWindowText(hwndInfo,"Rebooting target..");
		retval = RebootTarget();
		if(retval != 0)
			ERROR_MESSAGE("Reboot target failed!,you need reboot it manully.");
		else
		{
			SetWindowText(hwndInfo,"Operation Success!");
			MessageBox(hwndMain,TEXT("Complete!"),szAppName,MB_ICONINFORMATION);
		}
	}
	
#ifdef DEVELOPMENT
	EnablePartitionList(TRUE);
#endif
	EnableWindow(hwndLinPage,TRUE);
}
static void OnBtnStopClick(void)
{
}
static void OnBtnRefreshClick(void)
{
}
static void OnBtnEnableTelnetClick(void)
{
	int retval = EnableTelnet();
	if(retval != 0)
		ERROR_MESSAGE("Enable telnet failed!");
}

static void OnBtnFileDownload(void)
{
	char file_for_pc[MAX_PATH];
	char file_for_target[MAX_PATH];
	int file_len = 0;
	int retval = -1;
	FILE *fp = NULL;
	unsigned char *buff = NULL;
	GetWindowText(hwndFdEditDestFile,file_for_target,MAX_PATH);
	if(file_for_target[0] == 0)
	{
		SetWindowText(hwndFdNotify,"Dest Path can't be empty.");
		ERROR_MESSAGE("Dest Path can't be empty.");
		return ;
	}
	GetWindowText(hwndFdStaticSrcFile,file_for_pc,MAX_PATH);
	get_abs_file_name(file_for_target,file_for_pc);
	printf("file_for_target = %s\n",file_for_target);
	printf("file_for_pc = %s\n",file_for_pc);
	/*Disable Main Window and prepare to download*/
	EnableWindow(hwndTab,FALSE);
	SetWindowText(hwndFdNotify,"Init network..");
	if(network_init() == FALSE)
	{
		EnableWindow(hwndTab,TRUE);
		SetWindowText(hwndFdNotify,"Init network error.");
		return ;
	}
	SetWindowText(hwndFdNotify,"Init network success.");
	
	SetWindowText(hwndFdNotify,"Download...");
#ifdef U3_LIB
	retval = file_download(file_for_pc,file_for_target);
#else
	/*read file into buff and call download_file*/		
	
	buff = (unsigned char*)malloc(MAX_BUFF_LEN);
	if(buff == NULL)
	{
		ERROR_MESSAGE("Out of memory.");
		goto malloc_error;
	}
	fp = fopen(file_for_pc,"rb");
	if(fp == NULL)
	{
		ERROR_MESSAGE("%s can't open.",file_for_pc);
		goto open_error;
	}
	fseek(fp,0,SEEK_END);
	file_len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	if(file_len != fread(buff,1,file_len,fp))
	{
		ERROR_MESSAGE("%s read error.",file_for_pc);
		goto read_error;
	}
	printf("before download_file\n");
	printf("file_for_target:%s\n",file_for_target);	
	#warning "need remove when libupgrade.dll upgraded"
	buff[file_len++] = 0;	
	retval = download_file(buff,file_len,file_for_target);
	printf("retval = %d\n",retval);
	printf("after download_file\n");	
#endif
	if(retval != 0)
	{
		SetWindowText(hwndFdNotify,"Download error.");
		ERROR_MESSAGE("Download error:%s",get_error_info(retval));
		goto EXIT;
	}
	SetWindowText(hwndFdNotify,"Download success.");
	
EXIT:/*exit for download and upload*/
#ifndef U3_LIB
download_error:		
read_error:
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
open_error:
	if(buff)
	{
		free(buff);
		buff = NULL;
	}
malloc_error:
#endif
	EnableWindow(hwndTab,TRUE);
	
}

static void OnBtnFileUpload(void)
{
	char file_for_pc[MAX_PATH];
	char file_for_target[MAX_PATH];
	int file_len = 0;
	int retval = -1;
	FILE *fp = NULL;
	unsigned char *buff = NULL;
	
	GetWindowText(hwndFuEditSrcFile,file_for_target,MAX_PATH);
	if(file_for_target[0] == 0)
	{
		SetWindowText(hwndFuNotify,"Src File can't be empty.");
		ERROR_MESSAGE("Src File can't be empty.");
		return ;
	}
	printf("file_for_target = %s\n",file_for_target);
	printf("file_for_pc = %s\n",file_for_pc);
	/*Disable Main Window and prepare to upload*/
	
	GetWindowText(hwndFuStaticDestFile,file_for_pc,MAX_PATH);
	
	EnableWindow(hwndTab,FALSE);
	SetWindowText(hwndFuNotify,"Init network..");
	if(network_init() == FALSE)
	{
		EnableWindow(hwndTab,TRUE);
		SetWindowText(hwndFuNotify,"Init network error.");
		return ;
	}
	SetWindowText(hwndFuNotify,"Upload...");
#ifdef U3_LIB
	retval = file_upload(file_for_pc,file_for_target);
#else
	/*read file into buff and call download_file*/			
	
	buff = (unsigned char*)malloc(MAX_BUFF_LEN);
	if(buff == NULL)
	{
		ERROR_MESSAGE("Out of memory.");
		goto malloc_error;
	}	
	
	retval = upload_file(buff,&file_len,file_for_target);
	
#endif
	if(retval != 0)
	{
		SetWindowText(hwndFuNotify,"Upload error.");
		ERROR_MESSAGE("Upload error:%s",get_error_info(retval));
		goto EXIT;
	}
	printf("recevied file_len = %d\n",file_len);
#ifndef U3_LIB
	/*then write buff into current files*/		
	fp = fopen(file_for_pc,"wb+");
	if(fp == NULL)
	{
		SetWindowText(hwndFuNotify,"Upload error.");
		ERROR_MESSAGE("%s can't open.",file_for_pc);
		goto open_error;
	}
	
	if(file_len != fwrite(buff,1,file_len,fp))
	{
		SetWindowText(hwndFuNotify,"Upload error.");
		ERROR_MESSAGE("%s write error.",file_for_pc);
		goto write_error;
	}
#endif
	SetWindowText(hwndFuNotify,"Upload success.");
	
EXIT:/*exit for download and upload*/
#ifndef U3_LIB
download_error:
write_error:
	if(fp)
	{
		fclose(fp);
		fp = NULL;
	}
open_error:
	if(buff)
	{
		free(buff);
		buff = NULL;
	}
malloc_error:
#endif
	EnableWindow(hwndTab,TRUE);
	
}
/*********************File Transfer Button Process***********************/

static BOOL ProcessFileTransferCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	char file_for_pc[MAX_PATH];
	if(hwnd == hwndFdButtonBrowse)
	{
		if(GetFileName(file_for_pc,MAX_PATH) == TRUE)
		{
			SetWindowText(hwndFdStaticSrcFile,file_for_pc);
			EnableWindow(hwndFdButtonDownload,TRUE);			
		}
	}
	else if(hwnd == hwndFdButtonDownload)
	{
		g_background_func = OnBtnFileDownload;
		WAKE_THREAD_UP();
	}
	else if(hwnd == hwndFuButtonBrowse)
	{
		if(GetFileName(file_for_pc,MAX_PATH) == TRUE)
		{
			SetWindowText(hwndFuStaticDestFile,file_for_pc);
			EnableWindow(hwndFuButtonUpload,TRUE);
		}
	}
	else if(hwnd == hwndFuButtonUpload)
	{
		g_background_func = OnBtnFileUpload;
		WAKE_THREAD_UP();
	}
	return TRUE;
}

static BOOL ProcessLinuxCommand(WPARAM wParam, LPARAM lParam)
{
	
/*haven't add devipaddr process yet*/	
	HWND hwnd = (HWND)lParam;
#ifdef MAINTAINMENT
	if(hwnd == hwndCheckBoxDelete)
	{
		if(Button_GetCheck(hwndCheckBoxDelete) == BST_CHECKED)
			SetWindowText(hwndLinBtnDown,TEXT("Download"));
		else
			SetWindowText(hwndLinBtnDown,TEXT("Upgrade"));
		return TRUE;
	}
#endif
	
	if(hwnd == hwndLinBtnCheckImg)	
		g_background_func = OnBtnCheckImgClick;	
	else if(hwnd == hwndLinBtnDown)
		g_background_func = OnBtnDownClick;	
	else if(hwnd == hwndBtnEnableTelnet)
		g_background_func = OnBtnEnableTelnetClick;
	else if(hwnd == hwndLinBtnRefresh)
		g_background_func = OnBtnRefreshClick;
	else 
		return FALSE;
	WAKE_THREAD_UP();	
	return TRUE;
}
static BOOL ProcessSPLCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
		
	if(hwnd == hwndSPLBtnDown)
		g_background_func = OnBtnDownClick;	
	else 
		return FALSE;
	WAKE_THREAD_UP();
	return TRUE;
}
static BOOL ProcessMFGCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;	
	if(hwnd == hwndMFGBtnDown)
		g_background_func = OnBtnDownClick;	
	else 
		return FALSE;
	WAKE_THREAD_UP();
	return TRUE;
}
/*
 *  Name:     DevToolsWindowProcedure
 *
 *  Purpose:  Application-defined function that processes messages sent to the Dev tools window
 *
 *  Params:
 *      hwnd:  A handle to the window
 *
 *      message:    The message sent to the Dev tools window
 *
 *      wParam lParam:  Additional message information
 *                 The contents of this parameter depend on the value of the message parameter
 *
 *  Return:The return value is the result of the message processing and depends on the message sent
 *
 */
LRESULT CALLBACK DevToolsWindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int retval = 0;
    HWND child;

    switch(message)
    {
        case WM_COMMAND:		
		if(g_locking == TRUE)
			break;
		if(hwnd == hwndLinPage)
			return ProcessLinuxCommand(wParam,lParam);
		else if(hwnd == hwndSPLPage)
			return ProcessSPLCommand(wParam,lParam);
		else if(hwnd == hwndMFGPage)
			return ProcessMFGCommand(wParam,lParam);
		else if(hwnd == hwndFileTransferPage)
			return ProcessFileTransferCommand(wParam,lParam);
		break;
		
        case WM_CREATE:
            break;
        case WM_DESTROY: /* trigger by callback */
            {
                if (hwnd == hwndMain)
                {
                    unregister_notifyer();
					PostQuitMessage(0);
                }                
            }
            break;
        case WM_TIMER: /* trigger by timeout */            
                
            return TRUE;
        case WM_NOTIFY: /* trigger by user click */
            switch(((LPNMHDR)lParam)->code)
            {
				/*only development have mutiple tab controll*/
#ifdef DEVELOPMENT
                case TCN_SELCHANGE:
                    {						
                        burn_mode = TabCtrl_GetCurSel(hwndTab);
                        switch(burn_mode)
						{
							case SELECT_MFG_PROGRAMMING:
							ShowWindow(hwndMFGPage,TRUE);
							ShowWindow(hwndSPLPage,FALSE);
							ShowWindow(hwndLinPage,FALSE);
							ShowWindow(hwndFileTransferPage,FALSE);
							ini_file = "for_mfg_file.ini";							
							break;
							case SELECT_SPL_PROGRAMMING:
							ShowWindow(hwndMFGPage,FALSE);
							ShowWindow(hwndSPLPage,TRUE);
							ShowWindow(hwndLinPage,FALSE);
							ShowWindow(hwndFileTransferPage,FALSE);
							ini_file = "for_user_file.ini";
							break;
							case SELECT_LINUX_PROGRAMMING:
							ShowWindow(hwndMFGPage,FALSE);
							ShowWindow(hwndSPLPage,FALSE);
							ShowWindow(hwndLinPage,TRUE);
							ShowWindow(hwndFileTransferPage,FALSE);
							ini_file = "for_user_file.ini";
							break;
							case SELECT_FILE_TRANSFER:
							ShowWindow(hwndMFGPage,FALSE);
							ShowWindow(hwndSPLPage,FALSE);
							ShowWindow(hwndLinPage,FALSE);
							ShowWindow(hwndFileTransferPage,TRUE);
							ini_file = "for_user_file.ini";
							break;
						}						
                    }
                    return TRUE;
#endif
                default:
                    break;
            }
            return FALSE;

        case WM_DEVICECHANGE: /* trigger by USB cable insert */
		{
			/*needed by MFG burning*/
			int retval;				
		}
		break;
		case WM_CREATE_PARTITION_LIST:
		CreatePartitionList();
		break;
		case WM_DESTROY_PARTITION_LIST:
		DestoryPartitionList();
		break;
		case WM_CREATE_PROGRESS_BAR:
		PopProgressBar();
		break;		
		case WM_DESTROY_PROGRESS_BAR:
		DestoryProgressBar();
		break;
		/*just print the error information for errcode*/
		case WM_ERROR:
		ERROR_MESSAGE("%s",get_error_info(wParam));
		break;
		case WM_INVOKE_CALLBACK:
		if (g_complete_func)
		{
			g_complete_func(g_retval, g_complete_private_data);
		}
		break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return FALSE;
}
static void InitFileTransferWindow(void)
{
#ifdef	DEVELOPMENT
    int relative_x = 0;
    int relative_y = 0;
    int temp = 0;

    hwndFileTransferPage = CreateWindowEx(0, szAppName, TEXT (""),
                         WS_VISIBLE | WS_CHILD,
                         TAB_CLIENT_START_X, TAB_CLIENT_START_Y,
                         TAB_CLIENT_WIDTH, TAB_CLIENT_HEIGHT,
                         hwndTab, NULL, hInst, NULL);

    /***********************************************************/
    /*                                                         */
    /***********************************************************/
	hwndFtNotice = CreateWindow(TEXT("static"),TEXT("Notice : This Page is for Linux Target."),
						 WS_CHILD | WS_VISIBLE | SS_LEFT,
						 FILE_DL_START_X,FILE_DL_START_Y,
						 WIDTH_EDIT_NAND,HEIGHT_CONTROL,
						 hwndFileTransferPage,NULL,
						 hInst,NULL);
						 
    relative_y = FILE_DL_START_Y + HEIGHT_CONTROL + Y_MARGIN;

    hwndFdGroupBox = CreateWindow(TEXT("button"), TEXT("File Download"),
                         WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                         FILE_DL_START_X, relative_y,
                         FILE_DL_WIDTH, FILE_DL_HEIGHT,
                         hwndFileTransferPage , NULL,
                         hInst, NULL);
    hwndFdSrcFile = CreateWindow(TEXT("static"), TEXT("Src File:"),
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         FILE_DL_START_X + X_MARGIN, relative_y+Y_MARGIN*3+WIN_STATIC_OFFSET,
                         WIDTH_TEXT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x = FILE_DL_START_X + X_MARGIN + WIDTH_TEXT;
    relative_y += Y_MARGIN*3;

    hwndFdStaticSrcFile = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("static"), NULL,
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         relative_x + X_MARGIN, relative_y,
                         WIDTH_EDIT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x += X_MARGIN + WIDTH_EDIT;
                
    hwndFdButtonBrowse = CreateWindow(TEXT ("button"), "Browse...",
                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         relative_x + X_MARGIN, relative_y,
                         WIDTH_BUTTON, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    temp = relative_x + X_MARGIN;
    relative_y += HEIGHT_CONTROL + Y_MARGIN*2;

    hwndFdDestFile = CreateWindow(TEXT("static"), "Dest Path:",
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         FILE_DL_START_X + X_MARGIN, relative_y + WIN_STATIC_OFFSET,
                         WIDTH_TEXT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x = FILE_DL_START_X + X_MARGIN + WIDTH_TEXT;

    hwndFdEditDestFile = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), NULL,
                         WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
                         relative_x + X_MARGIN, relative_y,
                         WIDTH_EDIT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x += X_MARGIN + WIDTH_EDIT;

    
    hwndFdButtonDownload = CreateWindow(TEXT("button"), "Download",
                         WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON,
                         temp, relative_y,
                         WIDTH_BUTTON, HEIGHT_CONTROL,
                        hwndFileTransferPage, NULL,
                         hInst, NULL);

	relative_y += HEIGHT_CONTROL + Y_MARGIN;
	hwndFdNotify = CreateWindow(TEXT("static"), TEXT(""),
                         WS_CHILD | WS_VISIBLE ,
                         FILE_DL_START_X + X_MARGIN, relative_y,
                         WIDTH_EDIT_NAND, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);
    /***********************************************************/
    /*                                                         */
    /***********************************************************/
    hwndFuGroupBox = CreateWindow(TEXT("button"), "File Upload",
                         WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
                         FILE_DL_START_X, FILE_DL_START_Y + HEIGHT_CONTROL + 2*Y_MARGIN + FILE_DL_HEIGHT,
                         FILE_DL_WIDTH, FILE_DL_HEIGHT,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_y = FILE_DL_START_Y + HEIGHT_CONTROL + 3*Y_MARGIN + FILE_DL_HEIGHT;

    hwndFuSrcFile = CreateWindow(TEXT("static"), "Src File:",
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         FILE_DL_START_X + X_MARGIN, relative_y + 3*Y_MARGIN + WIN_STATIC_OFFSET,
                         WIDTH_TEXT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x = FILE_DL_START_X + X_MARGIN + WIDTH_TEXT;
    relative_y += 3*Y_MARGIN;

    hwndFuEditSrcFile = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("edit"), NULL,
                         WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
                         relative_x + X_MARGIN, relative_y,
                         WIDTH_EDIT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x += X_MARGIN + WIDTH_EDIT;
                
    hwndFuButtonUpload = CreateWindow(TEXT("button"), "Upload",
                         WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON,
                         relative_x + X_MARGIN, relative_y,
                         WIDTH_BUTTON, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);
    
    relative_y = FILE_DL_START_Y + HEIGHT_CONTROL + 3*Y_MARGIN + FILE_DL_HEIGHT + 3*Y_MARGIN;

    hwndFuDestFile = CreateWindow(TEXT("static"), "Dest File:",
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         FILE_DL_START_X + X_MARGIN, relative_y + HEIGHT_CONTROL + Y_MARGIN*2+WIN_STATIC_OFFSET,
                         WIDTH_TEXT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_y += HEIGHT_CONTROL + Y_MARGIN*2;
    relative_x = FILE_DL_START_X + X_MARGIN + WIDTH_TEXT;


    hwndFuStaticDestFile = CreateWindowEx(WS_EX_CLIENTEDGE,TEXT("static"), NULL,
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         relative_x + X_MARGIN, relative_y+WIN_STATIC_OFFSET,
                         WIDTH_EDIT, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    relative_x += WIDTH_EDIT + X_MARGIN;
                             
	
    hwndFuButtonBrowse = CreateWindow(TEXT("button"), "Browse...",
                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         temp, relative_y,
                         WIDTH_BUTTON, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);
	relative_y += HEIGHT_CONTROL + Y_MARGIN;
	hwndFuNotify = CreateWindow(TEXT("static"), TEXT(""),
                         WS_CHILD | WS_VISIBLE ,
                         FILE_DL_START_X + X_MARGIN, relative_y,
                         WIDTH_EDIT_NAND, HEIGHT_CONTROL,
                         hwndFileTransferPage, NULL,
                         hInst, NULL);

    ShowWindow(hwndFileTransferPage, FALSE);

#endif

    return;
}

/*
 *  Name:     WinMain
 *
 *  Purpose:  User-defined entry point for Unication Dev Tools application
 *
 *  Params:
 *      hInstance:  A handle to the current instance of the application
 *
 *      hPrevInstance:    A handle to the previous instance of the application
 *                        This parameter is always NULL
 *
 *      szCmdLine:  The command line for the application, excluding the program name
 *
 *      iCmdShow:  Controls how the window is to be shown
 *
 *  Return:If the function succeeds, terminating when it receives a WM_QUIT message,
 *           it should return the exit value contained in that message's wParam parameter.
 *            If the function terminates before entering the message loop, it should return zero.
 *
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    MSG          msg;
    BOOL         bRet;

    hInst = hInstance;

    /* Initialize debug UniDevTools */
    if (InitDebugConsole() == FALSE)
    {
        MessageBox(NULL, TEXT ("Can not init debug "APP_TITLE), szAppName, MB_ICONERROR);
        return 0 ;
    }
	//Initialize net
	WSADATA wsaData;
    int retval;
	retval = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (retval != 0)
    {
        printf("%s_%d: WSAStartup() error, error code is %d\n", __FILE__, __LINE__, WSAGetLastError());
        return 0;
    }
    g_event = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset
    if (StartThread() == FALSE)
    {
        MessageBox(NULL, TEXT ("Create thread error"), szAppName, MB_ICONERROR);
        ExitDebugConsole();
        return 0;
    }

    g_event_wait = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset
    if (start_wait_thread() == FALSE)
    {
        MessageBox(NULL, TEXT ("Create wait thread error"), szAppName, MB_ICONERROR);
        ExitDebugConsole();
        return 0;
    }

    /* Application init */
    if (InitApplication() == FALSE)
    {
        MessageBox(NULL, TEXT (APP_TITLE" is running"), szAppName, MB_ICONERROR);
        ExitDebugConsole();
        return 0;
    }

    /* Unication Dev Tools main window init */
    if (InitMainWindow() == FALSE)
    {
        MessageBox(NULL, TEXT ("Can not create main window"), szAppName, MB_ICONERROR);
        CloseHandle(hMutex);
        ExitDebugConsole();
        return 0;
    }   
#ifdef DEVELOPMENT
    InitLinuxWindow();
	InitSPLWindow();
	InitMFGWindow();
	InitFileTransferWindow();
#elif defined(MAINTAINMENT)
	InitLinuxWindow();
#elif defined(PRODUCTION)
	InitMFGWindow();
#endif	
    register_notifyer();

    /* Show the window and send a WM_PAINT message to the window procedure */
    ShowWindow(hwndMain, iCmdShow);
    UpdateWindow(hwndMain);
	
    /* Start the message loop */
    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            /* handle the error and possibly exit */
            CloseHandle(hMutex);
            ExitDebugConsole();
            MessageBox(NULL, TEXT("GetMessage error"), szAppName, MB_ICONERROR) ;
            return -1;
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
  
	free(image_buffer);
    CloseHandle(g_event);
    CloseHandle(g_event_wait);

    /* Receive the WM_QUIT message, release mutex and return the exit code to the system */
    CloseHandle(hMutex);
    ExitDebugConsole();

    return msg.wParam;
}
