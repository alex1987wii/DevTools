/*
 *  Copyright: (c) 2013 Unication Co., Ltd.
 *  All rights reserved.
 *
 *  Name: error_code.h
 *
 *  Created By: kamingli <kaming_li@unication.com.cn>
 *  Created Date: 2012-05-24
 *
 *  This program defines macros used in file transfer module
 *  for shakehand transmit and receive.
 *
 *  This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public
 *  License version 2 as published by the Free Software
 *  Foundation.
 *
 *  ChangLog:
 *  Date                Author              Purpose
 *  2012-05-24          kamingli            Create
 *
 */

#ifndef _ERROR_CODE_H_
#define _ERROR_CODE_H_

#define ERROR_CODE_TRANSFER(ec)             (0x10000 - ((unsigned short)ec))

#if (defined (FT_LINUX) || defined (FT_WINDOW))
#ifdef ENABLE_DEBUG
#define PRINTF_ERROR_CODE(from, to)    do {int i; \
    for (i=from; i<=to; i++) printf("%/%/ %04X\n", (unsigned short)i);} while (0)
#endif
#endif

enum _ERROR_CODE
{
    ERROR_CODE_OPEN_FILE_ERROR = -1000,            // FC18
    ERROR_CODE_FREAD_FILE_ERROR,                   // FC19
    ERROR_CODE_FWRITE_FILE_ERROR,                  // FC1A
    ERROR_CODE_MALLOC_ERROR,                       // FC1B
    ERROR_CODE_FT_CMD_ERROR,                       // FC1C
    ERROR_CODE_FT_CMD_NOT_SUPPORT,                 // FC1D
    ERROR_CODE_TRANSFER_TYPE_NOT_SUPPORT,          // FC1E
    ERROR_CODE_TRANSFER_TYPE_ERROR,                // FC1F
    ERROR_CODE_RETRY_WEAR_OUT,                     // FC20
    ERROR_CODE_WINUSB_INIT_ERROR,                  // FC21
    ERROR_CODE_NETWORK_INIT_ERROR,                 // FC22
    ERROR_CODE_NETWORK_CONNECT_ERROR,              // FC23
    ERROR_CODE_BUFFER_SIZE_NOT_ENOUGH,             // FC24
    ERROR_CODE_HW_TYPE_ERROR,                      // FC25
    ERROR_CODE_PHYADDR_ERROR,                      // FC26
    ERROR_CODE_PHYADDR_OUT_OF_RANGE,               // FC27
    ERROR_CODE_BLOCK_ALIGN_ERROR,                  // FC28
    ERROR_CODE_PAGE_ALIGN_ERROR,                   // FC29
    ERROR_CODE_SIZE_OUT_OF_RANGE,                  // FC2A
    ERROR_CODE_TOO_MUCH_BAD_BLOCK,                 // FC2B
    ERROR_CODE_NFC_WRITE_PAGE_ERROR,               // FC2C
    ERROR_CODE_CRC_ERROR,                          // FC2D
    ERROR_CODE_IOCTL_MEMGETBADBLOCK_ERROR,         // FC2E
    ERROR_CODE_IOCTL_MEMERASE_ERROR,               // FC2F
    ERROR_CODE_IOCTL_MEMWRITEOOB_ERROR,            // FC30
    ERROR_CODE_IOCTL_MEMREADOOB_ERROR,             // FC31
    ERROR_CODE_IOCTL_MEMGETINFO_ERROR,             // FC32
    ERROR_CODE_IOCTL_MTDFILEMODE_ERROR,            // FC33
    ERROR_CODE_PWRITE_ERROR,                       // FC34
    ERROR_CODE_PREAD_ERROR,                        // FC35
    ERROR_CODE_OPEN_MTD_ERROR,                     // FC36
    ERROR_CODE_TRANSFER_LEN_TOO_LARGE,             // FC37
    ERROR_CODE_IMAGE_NOT_CORRECT,                  // FC38
    ERROR_CODE_IOCTL_NANDSETINECC,                 // FC39
    ERROR_CODE_FT_CMD_VERSION_ERROR,               // FC3A
    ERROR_CODE_IOCTL_NANDWRITEPAGE,                // FC3B
    ERROR_CODE_RUNNING_MODE_UNKNOW,                // FC3C
    ERROR_CODE_NOT_SUPPORT_IN_THIS_MODE,           // FC3D
    ERROR_CODE_DATA_VERIFY_NOT_PASSED,             // FC3E
    ERROR_CODE_DATA_VERIFY_MEMORY_ALLOC_ERROR,     // FC3F
    ERROR_CODE_DATA_VERIFY_TOO_MANY_BAD_BLOCK,     // FC40
    ERROR_CODE_FT_VER_ERROR,                       // FC41
    ERROR_CODE_PARTITION_INDEX_NOT_SUPPORT,        // FC42
    ERROR_CODE_CALCULATE_CRC_ERROR,                // FC43
    ERROR_CODE_FT_WANTED_NOT_MATCH,                // FC44
    ERROR_CODE_FT_UNKNOW_ERROR,                    // FC45
    ERROR_CODE_FILE_NOT_EXIST,                     // FC46
    ERROR_CODE_WORKING_THREAD_IS_BUSY,             // FC47
    ERROR_CODE_MALLOC_FILENAME_ERROR,              // FC48
    ERROR_CODE_SEQ_NUMBER_NOT_MATCH,               // FC49
    ERROR_CODE_UNKNOW_ASSIGNMENT_JOB,              // FC4A
    ERROR_CODE_NOT_ENOUGH_SPACES,                  // FC4B
    ERROR_CODE_WANTED_FILE_CAN_NOT_BE_SAVED,       // FC4C
    ERROR_CODE_WANTED_OPERATION_IS_NOT_SUPPORT,    // FC4D
    ERROR_CODE_FORK_CHILD_ERROR,                   // FC4E
    ERROR_CODE_WAIT_CHILD_PID_ERROR,               // FC4F
    ERROR_CODE_DLOPEN_ERROR,                       // FC50
    ERROR_CODE_DLSYM_ERROR,                        // FC51
    ERROR_CODE_CALCULATE_HEADER_CRC_ERROR,         // FC52
    ERROR_CODE_IMAGE_IS_NOT_FOR_THIS_TARGET_BOARD, // FC53
    ERROR_CODE_PARTITION_IS_PROTECTED,             // FC54
    ERROR_CODE_JOB_THREAD_CREATE_ERROR,            // FC55
    ERROR_CODE_IS_NOT_IN_UPGRADE_MODE,             // FC56
    ERROR_CODE_IS_NOT_A_DIR,                       // FC57
    ERROR_CODE_IOCTL_NANDFLAHSRESET_ERROR,         // FC58
    ERROR_CODE_BATTERY_VOLTAGE_IS_NOT_SATISFIED,   // FC59
    ERROR_CODE_REMOUNT_UPDATE_PAR_AS_RO_ERROR,     // FC5A
    ERROR_CODE_REMOUNT_UPDATE_PAR_AS_RW_ERROR,     // FC5B

