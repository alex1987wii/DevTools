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

int progress_reply_status_get (char *index, unsigned char *percent, unsigned short *status );

int exec_file_in_tg(const char *TG_ip, char* filepath);

/*beta*/
int execute_cmd_in_tg(const char *TG_ip, char *cmd);
#ifdef __cplusplus

}
#endif
#endif