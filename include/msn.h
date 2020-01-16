#ifndef _MSN_H_
#define _MSN_H_

#ifdef __cplusplus
extern "C" {
#endif
enum {
	ALG_NONE = 0,
	ALG_MD5,
	ALG_AES,
	ALG_DES,
	ALG_RSA,
	ALG_DSA,
	ALG_SHA1,
	ALG_SHA256,
	ALG_RC4,
};

#define MAJOR_VER	0
#define MINOR_VER	1
#define MSN_MAGIC	"MSN"

#ifndef PROJECT
#define PROJECT		"g4_bba"
#endif
struct msn_header_t{
	char magic[4]; /* "MSN" */
	short major_ver; /* 0 */
	short minor_ver; /* 1 */
	unsigned int header_size; /* size of this header */
	unsigned short header_crc; /* crc16 */
	unsigned short content_crc; /* crc16, this member should just after header_crc*/
	char project[16]; /* "g4_bba" */
	int encrypt_alg; /* encryption algorithm for MSN */
	unsigned int entities; /* how many entities(MSN) in this file */
	unsigned int entity_size; /* the size of entity(MSN) */
};

#define offset_of(type, member)	((size_t)&((type *)0)->member)


#define MSN_LEN		10
struct msn_node_t{
	struct msn_node_t *next;
	unsigned char msn[MSN_LEN];
};

/* encrypt a msn and add into msn_list
 * msn_list : the address of msn root list
 * msn : the string(null-terminate) of msn, all msn should have the same length
 * return value : NULL -> failed
 * 		the address of new msn_node
 */
struct msn_node_t *RC4_encrypt_and_add_msn(struct msn_node_t **msn_list, const char *msn);

/* write msn list into a file
 * msn_list : the msn_list root
 * filename : the filename which save the msn_list
 * return value : 0 -> OK
 *		!= 0 -> error
 */
int write_msn_to_file(struct msn_node_t *msn_list, const char *filename);

//int verify_msn_file(const char *filename);

/* open a msn_list file 
 * filename : the file to open
 * return value : NULL -> failed
 * 		the msn_list root
 */
struct msn_node_t *msn_open(const char *filename);

/* judge if a msn in a msn_list
 * msn_list : the msn_list root
 * msn : the msn(un-encrypt) string
 * return value : 0 -> not in msn_list
 * 		  1 -> is in msn_list
 */
int msn_in_list(struct msn_node_t *msn_list, const char *msn);

/* release the list space
 * msn_list :  the msn_list root, generally returned by msn_open
 */
void msn_release(struct msn_node_t *msn_list);

#ifdef __cplusplus
}
#endif
#endif
