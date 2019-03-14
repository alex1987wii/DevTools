/*
 *  Name:  devToolsUI_private.h  
 *
 *  Purpose: Debug utility for Unication console application
 *
 *  For AD6900 Project ONLY.
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
 *  Copyright (c) $Date: 2016/07/28 06:43:26 $ Unication Co., Ltd.
 *
 *  All rights reserved
 */
#ifndef _DEVTOOlSUI_PRIVATE_H_
#define _DEVTOOlSUI_PRIVATE_H_


#define IS_LISTENING_ON_NOTHING   (0)
#define IS_LISTENING_ON_MFG       (1)
#define IS_LISTENING_ON_SPL       (2)

struct _error_code_info
{
    unsigned short ec;
    const char* string;
};

/*const*/ struct _error_code_info error_code_info[] = 
{	
    {0xFC18, "ERROR_CODE_OPEN_FILE_ERROR, There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package and try again, or contact your support personnel for assistance.."},                    // FC18    
    {0xFC1C, "ERROR_CODE_FT_CMD_ERROR, There is a problem with this update package. The installer may not match your target. Please verify the installer and try again."}, // FC1C  
    {0xFC1D, "ERROR_CODE_FT_CMD_NOT_SUPPORT, There is a problem with this update package. The installer may not match your target. Please verify the installer and try again."},                 // FC1D   
    {0xFC1E, "ERROR_CODE_TRANSFER_TYPE_NOT_SUPPORT, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},          // FC1E   
    {0xFC1F, "ERROR_CODE_TRANSFER_TYPE_ERROR, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                // FC1F   
    {0xFC20, "ERROR_CODE_RETRY_WEAR_OUT, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."}, // FC20
    {0xFC21, "ERROR_CODE_WINUSB_INIT_ERROR, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                  // FC21   
    {0xFC22, "ERROR_CODE_NETWORK_INIT_ERROR, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                 // FC22   
    {0xFC23, "ERROR_CODE_NETWORK_CONNECT_ERROR, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},              // FC23   
    {0xFC24, "ERROR_CODE_BUFFER_SIZE_NOT_ENOUGH, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},             // FC24   
    {0xFC25, "ERROR_CODE_HW_TYPE_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                      // FC25   
    
    {0xFC2D, "ERROR_CODE_CRC_ERROR, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                          // FC2D    
    {0xFC2E, "ERROR_CODE_IOCTL_MEMGETBADBLOCK_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},         // FC2E    
    
    {0xFC34, "ERROR_CODE_PWRITE_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                       // FC34    
    {0xFC35, "ERROR_CODE_PREAD_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                        // FC35    
    {0xFC36, "ERROR_CODE_OPEN_MTD_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                     // FC36   
    
    {0xFC38, "ERROR_CODE_IMAGE_NOT_CORRECT, There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance."},                  // FC38
    {0xFC39, "ERROR_CODE_IOCTL_NANDSETINECC, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                 // FC39
    {0xFC3A, "ERROR_CODE_FT_CMD_VERSION_ERROR, There is a problem with this update package. The installer may not match your target. Please verify the installer and try again."},               // FC3A
    {0xFC3B, "ERROR_CODE_IOCTL_NANDWRITEPAGE, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                // FC3B
    {0xFC3C, "ERROR_CODE_RUNNING_MODE_UNKNOW, retry again"},   // FC3C
    
    {0xFC3E, "ERROR_CODE_DATA_VERIFY_NOT_PASSED, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},             // FC3E
    {0xFC3F, "ERROR_CODE_DATA_VERIFY_MEMORY_ALLOC_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},     // FC3F
    {0xFC40, "ERROR_CODE_DATA_VERIFY_TOO_MANY_BAD_BLOCK, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},     // FC40
    {0xFC41, "ERROR_CODE_FT_VER_ERROR, There is a problem with this update package. The installer may not match your target. Please verify the installer and try again."},                       // FC41   
    
    {0xFC43, "ERROR_CODE_CALCULATE_CRC_ERROR, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                // FC43
    {0xFC44, "ERROR_CODE_FT_WANTED_NOT_MATCH, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                // FC44
    {0xFC45, "ERROR_CODE_FT_UNKNOW_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                    // FC45
    {0xFC46, "ERROR_CODE_FILE_NOT_EXIST, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                     // FC46
    
    {0xFC4C, "ERROR_CODE_WANTED_FILE_CAN_NOT_BE_SAVED, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},       // FC4C
    {0xFC4D, "ERROR_CODE_WANTED_OPERATION_IS_NOT_SUPPORT, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},    // FC4D
    
    {0xFC52, "ERROR_CODE_CALCULATE_HEADER_CRC_ERROR, There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance."},         // FC52
    {0xFC53, "ERROR_CODE_IMAGE_IS_NOT_FOR_THIS_TARGET_BOARD, There is a problem with this update package. The image may not match your target. Please verify the image and try again."}, // FC53
    {0xFC54, "ERROR_CODE_PARTITION_IS_PROTECTED, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},             // FC54
    
    {0xFC58, "ERROR_CODE_IOCTL_NANDFLAHSRESET_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},         // FC58
    {0xFC59, "ERROR_CODE_BATTERY_VOLTAGE_IS_NOT_SATISFIED, Your battery is too low to update your target. Please charge target then try again."},   // FC59
    {0xFC5A, "ERROR_CODE_REMOUNT_UPDATE_PAR_AS_RO_ERROR, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},     // FC5A
    
    /* for USR tool {*/
    {0xFDA8, "EC_INI_FILE, There is a problem with this update package. A configuration file required for this update could not be run. Please verify the installer and try again."},    // FDA8
    {0xFDA9, "EC_FAILED_OPEN_NAND_IMG, please make sure image exists"},            // FDA9
    {0xFDAA, "EC_NAND_IMG_ILLEGAL, There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance."},              // FDA9
    {0xFDAB, "EC_DEVICE_DOES_NOT_MATCH_IMG, There is a problem with this update package. The image may not match your target. Please verify the image and try again."},     // FDA9
    {0xFDAC, "EC_REBOOT_TG_TO_UPG_MODE_FAILED, please retry again"},               // FDA9
    {0xFDAD, "EC_CONNECT_UPG_MODE_FAILED, please retry again"},                    // FDA9
    
    {0xFDAF, "EC_GET_FILES_FROM_TG, There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                                              // FDAF
    {0xFDB0, "EC_UMOUNT_FAILED, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                                                  // FDB0
    
    {0xFDB3, "EC_FT_INIT_FAILED, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                                                 // FDB3
    
    {0xFDB6, "EC_MFG_WAIT_RESCUE_LINUX_TIMEOUT, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                                  // FDB6
    {0xFDB7, "EC_FIND_PART_FROM_IMG, There is a problem with this image. The image/DLL required for this update could not be run properly. Please verify the update package or contact your support personnel for assistance."},                                             // FDB7
    {0xFDB8, "EC_OPEN_RESCUE_IMG_ERROR, There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the rescue image exist and try again."},                                          // FDB8
    {0xFDB9, "EC_CHECK_TARGET_SW_VERSION, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                                        // FDB9
    {0xFDBA, "EC_RUN_SYS_SHELL, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."},                                                    // FDBA
    {0xFDBB, "EC_USERDATA_UPGRADE_FAIL, There is a problem with this update package. If your still want to update your target, it may wipe all personal data like records, images, all DB file etc. DO YOU WANT TO CONTINUE?"},     // FDBB
    {0xFDBC, "EC_DLL_LOST, There is a problem with this update package. The DLL folder required for this update may not be complete. Please verify the update package or contact your support personnel for assistance."},   	 // FDBC

    /* for USR tool }*/
    {0xFFC4, "ERROR_CODE_USBI_STATUS_PENDING, please reconnect USB cable and try again."},                // FFC4     
    {0xFFC5, "ERROR_CODE_USBI_STATUS_PARAM, please reconnect USB cable and try again."},                  // FFC5     
    {0xFFC6, "ERROR_CODE_USBI_STATUS_BUSY, please reconnect USB cable and try again."},                   // FFC6     
    {0xFFC7, "ERROR_CODE_USBI_STATUS_NOMEM, please reconnect USB cable and try again."},                  // FFC7     
    {0xFFC8, "ERROR_CODE_USBI_STATUS_TIMEOUT, please reconnect USB cable and try again."},                // FFC8     
    {0xFFC9, "ERROR_CODE_USBI_STATUS_NODEV, please reconnect USB cable and try again."},                  // FFC9     
    {0xFFCA, "ERROR_CODE_USBI_STATUS_STATE, please reconnect USB cable and try again."},                  // FFCA     
    {0xFFCB, "ERROR_CODE_USBI_STATUS_NOT_SUPPORTED, please reconnect USB cable and try again."},          // FFCB   
    {0xFFCC, "ERROR_CODE_USBI_STATUS_UNKNOWN, please reconnect USB cable and try again."},                // FFCC    
    {0xFFD8, "ERROR_CODE_RETRY_AGAIN, The update installer operation is unsuccessful. Please reboot target and try again later. If same error occurs again, please contact your support personnel for assistance."},                        // FFD8     
    {0xFFD9, "ERROR_CODE_WSASTARTUP, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                         // FFD9     
    {0xFFDA, "ERROR_CODE_SOCKET, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                             // FFDA     
    {0xFFDB, "ERROR_CODE_SETSOCKOPT, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                         // FFDB     
    {0xFFDC, "ERROR_CODE_BIND, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                               // FFDC   
    {0xFFDD, "ERROR_CODE_LISTEN, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                             // FFDD     
    {0xFFDE, "ERROR_CODE_ACCEPT, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                             // FFDE     
    {0xFFDF, "ERROR_CODE_SELECT, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                             // FFDF     
    {0xFFE0, "ERROR_CODE_INVALID_SOCKET, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                     // FFE0     
    {0xFFE1, "ERROR_CODE_RECV_ERROR, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                         // FFE1     
    {0xFFE2, "ERROR_CODE_RECV_TIMEOUT, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                       // FFE2     
    {0xFFE3, "ERROR_CODE_SOCKET_CLOSE, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                       // FFE3     
    {0xFFE4, "ERROR_CODE_SEND_ERROR, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                         // FFE4     
    {0xFFE5, "ERROR_CODE_RECV_EXCEPTION, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},                     // FFE5
    {0xFFE6, "ERROR_CODE_SOCKET_IS_NOT_CONNECTED, A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance."},            // FFE6
    
    {0xFFF8, "ERROR_CHILD_EXECVP_CMD_ERROR_V1, The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance."}                // FFF8         
};
#endif
