/*
 *  Name:  devToolsUI_private.h  
 *
 *  Purpose: Debug utility for Unication console application
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
 *  Copyright (c) $Date: 2015/08/14 06:29:02 $ Unication Co., Ltd.
 *
 *  All rights reserved
 */
#ifndef _DEVTOOlSUI_PRIVATE_H_
#define _DEVTOOlSUI_PRIVATE_H_

#define WM_THREADS_WILL_BE_EXITED (WM_USER + 100)
#define WM_POPBAR_HAD_DESTORIED   (WM_USER + 101)
#define WM_INVOKE_CALLBACK        (WM_USER + 102)
#define WM_REFRESH_FINISHED       (WM_USER + 103)

#define WAIT_FOR_REBOOT_RETRY     (15)

/* what UDT working on */
enum _STATUS
{
    STATUS_START = 0,
    STATUS_DONE,                 
    STATUS_ERROR,              
                              
    STATUS_START_MFG_DOWNLOAD,
    STATUS_MFG_DOWNLOADEDING,
    STATUS_MFG_DOWNLOADED, 

    STATUS_START_MFG_UPLOAD,
    STATUS_MFG_UPLOADEDING,  
    STATUS_MFG_UPLOADED,   
                              
    STATUS_START_PAR_DOWNLOAD,
    STATUS_PAR_DOWNLOADEDING,
    STATUS_PAR_DOWNLOADED, 

    STATUS_START_PAR_UPLOAD,
    STATUS_PAR_UPLOADEDING,
    STATUS_PAR_UPLOADED,   
                              
    STATUS_START_RAM_DOWNLOAD,
    STATUS_RAM_DOWNLOADEDING,
    STATUS_RAM_DOWNLOADED,

    STATUS_START_RAM_UPLOAD,
    STATUS_RAM_UPLOADEDING,
    STATUS_RAM_UPLOADED,

    STATUS_WAITTING_DEV_REBOOT,
    STATUS_REFRESH_FINISHED,

    STATUS_WAIT_SPL_TIMEOUT,
    STATUS_PAR_DOWNLOADEDING_RETRY,
};

#define TIMER_WAITING_FOR_REBOOT  (1)
#define TIMER_COUNTING_INTERFACES (2)
#define TIMER_SCANNING_DEVS       (3)
#define TIMER_TRANSFER_ERROR      (4)

#define TIMER_TIMEOUT             (1)

#define IS_LISTENING_ON_NOTHING   (0)
#define IS_LISTENING_ON_MFG       (1)
#define IS_LISTENING_ON_SPL       (2)

#define TOTAL_INTERFACES          (3)

typedef struct _nand_flash_programming
{
    int total_partitions;
    int current_partition;
    int next_partition;
    int interfaces;
    int current_interface_counter;
} nand_flash_programming_t;

#define MAX_IP_ADDRS              (4)
const char *ip_addrs_of_target[MAX_IP_ADDRS] = 
{
    "10.10.12.11",
    "10.10.12.21",
    "10.10.12.22",
    "10.10.0.12", /*  master must be the last one, 
                      for if we reset master, master 
                      would reset all slaves, and 
                      master must be searched by USB ip
                    */
};

const unsigned int ALL_DEVS_READY[MAX_IP_ADDRS+1] =
{
    0xFFFFFFFF, // not valid
    0x00000002, // 1 ready
    0x00000006, // 2 ready
    0x0000000E, // 3 ready
    0x0000001E, // 4 ready
//    0x0000003E, // 5 ready
//    0x0000007E, // 6 ready
//    0x000000FE, // 7 ready
//    0x000001FE, // 8 ready
};

struct _error_code_info
{
    unsigned short ec;
    const char* string;
};

