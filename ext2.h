/* MODIFIED by Karen Reid for CSC369
 * to remove some of the unnecessary components */

/* MODIFIED by Tian Ze Chen for CSC369
 * to clean up the code and fix some bugs */

/*
 * Copyright (C) 1992, 1993, 1994, 1995
 * Remy Card (card@masi.ibp.fr)
 * Laboratoire MASI - Institut Blaise Pascal
 * Universite Pierre et Marie Curie (Paris VI)
 *
 *  from
 *
 *  linux/include/linux/minix_fs.h
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 */

#ifndef CSC369A3_EXT2_FS_H
#define CSC369A3_EXT2_FS_H

#define EXT2_BLOCK_SIZE 1024

#include<time.h>

/*
 * Structure of the super block
 */
struct ext2_super_block {
    unsigned int   s_inodes_count;      /* Inodes count */
    unsigned int   s_blocks_count;      /* Blocks count */
    unsigned int   s_r_blocks_count;    /* Reserved blocks count */
    unsigned int   s_free_blocks_count; /* Free blocks count */
    unsigned int   s_free_inodes_count; /* Free inodes count */
    unsigned int   s_first_data_block;  /* First Data Block */
    unsigned int   s_log_block_size;    /* Block size */
    unsigned int   s_log_frag_size;     /* Fragment size */
    unsigned int   s_blocks_per_group;  /* # Blocks per group */
    unsigned int   s_frags_per_group;   /* # Fragments per group */
    unsigned int   s_inodes_per_group;  /* # Inodes per group */
    unsigned int   s_mtime;             /* Mount time */
    unsigned int   s_wtime;             /* Write time */
    unsigned short s_mnt_count;         /* Mount count */
    unsigned short s_max_mnt_count;     /* Maximal mount count */
    unsigned short s_magic;             /* Magic signature */
    unsigned short s_state;             /* File system state */
    unsigned short s_errors;            /* Behaviour when detecting errors */
    unsigned short s_minor_rev_level;   /* minor revision level */
    unsigned int   s_lastcheck;         /* time of last check */
    unsigned int   s_checkinterval;     /* max. time between checks */
    unsigned int   s_creator_os;        /* OS */
    unsigned int   s_rev_level;         /* Revision level */
    unsigned short s_def_resuid;        /* Default uid for reserved blocks */
    unsigned short s_def_resgid;        /* Default gid for reserved blocks */
    /*
     * These fields are for EXT2_DYNAMIC_REV superblocks only.
     *
     * Note: the difference between the compatible feature set and
     * the incompatible feature set is that if there is a bit set
     * in the incompatible feature set that the kernel doesn't
     * know about, it should refuse to mount the filesystem.
     *
     * e2fsck's requirements are more strict; if it doesn't know
     * about a feature in either the compatible or incompatible
     * feature set, it must abort and not try to meddle with
     * things it doesn't understand...
     */
    unsigned int   s_first_ino;         /* First non-reserved inode */
    unsigned short s_inode_size;        /* size of inode structure */
    unsigned short s_block_group_nr;    /* block group # of this superblock */
    unsigned int   s_feature_compat;    /* compatible feature set */
    unsigned int   s_feature_incompat;  /* incompatible feature set */
    unsigned int   s_feature_ro_compat; /* readonly-compatible feature set */
    unsigned char  s_uuid[16];          /* 128-bit uuid for volume */
    char           s_volume_name[16];   /* volume name */
    char           s_last_mounted[64];  /* directory where last mounted */
    unsigned int   s_algorithm_usage_bitmap; /* For compression */
    /*
     * Performance hints.  Directory preallocation should only
     * happen if the EXT2_COMPAT_PREALLOC flag is on.
     */
    unsigned char  s_prealloc_blocks;     /* Nr of blocks to try to preallocate*/
    unsigned char  s_prealloc_dir_blocks; /* Nr to preallocate for dirs */
    unsigned short s_padding1;
    /*
     * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
     */
    unsigned char  s_journal_uuid[16]; /* uuid of journal superblock */
    unsigned int   s_journal_inum;     /* inode number of journal file */
    unsigned int   s_journal_dev;      /* device number of journal file */
    unsigned int   s_last_orphan;      /* start of list of inodes to delete */
    unsigned int   s_hash_seed[4];     /* HTREE hash seed */
    unsigned char  s_def_hash_version; /* Default hash version to use */
    unsigned char  s_reserved_char_pad;
    unsigned short s_reserved_word_pad;
    unsigned int   s_default_mount_opts;
    unsigned int   s_first_meta_bg; /* First metablock block group */
    unsigned int   s_reserved[190]; /* Padding to the end of the block */
};





