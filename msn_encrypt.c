#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "msn.h"
#include "rc4.h"

#define APP_NAME	"UniMSNEncryptTool.exe"
#define BUF_SIZE	1024
#define DEF_OUTFILE	"msn.db"

#ifndef NDEBUG
#define DBG	printf	
#else
#define DBG
#endif


enum{
	POLICY_SKIP = 0,
	POLICY_QUIT,
};
enum{
	OPT_OPT = 0,
	OPT_INFILE = 1,
	OPT_OUTFILE = 1 << 1,
};

static const char *input = NULL;
static const char *output = NULL;
static char line_buf[BUF_SIZE];
static int records = 0;
static int policy = POLICY_QUIT;

void usage(void)
{
	fprintf(stderr, "usage : "APP_NAME" -f infile -o outfile [--policy=[skip/quit]]\n");
	fprintf(stderr, "        "APP_NAME" -h,--help\n");
	fprintf(stderr, "        "APP_NAME" --version\n");
	exit(0);
}

void get_options(int argc, char *argv[], const char **input, const char **output)
{
	int index = 1;
	int opt_requisite = 0;
	int next_options = OPT_OPT;
	if(argc == 1 )
		usage();
	while(index < argc){
		switch(next_options){
			case OPT_OPT:
				if(!strcmp("-f", argv[index]))
					next_options = OPT_INFILE;
				else if(!strcmp("-o", argv[index]))
					next_options = OPT_OUTFILE;
				else if(!strcmp("--policy=skip", argv[index]))
					policy = POLICY_SKIP;
				else if(!strcmp("--policy=quit", argv[index]))
					policy = POLICY_QUIT;
				else if(!strcmp("-h", argv[index]) || !strcmp("--help", argv[index]))
					usage();
				else if(!strcmp("--version", argv[index])){
					fprintf(stderr, "Version : "VERSION);
					exit(-1);
				}
				else
					usage();
				break;
			case OPT_INFILE:
				*input = argv[index];
				next_options = OPT_OPT;
				opt_requisite |= OPT_INFILE;
				break;

			case OPT_OUTFILE:
				*output = argv[index];
				next_options = OPT_OPT;
				opt_requisite |= OPT_OUTFILE;
				break;

			default:
				usage();
				break;
		}
		index++;
	}
	if(!(opt_requisite & OPT_INFILE)){
		fprintf(stderr, "infile is required\n");
		exit(-1);
	}
	if(!(opt_requisite & OPT_OUTFILE)){
		fprintf(stderr, "outfile is required\n");
		exit(-1);
	}
	DBG("get_options parsed, infile = %s, outfile = %s, policy = %d\n",\
			*input, *output, policy);
}

/* strip string space */
void strip_space(char *str)
{
	if(str == NULL || strlen(str) == 0) return;
	char *p = str + strlen(str);
	while(p != str && isspace(*(p-1))) --p;
	*p = '\0';

	p = str;
	while(isspace(*p)) ++p;
	if(p == str) return;
	while(*p) *str++ = *p++;
	*str = '\0';
}
/* verify msn
 * msn : the msn should verify
 * return value : 0 -> OK
 * 		  !=0 -> not OK
 */
int msn_verify(const char *msn)
{

	if(msn == NULL || strlen(msn) != MSN_LEN)
		return 1;
	while(*msn){
		if(!isalnum(*msn))
			return 1;
		++msn;
	}
	return 0;
}

int main(int argc, char *argv[])
{
	struct msn_node_t *root = NULL;
	get_options(argc, argv, &input, &output);

	FILE *fp_in = fopen(input, "r");
	if(fp_in == NULL){
		fprintf(stderr, "%s open failed, check if it's exsit\n", input);
		exit(-1);
	}

	while(fgets(line_buf, BUF_SIZE, fp_in)){
		strip_space(line_buf);
		if(msn_verify(line_buf)){
			fprintf(stderr, "bad msn  : %s -> %s\n", line_buf, policy == POLICY_SKIP ? "skip" : "quit"); /* just skip */
			if(policy == POLICY_SKIP)
				continue;
			else
				exit(-1);
		}
		printf("encrypt -> %s\n", line_buf);
		RC4_encrypt_and_add_msn(&root, line_buf);
		++records;
	}
	fclose(fp_in);

	if(records == 0){
		fprintf(stderr, "no records found\n");
		exit(-1);
	}

	if(write_msn_to_file(root, output)){
		fprintf(stderr, "write db file(%s) error", output);
		exit(-1);
	}
	fprintf(stderr, "generate db file<%s> success, total %d record(s)\n", output, records);

	msn_release(root);
	return 0;
}
