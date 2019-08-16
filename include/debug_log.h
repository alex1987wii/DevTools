#ifndef _DEBUG_LOG_H_
#define _DEBUG_LOG_H_

#include <windows.h>
#include "winlog.h"
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
#define log_print(fmt, ...)			log_info(fmt, ##__VA_ARGS__)
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
inline BOOL InitDebugConsole( void )
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
inline void ExitDebugConsole( void )
{
	FreeConsole();
	return;
}
#elif (DEBUG_MODE == DEBUG_LOG)
inline BOOL InitDebugConsole( void )
{   
	return openlog(LOGFILE, LOG_INFO, LOG_OVERLAP) == 0;
}
inline void ExitDebugConsole( void )
{	
	closelog();
}

#else

inline BOOL InitDebugConsole( void )
{   
    return TRUE;

}
inline void ExitDebugConsole( void )
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

#endif
