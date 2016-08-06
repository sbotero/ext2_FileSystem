#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ext2.h"

unsigned char *disk;
int flag;
struct ext2_super_block *sb;
struct ext2_group_desc *gd;

struct ext2_dir_entry_2 * get_prev(char * path_name) {

	int i, found;
	struct ext2_inode * curr_inode = (struct ext2_inode *)(disk + EXT2_BLOCK_SIZE * gd->bg_inode_table + sb->s_inode_size);
	struct ext2_dir_entry_2 * curr = (struct ext2_dir_entry_2 *) NULL;
	struct ext2_dir_entry_2 * prev = (struct ext2_dir_entry_2 *) NULL;

	char *token = strtok(path_name, "/");

	// Find the directory entry before the one being deleted
	while (token) {

		found = 0;
		for (i = 0; i < BLOCK_N; i++) {

			int size;
			size = 0;

			curr = (struct ext2_dir_entry_2 *) (disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE);

			if (curr_inode->i_block[i]) {
				while (size < EXT2_BLOCK_SIZE) {

					char buf[curr->name_len + 1];
		            memcpy(buf, curr->name, curr->name_len);
		            buf[curr->name_len] = '\0';

					if (!strcmp(buf, token)){

						found = 1;
						break;   
		            }
		            
		            size += curr->rec_len;
		            prev = curr;
		            curr = (struct ext2_dir_entry_2 *) ((disk + (curr_inode->i_block[i]) * EXT2_BLOCK_SIZE) + size);
				}

				if (found) {
					break;
				}
			}
		}

		curr_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (curr->inode - 1)));
		token = strtok(NULL, "/");
	}

    if (found) {
    	return prev;
    }
    // If not found (ie/ root)
    return NULL;

}


void delete_entry(struct ext2_dir_entry_2 * dir, struct ext2_dir_entry_2 * prev) {

	// Increase previous entry's rec_len by the directory being deleted
	if (prev) {
		prev -> rec_len += dir -> rec_len;
	}

	// Get inode
	struct ext2_inode *curr_inode = (struct ext2_inode *)(disk + (EXT2_BLOCK_SIZE * gd->bg_inode_table) + (sb->s_inode_size * (dir->inode - 1)));
	char * bitmap;

	int i, block;

	// Go through all blocks of inode and unset corresponding bit on block bitmap 
	for (i = 0; i < BLOCK_N; i++) {
		if (i < 12) {
			block = curr_inode -> i_block[i];
			bitmap = (char *) (disk + (gd -> bg_block_bitmap * EXT2_BLOCK_SIZE));
			bitmap[(block - 1) / 8] &= ~(1 << ((block - 1) % 8));
			sb -> s_free_blocks_count++;
			gd -> bg_free_blocks_count++;
		}
		// Single indirection
		else {
			int k;
			for (k = 0; k < EXT2_BLOCK_SIZE - 4; k = k + 4) {
				if (*(disk + EXT2_BLOCK_SIZE * *(curr_inode->i_block + i) + k)) {
					block = *(disk + EXT2_BLOCK_SIZE * *(curr_inode->i_block + i) + k);
					bitmap = (char *) (disk + (gd -> bg_block_bitmap * EXT2_BLOCK_SIZE));
					bitmap[(block - 1) / 8] &= ~(1 << ((block - 1) % 8));
					sb -> s_free_blocks_count++;
					gd -> bg_free_blocks_count++;
				}
			}
		}
	}

	// Check if inode is still needed
	if (!(--curr_inode->i_links_count)){

		// If not, delete and unset corresponding bit on inode bitmap
		curr_inode -> i_dtime = (unsigned int) time(NULL);
		bitmap = (char *) (disk + (gd -> bg_inode_bitmap * EXT2_BLOCK_SIZE));
		bitmap[(block - 1) / 8] &= ~(1 << ((block - 1) % 8));
		
		sb -> s_free_inodes_count++;
		gd -> bg_free_blocks_count++;
	}
}

int main(int argc, char **argv){

	char *disk_name = argv[1];
	char *path_name = argv[2];

	char path[strlen(path_name)];
	strncpy(path, path_name, strlen(path_name) + 1);

	int fd = open(disk_name, O_RDWR);

	disk = mmap(NULL, 128 * EXT2_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(disk == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	close(fd);

	sb = (struct ext2_super_block *)(disk + EXT2_BLOCK_SIZE);
	gd = (struct ext2_group_desc *)(disk + 2 * EXT2_BLOCK_SIZE);
	
	// get directory entry to be deleted
	struct ext2_dir_entry_2 *dir = path_traversal(path);

	if (!dir){
		return ENOENT;
	}

	// Cannot delete directories
	if (dir->file_type == EXT2_FT_DIR){
		fprintf(stderr, "Cant remove a directory\n");
		exit(EISDIR);
	}

	strncpy(path, path_name, strlen(path_name) + 1);
	struct ext2_dir_entry_2 * prev = get_prev(path);

	if (prev) {
		delete_entry(dir, prev);
	} else {
		delete_entry(dir, NULL);
	}
 
	return 0;
}
