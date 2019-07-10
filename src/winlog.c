#include "winlog.h"

static FILE *log_fp = NULL;
static int log_level = LOG_INFO;
static const char *level_table[LOG_LEVEL_MAX] = {
	"EMERG",
	"ERROR",
	"WARN",
	"NOTICE",
	"INFO",
	"DEBUG",
};

static const char *get_local_time_str(void)
{
	static SYSTEMTIME local_time;
	static char time_str[256];
	GetLocalTime(&local_time);
	snprintf(time_str, 256, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
			local_time.wYear, local_time.wMonth,
			local_time.wDay, local_time.wHour,
			local_time.wMinute, local_time.wSecond,
			local_time.wMilliseconds);
	return time_str;
}

int openlog(const char *filename, int level, int type)
{
	FILE *fp;
	const char *mode = NULL;
	
	if(log_fp)
		return LOG_EBUSY;
	switch(type)
	{
		case LOG_OVERLAP:
			mode = "w+";
			break;
		case LOG_APPEND:
			mode = "a+";
			break;
		default:
			return LOG_EARG;
	}
	if(level >= LOG_LEVEL_MAX || level < 0)
		return LOG_EARG;
	fp = fopen(filename, mode);
	if(fp == NULL)
		return LOG_EOPEN;

	log_level = level;
	log_fp = fp;
	return LOG_SUCCESS;

}

int log(int level, const char *fmt, ...)
{
	int retlen = 0;
	static char level_info[12];

	if(log_fp == NULL)
		return LOG_EWRITE;
	if(level < 0 || level >= LOG_LEVEL_MAX)
		return LOG_EARG;
	else if(level > log_level)//filter un-neccessory message
		return retlen;

	/*log time*/
	retlen = fprintf(log_fp, get_local_time_str());
	fputc(' ', log_fp);
	retlen++;
	
	/*log level*/
	snprintf(level_info, 12, "%+6s",level_table[level]);

	retlen += fprintf(log_fp, level_info);
	fputc(' ', log_fp);
	retlen++;

	va_list ap;
	va_start(ap, fmt);
	retlen += vfprintf(log_fp, fmt, ap);
	va_end(ap);
	if(fmt && fmt[strlen(fmt) - 1] != '\n')
		retlen += fprintf(log_fp,"\n");
	fflush(log_fp);
}

int closelog()
{
	if(log_fp)
		close(log_fp);
	log_fp = NULL;
	return LOG_SUCCESS;
}
