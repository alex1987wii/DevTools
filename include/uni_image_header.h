#ifndef _UNI_IMAGE_HEADER_H_
#define _UNI_IMAGE_HEADER_H_

/*
 *  partitions' functions
 */
#define PARTITION_IS_MINISYS              (0x01)
#define PARTITION_IS_IPL                  (0x01)
#define PARTITION_IS_WORKBIN              (0x02)
#define PARTITION_IS_LINUX                (0x02)
#define PARTITION_IS_CALIBRATION          (0x03)
#define PARTITION_IS_ROOTFS               (0x04)
#define PARTITION_IS_QPE                  (0x05)
#define PARTITION_IS_DEFPARA              (0x06)
#define PARTITION_IS_OTAPARA              (0x07)
#define PARTITION_IS_RUNPARA              (0x08)
#define PARTITION_IS_LOG                  (0x09)
#define PARTITION_IS_USERDATA             (0x0A)
#define PARTITION_IS_UPDATE               (0x0B)

/* flags */
/* bit[3:0] used for file system */
#define FS_IS_FAT                         (1<<0)
#define FS_IS_EXT4                        (2<<0)

/* bit[7:4] used for packet type */
#define PACKET_TYPE_IS_TARGZ              (1<<4)
#define PACKET_TYPE_IS_TAR                (2<<4)
#define PACKET_TYPE_IS_UNICATION          (3<<4)

/* bit[8:15] used for property */
#define PARTITION_PROTECTED               (1<<8)
#define FORMAT_BEFORE_UPDATE              (1<<9)
#define UPDATE_WHEN_DIFFERENT             (1<<10)
#define UPDATE_FILES_ONLY                 (1<<11)

#define UNI_IMAGE_VERSION_LEN             (16)
#define UNI_MAX_REL_VERSION_LEN           (32)
#define UNI_MAX_PARTITION                 (20)
#define UNI_MAX_PARTITION_NAME_LEN        (32)
#define UNI_MAX_PARTITION_VER_LEN         (32)

#define UNI_IMAGE_HEADER_SIZE             (4*1024)

#define IMAGE_VERSION                     "2.2.0"

struct uni_partition {
    unsigned char partition_name[UNI_MAX_PARTITION_NAME_LEN];
    unsigned char partition_version[UNI_MAX_PARTITION_VER_LEN];
    unsigned int partition_crc32;
    unsigned int offset_in_image;
    unsigned int length_of_contents; // the length of partition.tar.gz 
    unsigned int flags;
    unsigned int partition_index;
    unsigned int partition_size; // the Pre-Allocate partition size by fdisk
    unsigned int length_after_extracted; // the length of extracted partition.tar.gz 
    union _u_in_uni_partition
    {
        unsigned char partition_features[68];
    } u; 
}__attribute__ ((packed)); /* 160 Bytes */

typedef struct _uni_image_header 
{
    unsigned char image_version[UNI_IMAGE_VERSION_LEN];
    unsigned char project_name[UNI_MAX_REL_VERSION_LEN];
    unsigned char sys_version[UNI_MAX_REL_VERSION_LEN];
    unsigned char app_version[UNI_MAX_REL_VERSION_LEN];
    unsigned int header_crc32;
    unsigned int total_partitions;
    unsigned int emmc_block_size;
    unsigned int timestamp;
    union _u_in_uni_image_header
    {
        unsigned char image_features[128];
    } u; /* 16+32x3+4x4+128 = 256 Bytes */
    struct uni_partition partition[UNI_MAX_PARTITION]; /* 20x160 = 3200 Bytes*/
}__attribute__ ((packed)) uni_image_header_t; /* total 3456 Bytes */

#endif

