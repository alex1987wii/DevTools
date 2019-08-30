/*
    This file claim API from upgrade tools PC library for U3 series products.
    UPGRADE_LIB_VERSION 004
    Joyce 2019/8/29
*/

#ifndef _UPGRADE_LIB_H_
#define _UPGRADE_LIB_H_
#ifdef __cplusplus
extern "C"
{
#endif
int open_debug_log();
/* Function: check the image, and found where spl located.
 *
 */
int burnMFGinit(char *nand_image_name);
	
/* Function: proceed spl transfer via rom code. 
 * NOTICE: 
 *		1. should call burnMFGinit first, then this api.
 *		2. which should be call immediately when MFG target insert.
 */
int burnMFG();

int burnSPL( char *rescue_image_name );

/* Function: init operation and config for linux burning. 
 * Parameters check_tg_or_not_flag:
 *		0. check target before linux update
 *		0x1. skip target db check before linux update
 */
int WinUpgradeLibInit(char *nand_image_name, unsigned long ImageLen, const char *TG_ip, unsigned char check_tg_or_not_flag);
int burnImage();
int burnpartition(int parts_selected);

/* index = 0 ipl; 1 spl; 2 linux; 3calibration; 4rootfs; 5 userdata; 0x33 SPL_BURNING; 0xCC MFG_BURNING
*  status = 
*/
int progress_reply_status_get (char *index, unsigned char *percent, unsigned short *status );

int file_upload(const char *TG_ip, char *write_file_to_pc, char *read_file_from_target);
int file_download(const char *TG_ip, char *read_file_from_pc, const char *save_file_to_target);
int exec_file_in_tg(const char *TG_ip, const char *special_file);

/*beta*/
int execute_cmd_in_tg(const char *TG_ip, char *cmd);
#ifdef __cplusplus
}
#endif
#endif