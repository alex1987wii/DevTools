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
#define WM_INVOKE_CALLBACK			(WM_USER+115)
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
#define DEV_TOOLS_NUMBER        3           /* Unication dev tools have three utilities */
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

static int listening_on = IS_LISTENING_ON_NOTHING;

static char work_path[MAX_PATH];/*the path for this program*/


static HANDLE g_event = NULL;    // event
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
static HWND    hwndInfo,hwndIpInfo,hwndProcessInfo,hwndStaticBrowser,hwndBtnBrowser,hwndBtnDown;

/**********************Linux Handler Declaration*************************/
static HWND    hwndLinPage;
static HWND    hwndLinGroupReadme;
static HWND    hwndLinTextTarget,hwndLinStaticInfo,hwndLinStaticIpInfo,hwndLinProcessInfo,hwndLinStaticNotice;

static HWND    hwndLinStaticDev,hwndLinComboDevList,hwndLinBtnRefresh,hwndLinBtnReset;/*for Br01*/
static HWND    hwndLinGroupOptions;

static HWND    hwndLinStaticImg;
static HWND    hwndLinStaticBrowser;

static HWND    hwndLinBtnBrowser,hwndLinBtnDown,hwndBtnEnableTelnet;
static HWND    hwndCheckBoxDelete,hwndCheckBoxSkipBatCheck;


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
	KillTimer(hwndMain,TIMER_DYNAMIC_INFO);
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
		WS_CHILD | BS_AUTOCHECKBOX | WS_VISIBLE,
		partition_x_pos + (i%cbs_per_line)*(WIDTH_CHECKBOX) + X_MARGIN,
		partition_y_pos + (i/cbs_per_line)*HEIGHT_CONTROL + Y_MARGIN,
		WIDTH_CHECKBOX, HEIGHT_CONTROL,
		hwndPage,NULL,
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