    /* for pc tool {*/
    EC_INI_FILE_LOST = -600,                       // FDA8
    EC_FAILED_OPEN_NAND_IMG,                       // FDA9
    EC_NAND_IMG_ILLIGAL,                           // FDAA
    EC_DEVICE_DOES_NOT_MATCH_IMG,                  // FDAB
    EC_REBOOT_TG_TO_UPG_MODE_FAILED,               // FDAC
    EC_CONNECT_UPG_MODE_FAILED,                    // FDAD
    EC_CREATE_DEVICE_FILE_FAILED,                  // FDAE
    EC_GET_FILES_FROM_TG,                          // FDAF
    EC_UMOUNT_FAILED,                              // FDB0
    EC_DUMP_FILE_LOST,                             // FDB1
    EC_NO_DEVICE_WAS_FOUND,                        // FDB2
    EC_FT_INIT_FAILED,                             // FDB3
    EC_MFG_WAIT_ROM_CANCEL,                        // FDB4
    EC_MFG_WAIT_SPL_TIMEOUT,                       // FDB5
    EC_MFG_WAIT_RESCUE_LINUX_TIMEOUT,              // FDB6
    EC_FIND_PART_FROM_IMG,                         // FDB7
    EC_OPEN_RESCUE_IMG_ERROR,                      // FDB8
    EC_CHECK_TARGET_SW_VERSION,                    // FDB9
    EC_RUN_SYS_SHELL,                              // FDBA
    /* for pc tool }*/

    ERROR_CODE_USBI_STATUS_PENDING = -60,          // FFC4
    ERROR_CODE_USBI_STATUS_PARAM,                  // FFC5
    ERROR_CODE_USBI_STATUS_BUSY,                   // FFC6
    ERROR_CODE_USBI_STATUS_NOMEM,                  // FFC7
    ERROR_CODE_USBI_STATUS_TIMEOUT,                // FFC8
    ERROR_CODE_USBI_STATUS_NODEV,                  // FFC9
    ERROR_CODE_USBI_STATUS_STATE,                  // FFCA
    ERROR_CODE_USBI_STATUS_NOT_SUPPORTED,          // FFCB
    ERROR_CODE_USBI_STATUS_UNKNOWN,                // FFCC

    ERROR_CODE_RETRY_AGAIN = -40,                  // FFD8
    ERROR_CODE_WSASTARTUP = -39,                   // FFD9
    ERROR_CODE_SOCKET,                             // FFDA
    ERROR_CODE_SETSOCKOPT,                         // FFDB
    ERROR_CODE_BIND,                               // FFDC
    ERROR_CODE_LISTEN,                             // FFDD
    ERROR_CODE_ACCEPT,                             // FFDE
    ERROR_CODE_SELECT,                             // FFDF
    ERROR_CODE_INVALID_SOCKET,                     // FFE0
    ERROR_CODE_RECV_ERROR,                         // FFE1
    ERROR_CODE_RECV_TIMEOUT,                       // FFE2
    ERROR_CODE_SOCKET_CLOSE,                       // FFE3
    ERROR_CODE_SEND_ERROR,                         // FFE4
    ERROR_CODE_RECV_EXCEPTION,                     // FFE5
    ERROR_CODE_SOCKET_IS_NOT_CONNECTED,            // FFE6

    ERROR_USB_WRITE_TIMEOUT = -19,                 // FFED
    ERROR_USB_RETRY_WEAR_OUT,                      // FFEE
    ERROR_USB_INTERRUPT_BY_CMD,                    // FFEF
    ERROR_USB_CABLE_UNPLUG,                        // FFF0

    ERROR_CHILD_MALLOC_FILENAME_ERROR = -10,       // FFF6
    ERROR_CHILD_EXECVP_CMD_ERROR,                  // FFF7
    ERROR_CHILD_EXECVP_CMD_ERROR_V1,               // FFF8

	ERROR_GET_DEVICEID_ERROR,                      // FFF9
};


#endif
