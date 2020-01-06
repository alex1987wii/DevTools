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
#include "UpgradeLib.h"
#include "uni_image_header.h"
#include "error_code.h"
#include "devToolsRes.h"
#include "iniparser.h"
#include "devToolsUI_private.h"

/*  ===========================================================================
 *  Macro definitions
 *  ===========================================================================
 */

#define DEBUG_NONE			0
#define DEBUG_CONSOLE		1
#define DEBUG_LOG			2

#define DEBUG_MODE			DEBUG_LOG	//fixme if you want change the debug mode
#define LOGFILE				"devToolUI.log"



#if (DEBUG_MODE == DEBUG_CONSOLE || DEBUG_LOG == DEBUG_MODE)
#define UI_DEBUG(fmt,args...)			do{\
TCHAR buff[1024];\
snprintf(buff,1024,TEXT("%s:%s:%d:")fmt,__FILE__,__func__,__LINE__,##args);\
MessageBox(hwndMain,buff,TEXT("UI_DEBUG"),MB_ICONWARNING);}while(0)

#if (DEBUG_CONSOLE == DEBUG_MODE)
#define console_print(fmt,args...)	printf(fmt,##args)
#define log_print(fmt,args...)		printf(fmt,##args)

#elif (DEBUG_MODE == DEBUG_LOG)
static FILE *log_fp = NULL;
#define log_print(fmt,args...)		do{if(log_fp){fprintf(log_fp,fmt,##args);fflush(log_fp);}}while(0)
#define console_print(fmt,args...)
#endif

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
#define WM_INVOKE_CALLBACK			(WM_USER+115)
#define WM_KILL_TIMER				(WM_USER+116)
/*****************TIMER ID********************/
#define TIMER_DYNAMIC_INFO			1

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
#if defined LIMITED
#define DEV_TOOLS_NUMBER        1  
#else
#define DEV_TOOLS_NUMBER        3           /* Unication dev tools have three utilities */
#endif
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
#if (defined(CONFIG_PROJECT_U3))
#define PROJECT			"U3"
#define U3_LIB
#elif defined(CONFIG_PROJECT_U3_2ND)
#define PROJECT			"U3_2ND"
#define U3_LIB
#elif defined CONFIG_PROJECT_U5
#define PROJECT			"U5"
#elif (defined(CONFIG_PROJECT_G4_BBA) || defined(CONFIG_PROJECT_G4_BBA_V2))
#define PROJECT			"G4_BBA"
#elif defined CONFIG_PROJECT_BR01_2ND
#define PROJECT			"BR01_2ND"
#elif defined CONFIG_PROJECT_M2
#define PROJECT			"M2"
#elif defined CONFIG_PROJECT_AD6900_BBA
#define PROJECT			"AD6900_BBA"
#elif defined CONFIG_PROJECT_BR01
#define PROJECT			"BR01"
#elif defined(CONFIG_PROJECT_U4)
#define PROJECT			"U4"
#elif defined(CONFIG_PROJECT_U4_BBA)
#define PROJECT			"U4_BBA"
#elif defined CONFIG_PROJECT_REPEATER_BBA
#define PROJECT			"REPEATER_BBA"
#else
	#error "must pass the macro CONFIG_PROJECT_XXX to indicate which project it is"
#endif


/*UI error code definition */
#define UI_ERROR_NUM_MAX	(100)
#define UI_ERROR_START		(-1000 - UI_ERROR_NUM_MAX)
#define OK		0
unsigned short error_code = OK;		
enum _UI_ERROR_CODE{
	EC_INI_FILE_NOT_EXSIT = UI_ERROR_START,
	EC_INI_FILE_SYNTAX_ERROR,
	EC_INI_IP_NOT_SPECIFY,
	EC_INI_IP_INVALID,
	EC_INI_IP_TOO_MANY,
	EC_INI_IMAGE_NOT_SPECIFY,
	EC_INI_IMAGE_NOT_EXSIT,
	EC_INI_IMAGE_INVALID,
	EC_INI_IMAGE_INCOMPATIBLE,
	EC_INI_RESCUE_IMAGE_NOT_SPECIFY,
	EC_INI_RESCUE_IMAGE_NOT_EXSIT,
	EC_INI_RESCUE_IMAGE_INVALID,
	EC_INI_RESCUE_IMAGE_INCOMPATIBLE,
	EC_NO_PARTITION_SELECTED,
	EC_WAIT_REBOOT_TIMEOUT
};

struct _error_code_info ui_error_code_info[] = {
	{EC_INI_FILE_NOT_EXSIT,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_FILE_SYNTAX_ERROR,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_IP_NOT_SPECIFY,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_IP_INVALID,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_IP_TOO_MANY,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_IMAGE_NOT_SPECIFY,"Tool package broken,please re-install this tool.","Tool package broken."},
	{EC_INI_IMAGE_NOT_EXSIT,"Can't find Image,please check if it's exsit.","Can't find Image."},
	{EC_INI_IMAGE_INVALID,"Image broken,please use the right image.","Image broken."},
	{EC_INI_IMAGE_INCOMPATIBLE,"Image incompatible,please use the right image for target.","Image incompatible."},
	{EC_INI_RESCUE_IMAGE_NOT_SPECIFY,"Ini file error,rescue_image not specified.","Ini file error."},
	{EC_INI_RESCUE_IMAGE_NOT_EXSIT,"Ini file error,rescue_image not exsit.","Ini file error."},
	{EC_INI_RESCUE_IMAGE_INVALID,"Ini file error,rescue_image invalid.","Ini file error."},
	{EC_INI_RESCUE_IMAGE_INCOMPATIBLE,"Ini file error,rescue_image incompatible.","Ini file error."},
	{EC_NO_PARTITION_SELECTED,"Error! No partition selected.","Error! No partition selected."},
	{EC_WAIT_REBOOT_TIMEOUT,"Wait for reboot timeout,please try it again.","Timeout."},
};
/*info*/
#define LINUX_INIT			"Preparing"

#ifdef MAINTAINMENT
const char *ec_not_found = "Error occurs,please try it again.If it happened again please contact your support personnel for assistance.";
#else
const char *ec_not_found = "Error code not found.";
#endif

static char partition_name[UNI_MAX_PARTITION][UNI_MAX_PARTITION_NAME_LEN];
static int total_partition = 0;
static int cbs_per_line = 0;

static HWND hwndPartitionCheckBox[UNI_MAX_PARTITION] = {NULL,};
int partition_x_pos = 0;
int partition_y_pos = 0;

#define IP_MAX			4
const char *ip_addr[IP_MAX] = {
	"10.10.0.12",
	"10.10.12.11",
	"10.10.12.21",
	"10.10.12.22"
};

/*********************Tab Select ID****************************/
#define SELECT_LINUX_PROGRAMMING		0
#define SELECT_SPL_PROGRAMMING			1
#define SELECT_MFG_PROGRAMMING			2
#define SELECT_FILE_TRANSFER			3


/* Unication UniDevTools lines buffer definitions */
#if defined DEVELOPMENT
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"For LINUX ", " For SPL ", " For MFG ","File Transfer",};
#elif defined MAINTAINMENT
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"Upgrade"};
#elif defined PRODUCTION
PTCHAR tabString[DEV_TOOLS_NUMBER] = {"Nand Flash programming"};
#endif


/**********************************************************************/
#if defined(DEVELOPMENT)
#ifdef LIMITED
#define APP_TITLE		   "Unication Limited Tools"
#else
#define APP_TITLE          "Unication Dev Tools"
#endif //LIMITED
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
typedef int (*background_func)(void );


static background_func g_background_func = NULL;/*INVOKE by BackGroundThread*/
//static void *g_background_arg = NULL;
#define ERROR_INFO_MAX			256
static TCHAR error_info[ERROR_INFO_MAX];//storage error information
static TCHAR error_msg[ERROR_INFO_MAX];//storage error message

typedef void (*complete_func)(int retval,void *private_data);
static complete_func g_complete_func = NULL;/*INVOKE by WM_INVOKE_CALLBACK*/
static void *g_complete_private_data = NULL;

static BOOL g_transfer_complete = FALSE;
static int g_locking = 0;
static BOOL g_processing = FALSE;
static int g_retval = 0;
static int partition_selected = 0;
static BOOL g_on_batch = FALSE;
#define IS_LISTENING_ON_NOTHING   (0)
#define IS_LISTENING_ON_MFG       (1)
#define IS_LISTENING_ON_SPL       (2)
#define IS_LISTENING_ON_LINUX	  (3)

static int listening_on = IS_LISTENING_ON_NOTHING;

static char work_path[MAX_PATH];/*the path for this program*/


static HANDLE g_event = NULL;    // event
static HANDLE g_lan_event = NULL;
static HANDLE g_spl_event = NULL;
static HANDLE g_hBackGround = NULL;
static HANDLE g_hTransfer = NULL;

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
	int ip_should_flash;
    char ip[4][16]; // xxx.xxx.xxx.xxx
    char name_of_image[MAX_PATH];
	char name_of_rescue_image[MAX_PATH];
} ini_file_info_t;

static ini_file_info_t ini_file_info;

static int image_length = 0;

static HWND    hwndDynamic = NULL;
static const char *dynamic_info = NULL;

#define CLEAR_INFO()	do{SetWindowText(hwndInfo,"");SetWindowText(hwndIpInfo,"");}while(0)
/************************General Handler Declaration********************/
#define BROWSER_MAX		56
static HWND    hwndInfo,hwndIpInfo,hwndProcessInfo,hwndStaticBrowser,hwndBtnBrowser,hwndBtnDown;
static TCHAR	BrowserImage[256];

