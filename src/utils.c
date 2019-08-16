#include "utils.h"

ini_file_info_t ini_file_info;

struct function_t tool_set[] = {
	{ .name = "DspDump", 
	.desc = NULL,//"Dump DSP log from /userdata/dspdump and save it in ./dump as a tarball, then delete all files in /userdata/dspdump.", 
	.func = DspDump},
};

/*******************static function declaration***************************/
static BOOL parse_ip_list(ini_file_info_t *ini_struct, const char *ip_list);

int exec_script_in_target(const char *ip, const char *cmdlines)
{
	#define TMPFILE		"tmpfile.sh"
	int ret = 0;
	log_info("exec script in target %s -> %s", ip, cmdlines);
	FILE *fp = fopen("./"TMPFILE, "wb+");
	fwrite("#!/bin/sh\n", 1, strlen("#!/bin/sh\n"), fp);
	fwrite(cmdlines,1, strlen(cmdlines), fp);
	fclose(fp);	
	if(ret = download_file(ip,"./"TMPFILE, "/tmp/"TMPFILE))
		return ret;	
	remove("./"TMPFILE);
	if(ret = exec_file_in_tg(ip, "/bin/chmod a+x /tmp/"TMPFILE, 1 ))
		return ret;	
	return exec_file_in_tg(ip, "/tmp/"TMPFILE, 1);	
}

int get_socid(const char *ip)
{
	#define FILE_SOCID	"tmpfile.socid"
	int ret;	
	ret = upload_file(ip,  FILE_SOCID, "/sys/devices/system/cpu/cpu0/cpuid");		
	if((ret & 0xFFFF) == 0xFDAF)//bug, ignore CRC error
		ret = 0;
	
	if(ret)
		return ret;
	FILE *fp = fopen(FILE_SOCID, "rb");
	char socid;
	if( 1 != fread(&socid, 1, 1, fp))
		return -1;
	switch(socid){
		case '0':ret = SEL_M1S1;
		break;
		case '1':ret = SEL_M1S2;
		break;
		default:
		ret = -1;
	}
	fclose(fp);	
	remove(FILE_SOCID);
	return ret;
}

int download_dsp_image(LPARAM target_ip, char *local_image)
{
	int ret = 0;
	char target_ip_str[16];
	snprintf(target_ip_str, 16, "%d.%d.%d.%d",FIRST_IPADDRESS(target_ip), 
				SECOND_IPADDRESS(target_ip), THIRD_IPADDRESS(target_ip),
				FOURTH_IPADDRESS(target_ip));
	log_info("download DSP Image : ip = %s, DspImage = %s\n",target_ip_str, local_image);
	//unlock rootfs
	if(ret = exec_file_in_tg(target_ip_str,"/usr/bin/unlock /", 1))
		return ret;	
	if(ret = download_file(target_ip_str, local_image, TARGET_DSP_IMAGE))
		return ret;
	return exec_file_in_tg(target_ip_str, "/usr/bin/lock /", 1);
}
	
int DspDump(void)
{
	#define PC_DUMP_DIR		"./dump"
	#define DSPDUMP_DIR		"/userdata/dspdump"
	#define DSPDUMP_TBALL	"dspdump.tar.gz"
	int retval;
	char ipstr[16];
	DWORD ip;
	extern HWND hwndUtilsIpAddrDeviceIP;
	SendMessage(hwndUtilsIpAddrDeviceIP, IPM_GETADDRESS, 0 , (LPARAM)&ip);
	DWORD_to_ipstr(ip, ipstr, 16);	
	char script[256];
	snprintf(script, 256, "[ -d \""DSPDUMP_DIR"\" ] && tar -zhcf /tmp/"DSPDUMP_TBALL" -C /userdata dspdump || exit %d", EC_NOENT);
	//pack directory
	if(retval = exec_script_in_target(ipstr, script))
		return retval;
	//upload package
	//Sleep(1500);//wait for pack done
	//prepre 'PC_DUMP_DIR' directory
	CreateDirectory(PC_DUMP_DIR, NULL);
	if(retval = upload_file(ipstr, PC_DUMP_DIR"/"DSPDUMP_TBALL, "/tmp/"DSPDUMP_TBALL))
		return retval;
	//delete file in target
	if(retval = exec_script_in_target(ipstr, "[ -d \""DSPDUMP_DIR"\" ] && rm -rf /userdata/dspdump/*\n"))
		return retval;
	return 0;
}

static BOOL parse_ip_list(ini_file_info_t *ini_struct, const char *ip_list)
{
	char buff[256],*ip;	
	int ip_cnt = 0;	
	if(strlen(ip_list) > 256)//only support 256 chars for ip addresses
		return FALSE;
	strcpy(buff,ip_list);
	ip = strtok(buff," \t");	
	while(ip)
	{
		if(strlen(ip) > 16)
			return FALSE;
		strcpy(ini_struct->ip[ip_cnt++],ip);		
		ip = strtok(NULL," \t");
	}
	if(ip_cnt > 4) //max 4 ip addresses
		return FALSE;
	ini_struct->ip_should_flash = ip_cnt;
	return TRUE;
}

BOOL parse_ini_file(ini_file_info_t *ini_struct, const char *file)
{	
	const char *ip_list,*image,*rescue_image;
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
		if(parse_ip_list(ini_struct, ip_list) == FALSE)
			goto EXIT;
	if(image && strlen(image) < MAX_PATH)
		strcpy(ini_struct->name_of_image,image);
	if(rescue_image && strlen(rescue_image) < MAX_PATH)
		strcpy(ini_struct->name_of_rescue_image,rescue_image);
	iniparser_freedict(ini_config);
	return TRUE;
EXIT:
	iniparser_freedict(ini_config);
	return FALSE;	
}
