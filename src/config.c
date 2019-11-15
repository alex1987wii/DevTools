#include "config.h"
#include "iniparser.h"

#define INI_DEBUG	printf
#define INI_ERROR	printf

struct ini_file_info_t ini_file_info_list[MAX_DEVICES];

static inline BOOL valid_ip_addr(const char *ip)
{
	return TRUE;
}

int parse_ini_file(const char *file)
{
	int i = 0;
	char key[32];
	char *ip, *rescue_image;
	dictionary *dict = iniparser_load(file);
	if(dict == NULL){
		INI_DEBUG("%s : ini file not accessable\n", file);
		return EC_INI_FILE_NOT_EXSIT;
	}
	for(; i < MAX_DEVICES; i++){
		snprintf(key, 32, "Device%d:ip", i);
		ip = iniparser_getstring(dict, key, NULL);
		snprintf(key, 32, "Device%d:rescue_image", i);
		rescue_image = iniparser_getstring(dict, key, NULL);
		if(ip == NULL || rescue_image == NULL){
			break;
		}
		strncpy(ini_file_info_list[i].ip, 16, ip);
		strncpy(ini_file_info_list[i].rescue_image, MAX_PATH, rescue_iamge);
	}
	iniparser_freedict(dict);
	return i;
}