void update_ui_resources(int enable)
{	
	/*BtnDown enable if error occurs,but disable by reset_ui_resources when successfully complete*/
	switch(burn_mode)
	{
		case SELECT_LINUX_PROGRAMMING:		
		EnableWindow(hwndLinBtnBrowser,enable);
		EnableWindow(hwndLinBtnDown,enable);
		EnableWindow(hwndLinBtnRefresh,enable);
		EnableWindow(hwndCheckBoxDelete,enable);
		EnableWindow(hwndBtnEnableTelnet,enable);
		EnableWindow(hwndCheckBoxSkipBatCheck,enable);
		
#ifdef DEVELOPMENT
		EnableWindow(hwndLinPage,enable);
		EnablePartitionList(enable);		
#endif
		break;
		case SELECT_SPL_PROGRAMMING:		
		EnableWindow(hwndSPLBtnBrowser,enable);		
		EnableWindow(hwndSPLBtnDown,enable);
		
#ifdef DEVELOPMENT
		EnableWindow(hwndSPLPage,enable);
		EnablePartitionList(enable);
		
#endif		
		break;
		case SELECT_MFG_PROGRAMMING:		
		EnableWindow(hwndMFGBtnBrowser,enable);		
		EnableWindow(hwndMFGBtnDown,enable);
		EnableWindow(hwndCheckBoxBatch,enable);
#ifdef DEVELOPMENT
		EnableWindow(hwndMFGPage,enable);
		EnablePartitionList(enable);		
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
		return FALSE;
	}
	uni_image_header_t image_header;
		
	if(sizeof(uni_image_header_t) != fread(&image_header,1,sizeof(uni_image_header_t),fp))
	{
		fclose(fp);
		snprintf(error_info,ERROR_INFO_MAX,"Invalid image.");
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:\n%s",image);
		return FALSE;
	}
	fclose(fp);
	printf("image_header.image_version = %s\n",image_header.image_version);
	printf("image_header.release_version = %s\n",image_header.release_version);
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
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:\n%s",image);
		return FALSE;
	}
	total_partition = image_header.total_partitions;
	if(total_partition <= 0 || total_partition > UNI_MAX_PARTITION)
	{
		snprintf(error_info,ERROR_INFO_MAX,"Invalid image.");
		snprintf(error_msg,ERROR_INFO_MAX,"Invalid image:\n%s",image);
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
static inline BOOL check_ini(void)
{
	/*read the ini file and parse it into struct*/
	/*if don't need a ini file then return TRUE*/
	if(ini_file == NULL)
		return TRUE;
	if(_access(ini_file,0))
	{
		snprintf(error_info,ERROR_INFO_MAX,"ini file lost.");
		snprintf(error_msg,ERROR_INFO_MAX,"%s lost.",ini_file);
		return FALSE;
	}	
	if(parse_ini_file(ini_file) == FALSE)
	{
		snprintf(error_info,ERROR_INFO_MAX,"ini file invalid configuration.");
		snprintf(error_msg,ERROR_INFO_MAX,"%s invalid configuration.",ini_file);		
		return FALSE;
	}
	switch(burn_mode)
	{

		case SELECT_LINUX_PROGRAMMING:
		if(ini_file_info.ip_should_flash == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No ip specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No ip specified in %s",ini_file);
			return FALSE;
		}
#ifdef MAINTAINMENT
		if(ini_file_info.name_of_image[0] == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No image specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No image specified in %s",ini_file);
			return FALSE;
		}
		if(_access(ini_file_info.name_of_image,0))
		{
			snprintf(error_info,ERROR_INFO_MAX,"can't find image.");
			snprintf(error_msg,ERROR_INFO_MAX,"can't find %s.",ini_file_info.name_of_image);
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
			return FALSE;
		}		
		if(ini_file_info.name_of_rescue_image[0] == 0)
		{
			snprintf(error_info,ERROR_INFO_MAX,"No rescue image specified.");
			snprintf(error_msg,ERROR_INFO_MAX,"No rescue image specified in %s",ini_file);
			return FALSE;
		}
		if(_access(ini_file_info.name_of_rescue_image,0))
		{
			snprintf(error_info,ERROR_INFO_MAX,"can't find rescue image.");
			snprintf(error_msg,ERROR_INFO_MAX,"can't find %s.",ini_file_info.name_of_rescue_image);
			return FALSE;
		}
		break;
	}
#ifdef ENABLE_DEBUG
	printf("ini_file = %s\n",ini_file);
	printf("ip_should_flash = %d\n",ini_file_info.ip_should_flash);
	int i;
	for(i = 0 ;i < ini_file_info.ip_should_flash;++i)
	{
		printf("ip = %s\n",ini_file_info.ip[i]);
	}	
	printf("rescue_image = %s\n",ini_file_info.name_of_rescue_image);
#endif
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


static DWORD WINAPI TransferThread(LPVOID lpParam)
{
	int retval = 0;
	unsigned char percent = 0,last_percent;
	unsigned short status = 0,last_status;
	unsigned char index = 0,last_index;
	
	TCHAR text[256];
	
	char *stage[MAX_STATUS] = {"Preparing","Flashing","Verifying","Executing","Finished","Transfer"};
		
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
			break;
		}
		
		retval = progress_reply_status_get(&index,&percent,&status);
#ifdef ENABLE_DEBUG
		printf("retval = %d, status = %d, index = %02x, percent = %d\n",
		retval,status,index,percent);
#endif
		if(retval != 0)
		{
			printf("progress_reply_status_get return errcode:%s\n",get_error_info(retval));
			break;
		}		
		
		if(!(last_index == index && last_percent == percent && last_status == status))//stage index or percent changed
		{
			if(percent != 0)
			{
				snprintf(text,256,"%s %s\nPercetage:[%d%%]",stage[status],index < total_partition ? partition_name[index] : "",percent);
			}
			else
			{
				snprintf(text,256,"%s %s",stage[status],index < total_partition ? partition_name[index] : "");
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

    printf("hwndLinGroupReadme's dim: x=%d, y=%d, width=%d, height=%d\n",
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
	readme = TEXT("1. Connect Device with PC via USB cable.\n2. Press Upgrade/Download Button and wait until it finished.\n3. If any error occurs during updating/downloading,please reconnect device and re-open this tool to try it again.\n4.Don't pull off the USB cable during process.");
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

      
    relative_y += HEIGHT_CONTROL+35;

	printf("hwndLinProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinProcessInfo, "hwndLinProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	
	printf("hwndLinStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndLinStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticIpInfo, "hwndLinStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	
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

    hwndLinGroupOptions = CreateWindow( TEXT("button"), "Options",
            WS_CHILD | WS_VISIBLE  | BS_GROUPBOX,
            GBOX1_START_X, start_y_of_Groupbox2,
            max_Groupbox2_width, max_Groupbox2_height,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinGroupOptions, "hwndLinGroupOptions");
	
	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

#ifndef MAINTAINMENT
    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	printf("hwndLinStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndLinStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinStaticImg, "hwndLinStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	printf("hwndLinStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndLinStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("static"), NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_EDIT, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinStaticBrowser, "hwndLinStaticBrowser");
	
	relative_x += WIDTH_EDIT + X_MARGIN;
	relative_y += HEIGHT_CONTROL + Y_MARGIN;
#endif

#if 0&&(defined(MAINTAINMENT) && !defined(U3_LIB))
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

	printf("hwndLinBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);
	hwndLinBtnDown = CreateWindow( TEXT ("button"), "Upgrade",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinBtnDown, "hwndLinBtnDown");
	
#elif defined(DEVELOPMENT)

	printf("hwndLinBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndLinBtnBrowser = CreateWindow(TEXT ("button"), "Browser",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndLinBtnBrowser, "hwndLinBtnBrowser");    
	
    relative_y += HEIGHT_CONTROL + V_GAPS*2;
	printf("hwndLinBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);
    hwndLinBtnDown = CreateWindow( TEXT ("button"), "Download",
            WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON ,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);
	debug_positions(hwndLinBtnDown, "hwndLinBtnDown");

#endif    
	
	printf("hwndLinBtnStop's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);


    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#ifdef DEVELOPMENT
    printf("hwndBtnEnableTelnet's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndBtnEnableTelnet = CreateWindow( TEXT("button"), "EnableTelnet",
            WS_CHILD | WS_VISIBLE  | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndLinPage, NULL,
            hInst, NULL);

    debug_positions(hwndBtnEnableTelnet, "hwndBtnEnableTelnet");
	relative_y += HEIGHT_CONTROL + V_GAPS*2;
#endif
#ifndef U3_LIB	
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

      
    relative_y += HEIGHT_CONTROL+35;
	
	printf("hwndSPLProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLProcessInfo, "hwndSPLProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	

	printf("hwndSPLStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndSPLStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);

    debug_positions(hwndSPLStaticIpInfo, "hwndSPLStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;
	

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

	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	printf("hwndSPLStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndSPLStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndSPLPage, NULL,
            hInst, NULL);
	debug_positions(hwndSPLStaticImg, "hwndSPLStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	printf("hwndSPLStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndSPLStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("static"), NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
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
	printf("hwndSPLBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
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

    printf("hwndSPLBtnDown's dim: x=%d, y=%d, width=%d, height=%d\n",
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

      
    relative_y += HEIGHT_CONTROL+35;
	
	printf("hwndMFGProcessInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGProcessInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGProcessInfo, "hwndMFGProcessInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;

	printf("hwndMFGStaticIpInfo's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, STATIC_WIDTH, HEIGHT_CONTROL);

    hwndMFGStaticIpInfo = CreateWindow( TEXT("static"), "",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            STATIC_WIDTH, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGStaticIpInfo, "hwndMFGStaticIpInfo");
	
	relative_y += HEIGHT_CONTROL + V_GAPS;

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

	relative_x = GBOX1_START_X;
    relative_y = start_y_of_Groupbox2;

    relative_x += H_GAPS * 2;
    relative_y += V_GAPS + HEIGHT_TAG;
	
	printf("hwndMFGStaticImg's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_TEXT, HEIGHT_CONTROL);
	hwndMFGStaticImg = CreateWindow( TEXT ("static"), "    Image:",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            relative_x, relative_y,
            WIDTH_TEXT, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);
	debug_positions(hwndMFGStaticImg, "hwndMFGStaticImg");
	relative_x += WIDTH_TEXT + X_MARGIN;
	
	printf("hwndMFGStaticBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_EDIT, HEIGHT_CONTROL);

    hwndMFGStaticBrowser = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT ("static"), NULL,
            WS_CHILD | WS_VISIBLE | SS_LEFT,
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


	printf("hwndMFGBtnBrowser's dim: x=%d, y=%d, width=%d, height=%d\n",
            relative_x, relative_y, WIDTH_BUTTON, HEIGHT_CONTROL);

    hwndMFGBtnBrowser = CreateWindow(TEXT ("button"), "Browser",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnBrowser, "hwndMFGBtnBrowser");   
	
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

			
    hwndMFGBtnStop = CreateWindow( TEXT ("button"), "Stop",
            WS_CHILD /*| WS_VISIBLE */| BS_PUSHBUTTON,
            relative_x, relative_y,
            WIDTH_BUTTON, HEIGHT_CONTROL,
            hwndMFGPage, NULL,
            hInst, NULL);

    debug_positions(hwndMFGBtnStop, "hwndMFGBtnStop");

    relative_y += HEIGHT_CONTROL + V_GAPS*2;
#if defined(PRODUCTION)
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

static BOOL linux_init(const char *ip)
{
	printf("linux_init..\n");
	int retval = -1;
	char image[256];
#ifdef MAINTAINMENT	
	strcpy(image,ini_file_info.name_of_image);
#else
	GetWindowText(hwndStaticBrowser,image,256);
#endif
	
	image_length = get_file_len(image);
	if(image_length <= 0)
	{		
		snprintf(error_info,ERROR_INFO_MAX,"image not exsit");
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",image);
		return FALSE;
	}	
	/*make WinUpgradeLibInit run in backgroud_func*/
	
	int battery_check_sign = 0;
	if(burn_mode == SELECT_LINUX_PROGRAMMING)
		battery_check_sign = Button_GetCheck(hwndCheckBoxSkipBatCheck) == BST_CHECKED ? 0 : 1;
	retval = WinUpgradeLibInit(image,image_length,ip,battery_check_sign);
	
	printf("name_of_image = %s\nimage_len = %d\nip = %s\n",image,image_length,ip);
	if(retval != 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"Linux init error.");
		snprintf(error_msg,ERROR_INFO_MAX,"Linux init error.Error code is %s.",get_error_info(retval));
		return FALSE;
	}
	return TRUE;
}

static int linux_download(void)
{	
	int retval = -1;
	const char *ip;
	int i;
	char ip_info[256];
	
	for(i = 0; i < ini_file_info.ip_should_flash; ++i)
	{		
		ip = ini_file_info.ip[i];
#if (defined(CONFIG_PROJECT_BR01) || defined(CONFIG_PROJECT_BR01_2ND) || defined(CONFIG_PROJECT_REPEATER_BBA))
		snprintf(ip_info,256,"Ip : %s",ip);
		printf("ip_info = %s\n",ip_info);
		SetWindowText(hwndIpInfo,ip_info);
#endif
		SetDynamicInfo("Linux init");
		BOOL ret = linux_init(ip);
		StopDynamicInfo();	
		if(FALSE == ret)
		{			
			goto linux_download_error;
		}
		
		transfer_start();/*start a progress thread*/	
		
#ifdef DEVELOPMENT
		printf("partition_selected = 0x%04x\n",partition_selected);
		retval = burnpartition(partition_selected);
#elif defined(MAINTAINMENT)
		retval = burnImage();
#if 0	
		if(Button_GetCheck(hwndCheckBoxDelete) == BST_CHECKED)
			retval = burnpartition(0xxxx);
#endif
#elif defined(PRODUCTION)
		/*burn all partition*/
		retval = burnpartition(((1<<total_partition)-1)/*&~(1<<3)*/);
#endif		
		
		transfer_complete();
		
#ifdef MAINTAINMENT
		if(retval == 0xFDBB)/*EC_USERDATA_UPGRADE_FAIL*/
		{
			if(IDYES == MessageBox(hwndMain,"Upgrade Failed,Force to download?(Warning:that will wipe userdata of your device)",szAppName,MB_YESNO|MB_ICONWARNING))
			{
				/*Select Yes,force to download by invoke burnpartition*/
				transfer_start();			
			#ifdef U3_LIB
				retval = burnpartition(1<<5);//userdata
			#else
				retval = burnpartition(0x03E0);
			#endif
				transfer_complete();
			}
			else
			{
				/*Select No,reboot this cpu,then do next cpu*/
				RebootTarget(ip);
				continue;
			}
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
	return -1;
}

static BOOL spl_init(void)
{	
	printf("spl_init..\n");	
	image_length = get_file_len(ini_file_info.name_of_rescue_image);
	if(image_length <= 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"%s not exsit.",ini_file_info.name_of_rescue_image);
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",ini_file_info.name_of_rescue_image);
		return FALSE;
	}
	return TRUE;
}
static int spl_download(void)
{
	int retval = -1;	
	printf("spl_download..\n");
	
	SetDynamicInfo("SPL init");
	BOOL ret = spl_init();
	StopDynamicInfo();
	if(FALSE == ret)
	{
		goto spl_download_error;
	}
	SetWindowText(hwndInfo,"Waiting for SPL download..");
	
	transfer_start();
	
	retval = burnSPL(ini_file_info.name_of_rescue_image);
	
	transfer_complete();
	
	if(retval != 0)
	{		
		snprintf(error_info,ERROR_INFO_MAX,"SPL download error.");		
		snprintf(error_msg,ERROR_INFO_MAX,"SPL download error,Error code is %s.",get_error_info(retval));
		goto spl_download_error;
	}
	retval = linux_download();	
	if(retval != 0)
		goto spl_download_error;	
	return 0;
spl_download_error:	
	return -1;
}
static BOOL mfg_init(void)
{
	int retval = -1;
	char image[256];
	printf("mfg_init..\n");
	
	GetWindowText(hwndStaticBrowser,image,256);
	
	image_length = get_file_len(image);
	if(image_length <= 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"%s not exsit.",image);
		snprintf(error_msg,ERROR_INFO_MAX,"%s not exsit.",image);
		return FALSE;
	}
	
	retval = burnMFGinit(image);

	if(retval != 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"MFG init error");
		snprintf(error_msg,ERROR_INFO_MAX,"MFG init error,Error code is %s.",get_error_info(retval));
		return FALSE;
	}
	return TRUE;
}
static int mfg_download(void)
{	
	int retval = -1;
	printf("mfg_download..\n");

	SetDynamicInfo("MFG init");
	BOOL ret = mfg_init();
	StopDynamicInfo();
	if(FALSE == ret)
	{			
		goto mfg_download_error;
	}
	SetWindowText(hwndInfo,"Waiting for MFG download..");
	//if(burn_mode == SELECT_MFG_PROGRAMMING)
		transfer_start();
	retval = burnMFG();
	transfer_complete();
	if(retval != 0)
	{
		snprintf(error_info,ERROR_INFO_MAX,"MFG download error.");
		snprintf(error_msg,ERROR_INFO_MAX,"MFG download error,Error code is %s.",get_error_info(retval));		
		goto mfg_download_error;
	}
		
	SetDynamicInfo("Waiting for SPL device");
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
	return -1;
}

static void linux_download_complete_cb(int retval,void *private_data)
{
	if(retval == 0)
	{
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
	char image[256];
	if(GetFileName(image,256) == TRUE)
	{			
		SetWindowText(hwndStaticBrowser,image);
		if(get_image_info(image) == FALSE)
		{
			EnableWindow(hwndBtnDown,FALSE);
			SendMessage(hwndMain,WM_ERROR,-1,0);
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
		//ERROR_MESSAGE("EnableTelnet failed! error code is %s.",get_error_info(retval));
	}
	else
		SetWindowText(hwndInfo,"EnableTelnet Success.");
	update_ui_resources(TRUE);
	/*restore LinBtnDown's status*/
	EnableWindow(hwndLinBtnDown,!(info_for_btn_down.dwStyle & WS_DISABLED));
	
	return 0;
}

static int OnBtnFileDownload(void)
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
		return 0 ;
	}
	GetWindowText(hwndFdStaticSrcFile,file_for_pc,MAX_PATH);
	get_abs_file_name(file_for_target,file_for_pc);
	printf("file_for_target = %s\n",file_for_target);
	printf("file_for_pc = %s\n",file_for_pc);
	/*Disable Main Window and prepare to download*/
	EnableWindow(hwndTab,FALSE);
	SetWindowText(hwndFdNotify,"Init network..");
	if(linux_init(ini_file_info.ip[0]) == FALSE)
	{
		EnableWindow(hwndTab,TRUE);
		SetWindowText(hwndFdNotify,"Init network error.");
		return 0;
	}
	SetWindowText(hwndFdNotify,"Init network success.");
	
	SetWindowText(hwndFdNotify,"Download...");
#ifdef U3_LIB
	retval = file_download(ini_file_info.ip[0],file_for_pc,file_for_target);
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
	return 0;
}

static int OnBtnFileUpload(void)
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
		return 0;
	}
	printf("file_for_target = %s\n",file_for_target);
	printf("file_for_pc = %s\n",file_for_pc);
	/*Disable Main Window and prepare to upload*/
	
	GetWindowText(hwndFuStaticDestFile,file_for_pc,MAX_PATH);
	
	EnableWindow(hwndTab,FALSE);
	SetWindowText(hwndFuNotify,"Init network..");
	if(linux_init(ini_file_info.ip[0]) == FALSE)
	{
		EnableWindow(hwndTab,TRUE);
		SetWindowText(hwndFuNotify,"Init network error.");
		return 0;
	}
	SetWindowText(hwndFuNotify,"Upload...");
#ifdef U3_LIB
	retval = file_upload(ini_file_info.ip[0],file_for_pc,file_for_target);
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
	/* if(g_processing == FALSE)
	{
		CLEAR_INFO();
	} */
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
	if(hwnd == hwndLinBtnBrowser)
	{
		return OnBtnBrowser();		
	}
	else if(hwnd == hwndLinBtnDown){
#ifndef MAINTAINMENT
		int i;
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}

		if(partition_selected == 0)
		{
			ERROR_MESSAGE("No partition select.");
			return TRUE;
		}
#endif
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,-1,0);
			return TRUE;
		}
		g_background_func = linux_download;
		g_complete_func = linux_download_complete_cb;
		g_processing = TRUE;
		update_ui_resources(FALSE);
	}
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
	/* if(g_processing == FALSE)
	{
		CLEAR_INFO();
	} */
	if(hwnd == hwndSPLBtnDown)
	{
		int i;
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}
		if(partition_selected == 0)
		{
			ERROR_MESSAGE("No partition select.");
			return TRUE;
		}
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,-1,0);
			return TRUE;
		}
		update_ui_resources(FALSE);
		listening_on = IS_LISTENING_ON_SPL;
		SetDynamicInfo("Waiting for SPL device");		
	}
	else if(hwnd == hwndSPLBtnBrowser)
	{
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
	if(hwnd == hwndMFGBtnDown)
	{
		
#ifdef DEVELOPMENT
		int i;
		partition_selected = 0;
		for(i = 0; i < total_partition;++i)
		{
			partition_selected |= (Button_GetCheck(hwndPartitionCheckBox[i]) == BST_CHECKED)?(1<<i):0;
		}
		if(partition_selected == 0)
		{
			ERROR_MESSAGE("No partition select.");
			return TRUE;
		}		
#endif
		if(check_ini() == FALSE)
		{
			SendMessage(hwndMain,WM_ERROR,-1,0);
			return TRUE;
		}
		update_ui_resources(FALSE);
		if(CHECKBOX_IS_CLICK(hwndCheckBoxBatch))
		{
			g_on_batch = TRUE;							
		}
		else
			g_on_batch = FALSE;
		listening_on = IS_LISTENING_ON_MFG;
		SetDynamicInfo("Waiting for MFG device");
#ifdef PRODUCTION
		ShowWindow(hwndMFGBtnDown,FALSE);		
		EnableWindow(hwndMFGBtnStop,TRUE);
		ShowWindow(hwndMFGBtnStop,TRUE);
#endif		
	}
	else if(hwnd == hwndMFGBtnBrowser)
	{
		return OnBtnBrowser();		
	}
	else if(hwnd == hwndMFGBtnStop)
	{		
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
    int retval = 0;
    HWND child;

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
				static int count = 0;
				++count;
				count %= 6;
				int i;
				for(i = 0; i < count; ++i)
				{
					text[strlen(dynamic_info) + i] = '.';
				}
				text[strlen(dynamic_info) + i] = 0;
				SetWindowText(hwndDynamic,text);
			}
			break;
		}
        return TRUE;
		case WM_CLOSE:
		return (g_processing == TRUE) ? 0 : DefWindowProc(hwnd, message, wParam, lParam);
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
					StopDynamicInfo();
					listening_on = IS_LISTENING_ON_NOTHING;
					g_background_func = spl_download;
					g_complete_func = spl_download_complete_cb;
					g_processing = TRUE;
					WAKE_THREAD_UP();
					
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
					WAKE_THREAD_UP();					
				}
				else if(!memcmp(&insert_dev,&GUID_DEVCLASS_AD6900_LAN,sizeof(GUID)))
				{
					//lan device insert	
					
				}
			}				
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
		
		case WM_ERROR:
		if(wParam != -1)//appending error code string
		{
			snprintf(error_msg+strlen(error_msg),ERROR_INFO_MAX-strlen(error_msg)," Error code is: %s",get_error_info(wParam));
		}		
		if(error_info[0] != 0)
			SetWindowText(hwndInfo,error_info);
		if(error_msg[0] != 0)
			ERROR_MESSAGE(error_msg);
		error_info[0] = 0;
		error_msg[0] = 0;
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
		MessageBox(NULL,TEXT("DLL lost"),szAppName,MB_ICONERROR);
		return 0;
	}

	if(_access("sys",0))
	{
		MessageBox(NULL,TEXT("sys lost"),szAppName,MB_ICONERROR);
		return 0;
	}
#endif
	//Initialize net
	WSADATA wsaData;
    int retval;
	retval = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (retval != 0)
    {
        printf("%s_%d: WSAStartup() error, error code is %d\n", __FILE__, __LINE__, WSAGetLastError());
        return 0;
    }
	open_debug_log();
    g_event = CreateEvent(NULL, TRUE, FALSE, NULL); // ManualReset
    if (StartThread() == FALSE)
    {
        MessageBox(NULL, TEXT ("Create thread error"), szAppName, MB_ICONERROR);
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
	reset_general_handler(SELECT_LINUX_PROGRAMMING);
#elif defined(MAINTAINMENT)
	InitLinuxWindow();
	reset_general_handler(SELECT_LINUX_PROGRAMMING);	
#elif defined(PRODUCTION)
	InitMFGWindow();
	reset_general_handler(SELECT_MFG_PROGRAMMING);
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
	
	unregister_notifyer();
	
    CloseHandle(g_event);   

    /* Receive the WM_QUIT message, release mutex and return the exit code to the system */
    CloseHandle(hMutex);
    ExitDebugConsole();

    return msg.wParam;
}
