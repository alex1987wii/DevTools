#include "error_code.h"

unsigned short error_code = OK;	
	
struct _error_code_info error_code_info[] = 
{
	{0xFC18, "ERROR_CODE_OPEN_FILE_ERROR"},
    {0xFC19, "ERROR_CODE_FREAD_FILE_ERROR"},
    {0xFC1A, "ERROR_CODE_FWRITE_FILE_ERROR"},
    {0xFC1B, "ERROR_CODE_MALLOC_ERROR"},
    {0xFC1C, "ERROR_CODE_FT_CMD_ERROR"},
    {0xFC1D, "ERROR_CODE_FT_CMD_NOT_SUPPORT"},
    {0xFC1E, "ERROR_CODE_TRANSFER_TYPE_NOT_SUPPORT"},
    {0xFC1F, "ERROR_CODE_TRANSFER_TYPE_ERROR"},
    {0xFC20, "ERROR_CODE_RETRY_WEAR_OUT"},
    {0xFC21, "ERROR_CODE_WINUSB_INIT_ERROR"},
    {0xFC22, "ERROR_CODE_NETWORK_INIT_ERROR"},
    {0xFC23, "ERROR_CODE_NETWORK_CONNECT_ERROR"},
    {0xFC24, "ERROR_CODE_BUFFER_SIZE_NOT_ENOUGH"},
    {0xFC25, "ERROR_CODE_HW_TYPE_ERROR"},
    {0xFC26, "ERROR_CODE_PHYADDR_ERROR"},
    {0xFC27, "ERROR_CODE_PHYADDR_OUT_OF_RANGE"},
    {0xFC28, "ERROR_CODE_BLOCK_ALIGN_ERROR"},
    {0xFC29, "ERROR_CODE_PAGE_ALIGN_ERROR"},
    {0xFC2A, "ERROR_CODE_SIZE_OUT_OF_RANGE"},
    {0xFC2B, "ERROR_CODE_TOO_MUCH_BAD_BLOCK"},
    {0xFC2C, "ERROR_CODE_NFC_WRITE_PAGE_ERROR"},
    {0xFC2D, "ERROR_CODE_CRC_ERROR"},
    {0xFC2E, "ERROR_CODE_IOCTL_MEMGETBADBLOCK_ERROR"},
    {0xFC2F, "ERROR_CODE_IOCTL_MEMERASE_ERROR"},
    {0xFC30, "ERROR_CODE_IOCTL_MEMWRITEOOB_ERROR"},
    {0xFC31, "ERROR_CODE_IOCTL_MEMREADOOB_ERROR"},
    {0xFC32, "ERROR_CODE_IOCTL_MEMGETINFO_ERROR"},
    {0xFC33, "ERROR_CODE_IOCTL_MTDFILEMODE_ERROR"},
    {0xFC34, "ERROR_CODE_PWRITE_ERROR"},
    {0xFC35, "ERROR_CODE_PREAD_ERROR"},
    {0xFC36, "ERROR_CODE_OPEN_MTD_ERROR"},
    {0xFC37, "ERROR_CODE_TRANSFER_LEN_TOO_LARGE"},
    {0xFC38, "ERROR_CODE_IMAGE_NOT_CORRECT"},
    {0xFC39, "ERROR_CODE_IOCTL_NANDSETINECC"},
    {0xFC3A, "ERROR_CODE_FT_CMD_VERSION_ERROR"},
    {0xFC3B, "ERROR_CODE_IOCTL_NANDWRITEPAGE"},
    {0xFC3C, "ERROR_CODE_RUNNING_MODE_UNKNOW"},
    {0xFC3D, "ERROR_CODE_NOT_SUPPORT_IN_THIS_MODE"},
    {0xFC3E, "ERROR_CODE_DATA_VERIFY_NOT_PASSED"},
    {0xFC3F, "ERROR_CODE_DATA_VERIFY_MEMORY_ALLOC_ERROR"},
    {0xFC40, "ERROR_CODE_DATA_VERIFY_TOO_MANY_BAD_BLOCK"},
    {0xFC41, "ERROR_CODE_FT_VER_ERROR"},
    {0xFC42, "ERROR_CODE_PARTITION_INDEX_NOT_SUPPORT"},
    {0xFC43, "ERROR_CODE_CALCULATE_CRC_ERROR"},
    {0xFC44, "ERROR_CODE_FT_WANTED_NOT_MATCH"},
    {0xFC45, "ERROR_CODE_FT_UNKNOW_ERROR"},
    {0xFC46, "ERROR_CODE_FILE_NOT_EXIST"},
    {0xFC47, "ERROR_CODE_WORKING_THREAD_IS_BUSY"},
    {0xFC48, "ERROR_CODE_MALLOC_FILENAME_ERROR"},
    {0xFC49, "ERROR_CODE_SEQ_NUMBER_NOT_MATCH"},
    {0xFC4A, "ERROR_CODE_UNKNOW_ASSIGNMENT_JOB"},
    {0xFC4B, "ERROR_CODE_NOT_ENOUGH_SPACES"},
    {0xFC4C, "ERROR_CODE_WANTED_FILE_CAN_NOT_BE_SAVED"},
    {0xFC4D, "ERROR_CODE_WANTED_OPERATION_IS_NOT_SUPPORT"},
    {0xFC4E, "ERROR_CODE_FORK_CHILD_ERROR"},
    {0xFC4F, "ERROR_CODE_WAIT_CHILD_PID_ERROR"},
    {0xFC50, "ERROR_CODE_DLOPEN_ERROR"},
    {0xFC51, "ERROR_CODE_DLSYM_ERROR"},
    {0xFC52, "ERROR_CODE_CALCULATE_HEADER_CRC_ERROR"},
    {0xFC53, "ERROR_CODE_IMAGE_IS_NOT_FOR_THIS_TARGET_BOARD"},
    {0xFC54, "ERROR_CODE_PARTITION_IS_PROTECTED"},
    {0xFC55, "ERROR_CODE_JOB_THREAD_CREATE_ERROR"},
    {0xFC56, "ERROR_CODE_IS_NOT_IN_UPGRADE_MODE"},
    {0xFC57, "ERROR_CODE_IS_NOT_A_DIR"},
    {0xFC58, "ERROR_CODE_IOCTL_NANDFLAHSRESET_ERROR"},
    {0xFC59, "ERROR_CODE_BATTERY_VOLTAGE_IS_NOT_SATISFIED"},
    {0xFC5A, "ERROR_CODE_REMOUNT_UPDATE_PAR_AS_RO_ERROR"},
    {0xFC5B, "ERROR_CODE_REMOUNT_UPDATE_PAR_AS_RW_ERROR"},
    {0xFC5C, "ERROR_CODE_CAN_NOT_FOUND_A_SAMBA_DEVICE"},
    {0xFC5D, "ERROR_CODE_THE_SAMBA_DEVICE_IS_OCCUPIED_BY_OTHERS"},
    {0xFC5E, "ERROR_CODE_SERIAL_PORT_OPERATES_ERROR"},
    {0xFC5F, "ERROR_CODE_RECV_FROM_SERIAL_PORT"},
    {0xFC60, "ERROR_CODE_SEND_TO_SERIAL_PORT"},
    {0xFC61, "ERROR_CODE_GET_SERIAL_PORT_OVERLAPPEDRESULT"},
    {0xFC62, "ERROR_CODE_MBR_IS_NOT_VALID"},
    {0xFC63, "ERROR_CODE_PREPARE_GLOBAL_EVENTS"},
    {0xFC64, "ERROR_CODE_PARTITION_INDEX_NOT_CORRECT"},
    {0xFC65, "ERROR_CODE_PARTITION_MOUNT_FAILED"},
    {0xFC66, "ERROR_CODE_PARTITION_UMOUNT_FAILED"},
    {0xFC67, "ERROR_CODE_GET_PARTITION_USAGE_FAILED"},
    {0xFC68, "ERROR_CODE_EXEC_SYSTEM_CMD_FAILED"},
    {0xFC69, "ERROR_CODE_PARTITION_INFO_INVALID"},
    {0xFC6A, "ERROR_CODE_PARSE_PARTITION_INFO_FAILED"},
    {0xFC6B, "ERROR_CODE_PARTITION_AND_IMAGE_NOT_MATCH"},
    {0xFC6C, "ERROR_CODE_IMAGE_IS_INVALID"},
    {0xFC6D, "ERROR_CODE_FSEEK_ERROR"},
    {0xFC6E, "ERROR_CODE_DISK_IS_NOT_FDISK_AND_FORMAT"},