/*
 * Structure of a blocks group descriptor
 */
struct ext2_group_desc
{
    unsigned int   bg_block_bitmap;      /* Blocks bitmap block */
    unsigned int   bg_inode_bitmap;      /* Inodes bitmap block */
    unsigned int   bg_inode_table;       /* Inodes table block */
    unsigned short bg_free_blocks_count; /* Free blocks count */
    unsigned short bg_free_inodes_count; /* Free inodes count */
    unsigned short bg_used_dirs_count;   /* Directories count */
    unsigned short bg_pad;
    unsigned int   bg_reserved[3];
};





/*
 * Structure of an inode on the disk
 */
struct ext2_inode {
	unsigned short i_mode;        /* File mode */
	unsigned short i_uid;         /* Low 16 bits of Owner Uid */
	unsigned int   i_size;        /* Size in bytes */
	unsigned int   i_atime;       /* Access time */
	unsigned int   i_ctime;       /* Creation time */
	unsigned int   i_mtime;       /* Modification time */
	unsigned int   i_dtime;       /* Deletion Time */
	unsigned short i_gid;         /* Low 16 bits of Group Id */
	unsigned short i_links_count; /* Links count */
	unsigned int   i_blocks;      /* Blocks count IN DISK SECTORS*/
	unsigned int   i_flags;       /* File flags */
	unsigned int   osd1;          /* OS dependent 1 */
	unsigned int   i_block[15];   /* Pointers to blocks */
	unsigned int   i_generation;  /* File version (for NFS) */
	unsigned int   i_file_acl;    /* File ACL */
	unsigned int   i_dir_acl;     /* Directory ACL */
	unsigned int   i_faddr;       /* Fragment address */
	unsigned int   extra[3];
};

/*
 * Type field for file mode
 */

/* #define EXT2_S_IFSOCK 0xC000 */ /* socket */
#define    EXT2_S_IFLNK  0xA000    /* symbolic link */
#define    EXT2_S_IFREG  0x8000    /* regular file */
/* #define EXT2_S_IFBLK  0x6000 */ /* block device */
#define    EXT2_S_IFDIR  0x4000    /* directory */
/* #define EXT2_S_IFCHR  0x2000 */ /* character device */
/* #define EXT2_S_IFIFO  0x1000 */ /* fifo */

/*
 * Special inode numbers
 */

/* #define EXT2_BAD_INO          1 */ /* Bad blocks inode */
#define    EXT2_ROOT_INO         2    /* Root inode */
/* #define EXT4_USR_QUOTA_INO    3 */ /* User quota inode */
/* #define EXT4_GRP_QUOTA_INO    4 */ /* Group quota inode */
/* #define EXT2_BOOT_LOADER_INO  5 */ /* Boot loader inode */
/* #define EXT2_UNDEL_DIR_INO    6 */ /* Undelete directory inode */
/* #define EXT2_RESIZE_INO       7 */ /* Reserved group descriptors inode */
/* #define EXT2_JOURNAL_INO      8 */ /* Journal inode */
/* #define EXT2_EXCLUDE_INO      9 */ /* The "exclude" inode, for snapshots */
/* #define EXT4_REPLICA_INO     10 */ /* Used by non-upstream feature */

/* First non-reserved inode for old ext2 filesystems */
#define EXT2_GOOD_OLD_FIRST_INO 11





/*
 * Structure of a directory entry
 */

#define EXT2_NAME_LEN 255

/* WARNING: DO NOT use this struct, ext2_dir_entry_2 is the
 * one to use for the assignement */
struct ext2_dir_entry {
    unsigned int   inode;    /* Inode number */
    unsigned short rec_len;  /* Directory entry length */
    unsigned short name_len; /* Name length */
    char           name[];   /* File name, up to EXT2_NAME_LEN */
};

