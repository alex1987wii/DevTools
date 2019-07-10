#ifndef _WIN_LOG_H_
#define _WIN_LOG_H_

#include <windows.h>
#include <stdio.h>

enum log_level_t{
	LOG_EMERG = 0,
	LOG_ERR,
	LOG_WARN,
	LOG_NOTICE,
	LOG_INFO,
	LOG_DEBUG,
	LOG_LEVEL_MAX,
};
enum log_type_t{
	LOG_OVERLAP = 0,
	LOG_APPEND,
	LOG_ROTATE,//not suppot yet
};

#define LOG_SUCCESS	0
enum log_error_code_t{
	LOG_EBUSY = -100,
	LOG_EARG,
	LOG_EOPEN,
	LOG_EWRITE,
	LOG_ENOTSUPPORT,

};
int openlog(const char *filename, int level, int type);
int log(int level, const char *fmt, ...);
int closelog();

#define log_emerg(fmt, ...)	log(LOG_EMERG, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_error(fmt, ...)	log(LOG_ERR, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_warn(fmt, ...)	log(LOG_WARN, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_notice(fmt, ...)	log(LOG_NOTICE, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_info(fmt, ...)	log(LOG_INFO, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define log_debug(fmt, ...)	log(LOG_DEBUG, "%s:%s<%d>:"fmt, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#endif