    {0xFDA8, "EC_INI_FILE_LOST"},
    {0xFDA9, "EC_FAILED_OPEN_NAND_IMG"},
    {0xFDAA, "EC_NAND_IMG_ILLIGAL"},
    {0xFDAB, "EC_DEVICE_DOES_NOT_MATCH_IMG"},
    {0xFDAC, "EC_REBOOT_TG_TO_UPG_MODE_FAILED"},
    {0xFDAD, "EC_CONNECT_UPG_MODE_FAILED"},
    {0xFDAE, "EC_CREATE_DEVICE_FILE_FAILED"},
    {0xFDAF, "EC_GET_FILES_FROM_TG"},
    {0xFDB0, "EC_UMOUNT_FAILED"},
    {0xFDB1, "EC_DUMP_FILE_LOST"},
    {0xFDB2, "EC_NO_DEVICE_WAS_FOUND"},
    {0xFDB3, "EC_FT_INIT_FAILED"},
    {0xFDB4, "EC_MFG_WAIT_ROM_CANCEL"},
    {0xFDB5, "EC_MFG_WAIT_SPL_TIMEOUT"},
    {0xFDB6, "EC_MFG_WAIT_RESCUE_LINUX_TIMEOUT"},
    {0xFDB7, "EC_FIND_PART_FROM_IMG"},
    {0xFDB8, "EC_OPEN_RESCUE_IMG_ERROR"},
    {0xFDB9, "EC_CHECK_TARGET_SW_VERSION"},
    {0xFDBA, "EC_RUN_SYS_SHELL"},
    {0xFDBB, "EC_MFG_WAIT_UPG_LINUX_TIMEOUT"},
    {0xFDBC, "EC_UTILITY_INTERNAL_ERROR"},
    {0xFDBD, "EC_INS_COMPATIBILITY_ERROR"},
    {0xFDBE, "EC_UTILITY_COMPATIBILITY_ERROR"},
    {0xFDBF, "EC_RESET_TARGET_ERROR"},
    {0xFDC0, "EC_FDISK_AND_FORMAT_EMMC_ERROR"},
    {0xFDC1, "EC_TRANSFER_FILE_ERROR"},
    {0xFDC2, "EC_EXECUTE_UPDATE_CMD_ERROR"},

