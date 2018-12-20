#ifndef _UPGRADE_LIB_H_
#define _UPGRADE_LIB_H_
#ifdef __cplusplus
extern "C"
{
#endif
int open_debug_log();
int burnMFGinit(char *nand_image);

int burnMFG();

int burnSPL( char *filename, unsigned long fileLen );

int WinUpgradeLibInit(char *filename, unsigned long ImageLen, const char *TG_ip, unsigned char battery_check_sign);
int burnImage();
int burnpartition(int parts_selected);

int progress_reply_status_get (char *index, unsigned char *percent, unsigned short *status );

int upload_file(char *buf, unsigned int* len_of_transfer, char *abs_filename_in_target);
int download_file(char *buf, unsigned int len_of_transfer, char *abs_filename_in_target);
int exec_file_in_tg(char* filepath);
#ifdef __cplusplus
}
#endif
#endif