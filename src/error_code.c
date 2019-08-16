#include "error_code.h"

unsigned short error_code = OK;	
	
struct _error_code_info error_code_info[] = 
{
	{0xFDFF, "Please check image again.","Please check image again."},// For U3 Only
    {0xFDFE, "Please init first","Please init first"}, // For U3 Only
    {0xFC18, "There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package and try again, or contact your support personnel for assistance..","Tool package broken."},                    // FC18    

    {0xFC1C, "There is a problem with this update package. The installer may not match your target. Please verify the installer and try again.","Tool package error."}, // FC1C  
    {0xFC1D, "There is a problem with this update package. The installer may not match your target. Please verify the installer and try again.","Tool package error."},                 // FC1D   
    {0xFC1E, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},          // FC1E   
    {0xFC1F, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},                // FC1F   
    {0xFC20, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."}, // FC20
    {0xFC21, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error"},                  // FC21   
    {0xFC22, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error"},                 // FC22   
    {0xFC23, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error"},              // FC23   
    {0xFC24, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},             // FC24   
    {0xFC25, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                      // FC25   
    
    {0xFC2D, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},                          // FC2D    
    {0xFC2E, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},         // FC2E    
    
    {0xFC34, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                       // FC34    
    {0xFC35, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                        // FC35    
    {0xFC36, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                     // FC36   
    
    {0xFC38, "There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance.","Tool package broken."},                  // FC38
    {0xFC39, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                 // FC39
    {0xFC3A, "There is a problem with this update package. The installer may not match your target. Please verify the installer and try again.","Tool package error."},               // FC3A
    {0xFC3B, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                // FC3B
    
    
    {0xFC3E, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},             // FC3E
    {0xFC3F, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},     // FC3F
    {0xFC40, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},     // FC40
    {0xFC41, "There is a problem with this update package. The installer may not match your target. Please verify the installer and try again.","Tool package error."},                       // FC41   

    {0xFC43, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},                // FC43
    {0xFC44, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                // FC44
    {0xFC45, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                    // FC45
    {0xFC46, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                     // FC46
    
    {0xFC4C, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},       // FC4C
    {0xFC4D, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Tranfer error."},    // FC4D
   
    {0xFC52, "There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance.","Tool package broken."},         // FC52
    {0xFC53, "There is a problem with this update package. The image may not match your target. Please verify the image and try again.","Tool package error."}, // FC53
    {0xFC54, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},             // FC54
    
    {0xFC58, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},         // FC58
    {0xFC59, "Your battery is too low to update your target. Please charge target then try again.","Low battery."},   // FC59
    {0xFC5A, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},     // FC5A
   

    /* for USR tool {*/
    {0xFDA8, "There is a problem with this update package. A configuration file required for this update could not be run. Please verify the installer and try again.","Tool package broken."},    // FDA8
   
    {0xFDAA, "There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the update package or contact your support personnel for assistance.","Tool package broken."},              // FDA9
    {0xFDAB, "There is a problem with this update package. The image may not match your target. Please verify the image and try again.","Tool package error."},     // FDA9
    
    {0xFDAF, "There is a problem with file transfer. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Transfer error."},                                              // FDAF
    {0xFDB0, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                                                  // FDB0
    
    {0xFDB3, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                                                 // FDB3
    
    {0xFDB6, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                                  // FDB6
    {0xFDB7, "There is a problem with this image. The image/DLL required for this update could not be run properly. Please verify the update package or contact your support personnel for assistance.","Tool package broken."},                                             // FDB7
    {0xFDB8, "There is a problem with this update package. A image/DLL required for this update could not be run. Please verify the rescue image exist and try again.","Tool package broken."},                                          // FDB8
    {0xFDB9, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                                        // FDB9
    {0xFDBA, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                                                    // FDBA
    {0xFDBB, "There is a problem with this update package. If your still want to update your target, it may wipe all personal data like records, images, all DB file etc. DO YOU WANT TO CONTINUE?","Userdata upgrade failed."},     // FDBB
    {0xFDBC, "There is a problem with this update package. The DLL folder required for this update may not be complete. Please verify the update package or contact your support personnel for assistance.","Tool package broken."},   	 // FDBC

    /* for USR tool }*/
    {0xFFC4, "please reconnect USB cable and try again.","Usb connect error."},                // FFC4     
    {0xFFC5, "please reconnect USB cable and try again.","Usb connect error."},                  // FFC5     
    {0xFFC6, "please reconnect USB cable and try again.","Usb connect error."},                   // FFC6     
    {0xFFC7, "please reconnect USB cable and try again.","Usb connect error."},                  // FFC7     
    {0xFFC8, "please reconnect USB cable and try again.","Usb connect error."},                // FFC8     
    {0xFFC9, "please reconnect USB cable and try again.","Usb connect error."},                  // FFC9     
    {0xFFCA, "please reconnect USB cable and try again.","Usb connect error."},                  // FFCA     
    {0xFFCB, "please reconnect USB cable and try again.","Usb connect error."},          // FFCB   
    {0xFFCC, "please reconnect USB cable and try again.","Usb connect error."},                // FFCC    
    {0xFFD8, "The update installer operation is unsuccessful. Please reboot target and try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."},                        // FFD8     
    {0xFFD9, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                         // FFD9     
    {0xFFDA, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                             // FFDA     
    {0xFFDB, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                         // FFDB     
    {0xFFDC, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                               // FFDC   
    {0xFFDD, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                             // FFDD     
    {0xFFDE, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                             // FFDE     
    {0xFFDF, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                             // FFDF     
    {0xFFE0, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                     // FFE0     
    {0xFFE1, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                         // FFE1     
    {0xFFE2, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                       // FFE2     
    {0xFFE3, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                       // FFE3     
    {0xFFE4, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                         // FFE4     
    {0xFFE5, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},                     // FFE5
    {0xFFE6, "A network error occurred while attempting to communicate with target. Please unplug and Reinsert USB cable and retry updating. If same error occurs again, please contact your support personnel for assistance.","Network error."},            // FFE6
     
    {0xFFF8, "The update installer operation is unsuccessful. Please try again later. If same error occurs again, please contact your support personnel for assistance.","Operation failed."}                // FFF8         
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
            return (char *)error_code_info[i].short_string;
        }
    }
	/*not found*/
    return ec_not_found;
}

