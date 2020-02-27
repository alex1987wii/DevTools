#include "msn.h"
#include "rc4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBG	printf

static unsigned char key[5] = {0x01, 0x11, 0x1a, 0xaa, 0xa1};

struct msn_node_t *RC4_encrypt_and_add_msn(struct msn_node_t **msn_list, const char *msn)
{
	struct msn_node_t *new_node;
	unsigned int msn_len = strlen(msn);
	if(msn_list == NULL || msn_len != MSN_LEN){
		if(msn_len != MSN_LEN)
			DBG("Bad MSN serial number : %s\n", msn);
		return NULL;
	}

	while(*msn_list){
		msn_list = &(*msn_list)->next;
	}

	new_node = malloc(sizeof(struct msn_node_t));
	if(new_node == NULL){
		DBG("malloc for new_node failed\n");
		return NULL;
	}

	memset(new_node, 0, sizeof(struct msn_node_t));
	/* encrypt msn */
	RC4_single(key, sizeof(key), msn, MSN_LEN, new_node->msn);

	*msn_list = new_node;
	return new_node;
}

int write_msn_to_file(struct msn_node_t *msn_list, const char *filename)
{
	struct msn_header_t header;
	unsigned int msn_cnt = 0;
	unsigned short gen_crc = 0;
	if(msn_list == NULL || filename == NULL){
		return -1;
	}
	FILE *fp = fopen(filename, "wb");
	if(fp == NULL){
		DBG("%s open failed\n", filename);
		return -2;
	}

	/* set msn_header */
	memset(&header, 0, sizeof(struct msn_header_t));
	strncpy(header.magic, MSN_MAGIC, 4);
	header.major_ver = MAJOR_VER;
	header.minor_ver = MINOR_VER;
	header.header_size = sizeof(struct msn_header_t);
	/*
	 * header.header_crc
	 * header.content_crc
	 */
	strncpy(header.project, PROJECT, 16);
	header.encrypt_alg = ALG_RC4;
	/*
	 * header.entities
	 */
	header.entity_size = MSN_LEN;
	
	/* write content */
	fseek(fp, header.header_size, SEEK_SET);

	while(msn_list){
		gen_crc = crc16Update(gen_crc, msn_list->msn, MSN_LEN);
		fwrite(msn_list->msn, MSN_LEN, 1, fp);
		++msn_cnt;
		msn_list = msn_list->next;
	}
	/* calculate crc */
	header.entities = msn_cnt;
	header.content_crc = gen_crc;
	header.header_crc = crc16Update(0, (uint8_t *)&header.content_crc, sizeof(header) - offset_of(struct msn_header_t, content_crc));

	/* write header into file */
	fseek(fp, 0, SEEK_SET);
	fwrite(&header, sizeof(header), 1, fp);
	fclose(fp);
	return 0;
}

struct msn_node_t *msn_open(const char *filename)
{
	FILE *fp;
	unsigned int filelen;
	struct msn_node_t *root = NULL;
	struct msn_node_t **p = &root;
	int i;
	unsigned short gen_crc = 0;
	struct msn_header_t header;
	if(filename == NULL)
		return NULL;
	fp = fopen(filename, "rb");
	if(fp == NULL)
		return NULL;
	/*check filelen */
	fseek(fp, 0, SEEK_END);
	filelen = ftell(fp);
	if(filelen < sizeof(struct msn_header_t)){
		DBG("file length too small, length = %d\n", filelen);
		fclose(fp);
		return NULL;
	}
	fseek(fp, 0, SEEK_SET);

	/* read header */
	fread(&header, sizeof(struct msn_header_t), 1, fp);
	if(header.header_size != sizeof(struct msn_header_t)){
		DBG("header size don't match, header.header_size = %d, current header_size = %d", header.header_size, (int)sizeof(struct msn_header_t));
		goto header_not_match;
	}
	gen_crc = crc16Update(0, (uint8_t *)&header.content_crc, header.header_size - offset_of(struct msn_header_t, content_crc));
	if(gen_crc != header.header_crc){
		DBG("header crc not match, gen_crc = %#x, header.header_crc = %#x\n", gen_crc, header.header_crc);
		goto header_crc_error;
	}
	if(strncmp(header.magic, MSN_MAGIC, 4)){
		DBG("magic not match, header.magic = %4s\n", header.magic);
		goto magic_not_match;
	}
	if(header.major_ver != MAJOR_VER || header.minor_ver != MINOR_VER){
		DBG("version not match, header.major_ver = %d, header.minor_ver = %d\n", header.major_ver, header.minor_ver);
		goto version_not_match;
	}
#if 0
	if(strncmp(header.project, PROJECT, 16)){
		DBG("project not match, header.project = %16s\n", header.project);
		goto project_not_match;
	}
#endif
	if(header.encrypt_alg != ALG_RC4){
		DBG("un-support alg, header.encrypt_alg = %d\n", header.encrypt_alg);
		goto encrypt_alg_not_support;
	}
	
	if(header.entity_size != MSN_LEN)
	{
		DBG("MSN_LEN not match, header.entity_size = %d\n", header.entity_size);
		goto msn_len_not_match;
	}

	/* check filesize */
	if(header.header_size + header.entities * header.entity_size != filelen){
		DBG("file length not match, header.header_size = %d, header.entities = %d, header.entity_size = %d\n", header.header_size, header.entities, header.entity_size);
		goto file_len_error;
	}

	gen_crc = 0;
	fseek(fp, header.header_size, SEEK_SET);
	for(i = 0; i < header.entities; ++i){
		*p = malloc(sizeof(struct msn_node_t));
		if(*p == NULL){
			goto malloc_error;
		}
		memset(*p, 0, sizeof(struct msn_node_t));
		fread((*p)->msn, MSN_LEN, 1, fp);
		gen_crc = crc16Update(gen_crc, (*p)->msn, MSN_LEN);
		p = &(*p)->next;
	}
	if(gen_crc != header.content_crc){
		DBG("content crc error, gen_crc = %#x, header.content_crc = %#x\n", gen_crc, header.content_crc);
		goto content_crc_error;
	}

	return root;

content_crc_error:
malloc_error:
	msn_release(root);
file_len_error:
msn_len_not_match:
encrypt_alg_not_support:
project_not_match:
version_not_match:
magic_not_match:
header_crc_error:
header_not_match:
	fclose(fp);
	return NULL;
}

int msn_in_list(struct msn_node_t *msn_list, const char *msn)
{
	unsigned char encode[MSN_LEN];

	if(msn_list == NULL || msn == NULL || strlen(msn) != MSN_LEN)
		return 0;

	RC4_single(key, 5, msn, MSN_LEN, encode);

	while(msn_list){
		if(!memcmp(encode, msn_list->msn, MSN_LEN))
			return 1;
		msn_list = msn_list->next;
	}
	return 0;
}
void msn_release(struct msn_node_t *msn_list)
{
	struct msn_node_t *next;
	while(msn_list){
		next = msn_list->next;
		free(msn_list);
		msn_list = next;
	}
}

int create_msn_db(const char *device_msn, const char *filename)
{
	struct msn_node_t *root = NULL;
	if(filename == NULL)
		filename = DEFAULT_MSN_DB;
	if(RC4_encrypt_and_add_msn(&root, device_msn) == NULL){
		return -1;
	}
	return write_msn_to_file(root, filename);
}