/*
 * The new version of the directory entry.  Since EXT2 structures are
 * stored in intel byte order, and the name_len field could never be
 * bigger than 255 chars, it's safe to reclaim the extra byte for the
 * file_type field.
 */

struct ext2_dir_entry_2 {
    unsigned int   inode;     /* Inode number */
    unsigned short rec_len;   /* Directory entry length */
    unsigned char  name_len;  /* Name length */
    unsigned char  file_type;
    char           name[];    /* File name, up to EXT2_NAME_LEN */
};

/*
 * Ext2 directory file types.  Only the low 3 bits are used.  The
 * other bits are reserved for now.
 */

#define    EXT2_FT_UNKNOWN  0    /* Unknown File Type */
#define    EXT2_FT_REG_FILE 1    /* Regular File */
#define    EXT2_FT_DIR      2    /* Directory File */
/* #define EXT2_FT_CHRDEV   3 */ /* Character Device */
/* #define EXT2_FT_BLKDEV   4 */ /* Block Device */
/* #define EXT2_FT_FIFO     5 */ /* Buffer File */
/* #define EXT2_FT_SOCK     6 */ /* Socket File */
#define    EXT2_FT_SYMLINK  7    /* Symbolic Link */


#define    EXT2_FT_MAX      8


#define    BLOCK_N      15

//global variables
unsigned char *disk;
struct ext2_super_block *sb;
struct ext2_group_desc *gd;

struct ext2_dir_entry_2 *path_traversal(char *path_name){
    char *token = strtok(path_name, "/");
    struct ext2_inode *curr_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * gd->bg_inode_table + sb->s_inode_size);
    struct ext2_dir_entry_2 * curr_dir = (struct ext2_dir_entry_2 *) NULL;
    int found;
    
    char *root = "/";
    if (!strcmp(root, path_name)) {
        struct ext2_dir_entry_2 root_dir = {.inode = 2, .rec_len = 12, .name_len = 1, .file_type = 2};
        curr_dir = &root_dir;
        return curr_dir;
    }
    
    //start at root directory
    //if the path given was the root, we're done
    if (!token){
        found = 1;
    }
    
    while (token){
        found = 0;
        int i;
        for (i = 0; i < BLOCK_N; i++){
            if (i < 12) {
                int size = 0;
                curr_dir = (struct ext2_dir_entry_2 *) (disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE);
                if (curr_inode->i_block[i]) {
                	//check each dir to see if it's name matches the one we're looking for
                    while(size < EXT2_BLOCK_SIZE) {
                        char buf[curr_dir->name_len + 1];
                        strncpy(buf, curr_dir->name, curr_dir->name_len);
                        buf[curr_dir->name_len] = '\0';
                        if (!strcmp(buf, token)){
                            found = 1;
                            break;
                        }
                        size += curr_dir->rec_len;
                        curr_dir = (struct ext2_dir_entry_2 *) ((disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE) + size);
                    }
                    if (found == 1){
                        break;
                    }
                    
                }
            }
            // Handle single indirection
            else {
                int k;
                for (k = 0; k < EXT2_BLOCK_SIZE - 4; k = k + 4) {
                    if (*(disk + EXT2_BLOCK_SIZE * *(curr_inode->i_block + i) + k)) {
                        int block_num = *(disk + EXT2_BLOCK_SIZE * *(curr_inode->i_block + i) + k);
                        int size = 0;
                        curr_dir = (struct ext2_dir_entry_2 *) (disk + block_num * EXT2_BLOCK_SIZE);
                        if (curr_inode->i_block[i]) {
                            while(size < EXT2_BLOCK_SIZE) {
                                char buf[curr_dir->name_len + 1];
                                strncpy(buf, curr_dir->name, curr_dir->name_len);
                                buf[curr_dir->name_len] = '\0';
                                if (!strcmp(buf, token)){
                                    found = 1;
                                    break;
                                }
                                size += curr_dir->rec_len;
                                curr_dir = (struct ext2_dir_entry_2 *) ((disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE) + size);
                            }
                            if (found == 1){
                                break;
                            }
                        }
                    }
                }
            }
        }
        //need to check if the file was deleted
        //move to next inode
        curr_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (curr_dir->inode - 1)));
        token = strtok(NULL, "/");
    }
    if (found){
        return curr_dir;
    }
    return (struct ext2_dir_entry_2 *) NULL;
}