/**********************Linux Handler Declaration*************************/
static HWND    hwndLinPage;
static HWND    hwndLinGroupReadme;
static HWND    hwndLinTextTarget,hwndLinStaticInfo,hwndLinStaticIpInfo,hwndLinProcessInfo,hwndLinStaticNotice;

static HWND    hwndLinStaticDev,hwndLinComboDevList,hwndLinBtnRefresh,hwndLinBtnReset;/*for Br01*/
static HWND    hwndLinGroupOptions;

static HWND    hwndLinStaticImg;
static HWND    hwndLinStaticBrowser;

static HWND    hwndLinBtnBrowser,hwndLinBtnDown,hwndBtnEnableTelnet;
static HWND    hwndCheckBoxUserdata,hwndCheckBoxSkipBatCheck;


/**********************SPL Handler Declaration*************************/
static HWND    hwndSPLPage;
static HWND    hwndSPLGroupReadme;
static HWND    hwndSPLTextTarget,hwndSPLStaticInfo,hwndSPLStaticIpInfo,hwndSPLProcessInfo,hwndSPLStaticNotice;

//static HWND    hwndSPLIpAddr,hwndSPLDevList,hwndSPLBtnRefresh;/*for Br01*/
static HWND    hwndSPLGroupOptions;
static HWND    hwndSPLStaticImg;
static HWND    hwndSPLStaticBrowser;
static HWND    hwndSPLBtnBrowser,hwndSPLBtnDown,hwndSPLBtnStop;
//static HWND    hwndCheckBoxSkipBatCheck;

/**********************MFG Handler Declaration*************************/

static HWND    hwndMFGPage;
static HWND    hwndMFGGroupReadme;
static HWND    hwndMFGTextTarget,hwndMFGStaticInfo,hwndMFGStaticIpInfo,hwndMFGProcessInfo,hwndMFGStaticNotice;

//static HWND    hwndMFGIpAddr,hwndMFGDevList,hwndMFGBtnRefresh;/*for Br01*/
static HWND    hwndMFGGroupOptions;

static HWND    hwndMFGStaticImg;
static HWND    hwndMFGStaticBrowser;
static HWND    hwndMFGBtnBrowser,hwndMFGBtnDown,hwndMFGBtnStop;
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
/*
 * return 1 check 0 not check
 */
#define CHECKBOX_IS_CLICK(x)  (Button_GetCheck(x) == BST_CHECKED)

