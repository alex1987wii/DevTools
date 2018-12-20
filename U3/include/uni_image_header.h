#ifndef _UNI_IMAGE_HEADER_H_
#define _UNI_IMAGE_HEADER_H_

#define PARTITION_IS_IPL                  (0x01)
#define PARTITION_IS_SPL                  (0x02)
#define PARTITION_IS_RAMDISK              (0x07)
#define PARTITION_PROTECTED               (1<<4)
#define PARTITION_WITH_OOB                (1<<5)
#define RECV_ALL_BEFORE_FLASH             (1<<6)

#define SET_PARTITION_INDEX(x)            (((x) & 0x0F) << 28) // maximum index is 0x0F
#define GET_PARTITION_INDEX(x)            (((x) >> 28) & 0x0F)

#define IS_RAW                            (1)
#define IS_YAFFS2                         (2)

#define UNI_IMAGE_VERSION_LEN             (8)
#define UNI_MAX_REL_VERSION_LEN           (64)
#define UNI_MAX_PARTITION                 (16)
#define UNI_MAX_PARTITION_NAME_LEN        (32)
#define UNI_IMAGE_HEADER_SIZE             (1024)

#define IMAGE_VERSION                     "1.0.0"

struct uni_partition {
    unsigned char partition_name[UNI_MAX_PARTITION_NAME_LEN];
    unsigned int partition_crc;
    unsigned int offset_in_nand;
    unsigned int offset_in_image;
    unsigned int length;
    unsigned int partition_size;
    unsigned int flags;
}__attribute__ ((packed)); /* 56 Bytes */

typedef struct _uni_image_header
{
    unsigned char image_version[UNI_IMAGE_VERSION_LEN];
    unsigned char release_version[UNI_MAX_REL_VERSION_LEN];
    unsigned int header_crc;
    unsigned int image_type;
    unsigned int total_partitions;
    struct uni_partition partition[UNI_MAX_PARTITION]; /* 16x56 = 896 Bytes*/
    unsigned int instruction_ver_of_ipl;
    unsigned long long image_features;
}__attribute__ ((packed)) uni_image_header_t;

#endif
