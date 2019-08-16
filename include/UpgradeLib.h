#ifndef _UPGRADE_LIB_H_
#define _UPGRADE_LIB_H_
#ifdef __cplusplus
extern "C"
{
#endif

struct target_progress_replay
{    
	char           index   ;
	unsigned char  percent ;
	unsigned short status  ;
	unsigned int   retval  ;
} target_progress_info;

int open_debug_log();

int burnMFG(char *rescue_image_name);

int burnSPL(char *rescue_image_name);

int WinUpgradeLibInit(char *nand_image_name, unsigned long ImageLen, const char *TG_ip, unsigned char battery_check_sign);

int burnImage();

int burnpartition(int parts_selected);

// index = 1-ipl; 2-linux; 3-calibration; 4-rootfs; 5-ap root; 6-def; 7-ota; 8-run; 9-log; 10-userdata
// 0x33 SPL_BURNING; 0x34 MFG_BURNING

int progress_reply_status_get(char *index, unsigned char *percent, unsigned short *status );

/*
//File operation API
 int download_file(const char *TG_ip, char *read_file_from_pc, char *save_file_to_target);
 int upload_file(const char *TG_ip, char *write_file_to_pc, char *read_file_from_target);
 int exec_file_in_tg(const char *TG_ip, char* filepath);
 int execute_cmd_in_tg(const char *TG_ip, char *cmd);
*/
#ifdef __cplusplus

}
#endif
#endif