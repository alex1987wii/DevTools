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

#ifdef ENABLE_DEBUG
#define PRINTF_ERROR_CODE(from, to)    do {int i; \
for (i=from; i<=to; i++) printf("%/%/ %04X\n", (unsigned short)i);} while (0)
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

    ERROR_USB_WRITE_TIMEOUT = -19,                 // FFED                                        
    ERROR_USB_RETRY_WEAR_OUT,                      // FFEE                                          
    ERROR_USB_INTERRUPT_BY_CMD,                    // FFEF                                        
    ERROR_USB_CABLE_UNPLUG,                        // FFF0                                       
};


#endif