    {0xFFC4, "ERROR_CODE_USBI_STATUS_PENDING"},
    {0xFFC5, "ERROR_CODE_USBI_STATUS_PARAM"},
    {0xFFC6, "ERROR_CODE_USBI_STATUS_BUSY"},
    {0xFFC7, "ERROR_CODE_USBI_STATUS_NOMEM"},
    {0xFFC8, "ERROR_CODE_USBI_STATUS_TIMEOUT"},
    {0xFFC9, "ERROR_CODE_USBI_STATUS_NODEV"},
    {0xFFCA, "ERROR_CODE_USBI_STATUS_STATE"},
    {0xFFCB, "ERROR_CODE_USBI_STATUS_NOT_SUPPORTED"},
    {0xFFCC, "ERROR_CODE_USBI_STATUS_UNKNOWN"},

    {0xFFD8, "ERROR_CODE_RETRY_AGAIN"},
    {0xFFD9, "ERROR_CODE_WSASTARTUP"},
    {0xFFDA, "ERROR_CODE_SOCKET"},
    {0xFFDB, "ERROR_CODE_SETSOCKOPT"},
    {0xFFDC, "ERROR_CODE_BIND"},
    {0xFFDD, "ERROR_CODE_LISTEN"},
    {0xFFDE, "ERROR_CODE_ACCEPT"},
    {0xFFDF, "ERROR_CODE_SELECT"},
    {0xFFE0, "ERROR_CODE_INVALID_SOCKET"},
    {0xFFE1, "ERROR_CODE_RECV_ERROR"},
    {0xFFE2, "ERROR_CODE_RECV_TIMEOUT"},
    {0xFFE3, "ERROR_CODE_SOCKET_CLOSE"},
    {0xFFE4, "ERROR_CODE_SEND_ERROR"},
    {0xFFE5, "ERROR_CODE_RECV_EXCEPTION"},
    {0xFFE6, "ERROR_CODE_SOCKET_IS_NOT_CONNECTED"},

    {0xFFED, "ERROR_USB_WRITE_TIMEOUT"},
    {0xFFEE, "ERROR_USB_RETRY_WEAR_OUT"},
    {0xFFEF, "ERROR_USB_INTERRUPT_BY_CMD"},
    {0xFFF0, "ERROR_USB_CABLE_UNPLUG"},

    {0xFFF6, "ERROR_CHILD_MALLOC_FILENAME_ERROR"},
    {0xFFF7, "ERROR_CHILD_EXECVP_CMD_ERROR"},
    {0xFFF8, "ERROR_CHILD_EXECVP_CMD_ERROR_V1"}     
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
	{EC_IPADDR_CONFLICT,"IP address conflict with default usb/intranet ip address.", "IP address invalid."},
	{EC_NOENT, "No such file or directory.", "No such file or directory."},
	{EC_NOT_IMPL, "Not implement yet.", "Not implement yet."},
};

const char *get_error_info(unsigned short ec)
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
const char *get_short_info(unsigned short ec)
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
            return (char *)error_code_info[i].string;
        }
    }
	/*not found*/
    return ec_not_found;
}