struct ext2_dir_entry_2 *create_entry(struct ext2_dir_entry_2 *dir, char *new_entry){
    int i,j,k;
    // Find a free inode in the inode bitmap and table for the new directory
    int inode_bitmap_block_num = gd->bg_inode_bitmap;
    int num_of_bytes_inode = (sb->s_inodes_count) / 8;
    int free_inode_num = 0;
    int current_inode_num = 0;
    
    for (i = 0; i < num_of_bytes_inode; i++) {
        k = *(disk + EXT2_BLOCK_SIZE * inode_bitmap_block_num + i);
        for (j = 0; j < 8; j++) {
            current_inode_num++;
            if ((free_inode_num == 0) && k % 2 == 0) {
                free_inode_num = current_inode_num;
            }
            k = k/2;
        }
    }
    
    printf("Free inode: %d\n", free_inode_num);
    
    if (free_inode_num == 0) {
        return NULL;
    }
    
    // Find a free block for inode's associated file data
    int block_bitmap_block_num = gd->bg_block_bitmap;
    int num_of_bytes_block = (sb->s_blocks_count) / 8;
    int free_block_num = 0;
    int current_block_num = 0;
    
    for (i = 0; i < num_of_bytes_block; i++) {
        k = *(disk + EXT2_BLOCK_SIZE * block_bitmap_block_num + i);
        for (j = 0; j < 8; j++) {
            current_block_num++;
            if ((free_block_num == 0) && k % 2 == 0) {
                free_block_num = current_block_num;
            }
            k = k/2;
        }
    }
    
    if (free_block_num == 0) {
        return NULL;
    }
    
    printf("Free block: %d\n", free_block_num);
    
