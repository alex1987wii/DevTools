#ifndef _UPGRADE_LIB_H_
#define _UPGRADE_LIB_H_
#ifdef __cplusplus
extern "C"
{
#endif
int open_debug_log();
int burnMFGinit(char *nand_image_name);

int burnMFG();

int burnSPL(char *rescue_image_name);

int WinUpgradeLibInit(char *nand_image_name, unsigned long ImageLen, const char *TG_ip, unsigned char battery_check_sign);
int burnImage();
int burnpartition(int parts_selected);

/* index = 0 ipl; 1 spl; 2 linux; 3calibration; 4rootfs; 5def; 6ota; 7run; 8userdata; 
9log; 10
0x33 SPL_BURNING; 0xCC MFG_BURNING
*/
int progress_reply_status_get (char *index, unsigned char *percent, unsigned short *status );

//File operation API
int download_file(const char *TG_ip, char *read_file_from_pc, char *save_file_to_target);
int upload_file(const char *TG_ip, char *write_file_to_pc, char *read_file_from_target);
int exec_file_in_tg(const char *TG_ip, char* filepath);
int execute_cmd_in_tg(const char *TG_ip, char *cmd);

#ifdef __cplusplus

}
#endif
#endif