const struct _error_code_info error_code_info[] = 
{
    {0xFC18, "ERROR_CODE_OPEN_FILE_ERROR"},                    // FC18    
    {0xFC19, "ERROR_CODE_FREAD_FILE_ERROR"},                   // FC19   
    {0xFC1A, "ERROR_CODE_FWRITE_FILE_ERROR"},                  // FC1A   
    {0xFC1B, "ERROR_CODE_MALLOC_ERROR"},                       // FC1B   
    {0xFC1C, "ERROR_CODE_FT_CMD_ERROR, try the latest winTools"}, // FC1C   
    {0xFC1D, "ERROR_CODE_FT_CMD_NOT_SUPPORT"},                 // FC1D   
    {0xFC1E, "ERROR_CODE_TRANSFER_TYPE_NOT_SUPPORT"},          // FC1E   
    {0xFC1F, "ERROR_CODE_TRANSFER_TYPE_ERROR"},                // FC1F   
    {0xFC20, "ERROR_CODE_RETRY_WEAR_OUT, try the latest winTools"}, // FC20   
    {0xFC21, "ERROR_CODE_WINUSB_INIT_ERROR"},                  // FC21   
    {0xFC22, "ERROR_CODE_NETWORK_INIT_ERROR"},                 // FC22   
    {0xFC23, "ERROR_CODE_NETWORK_CONNECT_ERROR"},              // FC23   
    {0xFC24, "ERROR_CODE_BUFFER_SIZE_NOT_ENOUGH"},             // FC24   
    {0xFC25, "ERROR_CODE_HW_TYPE_ERROR"},                      // FC25   
    {0xFC26, "ERROR_CODE_PHYADDR_ERROR"},                      // FC26   
    {0xFC27, "ERROR_CODE_PHYADDR_OUT_OF_RANGE"},               // FC27   
    {0xFC28, "ERROR_CODE_BLOCK_ALIGN_ERROR"},                  // FC28   
    {0xFC29, "ERROR_CODE_PAGE_ALIGN_ERROR"},                   // FC29   
    {0xFC2A, "ERROR_CODE_SIZE_OUT_OF_RANGE"},                  // FC2A   
    {0xFC2B, "ERROR_CODE_TOO_MUCH_BAD_BLOCK"},                 // FC2B   
    {0xFC2C, "ERROR_CODE_NFC_WRITE_PAGE_ERROR"},               // FC2C   
    {0xFC2D, "ERROR_CODE_CRC_ERROR"},                          // FC2D    
    {0xFC2E, "ERROR_CODE_IOCTL_MEMGETBADBLOCK_ERROR"},         // FC2E    
    {0xFC2F, "ERROR_CODE_IOCTL_MEMERASE_ERROR"},               // FC2F    
    {0xFC30, "ERROR_CODE_IOCTL_MEMWRITEOOB_ERROR"},            // FC30    
    {0xFC31, "ERROR_CODE_IOCTL_MEMREADOOB_ERROR"},             // FC31    
    {0xFC32, "ERROR_CODE_IOCTL_MEMGETINFO_ERROR"},             // FC32    
    {0xFC33, "ERROR_CODE_IOCTL_MTDFILEMODE_ERROR"},            // FC33    
    {0xFC34, "ERROR_CODE_PWRITE_ERROR"},                       // FC34    
    {0xFC35, "ERROR_CODE_PREAD_ERROR"},                        // FC35    
    {0xFC36, "ERROR_CODE_OPEN_MTD_ERROR"},                     // FC36   
    {0xFC37, "ERROR_CODE_TRANSFER_LEN_TOO_LARGE"},             // FC37    
    {0xFC38, "ERROR_CODE_IMAGE_NOT_CORRECT"},                  // FC38
    {0xFC39, "ERROR_CODE_IOCTL_NANDSETINECC"},                 // FC39
    {0xFC3A, "ERROR_CODE_FT_CMD_VERSION_ERROR"},               // FC3A
    {0xFC3B, "ERROR_CODE_IOCTL_NANDWRITEPAGE"},                // FC3B
    {0xFC3C, "ERROR_CODE_RUNNING_MODE_UNKNOW, retry again"},   // FC3C
    {0xFFC4, "ERROR_CODE_USBI_STATUS_PENDING"},                // FFC4                                
    {0xFFC5, "ERROR_CODE_USBI_STATUS_PARAM"},                  // FFC5      
    {0xFFC6, "ERROR_CODE_USBI_STATUS_BUSY"},                   // FFC6      
    {0xFFC7, "ERROR_CODE_USBI_STATUS_NOMEM"},                  // FFC7      
    {0xFFC8, "ERROR_CODE_USBI_STATUS_TIMEOUT"},                // FFC8      
    {0xFFC9, "ERROR_CODE_USBI_STATUS_NODEV"},                  // FFC9      
    {0xFFCA, "ERROR_CODE_USBI_STATUS_STATE"},                  // FFCA      
    {0xFFCB, "ERROR_CODE_USBI_STATUS_NOT_SUPPORTED"},          // FFCB                                        
    {0xFFCC, "ERROR_CODE_USBI_STATUS_UNKNOWN"},                // FFCC                        
    {0xFFD8, "ERROR_CODE_RETRY_AGAIN"},                        // FFD8      
    {0xFFD9, "ERROR_CODE_WSASTARTUP"},                         // FFD9     
    {0xFFDA, "ERROR_CODE_SOCKET"},                             // FFDA       
    {0xFFDB, "ERROR_CODE_SETSOCKOPT"},                         // FFDB          
    {0xFFDC, "ERROR_CODE_BIND"},                               // FFDC            
    {0xFFDD, "ERROR_CODE_LISTEN"},                             // FFDD     
    {0xFFDE, "ERROR_CODE_ACCEPT"},                             // FFDE     
    {0xFFDF, "ERROR_CODE_SELECT"},                             // FFDF     
    {0xFFE0, "ERROR_CODE_INVALID_SOCKET"},                     // FFE0     
    {0xFFE1, "ERROR_CODE_RECV_ERROR"},                         // FFE1     
    {0xFFE2, "ERROR_CODE_RECV_TIMEOUT"},                       // FFE2     
    {0xFFE3, "ERROR_CODE_SOCKET_CLOSE"},                       // FFE3     
    {0xFFE4, "ERROR_CODE_SEND_ERROR"},                         // FFE4     
    {0xFFED, "ERROR_USB_WRITE_TIMEOUT"},                       // FFED                                        
    {0xFFEE, "ERROR_USB_RETRY_WEAR_OUT"},                      // FFEE                                          
    {0xFFEF, "ERROR_USB_INTERRUPT_BY_CMD"},                    // FFEF                                        
    {0xFFF0, "ERROR_USB_CABLE_UNPLUG"},                         // FFF0                                       
     /* for USR tool {*/
    {0xFDA8, "EC_INI_FILE, please make sure the content of ini file is valid"},    // FDA8
    {0xFDA9, "EC_FAILED_OPEN_NAND_IMG, please make sure image exists"},            // FDA9
    {0xFDAA, "EC_NAND_IMG_ILLEGAL, please make sure image is valid"},              // FDA9
    {0xFDAB, "EC_DEVICE_DOES_NOT_MATCH_IMG, please make sure image is valid"},     // FDA9
    {0xFDAC, "EC_REBOOT_TG_TO_UPG_MODE_FAILED, please retry again"},               // FDA9
    {0xFDAD, "EC_CONNECT_UPG_MODE_FAILED, please retry again"},                    // FDA9
    {0xFDAE, "EC_CREATE_DEVICE_FILE_FAILED"},                                      // FDA9
    {0xFDAF, "EC_GET_FILES_FROM_TG"},                                              // FDAF
    {0xFDB0, "EC_UMOUNT_FAILED"},                                                  // FDB0
    {0xFDB1, "EC_DUMP_FILE_LOST"},                                                 // FDB1
    {0xFDB2, "EC_NO_DEVICE_WAS_FOUND"},                                            // FDB2
    {0xFDB3, "EC_FT_INIT_FAILED"},                                                 // FDB3
    {0xFDB4, "EC_MFG_WAIT_ROM_CANCEL"},                                            // FDB4
    {0xFDB5, "EC_MFG_WAIT_SPL_TIMEOUT"},                                           // FDB5
    {0xFDB6, "EC_MFG_WAIT_RESCUE_LINUX_TIMEOUT"},                                  // FDB6
    {0xFDB7, "EC_FIND_PART_FROM_IMG"},                                             // FDB7
    {0xFDB8, "EC_OPEN_RESCUE_IMG_ERROR"},                                          // FDB8
    {0xFDB9, "EC_CHECK_TARGET_SW_VERSION"},                                        // FDB9
    {0xFDBA, "EC_RUN_SYS_SHELL"},                                                  // FDBA
    /* for USR tool }*/
};
#endif