#if (DEBUG_MODE == DEBUG_CONSOLE)

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
#elif (DEBUG_MODE == DEBUG_LOG)
BOOL InitDebugConsole( void )
{   
	log_fp = fopen(LOGFILE,"w+");
	if(log_fp == NULL)
		return FALSE;	
    return TRUE;
}
void ExitDebugConsole( void )
{	
	if(log_fp)
		fclose(log_fp);
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

static inline void dump_project(void)
{	
	log_print("Tool type : "
#ifdef MAINTAINMENT
	"maintainment"
#elif defined(DEVELOPMENT)
	"developer"
#elif defined(PRODUCTION)
	"production"
#endif
	" tool for %s.\tVersion : %s.\n",PROJECT,VERSION);

}
#if (DEBUG_MODE == DEBUG_LOG)
static void dump_time(void)
{
	static time_t current_time;
	static char time_buff[256];
	time(&current_time);
	strftime(time_buff,256,"%Y/%m/%d\t%H:%M:%S\n",localtime(&current_time));
	log_print("%s",time_buff);
	//log_print("%s",asctime(localtime(&current_time)));
}
#else
#define dump_time()
#endif

static inline void dump_ini_info(void)
{
	int i;	
	log_print("ini info:\n");
	log_print("\tini_file = %s\n",ini_file);
	log_print("\tini_file_info.ip_should_flash = %d\n",ini_file_info.ip_should_flash);
	log_print("\tip list = ");
	for(i = 0; i < ini_file_info.ip_should_flash; ++i)
	{
		log_print("%s\t",ini_file_info.ip[i]);
	}
	log_print("\n");
	log_print("\tini_file_info.name_of_image = %s\n",ini_file_info.name_of_image);
	log_print("\tini_file_info.name_of_rescue_image = %s\n",ini_file_info.name_of_rescue_image);
}

static inline void dump_download_varibles(void)
{	
	log_print("global varibles:\n");	
	log_print("\tburn_mode = %s\n",burn_mode == SELECT_LINUX_PROGRAMMING ? "linux" :
	(burn_mode == SELECT_SPL_PROGRAMMING ? "spl" :
	(burn_mode == SELECT_MFG_PROGRAMMING ? "mfg" : "invalid burn mode")));
	
	log_print("\tlistening_on = %s\n",listening_on == IS_LISTENING_ON_MFG ? "IS_LISTENING_ON_MFG" : 
	(listening_on == IS_LISTENING_ON_SPL ? "IS_LISTENING_ON_SPL" :
	(listening_on == IS_LISTENING_ON_NOTHING ? "IS_LISTENING_ON_NOTHING" : "INVALID VALUE")));
	
	if(burn_mode == SELECT_MFG_PROGRAMMING)
		log_print("\tg_on_batch = %s\n",g_on_batch == TRUE ? "TRUE" : "FALSE");
	log_print("\tpartition_selected = 0x%04x\n",partition_selected);
}

static inline void reset_general_handler(int select)
{
	switch(select)
	{
		case SELECT_LINUX_PROGRAMMING:
		hwndInfo = hwndLinStaticInfo;
		hwndIpInfo = hwndLinStaticIpInfo;
		hwndProcessInfo = hwndLinProcessInfo;
		hwndStaticBrowser = hwndLinStaticBrowser;
		hwndBtnDown = hwndLinBtnDown;
		break;
		case SELECT_SPL_PROGRAMMING:
		hwndInfo = hwndSPLStaticInfo;
		hwndIpInfo = hwndSPLStaticIpInfo;
		hwndProcessInfo = hwndSPLProcessInfo;
		hwndStaticBrowser = hwndSPLStaticBrowser;
		hwndBtnDown = hwndSPLBtnDown;
		break;
		case SELECT_MFG_PROGRAMMING:
		hwndInfo = hwndMFGStaticInfo;
		hwndIpInfo = hwndMFGStaticIpInfo;
		hwndProcessInfo = hwndMFGProcessInfo;
		hwndStaticBrowser = hwndMFGStaticBrowser;
		hwndBtnDown = hwndMFGBtnDown;
		break;
		case SELECT_FILE_TRANSFER:
		break;
	}
}
static inline void SetDynamicInfo(const char *info)
{
	hwndDynamic = hwndInfo;
	dynamic_info = info;
	SetWindowText(hwndDynamic,info);/*show the initial information*/
	SetTimer(hwndMain,TIMER_DYNAMIC_INFO,500,NULL);/*update every half second*/
}
static inline void StopDynamicInfo()
{
	SendMessage(hwndMain,WM_KILL_TIMER,TIMER_DYNAMIC_INFO,0);
}

static inline void CreatePartitionList(void)
{
	//create partition box
	int i;
	HWND hwndPage;
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:
		hwndPage = hwndLinPage;
		break;
		case SELECT_SPL_PROGRAMMING:
		hwndPage = hwndSPLPage;
		break;
		case SELECT_MFG_PROGRAMMING:
		hwndPage = hwndMFGPage;
		break;
	}
	for(i = 0; i < total_partition; ++i)
	{
		hwndPartitionCheckBox[i] = CreateWindow(TEXT("button"),
		partition_name[i],		
		WS_CHILD | BS_CHECKBOX | WS_VISIBLE,		
		partition_x_pos + (i%cbs_per_line)*(WIDTH_CHECKBOX) + X_MARGIN,
		partition_y_pos + (i/cbs_per_line)*HEIGHT_CONTROL + Y_MARGIN,
		WIDTH_CHECKBOX, HEIGHT_CONTROL,
		hwndPage,NULL,
		hInst,NULL);
		#ifdef LIMITED
		if(i != 0 && i != 1 && i != 3)
			SendMessage(hwndPartitionCheckBox[i],BM_CLICK,0,0);
		EnableWindow(hwndPartitionCheckBox[i],FALSE);
		#endif
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
static BOOL process_partitionlist(HWND hwnd)
{
	int i;
	if(hwndPartitionCheckBox[0] == NULL)
		return FALSE;
	for(i = 0; i < total_partition; ++i)
	{
		if(hwnd == hwndPartitionCheckBox[i])
		{
			if(i == 3 && Button_GetCheck(hwndPartitionCheckBox[i]) == BST_UNCHECKED)//calibration
			{
				if(IDYES == MessageBox(hwndMain,TEXT("WARNING:This partition should never been checked unless you exectly known what it is repesent for.\nCheck it anyway?"),szAppName,MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
				{
					Button_SetCheck(hwndPartitionCheckBox[i],BST_CHECKED);
				}
			}
			else
				Button_SetCheck(hwndPartitionCheckBox[i],Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);
			return TRUE;
		}
	}
	return FALSE;
}
void update_ui_resources(int enable)
{	
	/*BtnDown enable if error occurs,but disable by reset_ui_resources when successfully complete*/
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:		
		EnableWindow(hwndLinBtnBrowser,enable);
		EnableWindow(hwndLinBtnDown,enable);
		EnableWindow(hwndLinBtnRefresh,enable);
		EnableWindow(hwndCheckBoxUserdata,enable);
		EnableWindow(hwndBtnEnableTelnet,enable);
		EnableWindow(hwndCheckBoxSkipBatCheck,enable);
		
#ifdef DEVELOPMENT
		EnableWindow(hwndLinPage,enable);
		#ifndef LIMITED
		EnablePartitionList(enable);
		#endif		
#endif
		break;
		case SELECT_SPL_PROGRAMMING:		
		EnableWindow(hwndSPLBtnBrowser,enable);		
		EnableWindow(hwndSPLBtnDown,enable);
		
#ifdef DEVELOPMENT
		EnableWindow(hwndSPLPage,enable);
		#ifndef LIMITED
		EnablePartitionList(enable);
		#endif
#endif		
		break;
		case SELECT_MFG_PROGRAMMING:		
		EnableWindow(hwndMFGBtnBrowser,enable);		
		EnableWindow(hwndMFGBtnDown,enable);
		EnableWindow(hwndCheckBoxBatch,enable);
#ifdef DEVELOPMENT
		EnableWindow(hwndMFGPage,enable);
		#ifndef LIMITED
		EnablePartitionList(enable);
		#endif		
#endif			
		break;
		case SELECT_FILE_TRANSFER:		
		break;
	}
#ifdef DEVELOPMENT
	EnableWindow(hwndTab,enable);
#endif
}
/*reset ui page into initial status when image download complete*/

static inline void reset_ui_resources(int page)
{	
	switch(page)
	{
		case SELECT_LINUX_PROGRAMMING:
		EnableWindow(hwndLinBtnDown,FALSE);
		break;
		case SELECT_SPL_PROGRAMMING:
		EnableWindow(hwndSPLBtnDown,FALSE);		
		break;
		case SELECT_MFG_PROGRAMMING:
		EnableWindow(hwndMFGBtnDown,FALSE);		
		break;
		case SELECT_FILE_TRANSFER:
		EnableWindow(hwndFdButtonDownload,FALSE);
		EnableWindow(hwndFuButtonUpload,FALSE);
		break;
		default:
		UI_DEBUG("Invalid page");
		break;
	}	
}

static BOOL get_image_info(const char*image)
{
	char image_type[UNI_MAX_REL_VERSION_LEN];
	FILE *fp = fopen(image,"rb");
	if(fp == NULL)
	{
		snprintf(error_info,ERROR_INFO_MAX,"%s not exsit.",image);
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit,check your image if it's in current work path.",image);
		error_code = EC_INI_IMAGE_NOT_EXSIT;
		return FALSE;
	}
	uni_image_header_t image_header;
		
	if(sizeof(uni_image_header_t) != fread(&image_header,1,sizeof(uni_image_header_t),fp))
	{
		fclose(fp);
		snprintf(error_info,ERROR_INFO_MAX,"Invalid image.");
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:%s",image);
		error_code = EC_INI_IMAGE_INVALID;
		return FALSE;
	}
	fclose(fp);
	
	/*dump image info */	
	log_print("image info:\n");
	log_print("\timage = %s\n",image);
	log_print("\timage_header.image_version = %s\n",image_header.image_version);
	log_print("\timage_header.release_version = %s\n",image_header.release_version);
	log_print("\timage_header.total_partitions = %d\n",image_header.total_partitions);
	log_print("\timage_header.instruction_ver_of_ipl = 0x%0x\n",image_header.instruction_ver_of_ipl);
	
	/*make some check if it's valid image,the init function in libupgrade will do that,but i want get partition info first*/
	memcpy(image_type,image_header.release_version,UNI_MAX_REL_VERSION_LEN);
	strtok(image_type,";");
	if(
#ifdef CONFIG_PROJECT_U4
	strcasecmp(image_type,"u4_bba")
#else
	strcasecmp(image_type,PROJECT)
#endif
	)
	{
		snprintf(error_info,ERROR_INFO_MAX,"Invalid image.");
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:%s",image);
		error_code = EC_INI_IMAGE_INVALID;
		return FALSE;
	}
	total_partition = image_header.total_partitions;
	if(total_partition <= 0 || total_partition > UNI_MAX_PARTITION)
	{
		snprintf(error_info,ERROR_INFO_MAX,"Invalid image.");
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:%s",image);
		error_code = EC_INI_IMAGE_INVALID;
		return FALSE;
	}
	int i;
	for(i = 0; i < total_partition; ++i)
	{		
		memcpy(partition_name[i],image_header.partition[i].partition_name,UNI_MAX_PARTITION_NAME_LEN);
	}
	cbs_per_line = total_partition/2;
	return TRUE;
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

/*just replace all '\' to '/' */
static inline char *dir_win32_to_linux(char *dir)
{
	static char linux_dir[256];
	strncpy(linux_dir,dir,256);
	dir = linux_dir;
	while(*dir)
	{
		if(*dir == '\\')
			*dir = '/';
		++dir;
	}
	return linux_dir;	
}
static inline int get_file_len(const char *file)
{
	int retval = -1;
	FILE *fp = fopen(file,"rb");
	if(fp == NULL)
	{		
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

static inline int RebootTarget(const char *ip)
{	
	return execute_cmd_in_tg(ip,"reboot");
}
static inline int EnableTelnet(const char *ip)
{
	return execute_cmd_in_tg(ip,"telnetd &");
}
static inline BOOL parse_ip_list(const char *ip_list)
{
	char buff[256],*ip;	
	int ip_cnt = 0;
	ini_file_info.ip_should_flash = 0;
	if(strlen(ip_list) > 256)
		return FALSE;
	strcpy(buff,ip_list);
	ip = strtok(buff," \t");	
	while(ip)
	{
		if(strlen(ip) > 16)
			return FALSE;
		strcpy(ini_file_info.ip[ip_cnt++],ip);		
		ip = strtok(NULL," \t");
	}
	if(ip_cnt > 4)
		return FALSE;
	ini_file_info.ip_should_flash = ip_cnt;
	return TRUE;
}
static inline BOOL parse_ini_file(const char *file)
{	
	char *ip_list,*image,*rescue_image;
	/*read the ini file and parse it into struct*/
	/*clear ini information*/
	memset(&ini_file_info,0,sizeof(ini_file_info));
	dictionary *ini_config = iniparser_load(file);
	if(ini_config == NULL)
	{		
		return FALSE;
	}
	if(iniparser_getnsec(ini_config) != 1)
	{
		goto EXIT;
	}
	ip_list = iniparser_getstring(ini_config,"Options:ip",NULL);
	image = iniparser_getstring(ini_config,"Options:image",NULL);
	rescue_image = iniparser_getstring(ini_config,"Options:rescue_image",NULL);
	if(ip_list != NULL)
		parse_ip_list(ip_list);
	if(image && strlen(image) < MAX_PATH)
		strcpy(ini_file_info.name_of_image,image);
	if(rescue_image && strlen(rescue_image) < MAX_PATH)
		strcpy(ini_file_info.name_of_rescue_image,rescue_image);
	iniparser_freedict(ini_config);
	return TRUE;
EXIT:
	iniparser_freedict(ini_config);
	return FALSE;	
}
/*Description:
* 		check if ini file exsit and it's valid for current burn_mode
* burn_mode = SELECT_LINUX_PROGRAMMING:
* 			a.if it's for development tools,image key is irrelevant
* 			b.if it's for maintainment tools,image key is necessary,and image must exsit
* 			ip key is necessary and rescue_image key is irrelevant for this burn_mode
* burn_mode = SELECT_SPL_PROGRAMMING:
* burn_mode = SELECT_MFG_PROGRAMMING:(include development tools and production tools)
*			ip key and rescue_image key is necessary,rescue_image must exsit,image key is irrelevant(specified by browser)
*/
static BOOL check_ini(void)
{
	/*read the ini file and parse it into struct*/
	/*if don't need a ini file then return TRUE*/
	if(ini_file == NULL)
		return TRUE;
	if(_access(ini_file,0))
	{
		snprintf(error_info,ERROR_INFO_MAX,"ini file lost.");
		snprintf(error_msg,ERROR_INFO_MAX,"%s lost.",ini_file);
		error_code = EC_INI_FILE_NOT_EXSIT;
		return FALSE;
	}
	BOOL ret = parse_ini_file(ini_file);
	dump_ini_info();
	if(ret == FALSE)
	{		
		snprintf(error_info,ERROR_INFO_MAX,"ini file invalid configuration.");
		snprintf(error_msg,ERROR_INFO_MAX,"%s invalid configuration.",ini_file);
		error_code = EC_INI_FILE_SYNTAX_ERROR;
		return FALSE;
	}
	switch(burn_mode)
	{

		case SELECT_LINUX_PROGRAMMING:
		if(ini_file_info.ip_should_flash == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No ip specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No ip specified in %s",ini_file);
			error_code = EC_INI_IP_NOT_SPECIFY;
			return FALSE;
		}
#ifdef MAINTAINMENT
		if(ini_file_info.name_of_image[0] == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No image specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No image specified in %s",ini_file);
			error_code = EC_INI_IMAGE_NOT_SPECIFY;
			return FALSE;
		}
		if(_access(ini_file_info.name_of_image,0))
		{
			snprintf(error_info,ERROR_INFO_MAX,"can't find image.");
			snprintf(error_msg,ERROR_INFO_MAX,"can't find %s.",ini_file_info.name_of_image);
			error_code = EC_INI_IMAGE_NOT_EXSIT;
			return FALSE;
		}
		if(get_image_info(ini_file_info.name_of_image) == FALSE)
		{			
			return FALSE;
		}
#endif
		break;
		case SELECT_SPL_PROGRAMMING:		
		case SELECT_MFG_PROGRAMMING:
		if(ini_file_info.ip_should_flash == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No ip specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No ip specified in %s",ini_file);
			error_code = EC_INI_IP_NOT_SPECIFY;
			return FALSE;
		}		
		if(ini_file_info.name_of_rescue_image[0] == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No rescue image specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No rescue image specified in %s",ini_file);
			error_code = EC_INI_RESCUE_IMAGE_NOT_SPECIFY;
			return FALSE;
		}
		if(_access(ini_file_info.name_of_rescue_image,0))
		{
			snprintf(error_info,ERROR_INFO_MAX,"can't find rescue image.");
			snprintf(error_msg,ERROR_INFO_MAX,"can't find %s.",ini_file_info.name_of_rescue_image);
			error_code = EC_INI_RESCUE_IMAGE_NOT_EXSIT;
			return FALSE;
		}
		break;
	}	

	return TRUE;
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

static char *get_error_info(unsigned short ec)
{
    int i;	
	/*first find it in ui_error_code_info table*/
	for(i = 0; i < (sizeof(ui_error_code_info)/sizeof(ui_error_code_info[0]));i++)
	{
		if(ec == ui_error_code_info[i].ec)
			return ui_error_code_info[i].string;
	}
	/*ec is not in ui_error_code_info table,then find it in error_code_info table*/
    for (i=0; i<(sizeof(error_code_info)/sizeof(error_code_info[0])); i++)
    {
		if (error_code_info[i].ec == ec)
        {
            return (char *)error_code_info[i].string;
        }
    }
	/*not found*/
    return ec_not_found;
}
static char *get_short_info(unsigned short ec)
{
	int i;	
	/*first find it in ui_error_code_info table*/
	for(i = 0; i < (sizeof(ui_error_code_info)/sizeof(ui_error_code_info[0]));i++)
	{
		if(ec == ui_error_code_info[i].ec)
			return ui_error_code_info[i].short_string;
	}
	/*ec is not in ui_error_code_info table,then find it in error_code_info table*/
    for (i=0; i<(sizeof(error_code_info)/sizeof(error_code_info[0])); i++)
    {
		if (error_code_info[i].ec == ec)
        {
            return (char *)error_code_info[i].short_string;
        }
    }
	/*not found*/
    return ec_not_found;
}

static const GUID GUID_DEVCLASS_AD6900_SPL = {0xb35924d6UL, 0x3e16, 0x4a9e, {0x97, 0x82, 0x55, 0x24, 0xa4, 0xb7, 0x9b, 0xac}};
static const GUID GUID_DEVCLASS_AD6900_MFG = {0xb35924d6UL, 0x3e16, 0x4a9e, {0x97, 0x82, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66}};
static const GUID GUID_DEVCLASS_AD6900_LAN = {0xad498944UL, 0x762f, 0x11d0, {0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c}};

HDEVNOTIFY ad6900_lan = NULL;
HDEVNOTIFY ad6900_spl = NULL;
HDEVNOTIFY ad6900_mfg = NULL;

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


#define WAKE_THREAD_UP() do {SetEvent(g_event); } while (0)

/*show progress information */
static DWORD WINAPI TransferThread(LPVOID lpParam)
{
	int retval = 0;
	unsigned char percent = -1,last_percent;
	unsigned short status = -1,last_status;
	unsigned char index = -1,last_index;
	BOOL processing_dynamic_info = FALSE;
	TCHAR text[256];
	
	char *stage[MAX_STATUS] = {"Preparing","Flashing","Verifying","Executing","Finished","Transfer"};
	dump_time();
	log_print("%s start\n",__func__);	
	while(1)
	{
		Sleep(250);
		//reset varibles
		last_percent = percent;
		last_status = status;
		last_index = index;
		percent = 0;
		status = 0;
		index = 0;
		if(g_transfer_complete)
		{
			dump_time();
			log_print("%s quit\n",__func__);
			//#ifdef MAINTAINMENT
			if(processing_dynamic_info == TRUE)
			{
				processing_dynamic_info = FALSE;
				StopDynamicInfo();
			}
			//#endif
			break;
		}
		
		retval = progress_reply_status_get(&index,&percent,&status);

		if(retval != 0)
		{
			dump_time();
			log_print("progress_reply_status_get return errcode : 0x%04x\n",retval);
			continue;
		}		
		//log_print("index = %d, percent = %d, status = %d\n",index,percent,status);
		if(!(last_index == index && last_percent == percent && last_status == status))//stage index or percent changed
		{
			if(status >= MAX_STATUS) break;
			#ifdef MAINTAINMENT
			if(index == 255) //upgrade userdata
			{
				if(processing_dynamic_info == FALSE)
				{
					processing_dynamic_info = TRUE;
					SetDynamicInfo("Upgrading");
				}
				continue;
			}				
			#endif
			if(index == 0 && percent == 0 && status == 0)
			{
				processing_dynamic_info = TRUE;
				SetDynamicInfo("Preparing");
				continue;
			}
			if(processing_dynamic_info == TRUE)
			{
				processing_dynamic_info = FALSE;
				StopDynamicInfo();				
			}		
			const char *partition = NULL;
			if(index < total_partition)
				partition = partition_name[index];
			else if(index == 11)
				partition = "image";
			else
				partition = "";
			if(percent != 0)
			{
				snprintf(text,256,"%s %s\r\nPercetage:[%d%%]",stage[status],partition,percent);
			}
			else
			{
				snprintf(text,256,"%s %s",stage[status],partition);
				
			}			
			SetWindowText(hwndInfo,text);
			
		}		
	}
	return retval;
}

static inline BOOL transfer_start(void)
{
	g_transfer_complete = FALSE;
	SetWindowText(hwndInfo,"Preparing..");
	g_hTransfer = CreateThread(NULL, 0, TransferThread, NULL, 0, NULL);	
	return NULL != g_hTransfer;
}
static inline void transfer_complete(void)
{	
	g_transfer_complete = TRUE;
	WaitForSingleObject(g_hTransfer,INFINITE);
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
				g_retval = g_background_func();
				PostMessage(hwndMain,WM_INVOKE_CALLBACK,0,0);
				
				g_background_func = NULL;
			}			
			g_locking = FALSE;			
        }
	}	
    return 0;
}

static BOOL StartThread(void)
{
    g_hBackGround = CreateThread(NULL, 0, BackGroundThread, NULL, 0, NULL);
    if (g_hBackGround == NULL)
    {
        ERROR_MESSAGE("create BackGroundThread error\n");
        return FALSE;
    }	
    return TRUE;
}


/*******************************************************/
/*                 helpers                             */
/*******************************************************/

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

    console_print("hwndLinPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndLinPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndLinPage, "hwndLinPage");
	
	relative_x = GBOX1_START_X + V_GAPS;
    relative_y = GBOX1_START_Y;
#if 0 && defined(DEVELOPMENT)
	
    hwndLinStaticDev = CreateWindow(TEXT("static"), "Devices list:",
                         WS_CHILD | WS_VISIBLE | SS_LEFT,
                         relative_x, relative_y + WIN_STATIC_OFFSET,
                         WIDTH_TEXT+30, HEIGHT_CONTROL,
                         hwndLinPage, NULL,
                         hInst, NULL);

    relative_x += WIDTH_TEXT + 30;
    hwndLinComboDevList = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("ComboBox"), NULL,
                         WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS|CBS_UPPERCASE|
                         CBS_AUTOHSCROLL,
                         relative_x, relative_y,
                         WIDTH_COMBO, HEIGHT_CONTROL*10,
                         hwndLinPage, NULL,
                         hInst, NULL); 

    relative_x += WIDTH_COMBO + 10;
    hwndLinBtnRefresh =  CreateWindow(TEXT("button"), "Refresh",
                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         relative_x, relative_y,
                         WIDTH_BUTTON, HEIGHT_CONTROL,
                         hwndLinPage, NULL,
                         hInst, NULL);

    relative_x += WIDTH_BUTTON + 65;
    hwndLinBtnReset =    CreateWindow(TEXT("button"), "ResetToAppMode",
                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                         relative_x, relative_y,
                         WIDTH_BUTTON+WIDTH_BUTTON/2, HEIGHT_CONTROL,
                         hwndLinPage, NULL,
                         hInst, NULL);
						 
	relative_y	+= HEIGHT_CONTROL + Y_MARGIN;
	max_Groupbox1_height -= relative_y;
#endif

    console_print("hwndLinGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, relative_y, max_Groupbox1_width, max_Groupbox1_height);

    hwndLinGroupReadme = CreateWindow( TEXT ("button"), "Information",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, relative_y,
            max_Groupbox1_width, max_Groupbox1_height,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinGroupReadme, "hwndLinGroupReadme");

    relative_x = GBOX1_START_X + V_GAPS;
    //relative_y = GBOX1_START_Y;

    sprintf(project_target, "%s%s%s", "Project:  ",PROJECT , target_type[0]);

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
#ifdef MAINTAINMENT
	readme = TEXT("1. Connect Device with PC via USB cable.\n2. Press Upgrade Button and wait until it finished.\n3. If any error occurs during upgrading,please reconnect device and re-open this tool to try it again.\n4.Don't pull off the USB cable during process.\n5.Target will reboot when upgrade complete.");
#else
	readme = project_target;
#endif
    console_print("hwndLinTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndLinPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndLinTextTarget, "hwndLinTextTarget");

      
    relative_y += HEIGHT_CONTROL+35;

	console_print("hwndLinProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinProcessInfo, "hwndLinProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	
	console_print("hwndLinStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticIpInfo, "hwndLinStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	
    console_print("hwndLinStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndLinStaticInfo = CreateWindow( TEXT("edit"), "",
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*2,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticInfo, "hwndLinStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    console_print("hwndLinGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndLinGroupOptions = CreateWindow( TEXT("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinGroupOptions, "hwndLinGroupOptions");
	
	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

	relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
#ifndef MAINTAINMENT
    
	console_print("hwndLinStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndLinStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinStaticImg, "hwndLinStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	console_print("hwndLinStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndLinStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("edit"), NULL,
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
            relative_x, relative_y,
            WIDTH_EDIT, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticBrowser, "hwndLinStaticBrowser");
	
	relative_x += WIDTH_EDIT + X_MARGIN;
	relative_y += HEIGHT_CONTROL + Y_MARGIN;

#endif

#if 0&&(defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
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
	partition_x_pos = GBOX1_START_X + V_GAPS;
	partition_y_pos = relative_y + HEIGHT_CONTROL + Y_MARGIN;

#endif
    relative_x = max_Groupbox2_width - WIDTH_BUTTON - X_MARGIN;
    relative_y = start_y_of_Groupbox2;
    relative_y += V_GAPS + HEIGHT_TAG;

    
#ifdef MAINTAINMENT

	console_print("hwndLinBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);
	hwndLinBtnDown = CreateWindow( TEXT ("button"), "Upgrade",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinBtnDown, "hwndLinBtnDown");
	
#elif defined(DEVELOPMENT)

	console_print("hwndLinBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndLinBtnBrowser = CreateWindow(TEXT ("button"), "Browser",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinBtnBrowser, "hwndLinBtnBrowser");    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;
	console_print("hwndLinBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);
    hwndLinBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinBtnDown, "hwndLinBtnDown");

#endif    
	
	console_print("hwndLinBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);


    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#ifdef DEVELOPMENT
    console_print("hwndBtnEnableTelnet's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndBtnEnableTelnet = CreateWindow( TEXT("button"), "EnableTelnet",
            WS_CHILD | WS_VISIBLE  | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndBtnEnableTelnet, "hwndBtnEnableTelnet");
	relative_y += HEIGHT_CONTROL + V_GAPS*2;
#elif defined MAINTAINMENT
	hwndCheckBoxUserdata = CreateWindow( TEXT("button"), "Keep User Data",
            WS_CHILD  | BS_CHECKBOX,
            relative_x-4*X_MARGIN, relative_y,
			WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5,
            //WIDTH_BUTTON*2, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL-5 + V_GAPS;
	SendMessage(hwndCheckBoxUserdata,BM_SETCHECK,BST_CHECKED,0);//default checked
#endif
#ifndef U3_LIB	
    console_print("hwndCheckBoxSkipBatCheck's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x-4*X_MARGIN, relative_y, WIDTH_BUTTON + X_MARGIN*6-5, HEIGHT_CONTROL-5);

    hwndCheckBoxSkipBatCheck = CreateWindow(TEXT("button"), TEXT("Skip battery check"),
            WS_CHILD | 
	#ifdef DEVELOPMENT
			WS_VISIBLE  | 
	#endif
			BS_AUTOCHECKBOX,
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

    console_print("hwndSPLPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndSPLPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndSPLPage, "hwndSPLPage");

    console_print("hwndSPLGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, GBOX1_START_Y, max_Groupbox1_width, max_Groupbox1_height);

    hwndSPLGroupReadme = CreateWindow( TEXT ("button"), "Information",
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
	console_print("hwndSPLTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndSPLPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndSPLTextTarget, "hwndSPLTextTarget");

      
    relative_y += HEIGHT_CONTROL+35;
	
	console_print("hwndSPLProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLProcessInfo, "hwndSPLProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	

	console_print("hwndSPLStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLStaticIpInfo, "hwndSPLStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	

    console_print("hwndSPLStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndSPLStaticInfo = CreateWindow( TEXT("edit"), "",
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*2,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLStaticInfo, "hwndSPLStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    console_print("hwndSPLGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndSPLGroupOptions = CreateWindow( TEXT ("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLGroupOptions, "hwndSPLGroupOptions");

	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	console_print("hwndSPLStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndSPLStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);
	debug_positions(hwndSPLStaticImg, "hwndSPLStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	console_print("hwndSPLStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndSPLStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("edit"), NULL,
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
            relative_x, relative_y,
            WIDTH_EDIT, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLStaticBrowser, "hwndSPLStaticBrowser");
	
	relative_x += WIDTH_EDIT + X_MARGIN;
	relative_y += HEIGHT_CONTROL + Y_MARGIN;
	

#if 0&&(defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
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

#ifdef DEVELOPMENT
	console_print("hwndSPLBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndSPLBtnBrowser = CreateWindow(TEXT ("button"), "Browser",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnBrowser, "hwndSPLBtnBrowser");
    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#endif	

    console_print("hwndSPLBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndSPLBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLBtnDown, "hwndSPLBtnDown");
	
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

    console_print("hwndMFGPage's dim: x=%d, y=%d, width=%d, height=%d\n",
            PAGE_START_X, PAGE_START_Y, max_page_width, max_page_height);

    hwndMFGPage = CreateWindowEx(0, szAppName, TEXT(""),
            WS_CHILD,
            PAGE_START_X, PAGE_START_Y,
            max_page_width, max_page_height,
            hwndTab, NULL, hInst, NULL);

    debug_positions(hwndMFGPage, "hwndMFGPage");

    console_print("hwndMFGGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, GBOX1_START_Y, max_Groupbox1_width, max_Groupbox1_height);

    hwndMFGGroupReadme = CreateWindow( TEXT ("button"), "Information",
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
	console_print("hwndMFGTextTarget's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGTextTarget = CreateWindow( TEXT ("static"), readme,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*5,
            hwndMFGPage, NULL,
            hInst, NULL);
    relative_y += HEIGHT_CONTROL + V_GAPS;

    debug_positions(hwndMFGTextTarget, "hwndMFGTextTarget");

      
    relative_y += HEIGHT_CONTROL+35;
	
	console_print("hwndMFGProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGProcessInfo, "hwndMFGProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;

	console_print("hwndMFGStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGStaticIpInfo, "hwndMFGStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;

    console_print("hwndMFGStaticInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL+10);

    hwndMFGStaticInfo = CreateWindow( TEXT("edit"), "",
            WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_AUTOVSCROLL | ES_MULTILINE,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL*2,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGStaticInfo, "hwndMFGStaticInfo");

    /***********************************************************/
    /*                                                         */
    /***********************************************************/

    console_print("hwndMFGGroupOptions's dim: x=%d, y=%d, width=%d, height=%d\n",
            GBOX1_START_X, start_y_of_Groupbox2, max_Groupbox2_width, max_Groupbox2_height);

    hwndMFGGroupOptions = CreateWindow( TEXT ("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGGroupOptions, "hwndMFGGroupOptions");

	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	console_print("hwndMFGStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndMFGStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);
	debug_positions(hwndMFGStaticImg, "hwndMFGStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	console_print("hwndMFGStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndMFGStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("edit"), NULL,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL | ES_READONLY,
            relative_x, relative_y,
            WIDTH_EDIT, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGStaticBrowser, "hwndMFGStaticBrowser");
	
	relative_x += WIDTH_EDIT + X_MARGIN;
	relative_y += HEIGHT_CONTROL + Y_MARGIN;

#if 0&&(defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	
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


	console_print("hwndMFGBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndMFGBtnBrowser = CreateWindow(TEXT ("button"), "Browser",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnBrowser, "hwndMFGBtnBrowser");   
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;
	
    console_print("hwndMFGBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndMFGBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnDown, "hwndMFGBtnDown");
	
	 console_print("hwndMFGBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

			
    hwndMFGBtnStop = CreateWindow( TEXT ("button"), "Stop",
            WS_CHILD /*| WS_VISIBLE */| BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnStop, "hwndMFGBtnStop");

    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#if defined(PRODUCTION)
    console_print("hwndCheckBoxBatch's dim: x=%d, y=%d, width=%d, height=%d\n",
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
    sprintf(mainTitle, TEXT("%s For %s %s"), APP_TITLE, PROJECT,VERSION);
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

static int linux_init(const char *ip)
{	
	int retval = -1;
	char image[256];
#ifdef MAINTAINMENT	
	strncpy(image,ini_file_info.name_of_image,256);
#else
	strncpy(image,BrowserImage,256);
#endif
	
	image_length = get_file_len(image);
	if(image_length <= 0)
	{		
		snprintf(error_info,ERROR_INFO_MAX,"image not exsit");
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",image);
		error_code = EC_INI_IMAGE_NOT_EXSIT;
		return error_code;
	}	
	/*make WinUpgradeLibInit run in backgroud_func*/
	
	int battery_check_sign = 0;
	#ifndef U3_LIB
	if(burn_mode == SELECT_LINUX_PROGRAMMING)
		battery_check_sign = Button_GetCheck(hwndCheckBoxSkipBatCheck) == BST_CHECKED ? 0 : 1;
	#elif defined(PRODUCTION)
	battery_check_sign = 1; //skip model number and bandtype check if it's U3 serial and PRODUCTION for tool
	#else
	battery_check_sign = 0;
	#endif
	dump_time();
	log_print("WinUpgradeLibInit() : image = %s,image_length = %d,ip = %s,battery_check_sign = %d.\n",
		image,image_length,ip,battery_check_sign);
	retval = WinUpgradeLibInit(image,image_length,ip,battery_check_sign);
	dump_time();
	log_print("retval = %d\n",retval);	
	if(retval < 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"Linux init error.");
		snprintf(error_msg,ERROR_INFO_MAX,"Linux init error.Error code is %s.",get_error_info(retval));		
	}
	return retval;
}

static int linux_download(void)
{	
	int retval = -1;
	const char *ip;
	int i;
	char ip_info[256];
	dump_time();
	log_print("linux_download() called.\n");
	
	for(i = 0; i < ini_file_info.ip_should_flash; ++i)
	{		
		ip = ini_file_info.ip[i];
#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND) || defined(CONFIG_PROJECT_REPEATER_BBA))
		if(burn_mode == SELECT_LINUX_PROGRAMMING)
		{
			snprintf(ip_info,256,"Ip : %s",ip);			
			SetWindowText(hwndIpInfo,ip_info);
		}
		log_print("ip = %s\n",ip);
#endif
start:
		SetDynamicInfo(LINUX_INIT);
		retval = linux_init(ip);
		StopDynamicInfo();	
		if(retval < 0)
		{			
			goto linux_download_error;
		}
		if(retval == 1)//Waiting for linux device insert
		{
			listening_on = IS_LISTENING_ON_LINUX;
			ResetEvent(g_lan_event);
			SetDynamicInfo("Waiting for reboot");
			WaitForSingleObject(g_lan_event,15000);	
			Sleep(10000);//wait for device reboot
			StopDynamicInfo();
			listening_on = IS_LISTENING_ON_NOTHING;
		}
		transfer_start();/*start a progress thread*/	
		dump_time();
#ifdef DEVELOPMENT
		log_print("burnpartition() : partition_selected = 0x%04x\n",partition_selected);
		retval = burnpartition(partition_selected);
#elif defined(MAINTAINMENT)
		
		if(Button_GetCheck(hwndCheckBoxUserdata) == BST_UNCHECKED)
		{
			log_print("burnpartition() : partition_selected = 0x%04x\n",partition_selected);
			retval = burnpartition(partition_selected);
		}
		else
		{
			log_print("burnImage() called.\n"); 
			retval = burnImage();
		}		

#elif defined(PRODUCTION)
		/*burn all partition*/
		log_print("burnpartition() : partition_selected = all partition.\n");
		retval = burnpartition(partition_selected);
#endif		
		
		transfer_complete();
		dump_time();
		log_print("retval = %d\n",retval);	
#ifdef MAINTAINMENT
		if(retval == 0xFDBB && Button_GetCheck(hwndCheckBoxUserdata) == BST_CHECKED)/*EC_USERDATA_UPGRADE_FAIL*/
		{		
			MessageBox(hwndMain,"Upgrade user data failed,Force to download(Notice:that will wipe user data).",szAppName,MB_ICONINFORMATION);
			dump_time();
			log_print("error code EC_USERDATA_UPGRADE_FAIL catched,Force to download.\n");
			/*Select Yes,force to download by invoke burnpartition*/
			transfer_start();			
		#ifdef U3_LIB
			log_print("burnpartition() : partition_selected = 0x%04x.\n",1<<5);
			//retval = burnpartition(1<<5);//userdata
			partition_selected = 1<<5;
		#else
			log_print("burnpartition() : partition_selected = 0x%04x.\n",0x03E0);
			//retval = burnpartition(0x03E0);
			partition_selected = 0x03E0;
		#endif
			transfer_complete();
			//need re-initialize
			Button_SetCheck(hwndCheckBoxUserdata, BST_UNCHECKED);
			goto start;			
			
		}
		
#endif
		if(retval != 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"Linux download failed!");
			snprintf(error_msg,ERROR_INFO_MAX,"Linux download failed! Error code is %s.",get_error_info(retval));
			goto linux_download_error;
		}	
	}
	
	return 0;
linux_download_error:	
	return retval;
}

static int spl_init(void)
{	
	image_length = get_file_len(ini_file_info.name_of_rescue_image);
	if(image_length <= 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"%s not exsit.",ini_file_info.name_of_rescue_image);
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",ini_file_info.name_of_rescue_image);
		error_code = EC_INI_RESCUE_IMAGE_NOT_EXSIT;
		return error_code;
	}
	return 0;
}
static int spl_download(void)
{
	int retval = -1;
	dump_time();
	log_print("spl_download() called.\n");
	
	SetDynamicInfo("SPL init");
	retval= spl_init();
	StopDynamicInfo();
	if(retval)
	{
		goto spl_download_error;
	}
	SetWindowText(hwndInfo,"Waiting for SPL download..");
	
	dump_time();
	log_print("burnSPL() : rescue_image = %s.\n",ini_file_info.name_of_rescue_image);
	
	transfer_start();
	
	retval = burnSPL(ini_file_info.name_of_rescue_image);
	
	transfer_complete();
	dump_time();
	log_print("retval = %d\n",retval);	
	if(retval != 0)
	{		
		snprintf(error_info,ERROR_INFO_MAX,"SPL download error.");		
		snprintf(error_msg,ERROR_INFO_MAX,"SPL download error,Error code is %s.",get_error_info(retval));
		goto spl_download_error;
	}
	if(partition_selected == 0)
		return 0;
	/*waiting for Linux device*/
	
	SetDynamicInfo("Waiting for linux device");
	listening_on = IS_LISTENING_ON_LINUX;
	ResetEvent(g_lan_event);
	WaitForSingleObject(g_lan_event, 15000);
	listening_on = IS_LISTENING_ON_NOTHING;
	Sleep(5000);
	StopDynamicInfo();
	
	retval = linux_download();	
	if(retval != 0)
		goto spl_download_error;	
	return 0;
spl_download_error:	
	return retval;
}
static int mfg_init(void)
{
	int retval = -1;
	char image[256];	
	
	strncpy(image,BrowserImage,256);
	
	image_length = get_file_len(image);
	if(image_length <= 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"%s not exsit.",image);
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",image);
		error_code = EC_INI_IMAGE_NOT_EXSIT;
		return error_code;
	}
	dump_time();
	log_print("burnMFGinit() : image = %s\n",image);
	retval = burnMFGinit(image);
	dump_time();
	log_print("retval = %d\n",retval);	
	if(retval != 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"MFG init error");
		snprintf(error_msg,ERROR_INFO_MAX,"MFG init error,Error code is %s.",get_error_info(retval));
		//return retval;
	}
	return retval;
}
static int mfg_download(void)
{	
	int retval = -1;
	dump_time();
	log_print("mfg_download() called.\n");

	SetDynamicInfo("MFG init");
	retval = mfg_init();
	StopDynamicInfo();
	if(retval)
	{			
		goto mfg_download_error;
	}
	SetWindowText(hwndInfo,"Waiting for MFG download..");
	dump_time();
	log_print("burnMFG() called.\n");
	transfer_start();
	retval = burnMFG();
	transfer_complete();
	dump_time();
	log_print("retval = %d\n",retval);	
	if(retval != 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"MFG download error.");
		snprintf(error_msg,ERROR_INFO_MAX,"MFG download error,Error code is %s.",get_error_info(retval));		
		goto mfg_download_error;
	}
		
	SetDynamicInfo("Waiting for SPL device");
	
	listening_on = IS_LISTENING_ON_SPL;
	ResetEvent(g_spl_event);
	WaitForSingleObject(g_spl_event, 8000);
	listening_on = IS_LISTENING_ON_NOTHING;
	
	Sleep(2000);
	StopDynamicInfo();
	
	SetWindowText(hwndInfo,"Waiting for SPL download..");	
	retval = spl_download();	
	if(retval != 0)
	{
		goto mfg_download_error;
	}		
	/*mfg dowload complete*/	
	
	return 0;
mfg_download_error:	
	return retval;
}

static void linux_download_complete_cb(int retval,void *private_data)
{
	if(retval == 0)
	{
		dump_time();
		log_print("linux download success.\n");
		SetWindowText(hwndInfo,"Complete");
		MessageBox(hwndMain,"Complete",szAppName,MB_ICONINFORMATION);
	}
	else
		SendMessage(hwndMain,WM_ERROR,(WPARAM)retval,0);
	
	g_processing = FALSE;
	update_ui_resources(TRUE);	
}
static void spl_download_complete_cb(int retval,void *private_data)
{
	if(retval == 0)
	{
		dump_time();
		if(partition_selected == 0)
			log_print("run rescue_image success.\n");
		else
			log_print("spl download success.\n");
		SetWindowText(hwndInfo,"Complete");
		MessageBox(hwndMain,"Complete",szAppName,MB_ICONINFORMATION);
	}
	else
	{
		//snprintf(error_msg+strlen(error_msg),ERROR_INFO_MAX-strlen(error_msg),"Please reset your device and try it again.");
		SendMessage(hwndMain,WM_ERROR,(WPARAM)retval,0);
	}
	g_processing = FALSE;
	update_ui_resources(TRUE);
}
static void mfg_download_complete_cb(int retval,void *private_data)
{
	if(retval == 0)
	{
		dump_time();
		log_print("mfg download success.\n");
		SetWindowText(hwndInfo,"Complete");
		MessageBox(hwndMain,"Complete",szAppName,MB_ICONINFORMATION);
	}
	else
	{
		g_on_batch = FALSE;
		SendMessage(hwndMain,WM_ERROR,(WPARAM)retval,0);
	}
	g_processing = FALSE;
	update_ui_resources(TRUE);	
	if(g_on_batch)
	{
		update_ui_resources(FALSE);
		listening_on = IS_LISTENING_ON_MFG;
		SetDynamicInfo("Waiting for MFG device");		
		dump_download_varibles();
		log_print("Waiting for MFG device..\n");
#ifdef PRODUCTION
		ShowWindow(hwndMFGBtnDown,FALSE);		
		EnableWindow(hwndMFGBtnStop,TRUE);
		ShowWindow(hwndMFGBtnStop,TRUE);
#endif		
	}
	else
	{
		ShowWindow(hwndMFGBtnDown,TRUE);
		ShowWindow(hwndMFGBtnStop,FALSE);
	}		
}
/*********************Nand Programing Button Process***********************/
static BOOL OnBtnBrowser(void)
{
	EnableWindow(hwndTab,FALSE);	
	if(GetFileName(BrowserImage,256) == TRUE)
	{		
		SetWindowText(hwndStaticBrowser,BrowserImage);
		if(get_image_info(BrowserImage) == FALSE)
		{
#ifdef DEVELOPMENT
			SendMessage(hwndMain,WM_DESTROY_PARTITION_LIST,0,0);			
#endif
			EnableWindow(hwndBtnDown,FALSE);
			SendMessage(hwndMain,WM_ERROR,error_code,0);
		}
		else
		{
#ifdef DEVELOPMENT
			SendMessage(hwndMain,WM_DESTROY_PARTITION_LIST,0,0);		
			SendMessage(hwndMain,WM_CREATE_PARTITION_LIST,0,0);
#endif
			reset_ui_resources(SELECT_LINUX_PROGRAMMING);
			reset_ui_resources(SELECT_SPL_PROGRAMMING);
			reset_ui_resources(SELECT_MFG_PROGRAMMING);
			EnableWindow(hwndBtnDown,TRUE);
		}				
	}
	EnableWindow(hwndTab,TRUE);
	return TRUE;
}

static int OnBtnRefreshClick(void)//unused
{
	return 0;
}
static int OnBtnEnableTelnetClick(void)
{
#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND))
	int ip_count = 4;
#elif defined(CONFIG_PROJECT_REPEATER_BBA)
	int ip_count = 2;
#else
	int ip_count = 1;
#endif	
	int error_flag = 0;
	int i;
	int retval = -1;
	/*save button LinBtnDown's status*/
	WINDOWINFO info_for_btn_down;
	info_for_btn_down.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hwndLinBtnDown,&info_for_btn_down);
	update_ui_resources(FALSE);
	
	SetDynamicInfo("EnableTelnet");
	//EnableTelnet for all ip
	for(i = 0; i < ip_count; ++i)
	{
		retval = EnableTelnet(ip_addr[i]);
		if(retval != 0)
			error_flag |= 1<<i;
	}	
	StopDynamicInfo();
	
	if(error_flag != 0)
	{
		/*for example:10.10.0.12 10.10.12.11 success,but 10.10.12.21 10.10.12.22 Failed. it will be output like this
		* EnableTelnet 10.10.12.21 10.10.12.22 Failed.
		*/
		char info[256];
		strcpy(info,"EnableTelnet ");
		for(i = 0; i < ip_count; ++i)
		{			
			if(error_flag & (1<<i))
			{
				strcat(info,ip_addr[i]);
				strcat(info," ");
			}
		}
		strcat(info,"Failed.");
		SetWindowText(hwndInfo,info);
		log_print("%s\n",info);
		//ERROR_MESSAGE("EnableTelnet failed! error code is %s.",get_error_info(retval));
	}
	else
	{
		log_print("EnableTelnet Success.\n");
		SetWindowText(hwndInfo,"EnableTelnet Success.");
	}
	update_ui_resources(TRUE);
	/*restore LinBtnDown's status*/
	EnableWindow(hwndLinBtnDown,!(info_for_btn_down.dwStyle & WS_DISABLED));
	
	return 0;
}

static int OnBtnFileDownload(void)
{	
	return 0;
}

static int OnBtnFileUpload(void)
{	
	return 0;
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
	
	HWND hwnd = (HWND)lParam;
	/*if(g_processing == FALSE)
	{
		CLEAR_INFO();
	}*/
	if(process_partitionlist(hwnd))
		return TRUE;
#ifdef MAINTAINMENT
	if(hwnd == hwndCheckBoxUserdata)
	{
		if(Button_GetCheck(hwndCheckBoxUserdata) == BST_CHECKED && IDYES == MessageBox(hwndMain,TEXT("**CAUTION**\nAll userdata[profile and encryption key] will be deleted when upgrade if this option is unchecked.\nUncheck it anyway?"),szAppName,MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2))
		{
			SendMessage(hwndCheckBoxUserdata,BM_SETCHECK,BST_UNCHECKED,0);			
		}			
		else
		{
			SendMessage(hwndCheckBoxUserdata,BM_SETCHECK,BST_CHECKED,0);			
		}
		return TRUE;
	}
#endif	
	if(hwnd == hwndLinBtnBrowser)
	{
		log_print("\n");
		dump_time();
		log_print("button LinBtnBrowser clicked.\n");
		CLEAR_INFO();
		return OnBtnBrowser();		
	}
	else if(hwnd == hwndLinBtnDown){
		log_print("\n");
		dump_time();
		log_print("button LinBtnDown clicked.\n");
		CLEAR_INFO();
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,error_code,0);
			return TRUE;
		}
#ifdef MAINTAINMENT
		partition_selected = ((1<<total_partition)-1)&~0x000B;//every partition except ipl spl calibration
#else
		int i;
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}

		if(partition_selected == 0)
		{
			log_print("No partition select.\n");
			ERROR_MESSAGE("No partition select.");			
			return TRUE;
		}
#endif
		
		g_background_func = linux_download;
		g_complete_func = linux_download_complete_cb;		
		g_processing = TRUE;
		update_ui_resources(FALSE);		
		dump_download_varibles();
		log_print("Waiting for linux download..\n");
		
	}
	else if(hwnd == hwndBtnEnableTelnet)
	{
		log_print("\n");
		dump_time();
		log_print("button BtnEnableTelnet clicked.\n");
		CLEAR_INFO();
		g_background_func = OnBtnEnableTelnetClick;
	}
	else if(hwnd == hwndLinBtnRefresh)
	{
		log_print("\n");
		dump_time();
		log_print("button LinBtnRefresh clicked.\n");
		CLEAR_INFO();
		g_background_func = OnBtnRefreshClick;
	}
	else 
		return FALSE;
	WAKE_THREAD_UP();
	return TRUE;
}
static BOOL ProcessSPLCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	/* if(g_processing == FALSE)
	{
		CLEAR_INFO();
	} */
	if(process_partitionlist(hwnd))
		return TRUE;
	if(hwnd == hwndSPLBtnDown)
	{
		int i;
		log_print("\n");
		dump_time();
		log_print("button SPLBtnDown clicked.\n");
		CLEAR_INFO();
		
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,error_code,0);
			return TRUE;
		}		
		
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}
		if(partition_selected == 0 && IDYES != MessageBox(hwndMain,"No partition select.Are you sure want to run rescue image?",szAppName,MB_ICONINFORMATION|MB_YESNO))
		{
			log_print("No partition select.\n");
			//ERROR_MESSAGE("No partition select.");
			return TRUE;
		}
		
		g_background_func = spl_download;
		g_complete_func = spl_download_complete_cb;
		g_processing = TRUE;
		update_ui_resources(FALSE);		
		dump_download_varibles();
		log_print("Waiting for linux download..\n");		
		WAKE_THREAD_UP();		
	}
	else if(hwnd == hwndSPLBtnBrowser)
	{
		log_print("\n");
		dump_time();
		log_print("button SPLBtnBrowser clicked.\n");
		CLEAR_INFO();
		return OnBtnBrowser();
	}
	return TRUE;
}
static BOOL ProcessMFGCommand(WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)lParam;
	/* if(g_processing == FALSE)
	{
		CLEAR_INFO();
	} */
	if(process_partitionlist(hwnd))
		return TRUE;
	if(hwnd == hwndMFGBtnDown)
	{
		log_print("\n");
		dump_time();
		log_print("button MFGBtnDown clicked.\n");
		CLEAR_INFO();
		
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,error_code,0);
			return TRUE;
		}
#ifdef DEVELOPMENT
		int i;		
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}
		if(partition_selected == 0)
		{
			log_print("No partition select.\n");
			ERROR_MESSAGE("No partition select.");
			return TRUE;
		}
#elif defined PRODUCTION
		partition_selected = ((1<<total_partition)-1);
#endif
		
		update_ui_resources(FALSE);
		if(CHECKBOX_IS_CLICK(hwndCheckBoxBatch))
		{
			g_on_batch = TRUE;							
		}
		else
			g_on_batch = FALSE;
		listening_on = IS_LISTENING_ON_MFG;
		SetDynamicInfo("Waiting for MFG device");
		dump_download_varibles();
		log_print("Waiting for MFG device..\n");
		
#ifdef PRODUCTION		
		ShowWindow(hwndMFGBtnDown,FALSE);		
		EnableWindow(hwndMFGBtnStop,TRUE);
		ShowWindow(hwndMFGBtnStop,TRUE);		
#endif		
	}
	else if(hwnd == hwndMFGBtnBrowser)
	{
		log_print("\n");
		dump_time();
		log_print("button MFGBtnBrowser clicked.\n");
		CLEAR_INFO();
		return OnBtnBrowser();
	}
	else if(hwnd == hwndMFGBtnStop)
	{
		log_print("\n");
		dump_time();
		log_print("button MFGBtnStop clicked.\n");			
		if(g_processing == FALSE)
		{
			update_ui_resources(TRUE);
			listening_on = IS_LISTENING_ON_NOTHING;
			StopDynamicInfo();
			CLEAR_INFO();				
		}		
		g_on_batch = FALSE;
		ShowWindow(hwndMFGBtnDown,TRUE);
		ShowWindow(hwndMFGBtnStop,FALSE);		
	}
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
    HWND child;
	static int dot_count = 0;
	
    switch(message)
    {
        case WM_COMMAND:		
		/* if(g_locking == TRUE)
			break; */
		
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
					PostQuitMessage(0);
                }                
            }
            break;
        case WM_TIMER:
		switch(wParam)
		{
			case TIMER_DYNAMIC_INFO:
			{
				TCHAR text[256];
				strncpy(text,dynamic_info,250);				
				++dot_count;
				dot_count %= 6;
				int i;
				for(i = 0; i < dot_count; ++i)
				{
					text[strlen(dynamic_info) + i] = '.';
				}
				text[strlen(dynamic_info) + i] = 0;
				SetWindowText(hwndDynamic,text);
			}
			break;
		}
        return TRUE;
		case WM_KILL_TIMER:
		KillTimer(hwndMain,wParam);
		dot_count = 0;
		break;
		case WM_CLOSE:
		if(g_processing == FALSE || IDYES == MessageBox(hwndMain,TEXT("Warning:This operation may cause download incomplete.\nAre you sure want to quit?"),szAppName,MB_ICONWARNING | MB_YESNO))
			return DefWindowProc(hwnd, message, wParam, lParam);
		break;
        case WM_NOTIFY: /* trigger by user click */
            switch(((LPNMHDR)lParam)->code)
            {
				/*only development tool have mutiple tab controll*/
#ifdef DEVELOPMENT
                case TCN_SELCHANGE:					
					burn_mode = TabCtrl_GetCurSel(hwndTab);
					switch(burn_mode)
					{
						case SELECT_MFG_PROGRAMMING:
						ShowWindow(hwndMFGPage,TRUE);
						ShowWindow(hwndSPLPage,FALSE);
						ShowWindow(hwndLinPage,FALSE);
						ShowWindow(hwndFileTransferPage,FALSE);
						ini_file = "for_mfg_file.ini";
						reset_general_handler(SELECT_MFG_PROGRAMMING);						
						break;
						
						case SELECT_SPL_PROGRAMMING:
						ShowWindow(hwndMFGPage,FALSE);
						ShowWindow(hwndSPLPage,TRUE);
						ShowWindow(hwndLinPage,FALSE);
						ShowWindow(hwndFileTransferPage,FALSE);
						ini_file = "for_mfg_file.ini";
						reset_general_handler(SELECT_SPL_PROGRAMMING);
						break;
						
						case SELECT_LINUX_PROGRAMMING:
						ShowWindow(hwndMFGPage,FALSE);
						ShowWindow(hwndSPLPage,FALSE);
						ShowWindow(hwndLinPage,TRUE);
						ShowWindow(hwndFileTransferPage,FALSE);						
						ini_file = "for_user_file.ini";
						reset_general_handler(SELECT_LINUX_PROGRAMMING);
						
						break;
						case SELECT_FILE_TRANSFER:
						ShowWindow(hwndMFGPage,FALSE);
						ShowWindow(hwndSPLPage,FALSE);
						ShowWindow(hwndLinPage,FALSE);
						ShowWindow(hwndFileTransferPage,TRUE);
						ini_file = NULL;							
						hwndInfo = hwndFdNotify;
						hwndIpInfo = NULL;
						hwndProcessInfo = NULL;
						break;
					}
					dump_time();					
                    log_print("burn mode switch to %s\n",burn_mode == SELECT_LINUX_PROGRAMMING ? "linux" :
					(burn_mode == SELECT_SPL_PROGRAMMING ? "spl" :
					(burn_mode == SELECT_MFG_PROGRAMMING ? "mfg" : "invalid burn mode")));
                    return TRUE;
#endif
                default:
                    break;
            }
            return FALSE;

        case WM_DEVICECHANGE: /* trigger by USB cable insert */
		{
			if(wParam == DBT_DEVICEARRIVAL && 
			((PDEV_BROADCAST_HDR)lParam)->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				GUID insert_dev = ((PDEV_BROADCAST_DEVICEINTERFACE)lParam)->dbcc_classguid;
				if(!memcmp(&insert_dev,&GUID_DEVCLASS_AD6900_SPL,sizeof(GUID)) &&
					listening_on == IS_LISTENING_ON_SPL )
				{
					//spl device insert
					dump_time();
					log_print("SPL device inserted.\n");
					SetEvent(g_spl_event);					
				}
				else if(!memcmp(&insert_dev,&GUID_DEVCLASS_AD6900_MFG,sizeof(GUID)) &&
					listening_on == IS_LISTENING_ON_MFG)
				{
					//mfg device insert
					StopDynamicInfo();
					listening_on = IS_LISTENING_ON_NOTHING;
					g_background_func = mfg_download;
					g_complete_func = mfg_download_complete_cb;
					g_processing = TRUE;
					if(g_on_batch == FALSE)
					{
						ShowWindow(hwndMFGBtnDown,TRUE);
						ShowWindow(hwndMFGBtnStop,FALSE);
					}
					dump_time();
					log_print("mfg device inserted.\n");
					WAKE_THREAD_UP();					
				}
				else if(!memcmp(&insert_dev,&GUID_DEVCLASS_AD6900_LAN,sizeof(GUID)) &&
				listening_on == IS_LISTENING_ON_LINUX)
				{
					//lan device insert					
					dump_time();
					log_print("linux device inserted.\n");					
					SetEvent(g_lan_event);
				}
			}				
		}
		break;
		case WM_CREATE_PARTITION_LIST:
		log_print("create partition list.\n");
		CreatePartitionList();
		
		break;
		case WM_DESTROY_PARTITION_LIST:
		log_print("destory partition list.\n");
		DestoryPartitionList();
		break;
		case WM_CREATE_PROGRESS_BAR:
		PopProgressBar();
		break;		
		case WM_DESTROY_PROGRESS_BAR:
		DestoryProgressBar();
		break;
		
		case WM_ERROR:
		dump_time();
#if 0
		if(wParam != -1)//appending error code string
		{
			snprintf(error_msg+strlen(error_msg),ERROR_INFO_MAX-strlen(error_msg)," Error code is: %s",get_error_info(wParam));
		}		
		if(error_info[0] != 0)
		{
			log_print("error_info = %s\n",error_info);
			SetWindowText(hwndInfo,error_info);
		}
		if(error_msg[0] != 0)
		{
			log_print("error_msg = %s\n",error_msg);
			ERROR_MESSAGE(error_msg);
		}
		error_info[0] = 0;
		error_msg[0] = 0;
#else		
		/*not use error_info and error_msg buffer any more,just use error_code(for UI),wParam(for lib) and error code table to display error message*/
		error_code = wParam;		
#define back_trace()	//not implement yet just ignore it
		if(error_code == 0 || error_code == -1)
		{
			back_trace();
		}
#undef back_trace
		snprintf(error_info,ERROR_INFO_MAX,"%s(0x%04X)",get_error_info(error_code),error_code);
		strncpy(error_msg,get_short_info(error_code),ERROR_INFO_MAX);
		log_print("Error : error code is 0x%04X.\n",error_code);
		log_print("error_info : %s\n",error_info);
		log_print("error_msg  : %s\n",error_msg);
		SetWindowText(hwndInfo,error_info);
		ERROR_MESSAGE(error_msg);		
#endif//DEVELOPMENT
		error_code = OK;
		break;		
		case WM_INVOKE_CALLBACK:
		if (g_complete_func)
		{
			g_complete_func(g_retval, g_complete_private_data);
			g_complete_func = NULL;
			g_retval = 0;
			g_complete_private_data = NULL;
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
	
	/*check if ap's directory exsit*/

#ifdef MAINTAINMENT
	if(_access("DLL",0))
	{
		MessageBox(NULL,TEXT("Tool package broken,please re-install this tool."),szAppName,MB_ICONERROR);
		return 0;
	}
/* 
	if(_access("sys",0))
	{
		MessageBox(NULL,TEXT("sys lost"),szAppName,MB_ICONERROR);
		return 0;
	} */
#endif
	//Initialize net
	WSADATA wsaData;
    int retval;
	retval = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (retval != 0)
    {
        log_print("%s_%d: WSAStartup() error, error code is %d\n", __FILE__, __LINE__, WSAGetLastError());
        return 0;
    }
	open_debug_log();
    g_event = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset
	g_lan_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_spl_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (StartThread() == FALSE)
    {
        MessageBox(NULL, TEXT ("Create thread error"), szAppName, MB_ICONERROR);
        ExitDebugConsole();
        return 0;
    }


    /* Application init */
    if (InitApplication() == FALSE)
    {
        //MessageBox(NULL, TEXT (APP_TITLE" is running"), szAppName, MB_ICONERROR);
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
	reset_general_handler(SELECT_LINUX_PROGRAMMING);
#elif defined(MAINTAINMENT)
	InitLinuxWindow();
	reset_general_handler(SELECT_LINUX_PROGRAMMING);	
#elif defined(PRODUCTION)
	InitMFGWindow();
	reset_general_handler(SELECT_MFG_PROGRAMMING);
#endif

	dump_time();
	dump_project();
	
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
	unregister_notifyer();
	
    CloseHandle(g_event);
	CloseHandle(g_lan_event);
	CloseHandle(g_spl_event);

    /* Receive the WM_QUIT message, release mutex and return the exit code to the system */
    CloseHandle(hMutex);
    ExitDebugConsole();

    return msg.wParam;
}
