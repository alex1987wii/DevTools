#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <windowsx.h>
#include <Commctrl.h>
#include "uni_image_header.h"
#include "UpgradeLib.h"
#include "iniparser.h"
#include "error_code.h"
#include "winlog.h"

/*Notice: hosts and macros start with SEL_ must at that order*/
#define MAX_HOST	2
#define SEL_ALL 0
#define SEL_M1S2 1
#define SEL_M1S1 2
#define SEL_INTRANET 3

typedef struct _ini_file_info
{
	int ip_should_flash;
	DWORD dword_ip[4]; //used by br01 for store ip
    char ip[4][16]; // xxx.xxx.xxx.xxx
    char name_of_image[MAX_PATH];
	char name_of_rescue_image[MAX_PATH];
} ini_file_info_t;

extern ini_file_info_t ini_file_info;

typedef int (*tool_func_t)(void);	
struct function_t{
	TCHAR *name;
	TCHAR *desc;
	tool_func_t func;
};

extern struct function_t tool_set[];

int get_socid(const char *ip);
int DspDump(void);
int download_dsp_image(LPARAM target_ip, char *local_image);
int exec_script_in_target(const char *ip, const char *cmdlines);

BOOL parse_ini_file(ini_file_info_t *ini_struct, const char *file);


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

/*U5 have no these API yet*/
#define download_file(...)		0
#define exec_file_in_tg(...)	0
#define upload_file(...)		0
#define DWORD_to_ipstr(...) 0

#endif