    // Instantiate inode with relevant information
    struct ext2_inode *new_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (free_inode_num - 1)));
    
    new_inode->i_mode = EXT2_S_IFDIR;        /* File mode */
    
    new_inode->i_size = sizeof(new_inode->i_mode) + sizeof(new_inode->i_size) + sizeof(new_inode->i_dtime) + sizeof(new_inode->i_links_count) + sizeof(new_inode->i_blocks) + sizeof(new_inode->i_block);
    
    new_inode->i_dtime = 0;       /* Deletion Time */
    new_inode->i_links_count = 2; /* Links count */
    new_inode->i_blocks = 2;      /* Blocks count IN DISK SECTORS*/
    
    /* Pointers to blocks */
    for (i = 0; i < BLOCK_N; i++) {
        if (i == 0) {
            new_inode->i_block[i] = free_block_num;
        }
        else {
            new_inode->i_block[i] = 0;
        }
    }
    
    printf("Check 1: %d\n", free_block_num);


    // Find an address (location) to place the new directory entry
    struct ext2_inode *parent_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (dir->inode - 1)));
    
    int size_of_new_dir = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(new_entry);
    
    // Make new directory entry size 4B aligned
    int remainder = size_of_new_dir % 4;
    int lack_in_bytes = 4 - remainder;
    if (lack_in_bytes < 4) {
        size_of_new_dir += lack_in_bytes;
    }
    
    struct ext2_dir_entry_2 *new_dir;
    
    int found_space = 0;
    
    printf("Check 2: %d\n", free_block_num);
    
    for (i = 0; i < BLOCK_N; i++) {
        if (found_space) {
            break;
        }
        if (*(parent_inode->i_block + i)) {
            if (i < 12) {
                int off_set = 0;
                while (off_set != EXT2_BLOCK_SIZE) {
                    struct ext2_dir_entry_2 *curr_dir = (struct ext2_dir_entry_2 *) (disk + (parent_inode->i_block[i]) * EXT2_BLOCK_SIZE + off_set);
                    // Check if curr_dir is the last directory entry in the block
                    if ((off_set + curr_dir->rec_len) == EXT2_BLOCK_SIZE) {
                        
                        
                        return NULL;
                        
                        
                        
                        
                        // Check if the directory entry has enough empty space
                        int curr_dir_required_size = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(curr_dir->name);
                        remainder = curr_dir_required_size % 4;
                        lack_in_bytes = 4 - remainder;
                        if (lack_in_bytes < 4) {
                            curr_dir_required_size += lack_in_bytes;
                        }
                        int space_available = curr_dir->rec_len - curr_dir_required_size;
                        printf("Space Available: %d\n", space_available);
                        
                        // If there is enough space for the new directory entry
                        if (space_available >= size_of_new_dir) {
                            printf("Check 3: %d\n", free_block_num);
                            found_space = 1;
                            curr_dir->rec_len = curr_dir_required_size;
                            new_dir = (struct ext2_dir_entry_2 *) (disk + *(parent_inode->i_block + i) * EXT2_BLOCK_SIZE + off_set + curr_dir->rec_len);
                            new_dir->inode = free_inode_num;
                            new_dir->rec_len = size_of_new_dir;
                            new_dir->name_len = strlen(new_entry);
                            new_dir->file_type = EXT2_FT_DIR;
                            strncpy(new_dir->name, new_entry, new_dir->name_len);
                            break;
                        }
                        
                        
                    }
                    printf("CUR: %d\n", curr_dir->rec_len);
                    off_set += curr_dir->rec_len;
                }
            }
            // Handle single indirection
            else {
                for (j = 0; j < EXT2_BLOCK_SIZE; j = j + 4) {
                    if (*(disk + EXT2_BLOCK_SIZE * *(parent_inode->i_block + i) + j)) {
                        int curr_block_num = *(disk + EXT2_BLOCK_SIZE * *(parent_inode->i_block + i) + j);
                        int off_set = 0;
                        while (off_set != EXT2_BLOCK_SIZE) {
                            struct ext2_dir_entry_2 *curr_dir = (struct ext2_dir_entry_2 *) (disk + curr_block_num * EXT2_BLOCK_SIZE + off_set);
                            // Check if curr_dir is the last directory entry in the block
                            if ((off_set + curr_dir->rec_len) == EXT2_BLOCK_SIZE) {
                                // Check if the directory entry has enough empty space
                                int curr_dir_required_size = sizeof(unsigned int) + sizeof(unsigned short) + sizeof(unsigned char) + sizeof(unsigned char) + sizeof(unsigned char) * strlen(curr_dir->name);
                                remainder = curr_dir_required_size % 4;
                                lack_in_bytes = 4 - remainder;
                                if (lack_in_bytes < 4) {
                                    curr_dir_required_size += lack_in_bytes;
                                }
                                int space_available = curr_dir->rec_len - curr_dir_required_size;
                                
                                // If there is enough space for the new directory entry
                                if (space_available >= size_of_new_dir) {
                                    found_space = 1;
                                    curr_dir->rec_len = curr_dir_required_size;
                                    new_dir = (struct ext2_dir_entry_2 *) (disk + *(parent_inode->i_block + i) * EXT2_BLOCK_SIZE + off_set + curr_dir->rec_len);
                                    new_dir->inode = free_inode_num;
                                    new_dir->rec_len = size_of_new_dir;
                                    new_dir->name_len = strlen(new_entry);
                                    new_dir->file_type = EXT2_FT_DIR;
                                    strncpy(new_dir->name, new_entry, new_dir->name_len);
                                    break;
                                }
                                
                                
                            }
                            off_set += curr_dir->rec_len;
                        }
                    }
                }
            }
        }
    }
    
    if (found_space == 0) {
        return NULL;
    }

    // Create . and .. directory entry for the new directory
    struct ext2_dir_entry_2 *first_dir = (struct ext2_dir_entry_2 *) (disk + free_block_num * EXT2_BLOCK_SIZE);
    first_dir->inode = new_dir->inode;
    first_dir->rec_len = 12;
    first_dir->name_len = 1;
    first_dir->file_type = EXT2_FT_DIR;
    strncpy(first_dir->name, ".", first_dir->name_len);
    
    struct ext2_dir_entry_2 *second_dir = (struct ext2_dir_entry_2 *) (disk + free_block_num * EXT2_BLOCK_SIZE + first_dir->rec_len);
    second_dir->inode = dir->inode;
    second_dir->rec_len = 12;
    second_dir->name_len = 2;
    second_dir->file_type = EXT2_FT_DIR;
    strncpy(second_dir->name, "..", second_dir->name_len);
    
    return new_dir;
}

#endif
