/* INCLUDE BY devToolUI.c */
#include <stdio.h>
#include <windows.h>
#include "event.h"

static HANDLE hEventFile = NULL;

int event_recorder_init()
{
	int retval = 0;
	event_recorder_exit();
	HANDLE hFile = CreateFile(EVENT_FILE,
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return -1;
	hEventFile = hFile;

#if 0
	retval = event_record(EV_HEADER, 0, NULL) > 0 ? 0 : -1;
	if(retval)
		event_recorder_exit();
#endif
	return retval;
}

void event_recorder_exit()
{
	if(hEventFile){
		CloseHandle(hEventFile);
	}
	hEventFile = NULL;
}

int event_record(int ev_type, int ev_code, void *ev_value)
{
#ifndef BUFSIZ
#define BUFSIZ	1024
#endif

	SYSTEMTIME l_time;
	static char line_buffer[BUFSIZ];
	static char delim = ' ';
	GetLocalTime(&l_time);
	/* write local time */
	int len = snprintf(line_buffer, BUFSIZ, "%04d/%02d/%02d%c%02d:%02d:%02d%c",
			l_time.wYear,
			l_time.wMonth,
			l_time.wDay,
			delim,
			l_time.wHour,
			l_time.wMinute,
			l_time.wSecond,
			delim);
	
	switch(ev_type){
#if 0
		case EV_HEADER:
			snprintf(line_buffer + len, BUFSIZ - len,"DateTime,EventType,EventCode,EventValue\n");
			break;
#endif
		/* no ev_code and ev_value */
		case EV_START:
		case EV_UPGRADE:
		case EV_NETWORK_ERROR:
		case EV_MSN_DB_LOST:
		case EV_ERROR:
		case EV_MSN_NOT_MATCH:
		case EV_MSN_MATCH:
		case EV_INIT_SUCCESS:
		case EV_INIT_ERROR:
		case EV_BURNPARTITION:
		case EV_BURNIMAGE:
		case EV_UPGRADE_FAILED:
		case EV_COMPLETE:
		case EV_LOW_POWER:
		case EV_USERDATA_ERROR_SKIP:
		case EV_USERDATA_ERROR_MANDATORY:
			len += snprintf(line_buffer + len, BUFSIZ - len, "%#x\n", ev_type);
			//snprintf(line_buffer, BUFSIZ, "%#x,,,\n", ev_type);
			break;

		/* no ev_code */
		case EV_VERIFY_MSN:
			//snprintf(line_buffer, BUFSIZ, "%#x,,%s,\n", ev_type, (char *)ev_value);
			len += snprintf(line_buffer + len, BUFSIZ - len, "%#x%c%s\n",
					ev_type, delim, (char *)ev_value); 
			break;

		default:
			log_error("Bad event type(%d)\n", ev_type);
			return 0;

	}
	int bytesWritten = 0;
	int bytesToWrite = len;

	while(WriteFile(hEventFile, line_buffer + bytesWritten, bytesToWrite - bytesWritten, &bytesWritten, NULL) != FALSE && bytesWritten != bytesToWrite)
		;
	if(bytesWritten != bytesToWrite){
		log_error("WriteFile error\n");
		return 0;
	}
	FlushFileBuffers(hEventFile);

	return 1;
}
